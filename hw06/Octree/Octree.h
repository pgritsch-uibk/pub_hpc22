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

  public:
	Octree(Vector3D _domainFrom, Vector3D _domainTo)
	    : domainFrom(_domainFrom), domainTo(_domainTo),
	      particleBuffPool(VectorPool(100, std::vector<Particle*>(100))),
	      subNodePool(VectorPool(100000, OctreeNode(&particleBuffPool))),
	      root(OctreeNode(&particleBuffPool)) {

		root.initialize(domainFrom, domainTo);
		root.newNodesProvider = &subNodePool;
		subNodePool.for_each([&](OctreeNode& node) { node.newNodesProvider = &subNodePool; });
	}

	void fill(std::vector<Particle>& particles) {
		reset();

		initializeRoot(particles);

		for(auto& p : particles) {
			root.insert(&p, 100);
		}
	}

	void initializeRoot(std::vector<Particle>& particles) {
		root = OctreeNode(&particleBuffPool);
		root.newNodesProvider = &subNodePool;

		float min = std::numeric_limits<float>::min();
		float max = std::numeric_limits<float>::max();

		domainFrom = { max, max, max };
		domainTo = { min, min, min };

		for(auto& p : particles) {
			domainFrom.x = std::min(domainFrom.x, p.position.x);
			domainFrom.y = std::min(domainFrom.y, p.position.y);
			domainFrom.z = std::min(domainFrom.z, p.position.z);

			domainTo.x = std::max(domainTo.x, p.position.x);
			domainTo.y = std::max(domainTo.y, p.position.y);
			domainTo.z = std::max(domainTo.z, p.position.z);
		}

		root.initialize(domainFrom, domainTo);
	}

	void reset() {
		particleBuffPool.reset();
		subNodePool.reset();
	}
};

#endif // HPC22_OCTREE_H
