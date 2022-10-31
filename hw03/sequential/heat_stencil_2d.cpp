#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <string>
#include <vector>
#include "Matrix2D.h"

#define RESOLUTION_WIDTH 50
#define RESOLUTION_HEIGHT 50


void printTemperature(Matrix2D& temperature);

void writeTemperatureToFile(Matrix2D& temperature, const std::string& filename);

// -- simulation code ---

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

	printf("Computing heat-distribution for room size %dX%d for T=%d timesteps\n", N, N, T);

	// ---------- setup initial data ----------
	auto A = Matrix2D(N, 273.0);
	auto B = Matrix2D(N, 273.0);

	// and there is a heat source
	int source_x = N/4;
	int source_y = N/4;
	A(source_x, source_y) = 273 + 60;


	double start = omp_get_wtime();
#ifdef DEBUG
	printf("Initial:");
	printTemperature(A);
	printf("\n");
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
			printf("Step t=%d\n", t);
			printTemperature(A);
			printf("\n");
		}
#endif
	}

	double end = omp_get_wtime();

	// ---------- check ----------

	printf("Final:\n");
	printTemperature(A);
	printf("Elapsed: %f \n", end - start);

	writeTemperatureToFile(A, fileName);

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

void writeTemperatureToFile(Matrix2D& temperature, const std::string& filename) {

	std::ofstream file;
	file.open(filename);

	for (auto& val : temperature.vec) {
		file << std::setw(2) << val << std::endl;
	}

	file.close();
}

void printTemperature(Matrix2D& temperature) {
	const char* colors = " .-:=+*^X#%@";
	const int numColors = 12;

	// boundaries for temperature (for simplicity hard-coded)
	const double max = 273 + 30;
	const double min = 273 + 0;

	// set the 'render' resolution
	int W = RESOLUTION_WIDTH;
	int H = RESOLUTION_HEIGHT;

	// step size in each dimension
	std::size_t sW = temperature.size / W;
	std::size_t sH = temperature.size / H;

	// upper wall
	printf("\t");
	for(int u = 0; u < W + 2; u++) {
		printf("X");
	}
	printf("\n");
	// room
	for(int i = 0; i < H; i++) {
		// left wall
		printf("\tX");
		// actual room
		for(int j = 0; j < W; j++) {
			// get max temperature in this tile
			double max_t = 0;
			for(size_t x = sH * i; x < sH * i + sH; x++) {
				for(size_t y = sW * j; y < sW * j + sW; y++) {
					max_t = (max_t < temperature(x, y)) ? temperature(x, y) : max_t;
				}
			}
			double temp = max_t;

			// pick the 'color'
			int c = ((temp - min) / (max - min)) * numColors;
			c = (c >= numColors) ? numColors - 1 : ((c < 0) ? 0 : c);

			// print the average temperature
			printf("%c", colors[c]);
		}
		// right wall
		printf("X\n");
	}
	// lower wall
	printf("\t");
	for(int l = 0; l < W + 2; l++) {
		printf("X");
	}
	printf("\n");
}
