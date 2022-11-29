#include <iostream>

#include "NBody.hpp"
#include "misc.h"
#include <chrono>

int main() {
	//increaseStackSize();
	NBody nBody(5);
#ifndef PERFORMANCE_TEST
	const std::string filename = "output.txt";
#endif

#ifdef PERFORMANCE_TEST
	auto start = std::chrono::high_resolution_clock::now();
#endif
	for(int i = 0; i < 100; i++) {
#ifndef PERFORMANCE_TEST
		std::cout << "Iteration: " << i << std::endl;
		nBody.exportToFile(filename);
#endif
		nBody.update();
	}

#ifdef PERFORMANCE_TEST
	auto end = std::chrono::high_resolution_clock::now();

	std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " seconds" << std::endl;
#else
	nBody.exportToFile(filename); // last export
#endif

	return 0;
}