#include <algorithm>
#include <boost/mpi.hpp>
#include <functional>
#include <stdio.h>
#include <string>
#include <vector>
#define RESOLUTION 120

using value_t = double;

// -- vector utilities --
void printTemperature(std::vector<value_t>& m, int N);

// -- simulation code ---
int main(int argc, char** argv) {
	// 'parsing' optional input parameter = problem size
	int N = 512;
	if(argc > 1) {
		N = std::stoi(argv[1]);
	}

	int T = N * 500;
	printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);

	// ---------- setup ----------
	// set up initial conditions in A
	int source_x = N / 4;

	// create a buffer for storing temperature fields
	std::vector<value_t> A(N, 273);

	// and there is a heat source in one corner
	A[source_x] = 273 + 60;

	// ---------- compute ----------

	// create a second buffer for the computation
	std::vector<value_t> B(N);

	boost::mpi::timer timer;
	// for each time step ..
	for(int t = 0; t < T; t++) {
		// .. we propagate the temperature
		for(long long i = 0; i < N; i++) {
			// center stays constant (the heat is still on)
			if(i == source_x) {
				B[i] = A[i];
				continue;
			}

			// get temperature at current position
			value_t tc = A[i];

			// get temperatures of adjacent cells
			value_t tl = (i != 0) ? A[i - 1] : tc;
			value_t tr = (i != N - 1) ? A[i + 1] : tc;

			// compute new temperature at current position
			B[i] = tc + 0.2 * (tl + tr + (-2 * tc));
		}

		// swap matrices (just pointers, not content)
		std::swap(A, B);
	}
	std::cout << "\nElapsed: " << timer.elapsed() << std::endl;

	// ---------- check ----------
	int success = 1;
	for(long long i = 0; i < N; i++) {
		value_t temp = A[i];
		if(273 <= temp && temp <= 273 + 60) {
			continue;
		}
		success = 0;
		break;
	}

	printTemperature(A, N);

	std::cout << "\nVerification: " << ((success) ? "OK" : "FAILED") << std::endl;

	// done
	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}

void printTemperature(std::vector<value_t>& m, int N) {
	const char* colors = " .-:=+*^X#%@";
	const int numColors = 12;

	// boundaries for temperature (for simplicity hard-coded)
	const auto [min, max] = std::minmax_element(m.begin(), m.end());

	// set the 'render' resolution
	int W = RESOLUTION;

	// step size in each dimension
	int sW = N / W;

	// room
	// left wall
	std::cout << "X";
	// actual room
	for(int i = 0; i < W; i++) {
		// get max temperature in this tile
		value_t temp = *max_element(m.begin() + i * sW, m.begin() + (i * sW) + sW);

		// pick the 'color'
		int c = ((temp - *min) / (*max - *min)) * numColors;
		c = (c >= numColors) ? numColors - 1 : ((c < 0) ? 0 : c);

		// print the average temperature
		std::cout << colors[c];
	}
	// right wall
	std::cout << "X" << std::endl;
}