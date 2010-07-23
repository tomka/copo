#include "Mesh.h"
#include <Exceptions/ArgumentException.h>
#include <Utilities/Timer.h>

namespace SnowSim {

	Mesh::Mesh() { }

	/// Constructor for body, takes mesh file name, position, rotation angle, rotation vector, scaling factor
	Mesh::Mesh(const std::string& meshFilename
			   , bool recompute_normals
			   , float x, float y, float z
			   , float angle, float vecx, float vecy, float vecz
			   , float scaling)
			   : thePosition(point3f(x, y, z)),
			    theRotationAngle(angle), theRotationVec(point3f(vecx, vecy, vecz)),
				theScaling(scaling),
				doRenderVertexNormals(false),
				isConvex(false),
				isTriangulated(false), // must be checked after loading
				isVisible(true),
				doRenderFaces(true),
				doRenderEdges(true),
				doRenderVertices(false),
				doRenderFaceNormals(false),
				doRenderBoundingBox(false),
				orthographic(false)
	{
		theMesh = NULL;
		// load the mesh
		if (!meshFilename.empty()) {
				Utilities::Timer t;
				// read mesh
				SnowSim::IO::MeshReader reader(meshFilename);
				theMesh = reader.load(recompute_normals);
				t.start();
				m_materialMap = reader.GetMaterialMap();
				baseMesh = new SnowSim::polygonal_mesh();
				baseMesh->copy_from(theMesh);
				t.stop();
				m_name = std::string(meshFilename);
				std::cout << "[Mesh] Initialization time: " << t.diffTimeInMSec() << "msec" << std::endl;
		} else {
			throw Exceptions::ArgumentException("The mesh file name must not be empty!");
		}

		vertexColor = 0.7f * green_color;
		edgeColor = 0.8f * blue_color;
		faceColor = point4f(0.8f,0.8f,0.8f, 0.5f);

		marked_vertex_color = 0.8f*red_color;
		normal_color = red_color;
		select_offset = 70.0f;
		marked_color = red_color;
		boundingBoxColor = red_color;

		selected_edge = NULL;
		selected_vertex = NULL;
		selected_face = NULL;
		marked_vertex = NULL;
		vertex_size = 5.0f;
		border_vertex_size = 8.0f;
		edge_width = 1.0f;
		normal_width = 1.0f;
		border_edge_width = 1.0f;

		vertex_drag_mode = NO;
	}

	Mesh::Mesh(const Mesh* copy, bool copydata)
	{
		Init(copy, copydata);
	}

	Mesh::Mesh(const Mesh* copy)
	{
		Init(copy, true);
	}

	void Mesh::Init(const Mesh* copy, bool copydata)
	{
		if (copydata) {
			theMesh = new SnowSim::polygonal_mesh();
			theMesh->copy_from(copy->theMesh);

			baseMesh = new SnowSim::polygonal_mesh();
			baseMesh->copy_from(baseMesh);
		}

		isConvex = copy->isConvex;
		isTriangulated = copy->isTriangulated;
		doRenderFaces = copy->doRenderFaces;
		doRenderEdges = copy->doRenderEdges;
		doRenderVertices = copy->doRenderVertices;
		doRenderFaceNormals = copy->doRenderFaceNormals;
		doRenderVertexNormals = copy->doRenderVertexNormals;
		doRenderBoundingBox= copy->doRenderBoundingBox;
		orthographic = copy->orthographic;

		vertexColor = point3f(copy->vertexColor);
		edgeColor = point3f(copy->edgeColor);
		faceColor = point4f(copy->faceColor);

		marked_vertex_color = point3f(copy->marked_vertex_color);
		normal_color = point3f(copy->normal_color);
		select_offset = select_offset;
		marked_color = point3f(copy->marked_color);
		boundingBoxColor = point3f(copy->boundingBoxColor);

		selected_edge = copy->selected_edge;
		selected_vertex = copy->selected_vertex;
		selected_face = copy->selected_face;
		marked_vertex = copy->marked_vertex;
		vertex_size = copy->vertex_size;
		border_vertex_size = copy->border_vertex_size;
		edge_width = copy->edge_width;
		normal_width = copy->normal_width;
		border_edge_width = copy->border_edge_width;

		vertex_drag_mode = copy->vertex_drag_mode;

		m_name = copy->m_name;
	}


