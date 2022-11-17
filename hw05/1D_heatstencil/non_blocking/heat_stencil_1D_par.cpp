#include <boost/mpi.hpp>
#include <stdio.h>
#include <stdlib.h>

typedef double value_t;

#define RESOLUTION 120

// -- vector utilities --

typedef value_t* Vector;

Vector createVector(int N);

void releaseVector(Vector m);

void printTemperature(Vector m, int N);

// -- simulation code ---

int main(int argc, char** argv) {
	// initializing MPI
	int myRank, numProcs;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	double start_time = MPI_Wtime();

	// 'parsing' optional input parameter = problem size
	int N = 4096;
	if(argc > 1) {
		N = atoi(argv[1]);
	}

	if(N % numProcs != 0) {
		MPI_Finalize();
		return EXIT_FAILURE;
	}

	int T = N * 500;
	if(myRank == 0)
		printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);

	int start = myRank * N / numProcs;
	int end = (myRank + 1) * N / numProcs;

	// ---------- setup ----------

	// create a buffer for storing temperature fields
	Vector A = createVector(N);

	// set up initial conditions in A
	int source_x = N / 4;

	for(int i = start; i < end; i++) {
		A[i] = 273; // temperature is 0° C everywhere (273 K)
	}

	// and there is a heat source in one corner
	A[source_x] = 273 + 60;

	// ---------- compute ----------

	// create a second buffer for the computation
	Vector B = createVector(N);

	// for each time step ..
	for(int t = 0; t < T; t++) {
		MPI_Request request[4];

		if(myRank != 0) {
			MPI_Isend(&A[start], 1, MPI_DOUBLE, myRank - 1, 0, MPI_COMM_WORLD, &request[0]);
		}

		if(myRank != numProcs - 1) {
			MPI_Isend(&A[end - 1], 1, MPI_DOUBLE, myRank + 1, 0, MPI_COMM_WORLD, &request[1]);
		}

		if(myRank != numProcs - 1) {
			MPI_Irecv(&A[end], 1, MPI_DOUBLE, myRank + 1, 0, MPI_COMM_WORLD, &request[2]);
		}

		if(myRank != 0) {
			MPI_Irecv(&A[start - 1], 1, MPI_DOUBLE, myRank - 1, 0, MPI_COMM_WORLD, &request[3]);
		}

		// .. we propagate the temperature
		for(int i = start + 1; i < end - 1; i++) {
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

		if(myRank != 0) {
			MPI_Wait(&request[0], MPI_STATUS_IGNORE);
		}

		if(myRank != numProcs - 1) {
			MPI_Wait(&request[1], MPI_STATUS_IGNORE);
		}

		if(myRank != numProcs - 1) {
			MPI_Wait(&request[2], MPI_STATUS_IGNORE);
		}

		if(myRank != 0) {
			MPI_Wait(&request[3], MPI_STATUS_IGNORE);
		}

		value_t tc = A[start];
		value_t tl = (start != 0) ? A[start - 1] : tc;
		value_t tr = A[start + 1];

		B[start] = tc + 0.2 * (tl + tr + (-2 * tc));

		tc = A[end - 1];
		tl = A[end - 2];
		tr = ((end - 1) != N - 1) ? A[end] : tc;

		B[end - 1] = tc + 0.2 * (tl + tr + (-2 * tc));

		// swap matrices (just pointers, not content)
		Vector H = A;
		A = B;
		B = H;
	}

	// ---------- check ----------
	int success = 1;
	for(long long i = start; i < end; i++) {
		value_t temp = A[i];
		if(273 <= temp && temp <= 273 + 60) continue;
		success = 0;
		break;
	}

	releaseVector(B);

	MPI_Gather(&A[start], N / numProcs, MPI_DOUBLE, A, N / numProcs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	int total_success = 0;
	MPI_Reduce(&success, &total_success, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	// ---------- cleanup ----------

	releaseVector(A);

	if(myRank == 0) {
		success = total_success == numProcs;
		printf("Method execution took seconds: %.5lf\n", MPI_Wtime() - start_time);
	}

	MPI_Finalize();

	// done
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

Vector createVector(int N) {
	// create data and index vector
	return (Vector) malloc(sizeof(value_t) * N);
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
