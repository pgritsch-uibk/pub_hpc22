#ifndef HPC22_LOADBALANCER_H
#define HPC22_LOADBALANCER_H

#include <mpi.h>

class LoadBalancer {

	MPI_Win sharedNextYWindow{};
	int sharedNextY{};
	int myRank{};
	const int onRank;

  public:
	const int chunkSize{};

	LoadBalancer(int _onRank, int _chunkSize) : onRank(_onRank), chunkSize(_chunkSize) {
		MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
		if(myRank == onRank) {
			MPI_Win_create(&sharedNextY, sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD,
			               &sharedNextYWindow);
		} else {
			MPI_Win_create(nullptr, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &sharedNextYWindow);
		}
	}

	~LoadBalancer() { MPI_Win_free(&sharedNextYWindow); }

	int getNext() {
		int nextY;

		MPI_Win_lock(MPI_LOCK_SHARED, onRank, 0, sharedNextYWindow);
		MPI_Fetch_and_op(&chunkSize, &nextY, MPI_INT, onRank, 0, MPI_SUM, sharedNextYWindow);
		MPI_Win_unlock(onRank, sharedNextYWindow);

		return nextY;
	}
};

#endif // HPC22_LOADBALANCER_H