	Mesh::~Mesh(void)
	{
		delete theMesh;
	}

	/// draws faces in the local coordinate system
	void Mesh::draw_faces() {

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glDisable(GL_LIGHT1); // disable snow light

		face* f = theMesh->face_source;
		while (f!=NULL) {

			if(orthographic)
			{
				halfedge* h_start = f->m_halfedge;
				halfedge* h = h_start;

				glBegin(GL_POLYGON);
				glNormal3fv(f->normal);
				do {
					float c = h->origin->position.y();
					c = c > 1 ? 1: c;
					c = c < 0 ? 0: c;
					glColor3f(c,c,c);
					glVertex3fv(h->origin->position);
					h=h->next;
				} while (h != h_start);
				glEnd();
			}
			else
			{
				// use polygon offset to ensure edges and illustrated faces are drawn on top
				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(1.0f, 1.0f);

				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				std::map<int, SnowSim::Shading::Material>::iterator m_it = m_materialMap.find(f->material);
				assert( m_it != m_materialMap.end() );
				SnowSim::Shading::Material& mat = (*m_it).second;

				GLfloat front_diffuse[]  = {mat.diffuse_color.x(), mat.diffuse_color.y(), mat.diffuse_color.z(), mat.alpha};
				GLfloat front_ambient[]  = {mat.ambient_color.x(), mat.ambient_color.y(), mat.ambient_color.z(), mat.alpha};
				GLfloat front_specular[] = {mat.specular_color.x(), mat.specular_color.y(), mat.specular_color.z()};

				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);
				glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, front_ambient);
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_diffuse);
				glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat.shininess);

				if (selected_face == f)
				{
					point3f color = mat.diffuse_color + select_offset * (0.01f * (white_color - mat.diffuse_color));

					front_diffuse[0]  = color.x();
					front_diffuse[1]  = color.y();
					front_diffuse[2]  = color.z();

					glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_diffuse);
				}

				halfedge* h_start = f->m_halfedge;
				halfedge* h = h_start;


				//glPushName((GLuint)f);

				glBegin(GL_POLYGON);
					do {
						glNormal3fv(h->origin->normal);
						glVertex3fv(h->origin->position);
						h=h->next;
					} while (h != h_start);
				glEnd();

				//glPopName();

				glPolygonOffset(0.0, 0.0);
				glDisable(GL_POLYGON_OFFSET_FILL);
			}

			f=f->next_face;
		};
	}

	/// draws vertices in the local coordinate system
	void Mesh::draw_vertices() {

		glDisable(GL_LIGHTING);

		vertex* v = theMesh->vertex_source;
		while (v!=NULL) {


			point3f color;
			if (v == marked_vertex) {
				color = marked_vertex_color;
			} else {
				if (v->marked) {
					color = marked_color;
				} else {
					color = vertexColor;
				}
			}
			if (selected_vertex == v) {
				color = color + select_offset * (0.01f * (white_color - color));
			}
			glColor3f(color.x(), color.y(), color.z());
			if (v->border) {
				glPointSize(border_vertex_size);
			} else {
				glPointSize(vertex_size);
			}

			//glPushName((GLuint)v);

			glBegin(GL_POINTS);
			glVertex3fv(v->position);
			glEnd();

			//glPopName();

			v=v->next_vertex;
		};
	}

	/// renders normals
	void Mesh::draw_v_normals() {

		glDisable(GL_LIGHTING);

		glColor3f(normal_color.x(), normal_color.y(), normal_color.z());
		glLineWidth(normal_width);

		face* f = theMesh->face_source;
		while (f!=NULL) {

			halfedge* h_start = f->m_halfedge;
			halfedge* h = h_start;
			glBegin(GL_LINES);
				do {
					point3f n = h->origin->normal;
					n = theMesh->avg_edge_length * n;

					glVertex3fv(h->origin->position);
					glVertex3fv(h->origin->position + n);
					h=h->next;
				} while (h != h_start);
			glEnd();

			f=f->next_face;
		};
	}

	/// renders normals
	void Mesh::draw_f_normals() {

		glDisable(GL_LIGHTING);

		glColor3f(normal_color.x(), normal_color.y(), normal_color.z());
		glLineWidth(normal_width);

		face* f = theMesh->face_source;
		while (f!=NULL) {

			point3f c = f->compute_center();
			point3f n = f->normal;
			n = theMesh->avg_edge_length * n;
			glBegin(GL_LINE_LOOP);
			glVertex3fv(c);
			glVertex3fv(c + n);
			glEnd();

			f=f->next_face;
		};
	}

	void Mesh::draw_boundingbox() {
		point3f * min = &theMesh->m_bounding_box.min;
		point3f * max = &theMesh->m_bounding_box.max;
		glColor3f(boundingBoxColor.x(), boundingBoxColor.y(), boundingBoxColor.z());
		glLineWidth(1.0f);
		glBegin(GL_LINE_LOOP);
			glVertex3f(min->x(), min->y(), min->z());
			glVertex3f(min->x(), max->y(), min->z());
			glVertex3f(max->x(), max->y(), min->z());
			glVertex3f(max->x(), min->y(), min->z());
		glEnd();
		glBegin(GL_LINE_LOOP);
			glVertex3f(min->x(), min->y(), max->z());
			glVertex3f(min->x(), max->y(), max->z());
			glVertex3f(max->x(), max->y(), max->z());
			glVertex3f(max->x(), min->y(), max->z());
		glEnd();
		glBegin(GL_LINES);
			glVertex3f(min->x(), min->y(), min->z());
			glVertex3f(min->x(), min->y(), max->z());
			glVertex3f(min->x(), max->y(), min->z());
			glVertex3f(min->x(), max->y(), max->z());
			glVertex3f(max->x(), min->y(), min->z());
			glVertex3f(max->x(), min->y(), max->z());
			glVertex3f(max->x(), max->y(), min->z());
			glVertex3f(max->x(), max->y(), max->z());
		glEnd();
	}

	/// draws edges in the local coordinate system
	void Mesh::draw_edges() {

		glDisable(GL_LIGHTING);

		edge* e = theMesh->edge_source;
		while (e!=NULL) {

			if (e->border) {
				glLineWidth(border_edge_width);
			} else {
				glLineWidth(edge_width);
			}
			point3f color;
			if (e->marked) {
				color = marked_color;
			} else {
				color = edgeColor;
			}
			if (selected_edge == e) {
				color = color + select_offset * (0.01f * (white_color - color));
			}
			glColor3f(color.x(), color.y(), color.z());

			//glPushName(static_cast<GLuint>(e));

			glBegin(GL_LINE_LOOP);
				glVertex3fv(e->m_halfedge->origin->position);
				glVertex3fv(e->m_halfedge->inverse->origin->position);
			glEnd();

			//glPopName();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			e=e->next_edge;
		};
	}

	/// return the transformation matrix from local to world coordinates
	Mat44f Mesh::transformationMatrix() const
	{
		return translate_44(thePosition.x(), thePosition.y(), thePosition.z())
				* rotate_44(theRotationVec.x(), theRotationVec.y(), theRotationVec.z(),
					(3.14156f * 2 * (theRotationAngle / 360.0f)))
				* scale_44(theScaling);
	}

	/// render the body in world coordinates
	void Mesh::render() {
		glDoTransformations();
		if ((theMesh != 0) && isVisible) {
			if (doRenderFaces) draw_faces();
			if (doRenderVertices) draw_vertices();
			if (doRenderEdges) draw_edges();
			if (doRenderFaceNormals) draw_f_normals();
			if (doRenderVertexNormals) draw_v_normals();
			if (doRenderBoundingBox) draw_boundingbox();
		}
		glUndoTransformations();
	}

	void Mesh::triangulate() {
		theMesh->triangulate();
	}

	BBox3f Mesh::BoundingBox() {
		return theMesh->m_bounding_box;
	}
}
