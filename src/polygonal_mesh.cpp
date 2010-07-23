#include "polygonal_mesh.h"

namespace SnowSim {

	#pragma region Halfedge

	point3f polygonal_mesh::halfedge::compute_center() {
			return 0.5f * (origin->position + inverse->origin->position);
		}
		/// compures and returns halfedge length
		float  polygonal_mesh::halfedge::compute_length() {
			return length(origin->position - inverse->origin->position);
		}
		/// check whether vertex origin is convex
		bool  polygonal_mesh::halfedge::compute_convex() {
			point3f p0 = prev->origin->position;
			point3f p1 = origin->position;
			point3f p2 = next->origin->position;
			point3f n = cross(normalize(p1-p0), normalize(p2-p1));
			return dot(n, this->m_face->normal) > 0.0f;
		}

	#pragma endregion

	#pragma region Edge

		/// computes and returns the center of the edge
		point3f polygonal_mesh::edge::compute_center() {
			return this->m_halfedge->compute_center();
		}
		/// computes and returns edge length
		float polygonal_mesh::edge::compute_length() {
			return this->m_halfedge->compute_length();
		}

	#pragma endregion

	#pragma region Vertex

		/// check whether vertex is convex
		bool polygonal_mesh::vertex::compute_convex() {
			return this->m_halfedge->compute_convex();
		}

		/// computes and returns the unit normal of the face
		point3f polygonal_mesh::vertex::compute_normal()
		{
			point3f n = point3f(0.0f, 0.0f, 0.0f);
			polygonal_mesh::halfedge *hi = this->m_halfedge;
			do {
				n += cross(	hi->next->origin->position - hi->origin->position,
							hi->prev->origin->position - hi->origin->position);

				if ( hi->inverse->border == true ) {
					// the halfedge is a border edge
					do {
						hi = hi->prev->inverse;
					} while ( !(hi->prev->inverse->border == true ) );
				} else {
					hi = hi->inverse->next;
				}
			} while ( hi != this->m_halfedge );

			return normalize(n);
		}

	#pragma endregion

	#pragma region Face

		/// computes and returns the unit normal of the face
		point3f polygonal_mesh::face::compute_normal() {
			point3f n = point3f(0.0f,0.0f,0.0f);
			polygonal_mesh::halfedge *hi = this->m_halfedge;
			do {
				n += cross(hi->next->origin->position - hi->origin->position,
						hi->prev->origin->position - hi->origin->position);
				hi=hi->next;
			} while (hi!=this->m_halfedge);
			return normalize(n);
		}

		/// computes and returns the center of the face
		point3f polygonal_mesh::face::compute_center() {
			point3f c = point3f(0.0f,0.0f,0.0f);
			polygonal_mesh::halfedge *hi = this->m_halfedge;
			int i=0;
			do {
				i++;
				c += hi->origin->position;
				hi=hi->next;
			} while (hi!=this->m_halfedge);
			return (1.0f / i) * c;
		}
		/// find ear
		polygonal_mesh::halfedge* polygonal_mesh::face::find_ear() {
			polygonal_mesh::halfedge *hi = this->m_halfedge;
			do {
				if (hi->next->compute_convex()) {
					polygonal_mesh::halfedge *hl = hi->next->next->next;
					bool ear = true;
					while(hl != hi)	{
						if (point_in_triangle(hi->origin->position,
								hi->next->origin->position,
								hi->next->next->origin->position,
								hl->origin->position)) {
							ear = false;
							break;
						}
						hl = hl->next;
					}
					if (ear) {
						return hi;
					}
				}
				hi = hi->next;
			} while (hi != this->m_halfedge);
			return NULL;
		}

		//computes the XZ projected area
		float polygonal_mesh::face::compute_projected_area()
		{
			polygonal_mesh::halfedge *hi = this->m_halfedge;

			// project vertices to the XZ-plane
			point3f v1(hi->next->origin->position - hi->origin->position);
			point3f v2(hi->next->next->origin->position - hi->origin->position);
			v1.y() = 0.0f;
			v2.y() = 0.0f;

			return 0.5f * (length<float, 3>(cross<float>(v1,v2)));

		}

