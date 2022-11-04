#include "../Matrix2D.h"
#include "../MpiConfig.h"
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

#define DIMENSIONS 2

int main(int argc, char** argv) {
	int myRank, numProcs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	// 'parsing' optional input parameter = problem size
	int N = 200;
	std::string fileName = "temperature.txt";
	if(argc > 1) {
		std::stoi(argv[1]);
	}
	if(argc > 2) {
		fileName = argv[2];
	}
	int T = N * 500;

	if(myRank == 0) {
		std::cout << "Computing heat-distribution for room size " << N << "X" << N << " for T=" << T
		          << " timesteps" << std::endl;
	}

	// ---------- setup initial data ----------
	double sqrtProcs = std::sqrt(numProcs);
	if(std::abs(pow(sqrtProcs, 2) - numProcs) > 000.1) {
		perror("Processors must give a sqrt of 2");
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	int procsByDim = (int) std::round(sqrtProcs);
	int success = 1;

	std::array<int, DIMENSIONS> dims = { 0, 0 };
	std::array<int, DIMENSIONS> periods = { false, false };
	MPI_Dims_create(numProcs, DIMENSIONS, dims.begin());
	MPI_Comm cartesianCommunicator;
	MPI_Cart_create(MPI_COMM_WORLD, DIMENSIONS, dims.begin(), periods.begin(), true,
	                &cartesianCommunicator);

	MPI_Comm_rank(cartesianCommunicator, &myRank);
	std::array<int, DIMENSIONS> myCoords;
	MPI_Cart_coords(cartesianCommunicator, myRank, DIMENSIONS, myCoords.begin());

	int subSize = N / procsByDim;
	{
		auto A = Matrix2D(subSize, 273.0);
		auto B = Matrix2D(subSize, 273.0);

		MPIVectorConfig hConfig = A.getHorizontalGhostCellsConfig();
		MPI_Datatype horizontalGhostCells;
		MPI_Type_vector(hConfig.nBlocks, hConfig.blockSize, hConfig.stride, MPI_FLOAT,
		                &horizontalGhostCells);
		MPI_Type_commit(&horizontalGhostCells);

		MPIVectorConfig vConfig = A.getVerticalGhostCellsConfig();
		MPI_Datatype verticalGhostCells;
		MPI_Type_vector(vConfig.nBlocks, vConfig.blockSize, vConfig.stride, MPI_FLOAT,
		                &verticalGhostCells);
		MPI_Type_commit(&verticalGhostCells);

		int source_x = -1;
		int source_y = -1;
		if(myCoords[0] + 1 == procsByDim / 2 && myCoords[1] + 1 == procsByDim / 2) {
			// and there is a heat source
			source_x = subSize - 1;
			source_y = subSize - 1;
			A(source_x, source_y) = 273 + 60;
		}

		double start = MPI_Wtime();

		int toTheWest, toTheEast, toTheNorth, toTheSouth;
		// east is at coordinates (i, j + 1); south at (i + 1, j)
		MPI_Cart_shift(cartesianCommunicator, 1, 1, &toTheWest, &toTheEast);
		MPI_Cart_shift(cartesianCommunicator, 0, 1, &toTheNorth, &toTheSouth);

		for(int t = 0; t < T; t++) {
			// sending EAST to WEST
			MPI_Sendrecv(A.getInnerEast(), 1, verticalGhostCells, toTheEast, 0, A.getOuterWest(), 1,
			             verticalGhostCells, toTheWest, MPI_ANY_TAG, cartesianCommunicator,
			             MPI_STATUS_IGNORE);

			// sending WEST to EAST
			MPI_Sendrecv(A.getInnerWest(), 1, verticalGhostCells, toTheWest, 0, A.getOuterEast(), 1,
			             verticalGhostCells, toTheEast, MPI_ANY_TAG, cartesianCommunicator,
			             MPI_STATUS_IGNORE);

			// sending SOUTH to NORTH
			MPI_Sendrecv(A.getInnerSouth(), 1, horizontalGhostCells, toTheSouth, 0,
			             A.getOuterNorth(), 1, horizontalGhostCells, toTheNorth, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);

			// sending NORTH to SOUTH
			MPI_Sendrecv(A.getInnerNorth(), 1, horizontalGhostCells, toTheNorth, 0,
			             A.getOuterSouth(), 1, horizontalGhostCells, toTheSouth, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);

			for(int i = 0; i < A.size; ++i) {
				for(int j = 0; j < A.size; ++j) {
					if((i == source_x && j == source_y)) {
						B(i, j) = A(i, j);
					} else {
						B(i, j) = A(i, j) + 0.2 * (-4.0 * A(i, j) + A(i - 1, j) + A(i + 1, j) +
						                           A(i, j - 1) + A(i, j + 1));
					}
				}
			}

			A.swap(B);
		}

		double end = MPI_Wtime();

		// simple verification if nowhere the heat is more then the heat source
		for(long long i = 0; i < A.size; i++) {
			for(long long j = 0; j < A.size; j++) {
				double temp = A(i, j);

				if(273 <= temp && temp <= 273 + 60) {
					continue;
				}
				std::cout << "Verification failed at " << myRank << " " << i << " " << j << " "
				          << temp << std::endl;

				success = 0;
				break;
			}
		}

		// gathering all information
		int total_success = 0;
		MPI_Reduce(&success, &total_success, 1, MPI_INT, MPI_SUM, 0, cartesianCommunicator);

		auto GATHERED = Matrix2D(N, 273.0, false);
		MPI_Datatype sendSubMatrix;

		MPISendReceiveConfig<DIMENSIONS> sendConfig = A.getSendConfig();

		MPI_Type_create_subarray(2, sendConfig.sizes.begin(), sendConfig.subSizes.begin(),
		                         sendConfig.coords.begin(), MPI_ORDER_C, MPI_FLOAT, &sendSubMatrix);
		MPI_Type_commit(&sendSubMatrix);

		MPI_Datatype receiveSubMatrix, receiveOneLineBlock;

		MPISendReceiveConfig<DIMENSIONS> receiveConfig = GATHERED.getReceiveConfig(A);
		MPI_Type_create_subarray(DIMENSIONS, receiveConfig.sizes.begin(), receiveConfig.subSizes.begin(),
		                         receiveConfig.coords.begin(), MPI_ORDER_C, MPI_FLOAT,
		                         &receiveSubMatrix);

		MPI_Type_create_resized(receiveSubMatrix, 0, subSize * sizeof(float), &receiveOneLineBlock);
		MPI_Type_commit(&receiveOneLineBlock);

		std::vector<int> displacements(numProcs);
		int index = 0;
		for(int procRow = 0; procRow < procsByDim; procRow++) {
			for(int procColumn = 0; procColumn < procsByDim; procColumn++) {
				displacements[index++] = procColumn + procRow * (subSize * procsByDim);
			}
		}

		std::vector<int> counts(numProcs, 1);
		MPI_Gatherv(A.getOrigin(), 1, sendSubMatrix, GATHERED.getOrigin(), counts.data(),
		            displacements.data(), receiveOneLineBlock, 0, cartesianCommunicator);

		if(myRank == 0) {
			success = total_success == numProcs;
			std::cout << "Elapsed: " << end - start << std::endl;
			GATHERED.printHeatMap();
			GATHERED.writeToFile("gathered.txt");
		}

		MPI_Type_free(&horizontalGhostCells);
		MPI_Type_free(&verticalGhostCells);
		MPI_Type_free(&sendSubMatrix);
		MPI_Type_free(&receiveOneLineBlock);
	}

	MPI_Comm_free(&cartesianCommunicator);
	MPI_Finalize();

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}