#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef double value_t;

#define RESOLUTION 120

// -- vector utilities --

typedef value_t* Vector;

Vector createVector(int N);

void releaseVector(Vector m);

void printTemperature(Vector m, int N);

// -- simulation code ---

int main(int argc, char** argv) {
	// 'parsing' optional input parameter = problem size
	int N = 512;
	if(argc > 1) {
		N = atoi(argv[1]);
	}
	clock_t start = clock();

	int T = N * 500;
	printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);

	// ---------- setup ----------

	// create a buffer for storing temperature fields
	Vector A = createVector(N);

	// set up initial conditions in A
	for(int i = 0; i < N; i++) {
		A[i] = 273; // temperature is 0° C everywhere (273 K)
	}

	// and there is a heat source in one corner
	int source_x = N / 4;
	A[source_x] = 273 + 60;

	// ---------- compute ----------

	// create a second buffer for the computation
	Vector B = createVector(N);

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
		Vector H = A;
		A = B;
		B = H;
	}

	releaseVector(B);

	// ---------- check ----------

	int success = 1;
	for(long long i = 0; i < N; i++) {
		value_t temp = A[i];
		if(273 <= temp && temp <= 273 + 60) continue;
		success = 0;
		break;
	}

	printTemperature(A, N);

	printf("\n Verification: %s\n", (success) ? "OK" : "FAILED");
	printf("Method execution took seconds: %.5lf\n", (float)(clock() - start) / CLOCKS_PER_SEC);
	// ---------- cleanup ----------

	releaseVector(A);

	// done
	return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}

Vector createVector(int N) {
	// create data and index vector
	return (Vector)malloc(sizeof(value_t) * N);
}

void releaseVector(Vector m) {
	free(m);
}

void printTemperature(Vector m, int N) {
	const char* colors = " .-:=+*^X#%@";
	const int numColors = 12;

	// boundaries for temperature (for simplicity hard-coded)
	const value_t max = 273 + 30;
	const value_t min = 273 + 0;

	// set the 'render' resolution
	int W = RESOLUTION;

	// step size in each dimension
	int sW = N / W;

	// room
	// left wall
	printf("X");
	// actual room
	for(int i = 0; i < W; i++) {
		// get max temperature in this tile
		value_t max_t = 0;
		for(int x = sW * i; x < sW * i + sW; x++) {
			max_t = (max_t < m[x]) ? m[x] : max_t;
		}
		value_t temp = max_t;

		// pick the 'color'
		int c = ((temp - min) / (max - min)) * numColors;
		c = (c >= numColors) ? numColors - 1 : ((c < 0) ? 0 : c);

		// print the average temperature
		printf("%c", colors[c]);
	}
	// right wall
	printf("X");
}
