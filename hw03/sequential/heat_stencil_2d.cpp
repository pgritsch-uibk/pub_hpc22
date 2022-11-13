#include "../Matrix2D.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <mpi.h>

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
	int source_x = 99;
	int source_y = 99;
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
				if((i == source_x && j == source_y)) {
					B(i, j) = A(i, j);
				} else {
					B(i, j) = A(i, j) + 0.2 * (-4.0 * A(i, j) + A(i - 1, j) + A(i + 1, j) +
					                           A(i, j - 1) + A(i, j + 1));
				}
			}
		}

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
