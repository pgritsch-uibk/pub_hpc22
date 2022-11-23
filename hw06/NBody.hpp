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

	std::vector<Particle> particles;
};
