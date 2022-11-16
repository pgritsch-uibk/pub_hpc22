#include "Matrix2D.h"
#include <fstream>
#include <iomanip>
#include <iostream>

void Matrix2D::writeToFile(std::string filename) {

	std::ofstream file;
	file.open(filename);

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			file << std::setw(2) << operator()(i, j) << std::endl;
		}
	}

	file.close();
}

void Matrix2D::printHeatMap() {
	const char* colors = " .-:=+*^X#%@";
	const int numColors = 12;

	// boundaries for temperature (for simplicity hard-coded)
	const double max = 273 + 30;
	const double min = 273 + 0;

	// set the 'render' resolution
	int W = HEAT_MAP_RESOLUTION_WIDTH;
	int H = HEAT_MAP_RESOLUTION_HEIGHT;

	// step size in each dimension
	std::size_t sW = this->size / W;
	std::size_t sH = this->size / H;

	// upper wall
	std::cout << "\t";
	for(int u = 0; u < W + 2; u++) {
		std::cout << "X";
	}
	std::cout << std::endl;
	// room
	for(int i = 0; i < H; i++) {
		// left wall
		printf("\tX");
		// actual room
		for(int j = 0; j < W; j++) {
			// get max temperature in this tile
			double max_t = 0;
			for(size_t x = sH * i; x < sH * i + sH; x++) {
				for(size_t y = sW * j; y < sW * j + sW; y++) {
					max_t = (max_t < this->operator()(x, y)) ? this->operator()(x, y) : max_t;
				}
			}
			double temp = max_t;

			// pick the 'color'
			int c = ((temp - min) / (max - min)) * numColors;
			c = (c >= numColors) ? numColors - 1 : ((c < 0) ? 0 : c);

			// print the average temperature
			std::cout << colors[c];
		}
		// right wall
		std::cout << "X" << std::endl;
	}
	// lower wall
	std::cout << "\t";
	for(int l = 0; l < W + 2; l++) {
		std::cout << "X";
	}
	std::cout << std::endl;
}

void Matrix2D::swap(Matrix2D& matrix) {
	std::swap(this->vec, matrix.vec);
}