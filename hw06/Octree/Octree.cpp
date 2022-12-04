#include "Octree.h"

void Octree::fill(std::vector<Particle>& particles, std::vector<Particle*>& orderedParticles) {

	reset();

	initializeRoot(particles);

	for(auto& p : particles) {
		root.insert(&p);
	}

	root.computeMassDistributionAndTraverse(orderedParticles);
}

void Octree::initializeRoot(std::vector<Particle>& particles) {
	root = OctreeNode(&particleBuffPool);
	root.newNodesProvider = &subNodePool;

	float float_min = std::numeric_limits<float>::min();
	float float_max = std::numeric_limits<float>::max();
	float lower = float_max;
	float upper = float_min;

	for(auto& p : particles) {
		lower = std::min({ p.position.x, p.position.y, p.position.z, lower });
		upper = std::max({ p.position.x, p.position.y, p.position.z, upper });
	}
	domainFrom = { lower, lower, lower };
	domainTo = { upper, upper, upper };

	root.initialize(domainFrom, domainTo);
}

void Octree::reset() {
	particleBuffPool.reset();
	subNodePool.reset();
}

void Octree::updateForce(Particle* particle) const {
	particle->force = {};

	root.updateForce(particle);
}