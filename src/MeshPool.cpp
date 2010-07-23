#include "MeshPool.h"
#include "Exceptions/ArgumentException.h"

namespace SnowSim {

	MeshPool::MeshPool(void)
		{
		}

	MeshPool::~MeshPool(void)
		{
		}

	void MeshPool::render() {
		// render bodies
		std::vector<Mesh*>::iterator bIt;
		for( bIt = bodies.begin(); bIt != bodies.end(); bIt++ ) {
			(*bIt)->render();
		}
	}

	// true if all meshes are triangulated
	// false othewrise
	bool MeshPool::isTriangulated() {
		// check every mesh
		std::vector<Mesh*>::iterator bIt;
		bool isTriangulated = true;

		for( bIt = bodies.begin(); bIt != bodies.end(); bIt++ ) {
			isTriangulated == isTriangulated && (*bIt)->IsTriangulated();
		}

		return isTriangulated;
	}

	void MeshPool::addMesh(Mesh *newBody) {

		bodies.push_back(newBody);
		checkBoundingBox();
		
		//if (_autoSeparate) SeparateObject(bodies.size() - 1);

		Changed(this);
	}

	Mesh* MeshPool::getMesh(unsigned int bodyIndex) {
		if (bodyIndex > bodies.size())
			throw new SnowSim::Exceptions::ArgumentException("bodyIndex must be in correct range");

		return bodies.at(bodyIndex);
	}

	void MeshPool::removeMesh(unsigned int bodyIndex) {
		if (bodyIndex > bodies.size())
			throw new SnowSim::Exceptions::ArgumentException("bodyIndex must be in correct range");

		bodies.erase(bodies.begin() + bodyIndex, bodies.begin() + bodyIndex + 1);

		checkBoundingBox();

		Changed(this);
	}

	void MeshPool::checkBoundingBox() {

		float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
		float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;

		if (bodies.size() > 0) {
			std::vector<Mesh*>::iterator it = bodies.begin();

			if (it == bodies.end())
				return;

			BBox3f bb = (*it)->BoundingBox();
			point3f minBB = bb.min;
			minX = minBB.x(), minY = minBB.y(), minZ = minBB.z();
			point3f maxBB = bb.max;
			maxX = maxBB.x(), maxY = maxBB.y(), maxZ = maxBB.z();
			
			++it;

			while (it!=bodies.end()) {
				bb = (*it)->BoundingBox();
				minBB = bb.min;
				maxBB = bb.max;;

				if ( minBB.x() < minX ) minX = minBB.x();
				if ( minBB.y() < minY ) minY = minBB.y();
				if ( minBB.z() < minZ ) minZ = minBB.z();
				if ( maxBB.x() > maxX ) maxX = maxBB.x();
				if ( maxBB.y() > maxY ) maxY = maxBB.y();
				if ( maxBB.z() > maxZ ) maxZ = maxBB.z();

				++it;
			}
		}

		m_boundingbox = BBox3f(point3f(minX, minY, minZ), point3f(maxX, maxY, maxZ));
	}

}