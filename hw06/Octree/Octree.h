#ifndef HPC22_OCTREE_H
#define HPC22_OCTREE_H

#include "../Particle.hpp"
#include <array>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

class OctreeNode {
  private:
	static inline std::vector<OctreeNode> nodes;
	static inline int current;

	static OctreeNode* getOne() {
		if(current + 1 >= nodes.size()) {
			std::cout << "hi" << std::endl;
			throw std::logic_error("vector too small");
		}
		return &nodes[current++];
	}

	static void reset() {
		if(nodes.size() == 0) {
			nodes.resize(10, OctreeNode{});
		}
		current = 0;
	}

	int children{};

	float mass{};

	Particle* particle{};

	bool isVirtual{};

  public:
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

	static void resetNodePool() { reset(); }

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


	void insert(Particle* _particle, int depth = 0) {
		mass += _particle->mass;
		children++;

		if(particle == nullptr) {
			particle = _particle;
			return;
		}
		/*else {
			if (p000 == nullptr) {
				p000 = getOne();
				p000->initialize(domainFrom, domainTo);
			}
			p000->insert(_particle, depth+1);
		} */

		if(!isVirtual) {
			isVirtual = true;
			getQuadrant(particle)->insert(particle);
		}

		getQuadrant(_particle)->insert(_particle);

		/*		if(p000 == nullptr) {
		            // initializeOctants();

		            // particle = nullptr;
		        }*/


	}

	Particle getParticle() {
		return isVirtual ? *particle : Particle(center, mass, 0.f);
	}

	/*
	void initializeOctants() {

	    p000 = std::make_shared<OctreeNode>(OctreeNode(domainFrom, {
	                                                                   center.x,
	                                                                   center.y,
	                                                                   center.z,
	                                                               }));

	    p001 = std::make_shared<OctreeNode>(
	        OctreeNode({ domainFrom.x, domainFrom.y, center.z }, {
	                                                                 center.x,
	                                                                 center.y,
	                                                                 domainTo.z,
	                                                             }));

	    p010 = std::make_shared<OctreeNode>(
	        OctreeNode({ domainFrom.x, center.y, domainFrom.z }, {
	                                                                 center.x,
	                                                                 domainTo.y,
	                                                                 center.z,
	                                                             }));

	    p011 = std::make_shared<OctreeNode>(
	        OctreeNode({ domainFrom.x, center.y, center.z }, {
	                                                             center.x,
	                                                             domainTo.y,
	                                                             domainTo.z,
	                                                         }));

	    p100 = std::make_shared<OctreeNode>(
	        OctreeNode({ center.x, domainFrom.y, domainFrom.z }, {
	                                                                 domainTo.x,
	                                                                 center.y,
	                                                                 center.z,
	                                                             }));

	    p101 = std::make_shared<OctreeNode>(
	        OctreeNode({ center.x, domainFrom.y, center.z }, {
	                                                             domainTo.x,
	                                                             center.y,
	                                                             domainTo.z,
	                                                         }));

	    p110 = std::make_shared<OctreeNode>(
	        OctreeNode({ center.x, center.y, domainFrom.z }, {
	                                                             domainTo.x,
	                                                             domainTo.y,
	                                                             center.z,
	                                                         }));

	    p111 = std::make_shared<OctreeNode>(
	        OctreeNode({ center.x, center.y, center.z }, {
	                                                         domainTo.x,
	                                                         domainTo.y,
	                                                         domainTo.z,
	                                                     }));
	}*/

	OctreeNode* getQuadrant(Particle* _particle) {
		bool particleIsInsideBoundary =
		    domainFrom <= _particle->position && domainTo >= _particle->position;
				if(!particleIsInsideBoundary) {
		            throw std::logic_error("particle outside of boundaries");
		        }

		bool fromXIsNearer = (_particle->position.x - center.x) <= 0;

		bool fromYIsNearer = (_particle->position.y - center.y) <= 0;

		bool fromZIsNearer = (_particle->position.z - center.z) <= 0;

		OctreeNode* quadrant;
		if (current > 8) {
			std::cout << "hiallo" << std::endl;
		}

		if(fromXIsNearer && fromYIsNearer && fromZIsNearer) {
			if(p000 == nullptr) {
				p000 = getOne();
				p000->initialize(domainFrom, {
				                                 center.x,
				                                 center.y,
				                                 center.z,
				                             });
			}
			quadrant = p000;
		} else if(fromXIsNearer && fromYIsNearer) {
			if(p001 == nullptr) {
				p001 = getOne();
				p001->initialize({ domainFrom.x, domainFrom.y, center.z }, {
				                                                               center.x,
				                                                               center.y,
				                                                               domainTo.z,
				                                                           });
			}
			quadrant = p001;
		} else if(fromXIsNearer && fromZIsNearer) {
			if(p010 == nullptr) {
				p010 = getOne();
				p010->initialize({ domainFrom.x, center.y, domainFrom.z }, {
				                                                               center.x,
				                                                               domainTo.y,
				                                                               center.z,
				                                                           });
			}
			quadrant = p010;
		} else if(fromXIsNearer) {
			if(p011 == nullptr) {
				p011 = getOne();
				p011->initialize({ domainFrom.x, center.y, center.z }, {
				                                                           center.x,
				                                                           domainTo.y,
				                                                           domainTo.z,
				                                                       });
			}
			quadrant = p011;
		} else if(fromYIsNearer && fromZIsNearer) {
			if(p100 == nullptr) {
				p100 = getOne();
				p100->initialize({ center.x, domainFrom.y, domainFrom.z }, {
				                                                               domainTo.x,
				                                                               center.y,
				                                                               center.z,
				                                                           });
			}
			quadrant = p100;
		} else if(fromYIsNearer) {
			if(p101 == nullptr) {
				p101 = getOne();
				p101->initialize({ center.x, domainFrom.y, center.z }, {
				                                                           domainTo.x,
				                                                           center.y,
				                                                           domainTo.z,
				                                                       });
			}

			quadrant = p101;
		} else if(fromZIsNearer) {
			if(p110 == nullptr) {
				p110 = getOne();
				p110->initialize({ center.x, center.y, domainFrom.z }, {
				                                                           domainTo.x,
				                                                           domainTo.y,
				                                                           center.z,
				                                                       });
			}
			quadrant = p110;
		} else {
			if(p111 == nullptr) {
				p111 = getOne();
				p111->initialize({ center.x, center.y, center.z }, {
				                                                       domainTo.x,
				                                                       domainTo.y,
				                                                       domainTo.z,
				                                                   });
			}
			quadrant = p111;
		}

		return quadrant;
	}
};


#endif // HPC22_OCTREE_H
