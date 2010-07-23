#pragma once

#include "polygonal_mesh.h"
#include <cgv/utils/obj_reader.h>
#include <Utilities/Intersection/Intersection.h>
#include <map>
#include <Exceptions/MeshLoaderException.h>

/**
* Wrapper from obj_reader to polygonal_mesh, i.e. from obj to halfedge format.
*/

using namespace SnowSim::Utilities::Intersect;

namespace SnowSim {
class polygonal_mesh_obj_reader : public cgv::utils::obj_reader {
protected:
	/// halfedge
	typedef polygonal_mesh::halfedge halfedge;
	/// edge
	typedef polygonal_mesh::edge edge;
	/// vertex
	typedef polygonal_mesh::vertex vertex;
	/// face
	typedef polygonal_mesh::face face;
	///mesh pointer
	polygonal_mesh *mesh;
	/// vertex pointers
	std::vector<vertex*> _vertices;

	std::vector<point3f*> _normals;

public:
	///contructor
	polygonal_mesh_obj_reader(const std::string& filename)
		:	cgv::utils::obj_reader(filename),
			mesh(NULL)
	{
	}
	/// destructor
	~polygonal_mesh_obj_reader() {
	}
	///load
	polygonal_mesh* load(bool force_new_normals = false) {
		std::cout << "[Obj-Reader] loading model " << this->filename << "...";
		mesh = new polygonal_mesh;
		if (open()) {
			read();
			std::cout << "ready" << std::endl << "\tcomputing half edge connectivity...";
			find_inverses();
			find_borders();
			find_edges();

			mesh->vertex_end = _vertices[mesh->number_vertices-1];
			// TODO: Check if there are already normals
			std::cout << "ready" << std::endl << "\tcomputing face normals...";
			mesh->compute_face_normals();

			if (_normals.size() == 0 || force_new_normals) {
				std::cout << "ready" << std::endl << "\tcomputing vertex normals...";
				mesh->compute_vertex_normals();
			}

			std::cout << "ready" << std::endl << "\tcomputing bounding box...";
			compute_boundingbox();
			std::cout << "ready" << std::endl << "\tcomputing projected face areas...";
			compute_areas();
			std::cout << "ready" << std::endl << "\tcomputing slope angles...";
			compute_slope_angles();
			//std::cout << "ready" << std::endl << "\tcomputing insides/outsides (TODO)...";
			//compute_ifOutside();
			std::cout << "ready" << std::endl;

			read_marks();
			return mesh;
		} else {
			delete mesh;
			return NULL;
		}
	}
	/// read which vertices are marked
	bool read_marks() {
		std::stringstream s;
		s << filename << ".mrk";
		std::ifstream markFile(s.str().c_str());
		if (markFile.fail()) {
			return false;
		}
		std::cout << "ready" << std::endl << "\reading .mrk file...";
		vertex *v = mesh->vertex_source;
		int vi;
		do {
			markFile >> vi;
			if (markFile.fail()) {
				return false;
			}
			if (vi != -1) {
				while ((v != NULL) && (v->index != vi)) {
					v = v->next_vertex;
				}
				v->marked = true;
			}
		} while (vi != -1);
		while (!markFile.eof()) {
			int vi, vj;
			markFile >> vi >> vj;
			if (markFile.fail()) {
				return false;
			}
			v = mesh->vertex_source;
			while ((v != NULL) && (v->index != vi)) {
				v = v->next_vertex;
			}
			halfedge *h = v->m_halfedge;
			halfedge *hi = h;
			do {
				if (hi->inverse->origin->index == vj) {
					hi->m_edge->marked = true;
					break;
				}
				hi = hi->inverse->next;
			} while (hi != h);
		}
		std::cout << "ready" << std::endl;
		return true;
	}

	void compute_boundingbox() {
		vertex *v = mesh->vertex_source;
		point3f min = mesh->vertex_source->position;
		point3f max = mesh->vertex_source->position;

		v = v->next_vertex;
		while (v!=NULL) {
			point3f pos = v->position;

			if ( pos.x() < min.x() ) min.x() = pos.x();
			if ( pos.y() < min.y() ) min.y() = pos.y();
			if ( pos.z() < min.z() ) min.z() = pos.z();
			if ( pos.x() > max.x() ) max.x() = pos.x();
			if ( pos.y() > max.y() ) max.y() = pos.y();
			if ( pos.z() > max.z() ) max.z() = pos.z();

			v = v->next_vertex;
		}
		mesh->m_bounding_box = BBox3f(min, max);
	}

