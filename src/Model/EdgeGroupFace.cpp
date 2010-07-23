#include <GL/glew.h>
#include "EdgeGroupFace.h"
#include <Exceptions/ArgumentException.h>
#include "EdgeGroup.h"

namespace SnowSim {
	namespace Model {

	EdgeGroupFace::EdgeGroupFace(EdgeGroup* eg, Mesh::face * face) {

		if (eg == NULL)
			throw Exceptions::ArgumentException("Edge group (eg) must not be null!");
		if (face == NULL)
			throw Exceptions::ArgumentException("The underlying face must not be null!");

		nVertices = face->valence;

		if (nVertices < 3)
			throw Exceptions::ArgumentException("face must have at least 3 vertices.");

		m_edgeGroup = eg;
		original_face = face;
		nLaunchSites = 0;
	};

	Mesh::face * EdgeGroupFace::GetOriginalFace()
	{
		return original_face;
	}

	EdgeGroup* const EdgeGroupFace::GetEdgeGroup() const
	{
		return m_edgeGroup;
	}

	void EdgeGroupFace::PropagateNewHeight(Model::ILaunchSite<PrecisionType>* ls)
	{
		m_edgeGroup->CheckBoundingBox(ls);
	}

	// get the bounding box of all meshes
	BBox3f EdgeGroupFace::GetBoundingBox()
	{
		// we have at least 3 vertices,
		// so is is ok to assume vertices[0] exists
		point3f min = vertices[0]->position;
		point3f max = min;

		for (int i = 1; i < nVertices; ++i) {
			point3f pos = vertices[i]->position;

			if ( pos.x() < min.x() ) min.x() = pos.x();
			if ( pos.x() > max.x() ) max.x() = pos.x();

			if ( pos.y() < min.y() ) min.y() = pos.y();
			if ( pos.y() > max.y() ) max.y() = pos.y();

			if ( pos.z() < min.z() ) min.z() = pos.z();
			if ( pos.z() > max.z() ) max.z() = pos.z();
		}

		return BBox3f(min, max);
	}
}
}
