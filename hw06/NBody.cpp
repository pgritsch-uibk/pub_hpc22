#include "NBody.hpp"

#include "Octree/Octree.h"
#include <algorithm>
#include <fstream>
#include <random>

NBody::NBody(int maxBodies) : particles(maxBodies) {
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
	Vector3D domainFrom = { -500.f, -500.f, -500.f };
	Vector3D domainTo = { 500.f, 500.f, 500.f };
	std::remove_if(particles.begin(), particles.end(), [domainFrom, domainTo](Particle& p) {
		return (!(domainFrom < p.position) || !(domainTo > p.position));
	});
	OctreeNode root(domainFrom, domainTo);

	for(auto& p : particles) {
		root.insert(&p);
	}

	std::for_each(particles.begin(), particles.end(),
	              [](Particle& particle) { particle.force = {}; });

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		std::for_each(particles.begin(), particles.end(), [&](Particle& other) {
			if(!(particle == other)) {
				Vector3D direction = other.position.direction(particle.position);

				float lengthNSqrt = direction.lengthNSqrt();
				float force = (particle.mass * other.mass) / (direction.lengthNSqrt());

				particle.force += direction / std::sqrt(lengthNSqrt) * force;
			}
		});
	});
}

void NBody::exportToFile(const std::string& filename) {
	std::ofstream file(filename, std::ios_base::app);

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		file << particle.position.x << " " << particle.position.y << " " << particle.position.z << std::endl;
	});

	file << std::endl;
	file << std::endl;

	file.close();
}