	void compute_slope_angles() {
		face *f = mesh->face_source;
		while (f!=NULL) {
			f->slope_angle = f->compute_slope();
			f=f->next_face;
		}
	}

	void compute_ifOutside() {
		face *f = mesh->face_source;
		while (f!=NULL) {
			face *f2 = mesh->face_source;
			point3f &normal = f->normal;

			while ( f2 != NULL) {

				// hits "normal"-ray (originating at the half edges origin vertex) the face?
				ray<float, 3> _ray = ray<float, 3>(f->m_halfedge->origin->position, normal);
				if ( intersect(f2, _ray) ) {
					// calculate angle between both normals
					// (do not divide with area, normals are normalized)
					double angle = asin(dot(normal, f2->normal));

					// hit we a face which "looks" in our direction
					// (or at least not away)
					if (angle > PI * 0.5) {
						// yes
					} else {
						// no

					}

				}

				f2 = f2->next_face;
			}

			f = f->next_face;
		}
	}

	void compute_areas()  {
		//mesh->compute_face_areas();
		mesh->compute_projected_face_areas();
	}

	/// find inverses
	/// Requires: that all halfedges are loaded
	void find_inverses() {
		typedef std::vector<halfedge*> haldedge_ids;

		// make vector (of size equally to the amount of vertices)
		// of pointers to vectors (of pointers to halfedges)
		std::vector<haldedge_ids*> orig_to_halfedge;
		for (int i = 0; i < mesh->number_vertices; i++) {
			orig_to_halfedge.push_back(new haldedge_ids);
		}

		// Go through all halfedges of the mesh
		// and save a pointer to them in the
		// to the index corresponding inner vector.
		// After this is done we many vectors of halfedges,
		// organized (and indexed by) their origin
		// Now the question "Which halfedges start at origin
		// with a specific index?" can be answered.
		halfedge *h = mesh->halfedge_source;
		while (h!=NULL) {
			orig_to_halfedge[h->origin->index]->push_back(h);
			h=h->next_halfedge;
		}

		// go again through all halfedges
		h = mesh->halfedge_source;
		while (h!=NULL) {
			// if the halfedge has no inverse...
			if (h->inverse == NULL) {
				// get start and end point of the halfedge
				vertex *e_orig = h->origin;
				vertex *e_next_orig = h->next->origin;

				// go through all the halfedges which starting points
				// are equal to the end point of the halfedge we are
				// searching an inverse for
				haldedge_ids* candidates = orig_to_halfedge[e_next_orig->index];
				for(unsigned j = 0; j < candidates->size(); ++j){

					// get the halfedge successing the candidate around a face
					halfedge *ej = (*candidates)[j]->next;

					// if the origin of the next halfedge is the same
					// as the one of the halfedge we are searching an inverse for...
					if (ej->origin == e_orig) {
						// setup the inverse relation
						(*candidates)[j]->inverse = h;
						h->inverse = (*candidates)[j];
						break;
					}
				}
			}
			h=h->next_halfedge;
		}
	}

	/// find edges
	void find_edges() {
		halfedge* h = mesh->halfedge_source;
		do {
			if ((h->m_edge == NULL) && (h->inverse->m_edge == NULL)) {
				edge* e = new edge;
				e->m_halfedge = h;
				e->next_edge = NULL;
				e->border = h->border || h->inverse->border;
				e->index = mesh->number_edges;
				e->marked = false;
				mesh->number_edges++;
				h->m_edge = e;
				h->inverse->m_edge = e;
				if (mesh->edge_source == NULL) {
					mesh->edge_source = e;
					e->previous_edge = NULL;
				} else {
					mesh->edge_end->next_edge = e;
					e->previous_edge = mesh->edge_end;
				}
				mesh->edge_end = e;
				h->origin->valence++;
				h->inverse->origin->valence++;
				mesh->avg_edge_length += length(h->origin->position - h->inverse->origin->position);
			}
			h=h->next_halfedge;
		} while (h!=NULL);
		mesh->avg_edge_length /= mesh->number_edges;
	}

