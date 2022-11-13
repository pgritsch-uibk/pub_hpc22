#include "../Matrix3D.h"
#include "../MpiConfig.h"
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <string>

#define DIMENSIONS 3

int main(int argc, char** argv) {
	// 'parsing' optional input parameter = problem size
	int N = 100;

	if(argc > 1) {
		N = std::stoi(argv[1]);
	}
	int T = (int)std::round(std::pow(N, 2) * 3.0);

	if(argc > 2) {
		T = std::stoi(argv[2]);
	}

	std::string fileName = "gathered3d_seq" + std::to_string(N) + "_" + std::to_string(T);
	std::cout << "Computing heat-distribution for room size " << N << "X" << N << "X" << N
	          << " for T=" << T << " timesteps" << std::endl;

	// ---------- setup initial data ----------
	int success = 1;
	{
		auto A = Matrix3D(N, 273.0);
		auto B = Matrix3D(N, 273.0);


		int source_x = N / 2 - 1;
		int source_y = N / 2 - 1;
		int source_z = N / 2 - 1;
		A(source_x, source_y, source_z) = 273 + 200;



		double start = MPI_Wtime();

		for(int t = 0; t < T; t++) {
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

					if(273 <= temp && temp <= 273 + 200) {
						continue;
					}
					std::cout << "Verification failed at " << " "
					          << i << " " << j << " " << k << " "
					          << temp << std::endl;

					success = 0;
					break;
				}
			}
		}

		std::cout << "Elapsed: " << end - start << std::endl;
		A.writeToFile(fileName);
	}


	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}