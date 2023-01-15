#include "hsv2rgbSSE.h"
#include <array>
#include <boost/mpi.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <thread>
#include <vector>

#define YIELD

// Include that allows to print result as an image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

constexpr int default_size_x = 3840;
constexpr int default_size_y = 2176;

// RGB image will hold 3 color channels
constexpr int num_channels = 3;
// max iterations cutoff
constexpr int max_iterations = 10000;

constexpr int ind(int y, int x, int size_y, int size_x, int channel) {
	return y * size_x * num_channels + x * num_channels + channel;
}

void calcMandelbrot(std::vector<uint8_t>& image, int sizeX, int sizeY) {
	int myRank, numProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	const float left = -2.5, right = 1;
	const float bottom = -1, top = 1;

	const __m128 _inverse_sizeX = _mm_set_ps1(1.f / static_cast<float>(sizeX));
	const __m128 _inverse_max_iter = _mm_set_ps1(1.f / max_iterations);
	const __m128 _left = _mm_set_ps1(static_cast<float>(left));
	const __m128 _right_minus_left = _mm_set_ps1(static_cast<float>(right - left));
	const __m128 _1 = _mm_set_ps1(1.f);
	const __m128 _2 = _mm_set_ps1(2.f);
	const __m128 _4 = _mm_set_ps1(4.f);
	const __m128 _200 = _mm_set_ps1(200.f);

	for(int pixelY = myRank; pixelY < sizeY; pixelY += numProcs) {
		// scale y pixel into mandelbrot coordinate system
		const __m128 _cy = _mm_set_ps1(
		    (static_cast<float>(pixelY) / static_cast<float>(sizeY)) * (top - bottom) + bottom);
		for(int pixelX = 0; pixelX < sizeX; pixelX += 4) {
			// scale _x pixel into mandelbrot coordinate system
			const auto pixelXFloat = static_cast<float>(pixelX);
			__m128 _cx =
			    _mm_set_ps(pixelXFloat + 3.f, pixelXFloat + 2.f, pixelXFloat + 1.f, pixelXFloat);
			_cx = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(_cx, _inverse_sizeX), _right_minus_left), _left);
			__m128 _x = _mm_set_ps1(0.f);
			__m128 _y = _mm_set_ps1(0.f);

			int numIterations = 0;

			__m128 _numIterations = _mm_set_ps1(0.f);

			// Check if the distance from the origin becomes
			// greater than 2 within the max number of iterations.
			while(numIterations++ < max_iterations) {
				const __m128 _x_sqrd = _mm_mul_ps(_x, _x);
				const __m128 _y_sqrd = _mm_mul_ps(_y, _y);
				const __m128 _x_dot_y = _mm_mul_ps(_x, _y);

				__m128 mask = _mm_cmple_ps(_mm_add_ps(_x_sqrd, _y_sqrd), _4);

				if(_mm_movemask_ps(mask) == 0) {
					break;
				}

				_numIterations = _mm_add_ps(_mm_and_ps(mask, _1), _numIterations);

				_x = _mm_add_ps(_mm_sub_ps(_x_sqrd, _y_sqrd), _cx);
				_y = _mm_add_ps(_mm_mul_ps(_2, _x_dot_y), _cy);
			}

			// Normalize iteration and write it to pixel position
			__m128 _value = _mm_mul_ps(_mm_mul_ps(_numIterations, _inverse_max_iter), _200);

			std::array<__m128, 4> rgbs = hsvToRgb(_value);

			for(int i = 0; i < 4; i++) {
				std::array<float, 4> rgb0;
				_mm_store_ps(rgb0.begin(), rgbs[i]);
				image[ind(pixelY, (pixelX + i), sizeY, sizeX, 0)] = (uint8_t)(rgb0[0]);
				image[ind(pixelY, (pixelX + i), sizeY, sizeX, 1)] = (uint8_t)(rgb0[1]);
				image[ind(pixelY, (pixelX + i), sizeY, sizeX, 2)] = (uint8_t)(rgb0[2]);
			}
		}
	}
}

int main(int argc, char** argv) {
	int sizeX = default_size_x;
	int sizeY = default_size_y;

	int numProcs, myRank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	if(argc == 3) {
		sizeX = std::atoi(argv[1]);
		sizeY = std::atoi(argv[2]);
	} else if(myRank == 0) {
		std::cout << "No arguments given, using default size" << std::endl;
	}

	if(sizeX % 4 != 0) {
		std::cerr << "Vectorization requires X parameter to be multiple of 4" << std::endl;
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}

	if(sizeY % numProcs != 0) {
		std::cerr << "Size Y must be multiple of processor number" << std::endl;
		MPI_Finalize();
		exit(EXIT_FAILURE);
	}

	int yPerProc = sizeY / numProcs;
	int blockCount = yPerProc;
	int blockSize = num_channels * sizeX;
	int stride = blockSize * numProcs;

	MPI_Datatype sendLines, receiveOneLineBlock;
	MPI_Type_vector(blockCount, blockSize, stride, MPI_UINT8_T, &sendLines);
	MPI_Type_commit(&sendLines);

	if(myRank == 0) {
		MPI_Type_create_resized(sendLines, 0, blockSize * sizeof(uint8_t), &receiveOneLineBlock);
		MPI_Type_commit(&receiveOneLineBlock);
	}

	std::vector<int> displacements(numProcs);
	for(int proc = 0; proc < numProcs; proc++) {
		displacements[proc] = proc;
	}
	std::vector<int> counts(numProcs, 1);

	std::vector<uint8_t> image = std::vector<uint8_t>(num_channels * sizeX * sizeY, 0);
	std::vector<uint8_t> result = std::vector<uint8_t>(num_channels * sizeX * sizeY, 0);

	u_int8_t* startOfData = image.data() + ind(myRank, 0, sizeY, sizeX, 0);

	boost::mpi::timer timer;

	calcMandelbrot(image, sizeX, sizeY);

	MPI_Request request;
	MPI_Igatherv(startOfData, 1, sendLines, result.data(), counts.data(), displacements.data(),
	             receiveOneLineBlock, 0, MPI_COMM_WORLD, &request);

#ifdef YIELD
	int complete = 0;
	while(true) {
		MPI_Test(&request, &complete, MPI_STATUS_IGNORE);
		if(!complete) {
			std::this_thread::yield();
		} else {
			break;
		}
	}
#else
	MPI_Wait(&request, MPI_STATUS_IGNORE);
#endif

	if(myRank == 0) {
		std::cout << "Mandelbrot set calculation for " << sizeX << "x" << sizeY
		          << " took: " << timer.elapsed() << " seconds." << std::endl;

		const int stride_bytes = 0;

		/*	stbi_write_png("mandelbrot_par.png", sizeX, sizeY, num_channels, result.data(),
		                   stride_bytes);
	*/
		MPI_Type_free(&receiveOneLineBlock);
	}

	MPI_Type_free(&sendLines);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
