#ifndef HPC22_MPIMATRIX2D_H
#define HPC22_MPIMATRIX2D_H

#include "Location.h"
#include "Matrix2D.h"
#include <array>
#include <memory>

namespace mpi {

class Matrix2D : public ::Matrix2D {

  private:
	MPI_Datatype horizontalGhostCells;

	MPI_Datatype verticalGhostCells;

	std::vector<int> displacements;

	std::array<MPI_Request, 8> requests;

	Location myLocation;

	MPI_Comm comm;

  public:
	Matrix2D(MPI_Comm& comm, int _size, float initial_value)
	    : ::Matrix2D(_size, initial_value, true) {
		myLocation = {};
		this->comm = comm;

		MPI_Comm_rank(comm, &myLocation.myRank);
		MPI_Cart_coords(comm, myLocation.myRank, 2, myLocation.myCoords.begin());

		MPI_Cart_shift(comm, 0, 1, &myLocation.rankToNorth, &myLocation.rankToSouth);
		MPI_Cart_shift(comm, 1, 1, &myLocation.rankToWest, &myLocation.rankToEast);

		MPI_Type_vector(size, 1, internal_size, MPI_FLOAT, &verticalGhostCells);
		MPI_Type_vector(1, size, internal_size, MPI_FLOAT, &horizontalGhostCells);
		MPI_Type_commit(&verticalGhostCells);
		MPI_Type_commit(&horizontalGhostCells);
	};

	~Matrix2D() {
		MPI_Type_free(&verticalGhostCells);
		MPI_Type_free(&horizontalGhostCells);
	}

	void beginGhostCellsExchange();

	void waitReceive();

	void performSendRecv();

	static std::shared_ptr<::Matrix2D> gatherAll(MPI_Comm& comm, mpi::Matrix2D& subMatrix,
	                                             int targetRank, int procsByDim, int numProcs) {

		int gatheredSize = subMatrix.size * procsByDim;
		int myRank;
		MPI_Comm_rank(comm, &myRank);

		MPI_Datatype sendSubMatrix;
		int startCoord = subMatrix.isGhostCellsEnabled() ? 1 : 0;
		std::array<int, 2> sizes = { subMatrix.internal_size, subMatrix.internal_size };
		std::array<int, 2> subSizes = { subMatrix.size, subMatrix.size };
		std::array<int, 2> coords = { startCoord, startCoord };

		MPI_Type_create_subarray(2, sizes.begin(), subSizes.begin(), coords.begin(), MPI_ORDER_C,
		                         MPI_FLOAT, &sendSubMatrix);
		MPI_Type_commit(&sendSubMatrix);

		std::shared_ptr<::Matrix2D> gathered = nullptr;
		if(myRank == targetRank) {
			gathered = std::make_shared<::Matrix2D>(::Matrix2D(gatheredSize, 0.f, false));

			MPI_Datatype receiveSubMatrix, subSizedLine;
			sizes = { gatheredSize, gatheredSize };
			subSizes = { subMatrix.size, subMatrix.size };
			coords = { 0, 0 };
			MPI_Type_create_subarray(2, sizes.begin(), subSizes.begin(), coords.begin(),
			                         MPI_ORDER_C, MPI_FLOAT, &receiveSubMatrix);
			MPI_Type_create_resized(receiveSubMatrix, 0, subMatrix.size * (int)sizeof(float),
			                        &subSizedLine);
			MPI_Type_commit(&subSizedLine);

			std::vector<int> displacements(numProcs);
			int index = 0;
			std::vector<int> counts(numProcs, 1);
			for(int procRow = 0; procRow < procsByDim; procRow++) {
				for(int procColumn = 0; procColumn < procsByDim; procColumn++) {
					displacements[index++] = procColumn + procRow * (subMatrix.size * procsByDim);
				}
			}

			MPI_Gatherv(subMatrix.getOrigin(), 1, sendSubMatrix, gathered->getOrigin(),
			            counts.data(), displacements.data(), subSizedLine, targetRank, comm);

			MPI_Type_free(&subSizedLine);
		} else {
			MPI_Gatherv(subMatrix.getOrigin(), 1, sendSubMatrix, nullptr, nullptr, nullptr, nullptr,
			            targetRank, comm);
		}

		MPI_Type_free(&sendSubMatrix);

		return gathered;
	}
};

} // namespace mpi
#endif // HPC22_MPIMATRIX2D_H
