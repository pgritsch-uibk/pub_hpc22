#ifndef HPC22_OCTREENODE_H
#define HPC22_OCTREENODE_H

#include "../Particle.hpp"
#include "VectorPool.h"
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#define MAX_DEPTH 100

class OctreeNode {
  private:
	int children{};

	float mass{};

	Particle* particle{};

	bool isVirtual{};

  public:
	explicit OctreeNode(VectorPool<std::vector<Particle*>>* particleBuffPool)
	    : bufferProvider(particleBuffPool) {}

	VectorPool<std::vector<Particle*>>* bufferProvider;
	VectorPool<OctreeNode>* newNodesProvider{};

	OctreeNode* p000{};
	OctreeNode* p001{};
	OctreeNode* p010{};
	OctreeNode* p011{};
	OctreeNode* p100{};
	OctreeNode* p101{};
	OctreeNode* p110{};
	OctreeNode* p111{};
	std::vector<Particle*>* buffer{};

	Vector3D domainFrom;
	Vector3D domainTo;
	Vector3D center;
	Vector3D com;

	void initialize(Vector3D _domainFrom, Vector3D _domainTo) {

		children = 0;
		mass = 0;
		particle = nullptr;
		isVirtual = false;

		p000 = nullptr;
		p001 = nullptr;
		p010 = nullptr;
		p011 = nullptr;
		p100 = nullptr;
		p101 = nullptr;
		p110 = nullptr;
		p111 = nullptr;
		domainFrom = _domainFrom;
		domainTo = _domainTo;
		center = { _domainFrom.x + ((domainTo.x - domainFrom.x) / 2.f),
			       _domainFrom.y + ((domainTo.y - domainFrom.y) / 2.f),
			       _domainFrom.z + ((domainTo.z - domainFrom.z) / 2.f) };
		com = center;
	}

	void insert(Particle* _particle, int maxDepth, int depth = 0);
	OctreeNode* getQuadrant(Particle* _particle);

	void computeMassDistributionAndTraverse(std::vector<Particle*>& depthFirstTraversalOrderedVector);

	void updateForce(Particle& _particle);
};

#endif // HPC22_OCTREENODE_H