		/// compute area
		float polygonal_mesh::face::compute_area() {

			polygonal_mesh::halfedge *hi = this->m_halfedge;

			/// Method 1

			return 0.5f * (length<float, 3>(cross<float>(
					hi->next->origin->position - hi->origin->position,
					hi->next->next->origin->position - hi->origin->position)));

			// Method 2

			return
				abs(
					(hi->next->origin->position.x() - hi->origin->position.x())
						*(hi->next->next->origin->position.y() - hi->origin->position.x())
					- (hi->next->origin->position.y() - hi->origin->position.y())
						*(hi->next->next->origin->position.x() - hi->origin->position.x())
				) * 0.5f;


			/// Method 3


			float area = 0;
			float an, ax, ay, az;  // abs value of normal and its coords
			int   coord;           // coord to ignore: 1=x, 2=y, 3=z

			// select largest abs coordinate to ignore for projection
			ax = abs(normal.x());     // abs x-coord
			ay = abs(normal.y());     // abs y-coord
			az = abs(normal.z());     // abs z-coord

			coord = 3;                     // ignore z-coord
			if (ax > ay) {
				if (ax > az) coord = 1;    // ignore x-coord
			}
			else if (ay > az) coord = 2;   // ignore y-coord

			// compute area of the 2D projection
			//polygonal_mesh::halfedge *hi = halfedge;
			do {
				switch (coord) {
					case 1:
						area += (hi->next->origin->position.y()
									* (hi->next->next->origin->position.z()
										- hi->origin->position.z()));
						continue;
					case 2:
						area += (hi->next->origin->position.x()
									* (hi->next->next->origin->position.z()
										- hi->origin->position.z()));
						continue;
					case 3:
						area += (hi->next->origin->position.x()
									* (hi->next->next->origin->position.y()
										- hi->origin->position.y()));
						continue;
				}

				hi = hi->next;
			} while (hi != this->m_halfedge);

			// scale to get area before projection
			an = sqrt( ax*ax + ay*ay + az*az);  // length of normal vector
			switch (coord) {
				case 1:
					area *= (an / (2*ax));
					break;
				case 2:
					area *= (an / (2*ay));
					break;
				case 3:
					area *= (an / (2*az));
			}

			return area;
		}

		float polygonal_mesh::face::compute_slope() {
			float slope = dot(normal, ground_normal) / (length(normal) * length(ground_normal));
			slope = acosf(slope);

			if ( slope < 0.0 )
			{
				slope = -slope;
			}

			return slope;
		}

	#pragma endregion

	/// splits edge with halfedge h at the point p. returns the new halfedge, with the new vertex as origin and h as prev halfedge.
	polygonal_mesh::halfedge*
	polygonal_mesh::split_edge(polygonal_mesh::halfedge* h, point3f& p)
	{
		if (h==NULL) {
			return NULL;
		}
		//new things
		halfedge* new_h = new halfedge;
		halfedge* new_h_inv = new halfedge;
		edge* new_e = new edge;
		vertex* new_v = new vertex;
		//new halfedge
		new_h->border = h->border;
		new_h->m_edge = new_e;
		new_h->m_face = h->m_face;
		new_h->inverse = new_h_inv;
		new_h->next = h->next;
		new_h->origin = new_v;
		new_h->prev = h;
		//new halfedge
		new_h_inv->border = h->inverse->border;
		new_h_inv->m_edge = new_e;
		new_h_inv->m_face = h->inverse->m_face;
		new_h_inv->inverse = new_h;
		new_h_inv->next = h->inverse;
		new_h_inv->origin = h->inverse->origin;
		new_h_inv->prev = h->inverse->prev;
		//new edge
		new_e->border = h->m_edge->border;
		new_e->m_halfedge = new_h;
		new_e->marked = h->m_edge->marked;
		//new vertex
		new_v->border = h->m_edge->border;
		new_v->m_halfedge = new_h;
		new_v->marked = h->m_edge->marked;
		new_v->position = p;
		new_v->valence = 2;
		new_v->level = 0;
		// approx. the new normal between prev. and next normal
		new_v->normal = normalize(h->origin->normal + new_h->next->origin->normal);
		//update dependents
		h->inverse->origin->m_halfedge = new_h_inv;
		h->next = new_h;
		h->inverse->origin = new_v;
		h->inverse->prev = new_h_inv;
		new_h->next->prev = new_h;
		new_h_inv->prev->next = new_h_inv;
		if (new_h->m_face != NULL) {
			new_h->m_face->normal = new_h->m_face->compute_normal();
			new_h->m_face->valence += 1;
		}
		if (new_h_inv->m_face != NULL) {
			new_h_inv->m_face->normal = new_h_inv->m_face->compute_normal();
			new_h_inv->m_face->valence += 1;
		}
		avg_edge_length = (avg_edge_length * number_edges) / (number_edges + 1);
		//insert new things
		insert_halfedge_in_list(new_h);
		insert_halfedge_in_list(new_h_inv);
		insert_edge_in_list(new_e);
		insert_vertex_in_list(new_v);

		isTriangulated = false;

		//return new half edge
		return new_h;
	}

