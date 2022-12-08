#pragma once

#include "Vector3D.hpp"
#include <array>
#include <mpi.h>

struct Particle {
	Vector3D position;
	Vector3D velocity;
	Vector3D force;
	float mass;
	float radius;

	Particle(Vector3D position, Vector3D velocity, float mass, float radius)
	    : position(position), velocity(velocity), mass(mass), radius(radius) {}

	Particle(Vector3D position, float mass, float radius)
	    : position(position), velocity(), mass(mass), radius(radius) {}

	explicit Particle(Particle* particle)
	    : position(particle->position), velocity(particle->velocity), mass(particle->mass),
	      radius(particle->radius) {}

	~Particle() = default;

	Particle() : position(), velocity(), mass(0), radius(0) {}

	Particle& operator=(const Particle& particle) {
		position = particle.position;
		velocity = particle.velocity;
		mass = particle.mass;
		radius = particle.radius;
		return *this;
	}

	bool operator==(const Particle& particle) const {
		return position == particle.position && velocity == particle.velocity &&
		       mass == particle.mass && radius == particle.radius;
	}

	static MPI_Datatype createMPIDatatype() {
		MPI_Datatype type;

		std::array<int, 2> blockLengths = { 3, 2 };
		std::array<MPI_Aint, 2> displacements = {
			offsetof(Particle, position),
			offsetof(Particle, mass),
		};
		std::array<MPI_Datatype, 2> dataTypes = { Vector3D::createMPIDatatype(), MPI_FLOAT };
		MPI_Type_create_struct(2, blockLengths.begin(), displacements.begin(), dataTypes.begin(),
		                       &type);
		return type;
	}
};
