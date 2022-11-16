#ifndef HPC22_LOCATION_H
#define HPC22_LOCATION_H

#include <array>

struct Location {

	int myRank;

	std::array<int, 2> myCoords;

	int rankToNorth;
	int rankToSouth;
	int rankToWest;
	int rankToEast;
};

#endif // HPC22_LOCATION_H