	/// inserts an edge that connects the vertices v1 and v2 and returns the new edge. v1 and v2 have to share the same face. it is not allowed that v1 and v2 already share an edge.
	polygonal_mesh::edge*
	polygonal_mesh::insert_edge(vertex *v1, vertex *v2) {
		if ((v1==NULL) || (v2==NULL)) {
			return NULL;
		}
		halfedge *h1 = v1->m_halfedge;
		halfedge *hi1 = h1;
		do {
			halfedge *h2 = v2->m_halfedge;
			halfedge *hi2 = h2;
			do {
				if (hi1->m_face == hi2->m_face) {
					halfedge *h_new = insert_edge(hi1, hi2);
					if (h_new) {
						isTriangulated = false;
						return h_new->m_edge;
					} else {
						return NULL;
					}
				}
				hi2 = hi2->prev->inverse;
			} while (hi2 != h2);
			hi1 = hi1->prev->inverse;
		} while (hi1 != h1);
		return NULL;
	}

	/// splits edge e at the point p. returns the new edge.
	polygonal_mesh::edge*
	polygonal_mesh::split_edge(edge* e, point3f& p) {
		if (e==NULL) {
			return NULL;
		}
		return split_edge(e->m_halfedge,p)->m_edge;
	}

	/// splits edge e at edge center. returns the new edge.
	polygonal_mesh::edge*
	polygonal_mesh::split_edge(edge* e) {
		if (e==NULL) {
			return NULL;
		}
		point3f center = e->m_halfedge->compute_center();
		return split_edge(e->m_halfedge, center)->m_edge;
		//return split_edge(e->m_halfedge,e->compute_center())->m_edge;
	}

	/// splits edge with halfedge h at halfedge center. returns the new halfedge, with the new vertex as origin and h as prev halfedge.
	polygonal_mesh::halfedge*
	polygonal_mesh::split_edge(halfedge* h) {
		if (h==NULL) {
			return NULL;
		}
		point3f center = h->compute_center();
		return split_edge(h, center);
	}

