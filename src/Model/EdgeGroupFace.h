#pragma once

#include "Mesh.h"

namespace SnowSim {
	namespace Model {

	template<class T> class ILaunchSite;

	class EdgeGroup;

	/// edge group face - a face which is part of an edge group
	class EdgeGroupFace
	{
		public:
			typedef std::vector<Mesh::vertex*>::iterator	Vertex_iterator;

		private:
			// The edge group we are belonging to
			EdgeGroup* m_edgeGroup;

		public:
			EdgeGroupFace(EdgeGroup* eg, Mesh::face * face);

			int nVertices; // amount of vertices of this face
			int nNeighbors; // amount of neighbours
			int nLaunchSites; // amount of launchsites
			Mesh::face * original_face;
			float area;
			float slope_angle;
			point3f normal;
			std::vector<EdgeGroupFace*> neighbors; // pointers to the neigbors
			std::vector<Mesh::vertex*> vertices; // pointers to the vertices
			std::vector<Mesh::halfedge*> halfedges; // pointers to the halfedges surrounding the face

			Mesh::face * GetOriginalFace();

			// get the bounding box of all meshes
			BBox3f GetBoundingBox();

			// Gets the edge group the edge group face is belonging to
			EdgeGroup* const GetEdgeGroup() const;

			// Propagates a new height of launch site to the edge group
			void PropagateNewHeight(Model::ILaunchSite<PrecisionType>* ls);

			/// Gets the vertex iterator at the beginning
			Vertex_iterator VertexBegin() { return vertices.begin(); }
			/// Gets the past the end vertex iterator
			Vertex_iterator VertexEnd() { return vertices.end(); }
	};
}
}
