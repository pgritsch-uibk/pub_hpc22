#pragma once

#include <vector>

#include "Particle.hpp"

class NBody {
  private:
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