	/// copies a mesh
	void
	polygonal_mesh::copy_from(polygonal_mesh* mesh)  {

		// delete old target mesh content
		while (halfedge_end!=NULL) {
			halfedge *h = halfedge_end->previous_halfedge;
			delete halfedge_end;
			halfedge_end = h;
		}
		while (edge_end!=NULL) {
			edge *e = edge_end->previous_edge;
			delete edge_end;
			edge_end = e;
		}
		while (face_end!=NULL) {
			face *f = face_end->previous_face;
			delete face_end;
			face_end = f;
		}
		while (vertex_end!=NULL) {
			vertex *v = vertex_end->previous_vertex;
			delete vertex_end;
			vertex_end = v;
		}

		// get the size of each list
		number_halfedges	= mesh->number_halfedges;
		number_edges		= mesh->number_edges;
		number_faces		= mesh->number_faces;
		number_vertices		= mesh->number_vertices;

		// copy source mesh into a buffer
		std::vector<halfedge*>	halfedges(number_halfedges);
		std::vector<edge*>		edges(number_edges);
		std::vector<face*>		faces(number_faces);
		std::vector<vertex*>	vertices(number_vertices);

		halfedge *h = mesh->halfedge_source;
		while (h!=NULL) {
			halfedges[h->index] = h;
			h=h->next_halfedge;
		};
		edge *e = mesh->edge_source;
		while (e!=NULL) {
			edges[e->index] = e;
			e=e->next_edge;
		};
		face *f = mesh->face_source;
		while (f!=NULL) {
			faces[f->index] = f;
			f=f->next_face;
		};
		vertex *v = mesh->vertex_source;
		while (v!=NULL) {
			vertices[v->index] = v;
			v=v->next_vertex;
		};

		// create new vectors for the target
		std::vector<halfedge*>	new_halfedges(number_halfedges);
		std::vector<edge*>		new_edges(number_edges);
		std::vector<face*>		new_faces(number_faces);
		std::vector<vertex*>	new_vertices(number_vertices);

		for (int i = 0; i < number_halfedges; ++i) {
			new_halfedges[i] = new halfedge;
		}
		for (int i = 0; i < number_edges; ++i) {
			new_edges[i] = new edge;
		}
		for (int i = 0; i < number_faces; ++i) {
			new_faces[i] = new face;
		}
		for (int i = 0; i < number_vertices; ++i) {
			new_vertices[i] = new vertex;
		}

		// prepare the half edge data structure
		for (int i = 0; i < number_halfedges; i++) {
			new_halfedges[i]->origin = new_vertices[halfedges[i]->origin->index];
			new_halfedges[i]->inverse = new_halfedges[halfedges[i]->inverse->index];
			if (halfedges[i]->m_face == NULL) {
				new_halfedges[i]->m_face = NULL;
			} else {
				new_halfedges[i]->m_face = new_faces[halfedges[i]->m_face->index];
			}
			new_halfedges[i]->next = new_halfedges[halfedges[i]->next->index];
			new_halfedges[i]->prev = new_halfedges[halfedges[i]->prev->index];
			new_halfedges[i]->border = halfedges[i]->border;
			new_halfedges[i]->m_edge = new_edges[halfedges[i]->m_edge->index];
			new_halfedges[i]->index = halfedges[i]->index;
			if (i==0) {
				halfedge_source = new_halfedges[i];
				new_halfedges[i]->previous_halfedge = NULL;
			}
			if ((i>=0)&&(i<number_halfedges-1)) {
				new_halfedges[i]->next_halfedge = new_halfedges[i+1];
			}
			if (i==number_halfedges-1) {
				new_halfedges[i]->next_halfedge = NULL;
				halfedge_end = new_halfedges[i];
			}
			if ((i>=1)&&(i<number_halfedges)) {
				new_halfedges[i]->previous_halfedge = new_halfedges[i-1];
			}
		}
		for (int i = 0; i < number_edges; i++) {
			new_edges[i]->m_halfedge = new_halfedges[edges[i]->m_halfedge->index];
			new_edges[i]->border = edges[i]->border;
			new_edges[i]->index = edges[i]->index;
			new_edges[i]->marked = edges[i]->marked;
			if (i==0) {
				edge_source = new_edges[0];
				new_edges[i]->previous_edge = NULL;
			}
			if ((i>=0)&&(i<number_edges-1)) {
				new_edges[i]->next_edge = new_edges[i+1];
			}
			if (i==number_edges-1) {
				new_edges[i]->next_edge = NULL;
				edge_end = new_edges[i];
			}
			if ((i>=1)&&(i<number_edges)) {
				new_edges[i]->previous_edge = new_edges[i-1];
			}
		}

		// copy vertices
		for (int i = 0; i < number_vertices; i ++) {
			new_vertices[i]->position = vertices[i]->position;
			new_vertices[i]->m_halfedge = new_halfedges[vertices[i]->m_halfedge->index];
			new_vertices[i]->border = vertices[i]->border;
			new_vertices[i]->index = vertices[i]->index;
			new_vertices[i]->marked = vertices[i]->marked;
			new_vertices[i]->level = vertices[i]->level;
			new_vertices[i]->valence = vertices[i]->valence;
			new_vertices[i]->normal = vertices[i]->normal;
			if (i==0) {
				vertex_source = new_vertices[0];
				new_vertices[i]->previous_vertex = NULL;
			}
			if ((i>=0)&&(i<number_vertices-1)) {
				new_vertices[i]->next_vertex = new_vertices[i+1];
			}
			if (i==number_vertices-1) {
				new_vertices[i]->next_vertex = NULL;
				vertex_end = new_vertices[i];
			}
			if ((i>=1)&&(i<number_vertices)) {
				new_vertices[i]->previous_vertex = new_vertices[i-1];
			}
		}
		for (int i = 0; i < number_faces; i ++) {
			new_faces[i]->m_halfedge = new_halfedges[faces[i]->m_halfedge->index];
			new_faces[i]->index = faces[i]->index;
			new_faces[i]->normal = faces[i]->normal;
			new_faces[i]->valence = faces[i]->valence;
			new_faces[i]->area = faces[i]->area;
			new_faces[i]->slope_angle = faces[i]->slope_angle;
			new_faces[i]->material = faces[i]->material;
			if (i==0) {
				face_source = new_faces[0];
				new_faces[i]->previous_face = NULL;
			}
			if ((i>=0)&&(i<number_faces-1)) {
				new_faces[i]->next_face = new_faces[i+1];
			}
			if (i==number_faces-1) {
				new_faces[i]->next_face = NULL;
				face_end = new_faces[i];
			}
			if ((i>=1)&&(i<number_faces)) {
				new_faces[i]->previous_face = new_faces[i-1];
			}
		}
		avg_edge_length = mesh->avg_edge_length;
	}

