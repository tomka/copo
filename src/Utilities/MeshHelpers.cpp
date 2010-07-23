#include "MeshHelpers.h"

bool shareOneSide(SnowSim::polygonal_mesh::face* face1, SnowSim::polygonal_mesh::face* face2) {
	if (face1 == NULL || face2 == NULL) return false;
	
	SnowSim::polygonal_mesh::halfedge* he_face1 = face1->m_halfedge;
	SnowSim::polygonal_mesh::halfedge* he_face2 = face2->m_halfedge;
	
	do {
		do {
			if (he_face1 == he_face2->inverse)
				return true;
		
			he_face2 = he_face2->next;
		} while (he_face2 != face2->m_halfedge);

		he_face1 = he_face1->next;
	} while (he_face1 != face1->m_halfedge);

	return false;
}