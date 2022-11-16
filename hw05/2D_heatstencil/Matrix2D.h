#ifndef HPC22_MATRIX2D_H
#define HPC22_MATRIX2D_H

#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <ostream>
#include <string>
#include <vector>

#define HEAT_MAP_RESOLUTION_WIDTH 50
#define HEAT_MAP_RESOLUTION_HEIGHT 50

class Matrix2D {
  private:
	int access_offset;

  protected:
	int internal_size;

	inline bool isGhostCellsEnabled() const { return internal_size != size; }

	inline float* get(std::size_t x, std::size_t y) { return vec.data() + (x * internal_size) + y; }

	std::vector<float> vec;

  public:
	const int size;

	Matrix2D(int _size, float initial_value, bool _initWithGhostCells = true)
	    : access_offset(_initWithGhostCells ? 1 : 0),
	      internal_size(_size + (_initWithGhostCells ? 2 : 0)),
	      vec(((_size + (_initWithGhostCells ? 2 : 0)) * (_size + (_initWithGhostCells ? 2 : 0))),
	          initial_value),
	      size(_size) {}

	inline float* getOuterNorth() { return get(0, 1); }

	inline float* getOuterWest() { return get(1, 0); }

	inline float* getOuterEast() { return get(1, internal_size - 1); }

	inline float* getOuterSouth() { return get(internal_size - 1, 1); }

	inline float* getInnerNorth() { return get(1, 1); }

	inline float* getInnerWest() { return get(1, 1); }

	inline float* getInnerSouth() { return get(internal_size - 2, 1); }

	inline float* getInnerEast() { return get(1, internal_size - 2); }

	inline float& operator()(size_t x, size_t y) { return vec[(x + access_offset) * internal_size + y + access_offset]; }

	void writeToFile(std::string filename);

	inline float* getOrigin() { return vec.data(); }

	void printHeatMap();

	void swap(Matrix2D& matrix);
};

#endif // HPC22_MATRIX2D_H
