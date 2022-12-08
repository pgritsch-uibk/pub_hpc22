#include "OctreeNode.h"

void OctreeNode::initialize(Vector3D _domainFrom, Vector3D _domainTo) {
	mass = 0;
	buffer = nullptr;
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

void OctreeNode::insert(Particle* particle, int depth) {

	bool particleIsInsideBoundary =
	    domainFrom <= particle->position && domainTo >= particle->position;
	if(!particleIsInsideBoundary) {
		throw std::logic_error("particle outside of boundaries");
	}

	if(!isVirtual && (buffer == nullptr || buffer->empty() || depth >= OCTREE_MAXDEPTH)) {
		if(buffer == nullptr) {
			buffer = bufferProvider->getNext();
			buffer->clear();
		}
		buffer->push_back(particle);
		return;
	}

	for(Particle* _particle : *buffer) {
		OctreeNode* quad = getQuadrant(_particle);
		quad->insert(_particle, depth + 1);
	}

	getQuadrant(particle)->insert(particle, depth + 1);

	buffer->clear();
	isVirtual = true;
}

OctreeNode* OctreeNode::getQuadrant(Particle* particle) {

	bool fromXIsNearer = (particle->position.x - center.x) <= 0;

	bool fromYIsNearer = (particle->position.y - center.y) <= 0;

	bool fromZIsNearer = (particle->position.z - center.z) <= 0;

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

void OctreeNode::computeMassDistributionAndTraverse(std::vector<Particle*>* depthFirstBuffer) {

	com = Vector3D::zeroVector();
	mass = 0.f;

	if(!isVirtual) {
		for(Particle* p : *buffer) {
			com += p->position * p->mass;
			mass += p->mass;
			if(depthFirstBuffer != nullptr) {
				depthFirstBuffer->push_back(p);
			}
		}
	}

	if(p000 != nullptr) {
		p000->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p000->com * p000->mass;
		mass += p000->mass;
	}
	if(p001 != nullptr) {
		p001->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p001->com * p001->mass;
		mass += p001->mass;
	}
	if(p010 != nullptr) {
		p010->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p010->com * p010->mass;
		mass += p010->mass;
	}
	if(p011 != nullptr) {
		p011->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p011->com * p011->mass;
		mass += p011->mass;
	}
	if(p100 != nullptr) {
		p100->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p100->com * p100->mass;
		mass += p100->mass;
	}
	if(p101 != nullptr) {
		p101->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p101->com * p101->mass;
		mass += p101->mass;
	}
	if(p110 != nullptr) {
		p110->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p110->com * p110->mass;
		mass += p110->mass;
	}
	if(p111 != nullptr) {
		p111->computeMassDistributionAndTraverse(depthFirstBuffer);
		com += p111->com * p111->mass;
		mass += p111->mass;
	}

	com /= mass;
}

float inv_sqrt(float number) {
	union {
		float f;
		uint32_t i;
	} conv = { .f = number };
	conv.i = 0x5f3759df - (conv.i >> 1);
	conv.f *= 1.5F - (number * 0.5F * conv.f * conv.f);
	return conv.f;
}

void OctreeNode::updateForce(Particle* particle) const {

	if(!isVirtual) {
		for(auto other : *buffer) {
			if(*other == *particle) {
				continue;
			}

			Vector3D direction = other->position.direction(particle->position);
			float distanceSquared = direction.lengthSquared();

			if(distanceSquared <=
			   (particle->radius * particle->radius + other->radius * other->radius)) {
				Vector3D velocityDirection = particle->velocity.direction(other->velocity);
				float velocity_relation = 2.f * other->mass / (particle->mass + other->mass);
				Vector3D velocityChange =
				    (velocityDirection * direction) / distanceSquared * direction;

				particle->velocity -= velocityChange * velocity_relation;
			} else {
				float force =
				    GRAVITATIONAL_CONSTANT * (particle->mass * other->mass) / distanceSquared;
				particle->force += direction * inv_sqrt(distanceSquared) * force;
			}
		}

		return;
	}

	Vector3D directionToCom = com.direction(particle->position);
	float lengthComSquared = directionToCom.lengthSquared();
	float boxLengthSquared = (domainTo.x - domainFrom.x) * (domainTo.x - domainFrom.x);

	if(boxLengthSquared < THETA_SQUARED * lengthComSquared) {
		float force = (particle->mass * mass) / lengthComSquared;
		particle->force += directionToCom * inv_sqrt(lengthComSquared) * force;
	} else {
		if(p000 != nullptr) {
			p000->updateForce(particle);
		}
		if(p001 != nullptr) {
			p001->updateForce(particle);
		}
		if(p010 != nullptr) {
			p010->updateForce(particle);
		}
		if(p011 != nullptr) {
			p011->updateForce(particle);
		}
		if(p100 != nullptr) {
			p100->updateForce(particle);
		}
		if(p101 != nullptr) {
			p101->updateForce(particle);
		}
		if(p110 != nullptr) {
			p110->updateForce(particle);
		}
		if(p111 != nullptr) {
			p111->updateForce(particle);
		}
	}
}
