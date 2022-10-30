#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <string>
#include <vector>
#include <mpi.h>
#include <cmath>

#define RESOLUTION_WIDTH 50
#define RESOLUTION_HEIGHT 50

#define DIMENSIONS 2

// -- vector utilities --
struct Matrix2D {

	int size;

	std::vector<float> vec;

	Matrix2D(int size, float initial_value) {
		std::vector<float> temp(size * size, initial_value);
		std::swap(this->vec, temp);
		this->size = size;
	}

	float& operator()(int i, int j) { return this->vec[i * this->size + j]; }
};

void writeTemperatureToFile(Matrix2D& temperature, std::string filename);

// -- simulation code ---

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
		printf("Computing heat-distribution for room size %dX%d for T=%d timesteps\n", N, N, T);
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
	MPI_Type_vector(subSize, 1, subSize, MPI_FLOAT, &verticalGhostCells);
	MPI_Type_commit(&verticalGhostCells);

	auto A = Matrix2D(subSize, 273.0);
	auto B = Matrix2D(subSize, 273.0);


	int source_x = -1;
	int source_y = -1;
	if (myCoords[0] == 0 && myCoords[1] == 0) {
		// and there is a heat source
		source_x = 0;
		source_y = 0;
		A(source_x, source_y) = 273 + 60;
	}

	double start = MPI_Wtime();

	// ---------- compute ----------

	for(int t = 0; t < T; t++) {

		std::vector<float> ghostsNorth(subSize, 273.0);
		std::vector<float> ghostsSouth(subSize, 273.0);
		std::vector<float> ghostsWest(subSize, 273.0);
		std::vector<float> ghostsEast(subSize, 273.0);

		if (myCoords[0] % 2) {
			//receive left
			// send right

			//receive right
			//send left

		} else {
			//send right
			//receive left
		}

		MPI_Sendrecv(&A.vec[subSize - 1], 1, verticalGhostCells, )

		for(int i = 0; i < A.size; ++i) {
			for(int j = 0; j < A.size; ++j) {
				if((i == source_x && j == source_y)) {
					B(i, j) = A(i, j);
				} else {
					B(i, j) =
					    A(i, j) + 0.01 * (-4.0 * A(i, j) + A(i == 0 ? i : i - 1, j) +
					                      A(i == N - 1 ? i : i + 1, j) + A(i, j == 0 ? j : j - 1) +
					                      A(i, j == N - 1 ? j : j + 1));
				}
			}
		}

		std::swap(A, B);
	}

	double end = MPI_Wtime();

	// ---------- check ----------

	printf("Elapsed: %f \n", end - start);

	// simple verification if nowhere the heat is more then the heat source
	int success = 1;
	for(long long i = 0; i < N; i++) {
		for(long long j = 0; j < N; j++) {
			double temp = A(i, j);
			if(273 <= temp && temp <= 273 + 60) continue;
			success = 0;
			break;
		}
	}

	printf("Verification: %s\n", (success) ? "OK" : "FAILED");

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void writeTemperatureToFile(Matrix2D& temperature, std::string filename) {

	std::ofstream file;
	file.open(filename);

	for (auto& val : temperature.vec) {
		file << std::setw(2) << val << std::endl;
	}

	file.close();
}