	/// inserts an edge that connects the origin vertices if halfedges h1
	/// and h2 and returns the new halfedge. h1 and h2 have to lie in the
	/// same face and not at border. between h1 and h2 there has to be at
	/// least one halfedge. h2 will be in the new face. returns the halfedge
	/// of the new edge that lies in the old face.
	polygonal_mesh::halfedge*
	polygonal_mesh::insert_edge(halfedge* h1, halfedge* h2) {
		if (	   (h1==NULL) || (h2==NULL)
				|| (h1->border) || (h2->border)
				|| (h1->m_face != h2->m_face) || (h1->next == h2)
				|| (h2->next == h1) || (h1 == h2)) {
			return NULL;
		}

		//new things
		face *new_f = new face;
		edge *new_e = new edge;
		halfedge *new_h = new halfedge;
		halfedge *new_h_inv = new halfedge;
		//new halfedge
		new_h->border = false;
		new_h->m_edge = new_e;
		new_h->m_face = new_f;
		new_h->inverse = new_h_inv;
		new_h->next = h2;
		new_h->origin = h1->origin;
		new_h->prev = h1->prev;
		//new halfedge
		new_h_inv->border = false;
		new_h_inv->m_edge = new_e;
		new_h_inv->m_face = h1->m_face;
		new_h_inv->inverse = new_h;
		new_h_inv->next = h1;
		new_h_inv->origin = h2->origin;
		new_h_inv->prev = h2->prev;
		//new edge
		new_e->border = false;
		new_e->m_halfedge = new_h;
		new_e->marked = false;
		//new face
		new_f->m_halfedge = h2;
		new_f->slope_angle = h1->m_face->slope_angle; // copy the slope angle, it does not change
		new_f->material = h1->m_face->material; // copy material
		//update dependents
		h2->prev->next = new_h_inv;
		h1->prev->next = new_h;
		h2->prev = new_h;
		h1->prev = new_h_inv;
		h1->m_face->m_halfedge = h1;
		int val = 0;
		halfedge *h = h2;
		do {
			h->m_face = new_f;
			h = h->next;
			val++;
		} while (h!=new_h);
		new_h->m_face->valence = val + 1;
		new_h_inv->m_face->valence = new_h_inv->m_face->valence - val + 1;
		h1->origin->valence++;
		h2->origin->valence++;

		// compute normals and copy data
		if (new_h->m_face != NULL) {
			new_h->m_face->normal = new_h->m_face->compute_normal();
		}
		if (new_h_inv->m_face != NULL) {
			new_h_inv->m_face->normal = new_h_inv->m_face->compute_normal();
		}

		// compute new average edge lenth
		avg_edge_length = ((avg_edge_length * number_edges) + new_e->compute_length()) / (number_edges + 1);

		//insert new things
		insert_halfedge_in_list(new_h);
		insert_halfedge_in_list(new_h_inv);
		insert_edge_in_list(new_e);
		insert_face_in_list(new_f);
		isTriangulated = false;

		//update areas
		new_f->area = new_f->compute_projected_area();
		h1->m_face->area = h1->m_face->compute_projected_area();

		//return new half edge
		return new_h_inv;
	}

