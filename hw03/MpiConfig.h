#ifndef HPC22_MPIVECTORCONFIG_H
#define HPC22_MPIVECTORCONFIG_H

#include <array>

struct MPIVectorConfig {

	const int nBlocks;

	const int blockSize;

	const int stride;
};

struct MPISendReciveConfig {
	const std::array<int, 2> sizes;
	const std::array<int, 2> subSizes;
	const std::array<int, 2> coords;
};

#endif // HPC22_MPIVECTORCONFIG_H
