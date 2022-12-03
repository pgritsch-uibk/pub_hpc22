#include "NBody.hpp"

#include "Octree/Octree.h"
#include "Octree/OctreeNode.h"
#include <algorithm>
#include <fstream>
#include <limits>
#include <random>

NBody::NBody(int maxBodies)
    : octree({ -500.f, -500.f, -500.f }, { 500.f, 500.f, 500.f }), particles(maxBodies) {
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

	octree.fill(particles);

	std::for_each(particles.begin(), particles.end(),
	              [](Particle& particle) { particle.force = {}; });

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		std::for_each(particles.begin(), particles.end(), [&](Particle& other) {
			if(!(particle == other)) {
				Vector3D direction = other.position.direction(particle.position);
				float distanceSquared = direction.lengthSquared();

				if(distanceSquared <=
				   (particle.radius * particle.radius + other.radius * other.radius)) {
					Vector3D velocityDirection = particle.velocity.direction(other.velocity);
					float velocity_relation = 2.f * other.mass / (particle.mass + other.mass);
					Vector3D velocityChange =
					    (velocityDirection * direction) / distanceSquared * direction;

					particle.velocity -= velocityChange * velocity_relation;
				} else {
					float force = (particle.mass * other.mass) / distanceSquared;
					particle.force += direction / std::sqrt(distanceSquared) * force;
				}
			}
		});
	});
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