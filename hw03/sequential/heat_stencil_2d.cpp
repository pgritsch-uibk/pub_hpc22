#include "../Matrix2D.h"
#include <cstdlib>
#include <iostream>
#include <omp.h>
#include <string>
#include <vector>

int main(int argc, char** argv) {
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

	std::cout << "Computing heat-distribution for room size " << N << "X" << N << " for T=" << T << " timesteps" << std::endl;
	// ---------- setup initial data ----------
	auto A = Matrix2D(N, 273.0);
	auto B = Matrix2D(N, 273.0);

	// and there is a heat source
	int source_x = N/4;
	int source_y = N/4;
	A(source_x, source_y) = 273 + 60;


	double start = omp_get_wtime();
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

#ifdef DEBUG
		// every 1000 steps show intermediate step
		if(!(t % 1000)) {
			std::cout << "Step t = " << t << std ::endl;
			A.printHeatMap();
			std::cout << std::endl;
		}
#endif
	}

	double end = omp_get_wtime();

	// ---------- check ----------

	std::cout << "Final:" << std::endl;
	A.printHeatMap() ;
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
