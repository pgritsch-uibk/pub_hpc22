#ifndef HPC22_MPIVECTORCONFIG_H
#define HPC22_MPIVECTORCONFIG_H

#include <array>

struct MPIVectorConfig {

	const int nBlocks;

	const int blockSize;

	const int stride;
};

template <std::size_t dim>
struct MPISendReceiveConfig {
	const std::array<int, dim> sizes;
	const std::array<int, dim> subSizes;
	const std::array<int, dim> coords;
};

#endif // HPC22_MPIVECTORCONFIG_H
