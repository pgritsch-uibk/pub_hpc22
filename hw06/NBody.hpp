#pragma once

#include "Octree/Octree.h"
#include "Octree/VectorPool.h"
#include "Particle.hpp"
#include <vector>

class NBody {
  private:
	Octree octree;
	Vector3D domainFrom{};
	Vector3D domainTo{};

  public:
	NBody(int maxBodies);
	~NBody();

	void update();

	void updatePositions();
	void updateVelocities();
	void updateForces();

	void exportToFile(const std::string& filename);

	std::vector<Particle> particles;
};
