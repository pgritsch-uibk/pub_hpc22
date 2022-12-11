#include "dlb_specifics/Image.h"
#include "dlb_specifics/LoadBalancer.h"
#include "hsv2rgbSSE.h"
#include <array>
#include <boost/mpi.hpp>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <map>
#include <mpi.h>
#include <vector>

// Include that allows to print result as an image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

constexpr int rootRank = 0;

constexpr int default_size_x = 1280;
constexpr int default_size_y = 720;

// RGB image will hold 3 color numChannels
constexpr int num_channels = 3;
// max iterations cutoff
constexpr int max_iterations = 10000;

constexpr int ind(int y, int x, int size_y, int size_x, int channel) {
	return y * size_x * num_channels + x * num_channels + channel;
}

int getNextYValue(MPI_Win& nextYWindow) {
	int nextY;
	const int one = 1;

	MPI_Win_lock(MPI_LOCK_SHARED, 0, 0, nextYWindow);
	MPI_Fetch_and_op(&one, &nextY, MPI_INT, 0, 0, MPI_SUM, nextYWindow);
	MPI_Win_unlock(0, nextYWindow);

	return nextY;
}

void calcMandelbrot(Image& imageLine, int sizeX, int sizeY, int yStart, int yInArray) {
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

	int pixelY = yStart;

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
			imageLine(yInArray, pixelX + i, 0) = (rgb0[0]);
			imageLine(yInArray, pixelX + i, 1) = (rgb0[1]);
			imageLine(yInArray, pixelX + i, 2) = (rgb0[2]);
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

	// result buffers
	Image localResult(sizeY, sizeX, num_channels);
	Image globalResult(sizeY, sizeX, num_channels);

	// mpi datatypes
	MPI_Datatype sendLines = localResult.createDatatype();
	MPI_Datatype receiveLines = globalResult.createDatatype();
	MPI_Type_commit(&sendLines);
	MPI_Type_commit(&receiveLines);

	{
		LoadBalancer lb(rootRank);

		boost::mpi::timer timer;

		int yProcessed = 0;
		int nextY = 0;
		while((nextY = lb.getNextTwoY()) < sizeY - 1) {
			localResult.setY(nextY, yProcessed);
			calcMandelbrot(localResult, sizeX, sizeY, nextY, yProcessed++);
			localResult.setY(nextY + 1, yProcessed);
			calcMandelbrot(localResult, sizeX, sizeY, nextY, yProcessed++);
		}

		std::vector<int> counts(numProcs, 0);

		// prepare information for gatherv
		MPI_Gather(&yProcessed, 1, MPI_INT, counts.data(), 1, MPI_INT, rootRank, MPI_COMM_WORLD);

		int start = 0;
		std::vector<int> displacements(numProcs, 0);
		if(myRank == rootRank) {
			for(int proc = 0; proc < numProcs; proc++) {
				displacements[proc] = start;
				start += counts[proc];
			}
		}

		MPI_Gatherv(localResult.imageLine.data(), yProcessed, sendLines,
		            globalResult.imageLine.data(), counts.data(), displacements.data(),
		            receiveLines, rootRank, MPI_COMM_WORLD);

		if(myRank == rootRank) {
			std::cout << "Mandelbrot set calculation for " << sizeX << "x" << sizeY
			          << " took: " << timer.elapsed() << " seconds." << std::endl;

			const int stride_bytes = 0;

			std::vector<uint8_t> image = std::vector<uint8_t>(num_channels * sizeX * sizeY, 0);

			std::map<int, float*> lut;
			for(int y = 0; y < sizeY; y++) {
				std::pair<int, float*> lineNumberAndPointer =
				    globalResult.getLineNumberAndPointerFor(y);
				lut[lineNumberAndPointer.first] = lineNumberAndPointer.second;
			}

			for(int y = 0; y < sizeY; y++) {
				for(int x = 0; x < sizeX; x++) {
					for(int c = 0; c < 3; c++) {
						image[ind(y, x, sizeY, sizeX, c)] =
						    (uint8_t)globalResult.getValueForLine(lut[y], x, c);
					}
				}
			}

			stbi_write_png("mandelbrot_par_dlb.png", sizeX, sizeY, num_channels, image.data(),
			               stride_bytes);
		}
	}

	MPI_Type_free(&sendLines);
	MPI_Type_free(&receiveLines);
	MPI_Finalize();
	return EXIT_SUCCESS;
}
