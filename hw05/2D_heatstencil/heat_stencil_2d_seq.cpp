#include "heat_stencil_common_2d.h"
#include "Matrix2D.h"
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

int main(int argc, char** argv) {
	// 'parsing' optional input parameter = problem size
	int N = 200;
	if(argc > 1) {
		N = std::stoi(argv[1]);
	}

	int T = N * 500;
	if(argc > 2) {
		T = std::stoi(argv[2]);
	}

	std::string fileName = "gathered2d_seq" + std::to_string(N) + "_" + std::to_string(T);

	std::cout << "Computing heat-distribution for room size " << N << "X" << N << " for T=" << T
	          << " timesteps" << std::endl;
	// ---------- setup initial data ----------
	auto A = Matrix2D(N, 273.0);
	auto B = Matrix2D(N, 273.0);

	// and there is a heat source
	int source_x = N / 2 - 1;
	int source_y = N / 2 - 1;
	A(source_x, source_y) = 273 + 60;

	double start = MPI_Wtime();
#ifdef DEBUG
	std::cout << "Initial: " << std::endl;
	A.printHeatMap();
	std::cout << std::endl;
#endif

	// ---------- compute ----------

	for(int t = 0; t < T; t++) {
		for(int i = 0; i < A.size; ++i) {
			for(int j = 0; j < A.size; ++j) {
				B(i, j) = calculateNewValue(A, i, j);
			}
		}

		B(source_x, source_y) = HEAT_SOURCE;

		A.swap(B);

#ifdef DEBUG
		// every 1000 steps show intermediate step
		if(!(t % 1000)) {
			std::cout << "Step t = " << t << std ::endl;
			A.printHeatMap();
			std::cout << std::endl;
		}
#endif
	}

	double end = MPI_Wtime();

	// ---------- check ----------

	std::cout << "Final:" << std::endl;
	A.printHeatMap();
	std::cout << "Elapsed: " << end - start << std::endl;

	A.writeToFile(fileName);

	bool success = isResultSane(A);

	printf("Verification: %s\n", (success) ? "OK" : "FAILED");

	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}
