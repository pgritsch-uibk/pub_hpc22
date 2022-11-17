#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

typedef double value_t;

#define RESOLUTION 120

// -- vector utilities --

typedef value_t *Vector;

Vector createVector(int N);

void releaseVector(Vector m);

void printTemperature(Vector m, int N);

// -- simulation code ---

int main(int argc, char **argv) {

    // initializing MPI
    int myRank, numProcs;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    double start_time = MPI_Wtime();

    // 'parsing' optional input parameter = problem size
    int N = 4096;
    if (argc > 1) {
        N = atoi(argv[1]);
    }

    if (N % numProcs != 0) {
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    int T = N * 500;
    if (myRank == 0)
        printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);

    int start = myRank * N / numProcs;
    int end = (myRank + 1) * N / numProcs;

    // ---------- setup ----------

    // create a buffer for storing temperature fields
    Vector A = createVector(N);

    // set up initial conditions in A
    int source_x = N / 4;

    for (int i = start; i < end; i++) {
        A[i] = 273; // temperature is 0° C everywhere (273 K)
    }

    // and there is a heat source in one corner
    A[source_x] = 273 + 60;

    // ---------- compute ----------

    // create a second buffer for the computation
    Vector B = createVector(N);

    // for each time step ..
    for (int t = 0; t < T; t++) {

        if (myRank != 0) {
            // start is send to LEFT RANK
            MPI_Request request;
            MPI_Send(&A[start], 1, MPI_DOUBLE, myRank - 1, 0, MPI_COMM_WORLD);
        }

        if (myRank != numProcs - 1) {
            // I AM LEFT RANK and receive FROM RIGHT RANK
            MPI_Status status;
            MPI_Recv(&A[end], 1, MPI_DOUBLE, myRank + 1, 0, MPI_COMM_WORLD, &status);
        }

        if (myRank != numProcs - 1) {
            // end is send to RIGHT RANK
            MPI_Request request;
            MPI_Send(&A[end - 1], 1, MPI_DOUBLE, myRank + 1, 0, MPI_COMM_WORLD);
        }

        if (myRank != 0) {
            // I AM RIGHT RANK and receive FROM LEFT RANK
            MPI_Request request;
            MPI_Status status;
            MPI_Recv(&A[start - 1], 1, MPI_DOUBLE, myRank - 1, 0, MPI_COMM_WORLD, &status);
        }

        // .. we propagate the temperature
        for (int i = start; i < end; i++) {
            // center stays constant (the heat is still on)
            if (i == source_x) {
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

    // ---------- check ----------
    int success = 1;
    for (long long i = start; i < end; i++) {
        value_t temp = A[i];
        if (273 <= temp && temp <= 273 + 60)
            continue;
        success = 0;
        break;
    }

    releaseVector(B);

    MPI_Gather(&A[start], N / numProcs, MPI_DOUBLE, A, N / numProcs, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    int total_success = 0;
    MPI_Reduce(&success, &total_success, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // ---------- cleanup ----------

    releaseVector(A);

    if (myRank == 0) {
        success = total_success == numProcs;
        printf("Method execution took seconds: %.5lf\n", MPI_Wtime() - start_time);
    }

    MPI_Finalize();

    // done
    return success ? EXIT_SUCCESS : EXIT_FAILURE;
}

Vector createVector(int N) {
    // create data and index vector
    return malloc(sizeof(value_t) * N);
}

void releaseVector(Vector m) {
    free(m);
}

void printTemperature(Vector m, int N) {
    const char *colors = " .-:=+*^X#%@";
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
    for (int i = 0; i < W; i++) {
        // get max temperature in this tile
        value_t max_t = 0;
        for (int x = sW * i; x < sW * i + sW; x++) {
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
