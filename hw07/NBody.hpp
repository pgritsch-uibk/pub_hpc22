#pragma once

#include "Octree/Octree.h"
#include "Octree/VectorPool.h"
#include "Particle.hpp"
#include <mpi.h>
#include <vector>

class NBody {
  private:
	Octree octree;
	Vector3D domainFrom{};
	Vector3D domainTo{};

	MPI_Datatype particleDatatype;

	std::vector<Particle> particleRecvVector;
	std::vector<Particle> particleSendVector;
	std::vector<Particle*> depthFirstNodeBuffer;

	int loadPerProc{};

  public:
	NBody(int maxBodies);
	~NBody();

	void update();

	void exportToFile(const std::string& filename);

	std::vector<Particle> particles;
};
