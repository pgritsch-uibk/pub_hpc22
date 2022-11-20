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
	// initializing MPI
	int myRank, numProcs;
	boost::mpi::environment env{ argc, argv };
	boost::mpi::communicator world;

	numProcs = world.size();
	myRank = world.rank();

	// 'parsing' optional input parameter = problem size
	int N = 512;
	if(argc > 1) {
		N = std::stoi(argv[1]);
	}

	if(N % numProcs != 0) {
		return EXIT_FAILURE;
	}

	int T = N * 500;
	if(myRank == 0) {
		printf("Computing heat-distribution for room size N=%d for T=%d timesteps\n", N, T);
	}

	int start = myRank * N / numProcs;
	int end = (myRank + 1) * N / numProcs;

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

		if(myRank != 0) {
			// start is send to LEFT RANK
			world.send(myRank - 1, 0, A[start]);
		}

		if(myRank != numProcs - 1) {
			// I AM LEFT RANK and receive FROM RIGHT RANK
			world.recv(myRank + 1, 0, A[end]);
		}

		if(myRank != numProcs - 1) {
			// end is send to RIGHT RANK
			world.send(myRank + 1, 0, A[end - 1]);
		}

		if(myRank != 0) {
			// I AM RIGHT RANK and receive FROM LEFT RANK
			world.recv(myRank - 1, 0, A[start - 1]);
		}

		// .. we propagate the temperature
		for(int i = start; i < end; i++) {
			// get temperature at current position
			value_t tc = A[i];

			// get temperatures of adjacent cells
			value_t tl = (i != 0) ? A[i - 1] : tc;
			value_t tr = (i != N - 1) ? A[i + 1] : tc;

			// compute new temperature at current position
			B[i] = tc + 0.2 * (tl + tr + (-2 * tc));
		}

		// center stays constant (the heat is still on)
		B[source_x] = 273 + 60;

		// swap matrices (just pointers, not content)
		std::swap(A, B);
	}
	if(myRank == 0) {
		std::cout << "\nMethod execution took seconds: " << timer.elapsed() << std::endl;
	}

	// ---------- check ----------
	int success = 1;
	for(long long i = start; i < end; i++) {
		value_t temp = A[i];
		if(273 <= temp && temp <= 273 + 60) {
			continue;
		}
		success = 0;
		break;
	}

	if(world.rank() == 0) {
		boost::mpi::gather(world, &A[start], N / numProcs, A, 0);
	} else {
		boost::mpi::gather(world, &A[start], N / numProcs, 0);
	}

	int total_success = 0;
	boost::mpi::reduce(world, success, total_success, std::plus<int>(), 0);

	// ---------- cleanup ----------
	if(myRank == 0) {
		printTemperature(A, N);
		success = total_success == numProcs;
	}

	// done
	return success ? EXIT_SUCCESS : EXIT_FAILURE;
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