#include "../Matrix3D.h"
#include "../MpiConfig.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

#define DIMENSIONS 3

int main(int argc, char** argv) {
	int myRank, numProcs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	// 'parsing' optional input parameter = problem size
	int N = 100;

	if(argc > 1) {
		std::stoi(argv[1]);
	}

	int T = (int) std::round(std::pow(N, 2) * 3.0);

	std::string fileName = "gathered3d_par" + std::to_string(N) + "_" + std::to_string(T);

	if(myRank == 0) {
		std::cout << "Computing heat-distribution for room size " << N << "X" << N << "x" << N
		          << " for T=" << T << " timesteps" << std::endl;
	}

	// ---------- setup initial data ----------
	double cbrtProcs = std::cbrt(numProcs);
	if(std::abs(pow(cbrtProcs, 3) - numProcs) > 000.1) {
		perror("Processors must have an integer value when doing cbrt");
		MPI_Finalize();
		return EXIT_FAILURE;
	}
	int procsByDim = (int) std::round(cbrtProcs);
	int success = 1;
	if (N % procsByDim != 0) {
		std::cerr << "Problem size must be divisible by processors by dimension" << std::endl;
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	std::array<int, DIMENSIONS> dims = { 0, 0, 0 };
	std::array<int, DIMENSIONS> periods = { false, false, false };
	MPI_Dims_create(numProcs, DIMENSIONS, dims.begin());
	MPI_Comm cartesianCommunicator;
	MPI_Cart_create(MPI_COMM_WORLD, DIMENSIONS, dims.begin(), periods.begin(), true,
	                &cartesianCommunicator);

	MPI_Comm_rank(cartesianCommunicator, &myRank);
	std::array<int, DIMENSIONS> myCoords;
	MPI_Cart_coords(cartesianCommunicator, myRank, DIMENSIONS, myCoords.begin());

	int subSize = N / procsByDim;
	{
		auto A = Matrix3D(subSize, 273.0);
		auto B = Matrix3D(subSize, 273.0);

		std::vector<MPI_Datatype*> datatypes;

		auto ibConfig = A.getInnerBackGhostCellsConfig(),
		     obConfig = A.getOuterBackGhostCellsConfig(),
		     ifConfig = A.getInnerFrontGhostCellsConfig(),
		     ofConfig = A.getOuterFrontGhostCellsConfig();
		MPI_Datatype innerBackGhostCells, outerBackGhostCells, innerFrontGhostCells, outerFrontGhostCells;
		MPI_Type_create_subarray(DIMENSIONS, ibConfig.sizes.data(), ibConfig.subSizes.data(), ibConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerBackGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, obConfig.sizes.data(), obConfig.subSizes.data(), obConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerBackGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, ifConfig.sizes.data(), ifConfig.subSizes.data(), ifConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerFrontGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, ofConfig.sizes.data(), ofConfig.subSizes.data(), ofConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerFrontGhostCells);
		datatypes.insert(datatypes.end(), {&innerBackGhostCells, &outerBackGhostCells, &innerFrontGhostCells, &outerFrontGhostCells});

		auto ilConfig = A.getInnerLeftGhostCellsConfig(),
		     oLConfig = A.getOuterLeftGhostCellsConfig(),
		     iRConfig = A.getInnerRightGhostCellsConfig(),
		     oRConfig = A.getOuterRightGhostCellsConfig();
		MPI_Datatype innerLeftGhostCells, outerLeftGhostCells, innerRightGhostCells, outerRightGhostCells;
		MPI_Type_create_subarray(DIMENSIONS, ilConfig.sizes.data(), ilConfig.subSizes.data(), ilConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerLeftGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, oLConfig.sizes.data(), oLConfig.subSizes.data(), oLConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerLeftGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, iRConfig.sizes.data(), iRConfig.subSizes.data(), iRConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerRightGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, oRConfig.sizes.data(), oRConfig.subSizes.data(), oRConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerRightGhostCells);
		datatypes.insert(datatypes.end(), {&innerLeftGhostCells, &outerLeftGhostCells, &innerRightGhostCells, &outerRightGhostCells});

		auto iTConfig = A.getInnerTopGhostCellsConfig(),
		     oTConfig = A.getOuterTopGhostCellsConfig(),
		     iBConfig = A.getInnerBottomGhostCellsConfig(),
		     oBConfig = A.getOuterBottomGhostCellsConfig();
		MPI_Datatype innerTopGhostCells, outerTopGhostCells, innerBottomGhostCells, outerBottomGhostCells;
		MPI_Type_create_subarray(DIMENSIONS, iTConfig.sizes.data(), iTConfig.subSizes.data(), iTConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerTopGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, oTConfig.sizes.data(), oTConfig.subSizes.data(), oTConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerTopGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, iBConfig.sizes.data(), iBConfig.subSizes.data(), iBConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &innerBottomGhostCells);
		MPI_Type_create_subarray(DIMENSIONS, oBConfig.sizes.data(), oBConfig.subSizes.data(), oBConfig.coords.data(),
		                         MPI_ORDER_C, MPI_FLOAT, &outerBottomGhostCells);
		datatypes.insert(datatypes.end(), {&innerTopGhostCells, &outerTopGhostCells, &innerBottomGhostCells, &outerBottomGhostCells});

		std::for_each(datatypes.begin(), datatypes.end(), MPI_Type_commit);

		int source_x = -1;
		int source_y = -1;
		int source_z = -1;
		if(myCoords[0] + 1 == procsByDim / 2 &&
		   myCoords[1] + 1 == procsByDim / 2 &&
		   myCoords[2] + 1 == procsByDim / 2) {

			// and there is a heat source
			source_x = subSize - 1;
			source_y = subSize - 1;
			source_z = subSize - 1;
			A(source_x, source_y, source_z) = 273 + 200;
		}

		double start = MPI_Wtime();

		int toTheLeft, toTheRight, toTheTop, toTheBottom, toTheFront, toTheBack;
		// back is at coordinates (x, y, z + 1); right is at (x, y + 1, z); bottom is at (x + 1, y, z)
		MPI_Cart_shift(cartesianCommunicator, 2, 1, &toTheFront, &toTheBack);
		MPI_Cart_shift(cartesianCommunicator, 1, 1, &toTheLeft, &toTheRight);
		MPI_Cart_shift(cartesianCommunicator, 0, 1, &toTheTop, &toTheBottom);

		for(int t = 0; t < T; t++) {
			// LEFT cell to RIGHT cell && vice versa
			MPI_Sendrecv(A.getOrigin(), 1, innerRightGhostCells, toTheRight, 0,
			             A.getOrigin(), 1, outerLeftGhostCells, toTheLeft, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);
			MPI_Sendrecv(A.getOrigin(), 1, innerLeftGhostCells, toTheLeft, 0,
			             A.getOrigin(), 1, outerRightGhostCells, toTheRight, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);

			// TOP cell to BOTTOM cell && vice versa
			MPI_Sendrecv(A.getOrigin(), 1, innerBottomGhostCells, toTheBottom, 0,
			             A.getOrigin(), 1, outerTopGhostCells, toTheTop, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);
			MPI_Sendrecv(A.getOrigin(), 1, innerTopGhostCells, toTheTop, 0,
			             A.getOrigin(), 1, outerBottomGhostCells, toTheBottom, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);

			// FRONT cell to BACK cell && vice versa
			MPI_Sendrecv(A.getOrigin(), 1, innerFrontGhostCells, toTheFront, 0,
			             A.getOrigin(), 1, outerBackGhostCells, toTheBack, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);
			MPI_Sendrecv(A.getOrigin(), 1, innerBackGhostCells, toTheBack, 0,
			             A.getOrigin(), 1, outerFrontGhostCells, toTheFront, MPI_ANY_TAG,
			             cartesianCommunicator, MPI_STATUS_IGNORE);

			for(int i = 0; i < A.size; ++i) {
				for(int j = 0; j < A.size; ++j) {
					for (int k = 0; k < A.size; ++k) {
						if((i == source_x && j == source_y && k == source_z)) {
							B(i, j, k) = A(i, j, k);
						} else {
							B(i, j, k) = A(i, j, k) +
							             0.166 * (-6.0 * A(i, j, k) +
							                    A(i - 1, j, k) + A(i + 1, j, k) +
							                    A(i, j - 1, k) + A(i, j + 1, k) +
							                    A(i, j, k - 1) + A(i, j, k + 1));
						}
					}
				}
			}

			A.swap(B);
		}

		double end = MPI_Wtime();

		// simple verification if nowhere the heat is more then the heat source
		for(int i = 0; i < A.size; i++) {
			for(int j = 0; j < A.size; j++) {
				for (int k = 0; k < A.size; k++) {
					double temp = A(i, j, k);
					if (myRank == 0 && A(source_x, source_y, source_z) != 273.0 + 200) {
						success =0;
					}

					if(273 <= temp && temp <= 273 + 200) {
						continue;
					}
					std::cout << "Verification failed at " << myRank << " "
					          << i << " " << j << " " << k << " "
					          << temp << std::endl;

					success = 0;
					break;
				}
			}
		}

		// gathering all information
		int total_success = 0;
		MPI_Reduce(&success, &total_success, 1, MPI_INT, MPI_SUM, 0, cartesianCommunicator);

		auto GATHERED = Matrix3D(N, 273.0, false);
		MPI_Datatype sendSubMatrix;
		MPISubarrayConfig<DIMENSIONS> sendConfig = A.getSendConfig();

		MPI_Type_create_subarray(DIMENSIONS, sendConfig.sizes.begin(), sendConfig.subSizes.begin(),
		                         sendConfig.coords.begin(), MPI_ORDER_C, MPI_FLOAT, &sendSubMatrix);
		MPI_Type_commit(&sendSubMatrix);

		MPI_Datatype receiveSubMatrix, subSizedLine;
		MPISubarrayConfig<DIMENSIONS> receiveConfig = GATHERED.getReceiveConfig(A);
		MPI_Type_create_subarray(DIMENSIONS, receiveConfig.sizes.begin(), receiveConfig.subSizes.begin(),
		                         receiveConfig.coords.begin(), MPI_ORDER_C, MPI_FLOAT,
		                         &receiveSubMatrix);

		MPI_Type_create_resized(receiveSubMatrix, 0, subSize * sizeof(float), &subSizedLine);
		MPI_Type_commit(&subSizedLine);

		std::vector<int> displacements(numProcs);
		int index = 0;
		for(int procRow = 0; procRow < procsByDim; procRow++) {
			for(int procColumn = 0; procColumn < procsByDim; procColumn++) {
				for(int procDepth = 0; procDepth < procsByDim; procDepth++) {
					displacements[index++] = procDepth +
					                         procColumn * subSize * procsByDim +
					                         procRow * subSize * subSize * procsByDim * procsByDim;
				}
			}
		}

		std::vector<int> counts(numProcs, 1);
		MPI_Gatherv(A.getOrigin(), 1, sendSubMatrix, GATHERED.getOrigin(), counts.data(),
		            displacements.data(), subSizedLine, 0, cartesianCommunicator);

		if(myRank == 0) {
			std::cout << std::endl;
			success = total_success == numProcs;
			std::cout << "Elapsed: " << end - start << std::endl;
			GATHERED.writeToFile(fileName);
		}

		std::for_each(datatypes.begin(), datatypes.end(), MPI_Type_free);
		MPI_Type_free(&sendSubMatrix);
		MPI_Type_free(&subSizedLine);
	}

	MPI_Comm_free(&cartesianCommunicator);
	MPI_Finalize();

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}