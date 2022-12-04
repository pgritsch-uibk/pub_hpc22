#ifndef HPC22_OCTREE_H
#define HPC22_OCTREE_H

#include "OctreeNode.h"
#include "VectorPool.h"
#include <algorithm>
#include <limits>

class Octree {

  private:
	Vector3D domainFrom;
	Vector3D domainTo;

	VectorPool<std::vector<Particle*>> particleBuffPool;
	VectorPool<OctreeNode> subNodePool;

	OctreeNode root;

	void initializeRoot(std::vector<Particle>& particles);

	void reset();

  public:
	Octree(Vector3D _domainFrom, Vector3D _domainTo)
	    : domainFrom(_domainFrom), domainTo(_domainTo),
	      particleBuffPool(VectorPool(1000000, std::vector<Particle*>(100))),
	      subNodePool(VectorPool(1000000, OctreeNode(&particleBuffPool))),
	      root(OctreeNode(&particleBuffPool)) {

		root.initialize(domainFrom, domainTo);
		root.newNodesProvider = &subNodePool;
		subNodePool.for_each([&](OctreeNode& node) { node.newNodesProvider = &subNodePool; });
	}

	void fill(std::vector<Particle>& particles, std::vector<Particle*>& orderedParticles);

	void updateForce(Particle* particle) const;
};

#endif // HPC22_OCTREE_H
