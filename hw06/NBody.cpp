#include "NBody.hpp"

#include <iostream>
#include <random>

NBody::NBody(int maxBodies) : particles(maxBodies) {
	std::default_random_engine rng(std::random_device{}());
	std::uniform_real_distribution<float> dist(0.0001f, 0.001f);

	std::uniform_real_distribution<float> dist_two(-15.0f, 15.0f);

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		auto rnd = dist(rng);
		particle = Particle(Vector3D::random(-15, 15), rnd, rnd);
	});
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
	std::for_each(particles.begin(), particles.end(),
	              [](Particle& particle) { particle.force = {}; });

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		std::for_each(particles.begin(), particles.end(), [&](Particle& other) {
			if(!(particle == other)) {
				Vector3D direction = other.position.direction(particle.position);

				float distance = direction.length();
				float force = (particle.mass * other.mass) / (distance * distance);

				particle.force += direction.normalize() * force;
			}
		});
	});
}