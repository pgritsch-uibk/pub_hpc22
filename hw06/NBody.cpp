#include "NBody.hpp"

#include <iostream>

NBody::NBody(int maxBodies) : particles(maxBodies) {
	std::for_each(particles.begin(), particles.end(), [](Particle& particle) {
		particle = Particle(Vector3D::random(-20, 20), 0.001f, 0.0001f);
	});
	// std::for_each(particles.begin(), particles.end(), [](Particle& particle) {
	// 	std::cout << "x: " << particle.position.x << " y: " << particle.position.y
	// 	          << " z: " << particle.position.z << std::endl;
	// });
}

NBody::~NBody() = default;

void NBody::update() {
	updateForces();
	updateVelocities();
	updatePositions();
}

void NBody::updatePositions() {
	for(auto& particle : particles) {
		particle.position += particle.velocity;
	}
}

void NBody::updateVelocities() {
	for(auto& particle : particles) {
		particle.velocity += particle.force / particle.mass;
	}
}

void NBody::updateForces() {
	std::for_each(particles.begin(), particles.end(),
	              [](Particle& particle) { particle.force = Vector3D(); });

	std::for_each(particles.begin(), particles.end(), [&](Particle& particle) {
		std::for_each(particles.begin(), particles.end(), [&](Particle& other) {
			if(!(particle == other)) {
				Vector3D direction = other.position - particle.position;

				float distance = direction.length();
				float force = (particle.mass * other.mass) / (distance * distance);

				particle.force += direction.normalize() * force;

				// float distance = particle.position.distance(other.position);

				// particle.force += particle.mass * other.mass / (distance * distance);
			}
		});
	});
}