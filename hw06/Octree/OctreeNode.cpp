#include "OctreeNode.h"

#define MAX_DEPTH 100

void OctreeNode::insert(Particle* _particle, int maxDepth, int depth) {
	mass += _particle->mass;
	children++;

	if(particle == nullptr) {
		particle = _particle;
		return;
	}

	if(depth >= MAX_DEPTH) {
		if(buffer == nullptr) {
			buffer = bufferProvider->getNext();
		}
		buffer->push_back(particle);
		buffer->push_back(_particle);
		return;
	}

	if(!isVirtual) {
		isVirtual = true;
		getQuadrant(particle)->insert(particle, maxDepth, depth + 1);
	}

	getQuadrant(_particle)->insert(_particle, maxDepth, depth + 1);
}

OctreeNode* OctreeNode::getQuadrant(Particle* _particle) {
	bool particleIsInsideBoundary =
	    domainFrom <= _particle->position && domainTo >= _particle->position;
	if(!particleIsInsideBoundary) {
		throw std::logic_error("particle outside of boundaries");
	}

	bool fromXIsNearer = (_particle->position.x - center.x) <= 0;

	bool fromYIsNearer = (_particle->position.y - center.y) <= 0;

	bool fromZIsNearer = (_particle->position.z - center.z) <= 0;

	OctreeNode* quadrant;

	if(fromXIsNearer && fromYIsNearer && fromZIsNearer) {
		if(p000 == nullptr) {
			p000 = newNodesProvider->getNext();
			p000->initialize(domainFrom, {
			                                 center.x,
			                                 center.y,
			                                 center.z,
			                             });
		}
		quadrant = p000;
	} else if(fromXIsNearer && fromYIsNearer) {
		if(p001 == nullptr) {
			p001 = newNodesProvider->getNext();
			p001->initialize({ domainFrom.x, domainFrom.y, center.z }, {
			                                                               center.x,
			                                                               center.y,
			                                                               domainTo.z,
			                                                           });
		}
		quadrant = p001;
	} else if(fromXIsNearer && fromZIsNearer) {
		if(p010 == nullptr) {
			p010 = newNodesProvider->getNext();
			p010->initialize({ domainFrom.x, center.y, domainFrom.z }, {
			                                                               center.x,
			                                                               domainTo.y,
			                                                               center.z,
			                                                           });
		}
		quadrant = p010;
	} else if(fromXIsNearer) {
		if(p011 == nullptr) {
			p011 = newNodesProvider->getNext();
			p011->initialize({ domainFrom.x, center.y, center.z }, {
			                                                           center.x,
			                                                           domainTo.y,
			                                                           domainTo.z,
			                                                       });
		}
		quadrant = p011;
	} else if(fromYIsNearer && fromZIsNearer) {
		if(p100 == nullptr) {
			p100 = newNodesProvider->getNext();
			p100->initialize({ center.x, domainFrom.y, domainFrom.z }, {
			                                                               domainTo.x,
			                                                               center.y,
			                                                               center.z,
			                                                           });
		}
		quadrant = p100;
	} else if(fromYIsNearer) {
		if(p101 == nullptr) {
			p101 = newNodesProvider->getNext();
			p101->initialize({ center.x, domainFrom.y, center.z }, {
			                                                           domainTo.x,
			                                                           center.y,
			                                                           domainTo.z,
			                                                       });
		}

		quadrant = p101;
	} else if(fromZIsNearer) {
		if(p110 == nullptr) {
			p110 = newNodesProvider->getNext();
			p110->initialize({ center.x, center.y, domainFrom.z }, {
			                                                           domainTo.x,
			                                                           domainTo.y,
			                                                           center.z,
			                                                       });
		}
		quadrant = p110;
	} else {
		if(p111 == nullptr) {
			p111 = newNodesProvider->getNext();
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