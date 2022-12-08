#include "NBody.hpp"

#include <algorithm>
#include <fstream>
#include <random>

NBody::NBody(int maxBodies)
    : octree({ -500.f, -500.f, -500.f }, { 500.f, 500.f, 500.f }), particleRecvVector(maxBodies),
      particleSendVector(maxBodies), depthFirstNodeBuffer(maxBodies), particles(maxBodies) {

	int myRank, numProcs;
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);

	if(particles.size() % numProcs != 0) {
		std::cerr << "unsupported number of procs" << std::endl;
		MPI_Finalize();
		exit(1);
	}

	loadPerProc = (int)particles.size() / numProcs;

	if(myRank == 0) {
		std::default_random_engine rng(std::random_device{}());
		std::uniform_real_distribution<float> radius(0.0001f, 0.001f);
		std::uniform_real_distribution<float> mass(0.0001f, 0.001f);

		for(auto& particle : particles) {
			auto rrnd = radius(rng);
			particle = Particle(Vector3D::random(-20, 20), rrnd, rrnd);
		}
	}
	particleDatatype = Particle::createMPIDatatype();
	MPI_Type_commit(&particleDatatype);

	MPI_Bcast(particles.data(), (int)particles.size(), particleDatatype, 0, MPI_COMM_WORLD);
}

NBody::~NBody() {
	MPI_Type_free(&particleDatatype);
}

void NBody::update() {
	int myRank, numProcs;
	MPI_Comm_size(MPI_COMM_WORLD, &numProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

	depthFirstNodeBuffer.clear();
	octree.fill(particles, &depthFirstNodeBuffer);

	int start = myRank * loadPerProc;
	int end = start + loadPerProc;

	for(int i = start; i < end; i++) {
		octree.updateForce(depthFirstNodeBuffer[i]);
	}

	for(int i = start; i < end; i++) {
		Particle* particle = depthFirstNodeBuffer[i];
		particle->velocity += particle->force / particle->mass;
	}

	for(int i = start; i < end; i++) {
		Particle* particle = depthFirstNodeBuffer[i];
		particle->position += particle->velocity;
		particleSendVector[i].position = particle->position;
		particleSendVector[i].velocity = particle->velocity;
		particleSendVector[i].force = particle->force;
		particleSendVector[i].mass = particle->mass;
		particleSendVector[i].radius = particle->radius;
	}

	MPI_Allgather(particleSendVector.data() + start, loadPerProc, particleDatatype,
	              particleRecvVector.data(), loadPerProc, particleDatatype, MPI_COMM_WORLD);

	std::swap(particles, particleRecvVector);
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