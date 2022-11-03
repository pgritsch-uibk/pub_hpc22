#ifndef HPC22_MATRIX2D_H
#define HPC22_MATRIX2D_H

#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>

#define HEAT_MAP_RESOLUTION_WIDTH 50
#define HEAT_MAP_RESOLUTION_HEIGHT 50

class Matrix2D {
  private:
	std::vector<float> vec;

	int internal_size;

	inline float* get(std::size_t x, std::size_t y) {
		return this->vec.data() + (x * internal_size) + y;
	}

  public:
	const int size;

	Matrix2D(int _size, float initial_value)
	    : vec(((_size + 2) * (_size + 2)), initial_value), internal_size(_size + 2), size(_size) {
#ifdef DEBUG
		std::for_each(vec.begin(), vec.end(), [](float& val) { std::cout << val << " " });
		std::cout << std::endl;
#endif
	}

	inline float* getOuterNorth() { return this->get(0, 1); }

	inline float* getOuterWest() { return this->get(1, 0); }

	inline float* getOuterEast() { return this->get(1, internal_size - 1); }

	inline float* getOuterSouth() { return this->get(internal_size - 1, 1); }

	inline float* getInnerNorth() { return this->get(1, 1); }

	inline float* getInnerWest() { return this->get(1, 1); }

	inline float* getInnerSouth() { return this->get(internal_size - 2, 1); }

	inline float* getInnerEast() { return this->get(1, internal_size - 2); }

	inline float& operator()(size_t x, size_t y) {
		return this->vec[(x + 1) * this->internal_size + y + 1];
	}

	void writeToFile(std::string filename);

	void printHeatMap();

	void swap(Matrix2D& matrix);
};

#endif // HPC22_MATRIX2D_H