	/// computes all face normals and stores them in the list
	void
	polygonal_mesh::compute_face_normals()	{
		face *f = face_source;
		while (f!=NULL) {
			f->normal = f->compute_normal();
			f=f->next_face;
		}
	}

	/// computes all face normals and stores them in the list
	void
	polygonal_mesh::compute_vertex_normals() {
		vertex *v = vertex_source;
		while (v != NULL) {
			v->normal = v->compute_normal();
			v=v->next_vertex;
		}
	}

	/// computes all face areas
	void
	polygonal_mesh::compute_face_areas() {
		face *f = face_source;
		while (f!=NULL) {
			f->area = f->compute_area();
			f=f->next_face;
		}
	}

	/// computes all face areas
	void
	polygonal_mesh::compute_projected_face_areas() {
		face *f = face_source;
		while (f!=NULL) {
			f->area = f->compute_projected_area();
			f=f->next_face;
		}
	}

	/// saves a model
	bool
	polygonal_mesh::save(const char* filename) {
		if (!filename) {
			return false;
		}
		// save mesh
		std::ofstream os(filename);
		if (os.fail()) {
			return false;
		}
		vertex *v = vertex_source;
		while (v!=NULL) {
			os << "v " << v->position.x() << " " << v->position.y() << " " << v->position.z() << std::endl;
			v=v->next_vertex;
		}
		face *f = face_source;
		while (f!=NULL) {
			os << "f";
			halfedge *h = f->m_halfedge;
			halfedge *hi = h;
			do {
				os << " " << hi->origin->index + 1;
				hi=hi->next;
			} while (hi!=h);
			os << std::endl;
			f=f->next_face;
		}
		os.close();
		if (os.fail()) {
			return false;
		}
		// save marks
		std::stringstream s;
		s << filename << ".mrk";
		std::ofstream markFile(s.str().c_str());
		v = vertex_source;
		while (v!=NULL) {
			if (v->marked) {
				markFile << v->index << std::endl;
			}
			v = v->next_vertex;
		}
		markFile << "-1" << std::endl;
		edge* e = edge_source;
		while (e!=NULL) {
			if (e->marked) {
				markFile << e->m_halfedge->origin->index << " " << e->m_halfedge->inverse->origin->index << std::endl;
			}
			e  =e->next_edge;
		}
		markFile.close();
		if (markFile.fail()) {
			return false;
		}
		return true;
	}

	/// triangulates a face
	void
	polygonal_mesh::triangulate(face *f) {
		while (f->valence > 3)	{
			// cut ears (triangles) away as long the valence is > 3
			halfedge *h = f->find_ear();

			if (h != NULL)
				// insert this edge
				f = insert_edge(h, h->next->next)->inverse->m_face;
			else
				std::cout << "A face could not been triangulated. Continueing." << std::endl;
		}
	}

	/// triangulates the mesh
	void
	polygonal_mesh::triangulate() {
		face* f = face_source;
		while (f!=NULL) {
			triangulate(f);
			f=f->next_face;
		}
		compute_face_areas();
		isTriangulated = true;
	}

	/// set all marks
	void
	polygonal_mesh::set_all_marks(bool marked) {
		vertex *v = vertex_source;
		while (v!=NULL) {
			v->marked = marked;
			v=v->next_vertex;
		}
		edge *e = edge_source;
		while (e!=NULL) {
			e->marked = marked;
			e=e->next_edge;
		}
	}
}