	/// find borders
	/// Requires: Inverses to have been found and asigned
	void find_borders() {
		std::vector<halfedge*> border_halfedges;
		halfedge* h = mesh->halfedge_source;
		do {
			if (h->inverse == NULL) {
				halfedge* h_new = new halfedge;
				h_new->border = true;
				h_new->m_face = NULL;
				h_new->inverse = h;
				h_new->next_halfedge = NULL;
				h_new->previous_halfedge = mesh->halfedge_end;
				h_new->origin = h->next->origin;
				h_new->m_edge = NULL;
				h_new->index = mesh->number_halfedges;
				h_new->prev = NULL;
				h_new->next = NULL;
				mesh->number_halfedges++;
				h->inverse = h_new;
				h_new->origin->border = true;
				mesh->halfedge_end->next_halfedge = h_new;
				mesh->halfedge_end = h_new;
				border_halfedges.push_back(h_new);
			}
			h=h->next_halfedge;
		} while (h!=NULL);
		for (unsigned i1 = 0; i1 < border_halfedges.size(); ++i1) {

			std::vector<halfedge*> candidates;

			for (unsigned i2 = 0; i2 < border_halfedges.size(); ++i2) {
				if (border_halfedges[i2]->origin == border_halfedges[i1]->inverse->origin) {
					candidates.push_back(border_halfedges[i2]);
				}
			}

			if (candidates.size() == 1) {
				border_halfedges[i1]->next = candidates.front();
				candidates[0]->prev = border_halfedges[i1];
			} else if (candidates.size() > 1) {
				std::vector<halfedge*>::iterator it_3
					= candidates.begin();
				while ( it_3 != candidates.end() )
				{
					halfedge* pot_he = *it_3;

					if ( pot_he->prev == NULL )
					{
						border_halfedges[i1]->next = pot_he;
						pot_he->prev = border_halfedges[i1];
						break;
					}

					++it_3;
				}
			} else {
				throw Exceptions::MeshLoaderException("Could not connect all border halfedges.");
			}
		}
	}

	void process_normal(int index, float x, float y, float z)
	{
		_normals.push_back( new point3f(x, y, z) );
	}

	/// process vertex
	void process_vertex(int index, float x, float y, float z) {
		vertex* v = new vertex;
		v->m_halfedge = NULL;
		v->next_vertex = NULL;
		v->border = false;
		v->index = mesh->number_vertices;
		v->marked = false;
		v->level = 0;
		v->valence = 0;
		mesh->number_vertices++;
		v->position = point3f(x,y,z);
		if (mesh->vertex_source == NULL) {
			mesh->vertex_source = v;
			v->previous_vertex = NULL;
		} else {
			_vertices[index-2]->next_vertex = v;
			v->previous_vertex = _vertices[index-2];
		}
		_vertices.push_back(v);
	}
	/// process face
	void process_face(int index, int vcount, int *vertices, int *texcoords=NULL, int *normals=NULL) {
		face* f = new face;
		f->next_face = NULL;
		f->previous_face = mesh->face_end;
		f->index = mesh->number_faces;
		f->normal = point3f(0.0f,0.0f,0.0f);
		f->valence = vcount;
		mesh->number_faces++;
		std::vector<halfedge*> _halfedges(vcount);
		for(int i = 0; i < vcount; ++i) {
			halfedge* h = new halfedge;
			h->border = false;
			h->m_face = f;
			h->inverse = NULL;
			h->next_halfedge = NULL;
			h->previous_halfedge = mesh->halfedge_end;
			h->m_edge = NULL;
			h->origin = _vertices[vertices[i]-1]; // we have to substract 1 since OBJ-files indices are 1 based and arrays are 0 based

			if (normals != NULL) {
				// PERFORMANCE: Normailzation should be a second step after mesh init
				h->origin->normal = normalize(  ( h->origin->normal + *( _normals[normals[i]-1] )));
			}

			h->origin->m_halfedge = h;
			h->index = mesh->number_halfedges;
			++mesh->number_halfedges;
			if (mesh->halfedge_source == NULL) {
				mesh->halfedge_source = h;
			} else {
				mesh->halfedge_end->next_halfedge = h;
			}
			mesh->halfedge_end = h;
			_halfedges[i] = h;
		}
		f->m_halfedge = _halfedges[0];
		for(int i = 0; i < vcount-1; i++) {
			_halfedges[i]->next = _halfedges[i+1];
		}
		_halfedges[vcount-1]->next = _halfedges[0];
		for (int i = 1; i < vcount; i++) {
			_halfedges[i]->prev = _halfedges[i-1];
		}
		_halfedges[0]->prev = _halfedges[vcount-1];
		if (mesh->face_source == NULL) {
			mesh->face_source = f;
		} else {
			mesh->face_end->next_face = f;
		}
		mesh->face_end = f;
	}
};
}
