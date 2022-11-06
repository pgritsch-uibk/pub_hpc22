#ifndef HPC22_MATRIX3D_H
#define HPC22_MATRIX3D_H

#include "MpiConfig.h"
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <ostream>
#include <string>
#include <vector>

/**
 * Front/Back is in Y direction
 * Left/Right is in X direction
 * Above/Below is in Z direction
 */
class Matrix3D {

  private:
	std::vector<float> vec;

	int internal_size;

	int internal_size_2;

	inline float* get(std::size_t x, std::size_t y, std::size_t z) {
		return vec.data() + (x * internal_size_2) + (y * internal_size) + z;
	}

	inline bool isGhostCellsEnabled() const { return internal_size != size; }

  public:
	const int size;

	Matrix3D(int _size, float initial_value, bool _initWithGhostCells = true)
	    : vec(std::pow(_size + (_initWithGhostCells ? 2 : 0), 3), initial_value),
	      internal_size(_size + (_initWithGhostCells ? 2 : 0)),
	      internal_size_2(std::pow(_size + (_initWithGhostCells ? 2 : 0), 2)), size(_size){};

	inline float* getOrigin() { return vec.data(); }

	inline float& operator()(size_t x, size_t y, size_t z) {
		return vec[(x + 1) * internal_size_2 + (y + 1) * internal_size + z + 1];
	}

	void writeToFile(std::string filenameprefix);

	void swap(Matrix3D& matrix);

	MPISubarrayConfig<3> getOuterRightGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, 1, size },
			                         { 1, internal_size - 1, 1 } };
	}

	MPISubarrayConfig<3> getOuterLeftGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, 1, size },
			                         { 1, 0, 1 } };
	}

	MPISubarrayConfig<3> getInnerRightGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, 1, size },
			                         { 1, internal_size - 2, 1 } };
	}

	MPISubarrayConfig<3> getInnerLeftGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, 1, size },
			                         { 1, 1, 1 } };
	}

	MPISubarrayConfig<3> getOuterBackGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, size, 1 },
			                         { 1, 1, internal_size - 1 } };
	}

	MPISubarrayConfig<3> getOuterFrontGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, size, 1 },
			                         { 1, 1, 0 } };
	}

	MPISubarrayConfig<3> getInnerBackGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, size, 1 },
			                         { 1, 1, internal_size - 2 } };
	}

	MPISubarrayConfig<3> getInnerFrontGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, size, 1 },
			                         { 1, 1, 1 } };
	}

	MPISubarrayConfig<3> getOuterBottomGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { 1, size, size },
			                         { internal_size - 1, 1, 1 } };
	}

	MPISubarrayConfig<3> getOuterTopGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { 1, size, size },
			                         { 0, 1, 1 } };
	}

	MPISubarrayConfig<3> getInnerBottomGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { 1, size, size },
			                         { internal_size - 2, 1, 1 } };
	}

	MPISubarrayConfig<3> getInnerTopGhostCellsConfig() {
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { 1, size, size },
			                         { 1, 1, 1 } };
	}

	MPISubarrayConfig<3> getSendConfig() {
		int cord = isGhostCellsEnabled() ? 1 : 0;
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { size, size, size },
			                         { cord, cord, cord } };
	}

	MPISubarrayConfig<3> getReceiveConfig(Matrix3D& from) {
		if(isGhostCellsEnabled()) {
			throw std::logic_error("Receiver should not have ghost cells");
		}
		return MPISubarrayConfig<3>{ { internal_size, internal_size, internal_size },
			                         { from.size, from.size, from.size },
			                         { 0, 0, 0 } };
	}
};

#endif // HPC22_MATRIX3D_H
