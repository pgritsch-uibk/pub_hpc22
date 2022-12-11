#ifndef HPC22_LOADBALANCER_H
#define HPC22_LOADBALANCER_H

#include <mpi.h>

class LoadBalancer {

	MPI_Win sharedNextYWindow{};
	int sharedNextY{};
	int myRank{};
	int onRank{};
	const int two = 2;

  public:
	LoadBalancer(int onRank) {
		this->onRank = onRank;
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
		if(myRank == onRank) {
			MPI_Win_create(&sharedNextY, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
			               &sharedNextYWindow);
		} else {
			MPI_Win_create(nullptr, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &sharedNextYWindow);
		}
	}

	~LoadBalancer() { MPI_Win_free(&sharedNextYWindow); }

	int getNextTwoY() {
		int nextY;

		MPI_Win_lock(MPI_LOCK_SHARED, onRank, 0, sharedNextYWindow);
		MPI_Fetch_and_op(&two, &nextY, MPI_INT, onRank, 0, MPI_SUM, sharedNextYWindow);
		MPI_Win_unlock(onRank, sharedNextYWindow);

		return nextY;
	}
};

#endif // HPC22_LOADBALANCER_H
