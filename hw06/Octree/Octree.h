#ifndef HPC22_OCTREE_H
#define HPC22_OCTREE_H

#include "../Particle.hpp"
#include <array>
#include <memory>
#include <vector>

class OctreeNode {
  private:
	float mass;

	bool isVirtual;

	Particle* particle;

  public:
	std::shared_ptr<OctreeNode> p000;
	std::shared_ptr<OctreeNode> p001;
	std::shared_ptr<OctreeNode> p010;
	std::shared_ptr<OctreeNode> p011;
	std::shared_ptr<OctreeNode> p100;
	std::shared_ptr<OctreeNode> p101;
	std::shared_ptr<OctreeNode> p110;
	std::shared_ptr<OctreeNode> p111;

	Vector3D domainFrom;
	Vector3D domainTo;
	Vector3D center;

	OctreeNode(Vector3D _domainFrom, Vector3D _domainTo)
	    : mass(0.f), isVirtual(false), particle(nullptr), p000(nullptr), p001(nullptr), p010(nullptr), p011(nullptr),
	      p100(nullptr), p101(nullptr), p110(nullptr), p111(nullptr), domainFrom(_domainFrom),
	      domainTo(_domainTo), center(_domainFrom.x + std::abs(domainTo.x - domainFrom.x) / 2.f,
	                                  _domainFrom.y + std::abs(domainTo.y - domainFrom.y) / 2.f,
	                                  _domainFrom.z + std::abs(domainTo.z - domainFrom.z) / 2.f){};

	void insert(Particle* _particle) {
		mass += _particle->mass;

		if(particle == nullptr) {
			particle = _particle;
			return;
		}

		if(p000 == nullptr) {
			initializeOctants();
		}

		if (!isVirtual) {
			isVirtual = true;
			getQuadrant(particle)->insert(particle);
		}

		getQuadrant(_particle)->insert(_particle);
	}

	Particle getParticle() {
		return isVirtual ? *particle : Particle(center, mass, 0.f);
	}

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
	}

	std::shared_ptr<OctreeNode> getQuadrant(Particle* _particle) {
		bool particleIsInsideBoundary =
		    domainFrom <= _particle->position && domainTo >= _particle->position;
		if(!particleIsInsideBoundary) {
			throw std::logic_error("particle outside of boundaries");
		}

		bool fromXIsNearer = (_particle->position.x - center.x) <= 0;

		bool fromYIsNearer = (_particle->position.y - center.y) <= 0;

		bool fromZIsNearer = (_particle->position.z - center.z) <= 0;

		if(fromXIsNearer) {
			if(fromYIsNearer) {
				return fromZIsNearer ? p000 : p001;
			} else {
				return fromZIsNearer ? p010 : p011;
			}
		} else {
			if(fromYIsNearer) {
				return fromZIsNearer ? p100 : p101;
			} else {
				return fromZIsNearer ? p110 : p111;
			}
		}
	}
};

class Octree {};

#endif // HPC22_OCTREE_H
