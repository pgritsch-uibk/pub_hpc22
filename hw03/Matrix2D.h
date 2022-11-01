#ifndef HPC22_MATRIX2D_H
#define HPC22_MATRIX2D_H

#include <cstdlib>
#include <string>
#include <vector>
#include <ostream>

#define HEAT_MAP_RESOLUTION_WIDTH 50
#define HEAT_MAP_RESOLUTION_HEIGHT 50

class Matrix2D {

private:

    int internal_size;

    inline float *get(std::size_t x, std::size_t y) {
        return &(this->vec[x * internal_size + y]);
    }

public:

    int size;

    std::vector<float> vec;

    Matrix2D(int size, float initial_value) {
        std::vector<float> temp((size + 2) * (size + 2), initial_value);
        std::swap(this->vec, temp);
        this->size = size;
        this->internal_size = size + 2;
    }

    inline float *getOuterNorth() {
        return this->get(0, 1);
    }

    inline float *getOuterWest() {
        return this->get(1, 0);
    }

    inline float *getOuterEast() {
        return this->get(1, internal_size - 1);
    }

    inline float *getOuterSouth() {
        return this->get(internal_size - 1, 1);
    }

    inline float *getInnerNorth() {
        return this->get(1, 1);
    }

    inline float *getInnerWest() {
        return this->get(1, 1);
    }

    inline float *getInnerSouth() {
        return this->get(internal_size - 2, 1);
    }

    inline float *getInnerEast() {
        return this->get(1, internal_size - 2);
    }

    inline float &operator()(size_t x, size_t y) {
        return this->vec[(x + 1) * this->internal_size + y + 1];
    }

	void writeToFile(std::string filename);

	void printHeatMap();
};


#endif //HPC22_MATRIX2D_H
