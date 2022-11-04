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

	inline bool isGhostCellsEnabled() const {
		return internal_size != size;
	}

  public:

	const int size;

	Matrix3D(int _size, float initial_value, bool _initWithGhostCells = true)
		: vec(std::pow(_size + (_initWithGhostCells ? 2 : 0), 3), initial_value),
	      internal_size(_size + (_initWithGhostCells ? 2 : 0)),
	      internal_size_2(std::pow(_size + (_initWithGhostCells ? 2 : 0), 2)),
	      size(_size) {};

	inline float* getOuterFront() {
		return get(1, 0, 1);
	}

	inline float* getInnerFront() {
		return get(1, 1, 1);
	}

	inline float* getOuterBack() {
		return get(1, internal_size - 1, 1);
	}

	inline float* getInnerBack() {
		return get(1, internal_size - 2, 1);
	}

	inline float* getOuterLeft() {
		return get(0, 1, 1);
	}

	inline float* getInnerLeft() {
		return get(1, 1, 1);
	}

	inline float* getOuterRight() {
		return get(internal_size - 1, 1, 1);
	}

	inline float* getInnerRight() {
		return get(internal_size - 2, 1, 1);
	}

	inline float* getOuterTop() {
		return get(1, 1, 0);
	}

	inline float* getInnerTop() {
		return get(1, 1, 1);
	}

	inline float* getOuterBottom() {
		return get(1, 1, internal_size - 1);
	}

	inline float* getInnerBottom() {
		return get(1, 1, internal_size - 2);
	}

	inline float* getOrigin() {
		return vec.data();
	}

	inline float& operator()(size_t x, size_t y, size_t z) {
		return vec[(x + 1) * internal_size_2 + (y + 1) * internal_size + z + 1];
	}

	void writeToFile(std::string filename);

	//TODO: think about implementing void printHeatMap();

	void swap(Matrix3D& matrix);

	MPIVectorConfig getFrontBackGhostCellsConfig() {
		if (!isGhostCellsEnabled()) {
			throw std::logic_error("Matrix has no ghost cells");
		}
		return MPIVectorConfig { size * size, 1, internal_size };
	}

	MPIVectorConfig getTopBottomGhostCellsConfig() {
		if (!isGhostCellsEnabled()) {
			throw std::logic_error("Matrix has no ghost cells");
		}
		return MPIVectorConfig { size, size, internal_size };
	}

	MPIVectorConfig getLeftRightGhostCellsConfig() {
		if (!isGhostCellsEnabled()) {
			throw std::logic_error("Matrix has no ghost cells");
		}
		return MPIVectorConfig { size, size, internal_size_2};
	}

	MPISendReceiveConfig<3> getSendConfig() {
		int cord = isGhostCellsEnabled() ? 1 : 0;
		return MPISendReceiveConfig<3>{{internal_size, internal_size, internal_size},
			                            {size, size, size}, {cord, cord, cord}};
	}

	MPISendReceiveConfig<3> getReceiveConfig(Matrix3D& from) {
		if (isGhostCellsEnabled()) {
			throw std::logic_error("Receiver should not have ghost cells");
		}
		return MPISendReceiveConfig<3>{{internal_size, internal_size, internal_size},
			                            {from.size, from.size, from.size},
			                            {0, 0, 0}};
	}
};

#endif // HPC22_MATRIX3D_H
