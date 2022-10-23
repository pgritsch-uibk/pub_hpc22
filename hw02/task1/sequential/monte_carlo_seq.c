#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ITERATIONS 1000000000

// taken from https://en.wikipedia.org/wiki/Xorshift
struct xorshift64_state {
    uint64_t a;
};

uint64_t xorshift64(struct xorshift64_state *state) {
    uint64_t x = state->a;
    x ^= x << 13;
    x ^= x >> 7;
    x ^= x << 17;
    return state->a = x;
}

int main(int argc, char const *argv[]) {

    clock_t start = clock();
    uint32_t count = 0;
    struct xorshift64_state rand_state = {86484687};
    for (int i = 0; i < ITERATIONS; i++) {

        uint64_t random_2 = xorshift64(&rand_state);

        float x = (float) (uint32_t) random_2 / (float) UINT32_MAX;
        float y = (float) (uint32_t)(random_2 >> 32) / (float) UINT32_MAX;

        if (x * x + y * y <= 1) {
            count++;
        }
    }

    double pi = 4.0 * count / ITERATIONS;
    double exec_time = (float) (clock() - start) / CLOCKS_PER_SEC;

    printf("Pi %lf\nTime: %.5lfs\n", pi, exec_time);

    return EXIT_SUCCESS;
}
