#include <cstdlib>
#include <string>
#include <vector>
#include <mpi.h>
#include <cmath>
#include "../Matrix2D.h"

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

	if (myRank == 0) {
		std::cout << "Computing heat-distribution for room size " << N << "X" << N << " for T=" << T << " timesteps" << std::endl;
	}

	// ---------- setup initial data ----------
	double sqrtProcs = sqrt(numProcs);
	if (std::abs(pow(sqrtProcs, 2) - numProcs) > 000.1) {
		perror("Processors must give a sqrt of 2");
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	int dims[] = {0, 0};
	int periods[] = {false, false};
	MPI_Dims_create(numProcs, DIMENSIONS, dims);
	MPI_Comm cartesianCommunicator;
	MPI_Cart_create(MPI_COMM_WORLD, DIMENSIONS, dims, periods, true, &cartesianCommunicator);

	MPI_Comm_rank(cartesianCommunicator, &myRank);
	int myCoords[DIMENSIONS];
	MPI_Cart_coords(cartesianCommunicator, myRank, DIMENSIONS, myCoords);

	int subSize = N / (int)round(sqrtProcs);

	MPI_Datatype horizontalGhostCells;
	MPI_Type_vector(subSize, subSize, subSize, MPI_FLOAT, &horizontalGhostCells);
	MPI_Type_commit(&horizontalGhostCells);

	MPI_Datatype verticalGhostCells;
	MPI_Type_vector(subSize, 1, subSize + 2, MPI_FLOAT, &verticalGhostCells);
	MPI_Type_commit(&verticalGhostCells);

	auto A = Matrix2D(subSize, 273.0);
	auto B = Matrix2D(subSize, 273.0);

	int source_x = -1;
	int source_y = -1;
	if (myCoords[0] == 0 && myCoords[1] == 0) {
		// and there is a heat source
		source_x = subSize / 2;
		source_y = subSize / 2;
		A(source_x, source_y) = 273 + 60;
	}

	double start = MPI_Wtime();

	int toTheWest, toTheEast, toTheNorth, toTheSouth;
	MPI_Cart_shift(cartesianCommunicator, 1, 1, &toTheWest, &toTheEast);
	MPI_Cart_shift(cartesianCommunicator, 0, 1, &toTheNorth, &toTheSouth);
	for(int t = 0; t < T; t++) {

		// sending EAST to WEST
		MPI_Sendrecv(A.getInnerEast(), 1, verticalGhostCells, toTheEast, 0,
		             A.getOuterWest(), 1, verticalGhostCells, toTheWest, MPI_ANY_TAG,
		             cartesianCommunicator, MPI_STATUS_IGNORE);

		// sending WEST to EAST
		MPI_Sendrecv(A.getInnerWest(), 1, verticalGhostCells, toTheWest, 0,
		             A.getOuterEast(), 1, verticalGhostCells, toTheEast, MPI_ANY_TAG,
		             cartesianCommunicator, MPI_STATUS_IGNORE);

		// sending SOUTH to NORTH
		MPI_Sendrecv(A.getInnerSouth(), 1, horizontalGhostCells, toTheSouth, 0,
		             A.getOuterNorth(), 1, horizontalGhostCells, toTheNorth, MPI_ANY_TAG,
		             cartesianCommunicator, MPI_STATUS_IGNORE);

		// sending NORTH to SOUTH
		MPI_Sendrecv(A.getInnerNorth(), 1, horizontalGhostCells, toTheNorth, 0,
		             A.getOuterSouth(), 1, horizontalGhostCells, toTheSouth, MPI_ANY_TAG,
		             cartesianCommunicator, MPI_STATUS_IGNORE);


		for(int i = 0; i < A.getSize(); ++i) {
			for(int j = 0; j < A.getSize(); ++j) {
				if((i == source_x && j == source_y)) {
					B(i, j) = A(i, j);
				} else {
					B(i, j) =
					    A(i, j) + 0.2 * (-4.0 * A(i, j) +
					                     A(i - 1, j) +
					                     A(i + 1, j) +
					                     A(i, j - 1) +
					                     A(i, j + 1));
				}
			}
		}

		std::swap(A, B);
	}

	double end = MPI_Wtime();

	// ---------- check ----------

	if (myRank == 0) {
		std::cout << "Elapsed: " << end - start << std::endl;
		A.printHeatMap();
	}

	// simple verification if nowhere the heat is more then the heat source
	int success = 1;
	for(long long i = 0; i < A.getSize(); i++) {
		for(long long j = 0; j < A.getSize(); j++) {
			double temp = A(i, j);
			if(273 <= temp && temp <= 273 + 60) continue;
			success = 0;
			break;
		}
	}

	int total_success = 0;
	MPI_Reduce(&success, &total_success, 1, MPI_INT, MPI_SUM, 0, cartesianCommunicator);

	if (myRank == 0) {
		success = total_success == numProcs;
		std::cout << "Method execution took seconds: " <<  end - start << std::endl;
	}

	A.~Matrix2D();
	B.~Matrix2D();

	MPI_Comm_free(&cartesianCommunicator);
	MPI_Type_free(&horizontalGhostCells);
	MPI_Type_free(&verticalGhostCells);
	MPI_Finalize();

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}