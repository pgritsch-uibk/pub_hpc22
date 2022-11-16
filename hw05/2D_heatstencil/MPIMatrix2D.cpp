#include "MPIMatrix2D.h"

void mpi::Matrix2D::beginGhostCellsExchange() {

	auto firstNorth = mpi::Matrix2D::vec.begin() + internal_size + 1;
	auto lastNorth = firstNorth + size;
	auto innerNorth = std::vector<float>(firstNorth, lastNorth);

	auto lastSouth = mpi::Matrix2D::vec.end() - internal_size - 1;
	auto firstSouth = lastSouth - size;
	auto innerSouth = std::vector<float>(firstSouth, lastSouth);

	MPI_Isend(innerNorth.data(), size, MPI_FLOAT, myLocation.rankToNorth, 0, comm, &requests[0]);
	MPI_Isend(innerSouth.data(), size, MPI_FLOAT, myLocation.rankToSouth, 0, comm, &requests[1]);
	MPI_Isend(getInnerEast(), 1, verticalGhostCells, myLocation.rankToEast, 0, comm, &requests[2]);
	MPI_Isend(getInnerWest(), 1, verticalGhostCells, myLocation.rankToWest, 0, comm, &requests[3]);

	MPI_Irecv(getOuterSouth(), 1, horizontalGhostCells, myLocation.rankToSouth, MPI_ANY_TAG, comm,
	          &requests[4]);
	MPI_Irecv(getOuterNorth(), 1, horizontalGhostCells, myLocation.rankToNorth, MPI_ANY_TAG, comm,
	          &requests[5]);
	MPI_Irecv(getOuterWest(), 1, verticalGhostCells, myLocation.rankToWest, MPI_ANY_TAG, comm,
	          &requests[6]);
	MPI_Irecv(getOuterEast(), 1, verticalGhostCells, myLocation.rankToEast, MPI_ANY_TAG, comm,
	          &requests[7]);
}

void mpi::Matrix2D::waitReceive() {
	MPI_Waitall(8, requests.begin(), MPI_STATUSES_IGNORE);
}

void mpi::Matrix2D::performSendRecv() {

	// sending EAST to WEST
	MPI_Sendrecv(getInnerEast(), 1, verticalGhostCells, myLocation.rankToEast, 0, getOuterWest(), 1,
	             verticalGhostCells, myLocation.rankToWest, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);

	// sending WEST to EAST
	MPI_Sendrecv(getInnerWest(), 1, verticalGhostCells, myLocation.rankToWest, 0, getOuterEast(), 1,
	             verticalGhostCells, myLocation.rankToEast, MPI_ANY_TAG, comm, MPI_STATUS_IGNORE);

	// sending SOUTH to NORTH
	MPI_Sendrecv(getInnerSouth(), 1, horizontalGhostCells, myLocation.rankToSouth, 0,
	             getOuterNorth(), 1, horizontalGhostCells, myLocation.rankToNorth, MPI_ANY_TAG,
	             comm, MPI_STATUS_IGNORE);

	// sending NORTH to SOUTH
	MPI_Sendrecv(getInnerNorth(), 1, horizontalGhostCells, myLocation.rankToNorth, 0,
	             getOuterSouth(), 1, horizontalGhostCells, myLocation.rankToSouth, MPI_ANY_TAG,
	             comm, MPI_STATUS_IGNORE);
}
