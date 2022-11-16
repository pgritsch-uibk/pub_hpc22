#include "heat_stencil_common_2d.h"
#include "MPIMatrix2D.h"
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
	if(argc > 1) {
		N = std::stoi(argv[1]);
	}

	int T = N * 500;

	if(argc > 2) {
		T = std::stoi(argv[2]);
	}

	std::string fileName = "gathered2d_par" + std::to_string(N) + "_" + std::to_string(T);

	if(myRank == 0) {
		std::cout << "Computing heat-distribution for room size " << N << "X" << N << " for T=" << T
		          << " timesteps" << std::endl;
	}

	// ---------- setup initial data ----------
	double sqrtProcs = std::sqrt(numProcs);
	if(std::abs(pow(sqrtProcs, 2) - numProcs) > 000.1) {
		perror("Processors must give a sqrt of 2");
		MPI::Finalize();
		return EXIT_FAILURE;
	}
	int procsByDim = (int)std::round(sqrtProcs);
	bool success = true;

	std::array<int, DIMENSIONS> dims = { 0, 0 };
	std::array<int, DIMENSIONS> periods = { false, false };

	MPI_Comm cartesianCommunicator;
	MPI_Dims_create(numProcs, DIMENSIONS, dims.begin());
	MPI_Cart_create(MPI_COMM_WORLD, DIMENSIONS, dims.begin(), periods.begin(), true,
	                &cartesianCommunicator);

	MPI_Comm_rank(cartesianCommunicator, &myRank);
	std::array<int, DIMENSIONS> myCoords;
	MPI_Cart_coords(cartesianCommunicator, myRank, DIMENSIONS, myCoords.begin());

	int subSize = N / procsByDim;
	{
		auto A = mpi::Matrix2D(cartesianCommunicator, subSize, 273.0f);
		auto B = mpi::Matrix2D(cartesianCommunicator, subSize, 273.0f);

		int source_x = -1;
		int source_y = -1;
		bool myRankHasHeatSource =
		    myCoords[0] + 1 == procsByDim / 2 && myCoords[1] + 1 == procsByDim / 2;

		if(myRankHasHeatSource) {
			// and there is a heat source
			source_x = subSize - 1;
			source_y = subSize - 1;
			A(source_x, source_y) = HEAT_SOURCE;
		}

		double start = MPI_Wtime();

		for(int t = 0; t < T; t++) {
			A.performSendRecv();

			for(int i = 0; i < A.size; ++i) {
				for(int j = 0; j < A.size; ++j) {
					B(i, j) = calculateNewValue(A, i, j);
				}
			}

			if(myRankHasHeatSource) {
				B(source_x, source_y) = HEAT_SOURCE;
			}

			A.swap(B);
		}

		double end = MPI_Wtime();

		success = isResultSane(A);

		auto gathered = mpi::Matrix2D::gatherAll(cartesianCommunicator, A, 0, procsByDim, numProcs);

		if(myRank == 0) {
			std::cout << "Elapsed: " << end - start << std::endl;
			gathered->printHeatMap();
			gathered->writeToFile(fileName);
		}
	}

	MPI_Comm_free(&cartesianCommunicator);
	MPI_Finalize();

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}
