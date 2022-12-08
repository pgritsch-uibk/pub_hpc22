#include <iostream>

#include "NBody.hpp"
#include <chrono>
#include <mpi.h>

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	{
		NBody nBody(NUM_PARTICLES);
		int myRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
#ifndef PERFORMANCE_TEST
		const std::string filename = "output.txt";
#endif

#ifdef PERFORMANCE_TEST
		auto start = MPI_Wtime();
#endif
		for(int i = 0; i < 100; i++) {
#ifndef PERFORMANCE_TEST
			if(myRank == 0) {
				std::cout << "Iteration: " << i << std::endl;

				nBody.exportToFile(filename);
			}
#endif
			nBody.update();
		}

#ifdef PERFORMANCE_TEST
		auto end = MPI_Wtime();

		if(myRank == 0) {
			std::cout << "Elapsed: " << end - start << " seconds" << std::endl;
		}
#else
		nBody.exportToFile(filename); // last export
#endif
	}

	MPI_Finalize();
	return 0;
}