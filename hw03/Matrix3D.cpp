#include "Matrix3D.h"

#include <fstream>
#include <iomanip>
#include <iostream>

void Matrix3D::writeToFile(std::string filename) {

	std::ofstream file;
	file.open(filename);

	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			for (int k = 0; k < size; ++k) {
				file << std::setw(2) << operator()(i, j, k) << std::endl;
			}
		}
	}
}


void Matrix3D::swap(Matrix3D& matrix) {
	std::swap(this->vec, matrix.vec);
}