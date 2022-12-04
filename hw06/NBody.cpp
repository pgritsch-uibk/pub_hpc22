#include "NBody.hpp"

#include "Octree/Octree.h"
#include "Octree/OctreeNode.h"
#include <algorithm>
#include <fstream>
#include <random>

NBody::NBody(int maxBodies)
    : octree({ -500.f, -500.f, -500.f }, { 500.f, 500.f, 500.f }),
      treeOrderedParticles(maxBodies),
      particles(maxBodies) {
	std::default_random_engine rng(std::random_device{}());
	std::uniform_real_distribution<float> radius(0.0001f, 0.001f);
	std::uniform_real_distribution<float> mass(0.0001f, 0.001f);

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		auto rrnd = radius(rng);
		//	  	auto mrnd = mass(rng);
		particle = Particle(Vector3D::random(-20, 20), rrnd, rrnd);
	});

	//	particles[0] = Particle(Vector3D(0, 0, 0), 100, 0.01f);
}

NBody::~NBody() = default;

void NBody::update() {
	updateForces();
	updateVelocities();
	updatePositions();
}

void NBody::updatePositions() {
	std::for_each(particles.begin(), particles.end(),
	              [&](Particle& particle) { particle.position += particle.velocity; });
}

void NBody::updateVelocities() {
	std::for_each(particles.begin(), particles.end(),
	              [&](Particle& particle) { particle.velocity += particle.force / particle.mass; });
}

void NBody::updateForces() {
	treeOrderedParticles.clear();
	octree.fill(particles, treeOrderedParticles);
	for(Particle* p : treeOrderedParticles) {
		octree.updateForce(p);
	}
}

void NBody::exportToFile(const std::string& filename) {
	std::ofstream file(filename, std::ios_base::app);

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		file << particle.position.x << " " << particle.position.y << " " << particle.position.z
		     << std::endl;
	});

	file << std::endl;
	file << std::endl;

	file.close();
}