#include "xorshiftSSE.h"
#include <mpi.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>

#define ITERATIONS 1000000000

// using a vectorized xorshift this time: https://github.com/risingape/xorshiftSSE
int main(int argc, char *argv[]) {

    int myRank, numProcs;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

    int count_individual = 0;
    int iterations = ITERATIONS / numProcs;

    double start_time = 0;
    if (myRank == 0) {
        start_time = MPI_Wtime();
    }

    if ((ITERATIONS % numProcs) != 0) {
        perror("Iterations must be divisible by 4*n where n is the number of MPI ranks used");
        MPI_Finalize();
        return EXIT_FAILURE;
    }

    if ((iterations % 4) != 0) {
        perror("Iterations must be divisible by 4*n where n is the number of MPI ranks used");
        MPI_Finalize();
        return EXIT_FAILURE;
    }
    iterations /= 4;

    float ones[4] = {1.0, 1.0, 1.0, 1.0};
    __m128 _ones = _mm_load_ps(ones);

    float int_max[4] = {INT32_MAX, INT32_MAX, INT32_MAX, INT32_MAX};
    __m128 _int_max = _mm_load_ps(int_max);

    uint32_t seeds[4] = {myRank * 2 + 1913, myRank * 2 + 213, myRank * 2 + 1211, myRank * 2 + 941};

    for (int i = 0; i < iterations; i++) {

        __m128i _xi;
        __m128i _yi;

        xorshiftR0(seeds, (uint32_t * ) & _xi);
        xorshiftR0(seeds, (uint32_t * ) & _yi);

        // (float) _xi / INT32MAX
        __m128 _x = _mm_div_ps(_mm_cvtepi32_ps(_xi), _int_max);
        __m128 _y = _mm_div_ps(_mm_cvtepi32_ps(_yi), _int_max);

        // r = x*x + y*y
        __m128 _product_sum = _mm_add_ps(_mm_mul_ps(_x, _x), _mm_mul_ps(_y, _y));

        // r <= 1
        __m128 _logical_result = _mm_cmple_ps(_product_sum, _ones);

        float logical_result[4];
        _mm_store_ps(logical_result, _logical_result);

        for (int i = 0; i < 4; i++) {
            count_individual += ((int) logical_result[i] < 0) ? 1 : 0;
        }
    }

    int total_count = 0;
    MPI_Reduce(&count_individual, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (myRank == 0) {
        double pi = 4.0 * total_count / ITERATIONS;
        double exec_time = MPI_Wtime() - start_time;

        printf("Pi %lf\nTime: %.5lfs\n", pi, exec_time);
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
