#ifndef HPC22_HEAT_STENCIL_COMMON_2D_H
#define HPC22_HEAT_STENCIL_COMMON_2D_H

#include "Matrix2D.h"

#define HEAT_SOURCE (273.f + 60.f)
#define DIMENSIONS 2

inline float calculateNewValue(Matrix2D& matrix2D, int i, int j) {
	return matrix2D(i, j) + 0.2f * (-4.0f * matrix2D(i, j) + matrix2D(i - 1, j) +
	                                matrix2D(i + 1, j) + matrix2D(i, j - 1) + matrix2D(i, j + 1));
}

bool isResultSane(Matrix2D& result) {
	for(long long i = 0; i < result.size; i++) {
		for(long long j = 0; j < result.size; j++) {
			float temp = result(i, j);
			if(273.f <= temp && temp <= 273.f + 60.f) {
				continue;
			}
			return false;
		}
	}
	return true;
}

#endif // HPC22_HEAT_STENCIL_COMMON_2D_H
