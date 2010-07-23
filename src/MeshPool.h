#pragma once
#include "Mesh.h"
#include <vector>
#include "types.h"
#include "polygonal_mesh.h"
#include <Utilities/MeshHelpers.h>
#include <boost/signal.hpp>

namespace SnowSim {

class MeshPool
	{
	private:
		/// vector collection of bodies
		std::vector<Mesh*> bodies;

		/// The boudning box of the currently inherent meshes
		BBox3f m_boundingbox;

		/// A method to check and possibily update the bounding
		/// box of the mesh pool.
		void checkBoundingBox();

	public:
		MeshPool(void);
		~MeshPool(void);

		boost::signal<void (MeshPool*)> Changed;

		bool Triangulate();

		bool isTriangulated();

		/// render meshes
		void render();

		/// add a new mesh to the mesh pool
		void addMesh(Mesh* newBody);

		/// get pointer to the mesh with given index from the body pool
		Mesh* getMesh(unsigned int bodyIndex);

		unsigned int getNrMeshs() const {return (unsigned int) bodies.size(); }

		/// remove the mesh with the given index and his hierarchy from the body pool
		void removeMesh(unsigned int bodyIndex);

		/// remove the given body from the mesh pool (if part of)
		//void removeMesh(Mesh* meshToDelete);

		// get the bounding box of all meshes
		BBox3f BoundingBox() {
			return m_boundingbox;
		}
	};
}
