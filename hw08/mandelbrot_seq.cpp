#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <cstdint>

#include <boost/mpi.hpp>

// Include that allows to print result as an image
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

constexpr int default_size_x = 1280;
constexpr int default_size_y = 720;

// RGB image will hold 3 color channels
constexpr int num_channels = 3;
// max iterations cutoff
constexpr int max_iterations = 10000;

constexpr int ind(int y, int x, int size_y, int size_x, int channel) {
	return y * size_x * num_channels + x * num_channels + channel;
}

void HSVToRGB(double H, double S, double V, double* R, double* G, double* B);

void calcMandelbrot(std::vector<uint8_t> image, int sizeX, int sizeY) {
	boost::mpi::timer timer;
	const float left = -2.5, right = 1;
	const float bottom = -1, top = 1;

	for(int pixelY = 0; pixelY < sizeY; pixelY++) {
		// scale y pixel into mandelbrot coordinate system
		const float cy = (static_cast<float>(pixelY) / static_cast<float>(sizeY)) * (top - bottom) + bottom;
		for(int pixelX = 0; pixelX < sizeX; pixelX++) {
			// scale x pixel into mandelbrot coordinate system
			const float cx = (static_cast<float>(pixelX) / static_cast<float>(sizeX)) * (right - left) + left;
			float x = 0;
			float y = 0;
			int numIterations = 0;

			// Check if the distance from the origin becomes
			// greater than 2 within the max number of iterations.
			while((x * x + y * y <= 2 * 2) && (numIterations < max_iterations)) {
				float x_tmp = x * x - y * y + cx;
				y = 2 * x * y + cy;
				x = x_tmp;
				numIterations += 1;
			}

			// Normalize iteration and write it to pixel position
			double value = std::fabs((static_cast<float>(numIterations) / static_cast<float>(max_iterations))) * 200;

			double red = 0;
			double green = 0;
			double blue = 0;

			HSVToRGB(value, 1.0, 1.0, &red, &green, &blue);

			int channel = 0;
			image[ind(pixelY, pixelX, sizeY, sizeX, channel++)] = (uint8_t)(red * UINT8_MAX);
			image[ind(pixelY, pixelX, sizeY, sizeX, channel++)] = (uint8_t)(green * UINT8_MAX);
			image[ind(pixelY, pixelX, sizeY, sizeX, channel++)] = (uint8_t)(blue * UINT8_MAX);
		}
	}
	std::cout << "Mandelbrot set calculation for " << sizeX << "x" << sizeY << " took: " << timer.elapsed() << " seconds." << std::endl;
}

int main(int argc, char** argv) {
	int sizeX = default_size_x;
	int sizeY = default_size_y;

	if(argc == 3) {
		sizeX = std::atoi(argv[1]);
		sizeY = std::atoi(argv[2]);
	} else {
		std::cout << "No arguments given, using default size" << std::endl;
	}

	std::vector<uint8_t> image = std::vector<uint8_t>(num_channels * sizeX * sizeY);

	calcMandelbrot(image, sizeX, sizeY);

	const int stride_bytes = 0;
	stbi_write_png("mandelbrot_seq.png", sizeX, sizeY, num_channels, image.data(), stride_bytes);


	return EXIT_SUCCESS;
}

void HSVToRGB(double H, double S, double V, double* R, double* G, double* B) {
	if (H >= 1.00) {
		V = 0.0;
		H = 0.0;
	}

	double step = 1.0/6.0;
	double vh = H/step;

	int i = (int)floor(vh);

	double f = vh - i;
	double p = V*(1.0 - S);
	double q = V*(1.0 - (S*f));
	double t = V*(1.0 - (S*(1.0 - f)));

	switch (i) {
		case 0:
		{
			*R = V;
			*G = t;
			*B = p;
			break;
		}
		case 1:
		{
			*R = q;
			*G = V;
			*B = p;
			break;
		}
		case 2:
		{
			*R = p;
			*G = V;
			*B = t;
			break;
		}
		case 3:
		{
			*R = p;
			*G = q;
			*B = V;
			break;
		}
		case 4:
		{
			*R = t;
			*G = p;
			*B = V;
			break;
		}
		case 5:
		{
			*R = V;
			*G = p;
			*B = q;
			break;
		}
	}
}