#ifndef HPC22_IMAGE_H
#define HPC22_IMAGE_H

#include <mpi.h>
#include <vector>

struct Image {

	const int sizeY;
	const int sizeX;
	const int numChannels;

	std::vector<float> imageLine;

	Image(int _sizeY, int _sizeX, int _numChannels)
	    : sizeY(_sizeY), sizeX(_sizeX), numChannels(_numChannels),
	      imageLine(sizeY * (sizeX * numChannels + 1), 0) {}

	inline void setY(int y, int yInArray) { operator()(yInArray, 0, -1) = static_cast<float>(y); }

	inline float& operator()(int y, int x, int channel) {
		return imageLine[y * (1 + sizeX * numChannels) + x * numChannels + channel + 1];
	}

	inline std::pair<int, float*> getLineNumberAndPointerFor(int yInArray) {
		float* yLine = &imageLine[yInArray * (1 + sizeX * numChannels)];
		return std::make_pair(static_cast<int>(*yLine), yLine + 1);
	}

	inline float getValueForLine(float* line, int x, int c) {
		return *(line + x * numChannels + c);
	}

	MPI_Datatype createDatatype() {
		int blockSize = numChannels * sizeX + 1;
		MPI_Datatype datatype;
		MPI_Type_vector(1, blockSize, blockSize, MPI_FLOAT, &datatype);
		return datatype;
	}
};

#endif // HPC22_IMAGE_H
