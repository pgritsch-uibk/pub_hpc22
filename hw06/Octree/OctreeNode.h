#ifndef HPC22_OCTREENODE_H
#define HPC22_OCTREENODE_H

#include "../constants.h"
#include "../Particle.hpp"
#include "VectorPool.h"
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

class OctreeNode {
  private:

	float mass{};

	bool isVirtual{};

	OctreeNode* p000{};
	OctreeNode* p001{};
	OctreeNode* p010{};
	OctreeNode* p011{};
	OctreeNode* p100{};
	OctreeNode* p101{};
	OctreeNode* p110{};
	OctreeNode* p111{};

	Vector3D domainFrom;
	Vector3D domainTo;
	Vector3D center;
	Vector3D com;

	std::vector<Particle*>* buffer{};

  public:
	explicit OctreeNode(VectorPool<std::vector<Particle*>>* particleBuffPool)
	    : bufferProvider(particleBuffPool) {}

	VectorPool<std::vector<Particle*>>* bufferProvider;
	VectorPool<OctreeNode>* newNodesProvider{};

	void initialize(Vector3D _domainFrom, Vector3D _domainTo);

	void insert(Particle* particle, int depth = 0);

	OctreeNode* getQuadrant(Particle* particle);

	void
	computeMassDistributionAndTraverse(std::vector<Particle*>& depthFirstTraversalOrderedVector);

	void updateForce(Particle* particle) const;
};

#endif // HPC22_OCTREENODE_H
