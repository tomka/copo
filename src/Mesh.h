#pragma once

#ifdef WIN32
#include <windows.h>
#pragma warning (disable:4311)
#endif

#include "polygonal_mesh.h"
#include <IO/MeshReader.h>
#include <fltk/gl.h>
#include "types.h"
#include <vector>
#include <Shading/Material.h>

namespace SnowSim {

	class Mesh
		{
		friend class MeshPool;

		private:

			Mesh();

			/// pointer to the (modified) body mesh
			SnowSim::polygonal_mesh * theMesh;
			/// pointer to the original mesh
			SnowSim::polygonal_mesh * baseMesh;

			/// position of the mesh
			point3f thePosition;
			/// the rotation angle of the mesh
			float theRotationAngle;
			/// the rotation vector of the mesh
			Vec3f theRotationVec;
			/// scaling of the mesh
			float theScaling;

			/// is the mesh of the body convex
			bool isConvex;
			/// shall vertices be rendered
			bool doRenderVertices;
			/// shall edges be rendered
			bool doRenderEdges;
			/// shall faces be rendered
			bool doRenderFaces;
			/// shall bounding box be rendered
			bool doRenderBoundingBox;
			/// is the body currently visible
			bool isVisible;
			/// is the mesh triangulated
			bool isTriangulated;
			// shall face normals be rendered
			bool doRenderFaceNormals;
			// shall vertex normals be rendered
			bool doRenderVertexNormals;
			// shall we render in orthographic mode
			bool orthographic;

			/// color of vertices
			point3f vertexColor;
			/// color of edges
			point3f edgeColor;
			/// color of faces
			point4f faceColor;
			/// color of normals
			point3f normal_color;
			/// color of marked vertices
			point3f marked_color;
			/// color of bounding box
			point3f boundingBoxColor;

			/// color of the marked vertex
			point3f marked_vertex_color;
			/// size of vertices
			float vertex_size;
			/// size of border vertices
			float border_vertex_size;
			/// width of edges
			float edge_width;
			/// width of normals
			float normal_width;
			/// width of border edges
			float border_edge_width;
			/// determines the highlight of selected vertices and edges, value between 0.0f and 100.0f
			float select_offset;

			/// draws vertices in the local coordinate system
			void draw_vertices();
			/// draws edges in the local coordinate system
			void draw_edges();
			/// draws faces in the local coordinate system
			void draw_faces();
			/// draws face normals in the local coordinate system
			void draw_f_normals();
			/// draws vertex normals in the local coordinate system
			void draw_v_normals();
			/// draws bounding box in the local coordinate system
			void draw_boundingbox();

			/// filename
			const std::string meshFileName;

			point3f compute_center_of_face(int face);

			void Init(const Mesh* copy, bool copydata = true);

			/// A map that relates face indices to materials (and the access indices)
			std::map<int, SnowSim::Shading::Material > m_materialMap;

			// a descriptive name of the mesh
			// defaults to the file name
			std::string m_name;

		public:

			Mesh(const std::string& meshFilename, bool recompute_normals = false
				, float x = 0.0f, float y = 0.0f, float z = 0.0f
				, float angle = 0.0f, float vecx = 0.0f, float vecy = 1.0f, float vecz = 0.0f
				, float scaling = 1.0f);

			Mesh(const Mesh* copy);

			// creates a new Mesh based on the given Mesh
			// is copydata true the constructor behaves like
			// the copy constructor, is it false only the
			// non data fiels are copied
			Mesh(const Mesh* copy, bool copydata);

			~Mesh(void);

			typedef SnowSim::polygonal_mesh::halfedge	halfedge;
			typedef SnowSim::polygonal_mesh::edge		edge;
			typedef SnowSim::polygonal_mesh::vertex		vertex;
			typedef SnowSim::polygonal_mesh::face		face;

			/// the currently selected edge
			SnowSim::polygonal_mesh::edge *selected_edge;
			/// the currently selected vertex
			SnowSim::polygonal_mesh::vertex *selected_vertex;
			/// the currently selected face
			SnowSim::polygonal_mesh::face *selected_face;
			/// marked Vertex
			SnowSim::polygonal_mesh::vertex *marked_vertex;
			/// vertex drag mode
			Vertex_Drag_Mode vertex_drag_mode;


			/// check if a mesh is loaded
			bool meshLoaded() const { return (theMesh != NULL);}

			/// return a pointer to the mesh
			SnowSim::polygonal_mesh* mesh() { return theMesh; }

			/// return a pointer to the base mesh
			SnowSim::polygonal_mesh* base_mesh() const { return baseMesh; }

			/// reset the modified mesh back in the original state
			void resetModifiedMesh()
			{
				selected_edge = NULL;
				selected_vertex = NULL;
				selected_face = NULL;
				marked_vertex = NULL;

				theMesh->copy_from(baseMesh);
			}

			/// return the position of the body
			point3f position() const { return thePosition; }
			/// set the postion of the body
			void setPosition(point3f newPos) { thePosition = newPos; }

			/// return the rotation vector of the body
			Vec3f rotationVector() const { return theRotationVec; }
			/// set the rotation vector
			void setRotationVector(Vec3f newVec) { theRotationVec = newVec; }

			/// get the rotation angle of the body
			float rotationAngle() const { return theRotationAngle; }
			/// set the rotation angle
			void setRotationAngle(float newAngle) {theRotationAngle = newAngle; }

			/// get the scaling factor of the body
			float scaling() const { return theScaling; }
			/// set the scaling factor of the body
			void setScaling(float newScaling) { theScaling = newScaling; }

			/// return whether the body is convex
			bool convex() const { return isConvex; }
			/// set whether the body is convex
			void setConvex(bool convex) {isConvex = convex;}

			/// return whether vertices are rendered
			bool renderVertices() const { return doRenderVertices; }
			/// set whether vertices are rendered
			void setRenderVertices(bool render) { doRenderVertices = render; }

			/// return whether edges are rendered
			bool renderEdges() const { return doRenderEdges; }
			/// set whether edges are rendered
			void setRenderEdges(bool render) { doRenderEdges = render; }

			/// return whether faces are rendered
			bool renderFaces() const { return doRenderFaces; }
			/// set whether faces are rendered
			void setRenderFaces(bool render) { doRenderFaces = render; }

			/// return whether face normals are rendered
			bool renderNormals() const { return doRenderFaceNormals; }
			/// set whether face normals are rendered
			void setRenderFaceNormals(bool render) { doRenderFaceNormals = render; }

			/// return whether vertex normals are rendered
			bool renderVertexNormals() const { return doRenderVertexNormals; }
			/// set whether vertex normals are rendered
			void setRenderVertexNormals(bool render) { doRenderVertexNormals = render; }

			/// return whether faces are rendered
			bool renderBoundingBox() const { return doRenderBoundingBox; }
			/// set whether faces are rendered
			void setRenderBoundingBox(bool render) { doRenderBoundingBox = render; }

			/// return the transformation matrix from local to world coordinates
			Mat44f transformationMatrix() const;

			/// apply the open gl transformation (pushMatrix, translate, rotate, scale) for this body
			void glDoTransformations()
			{
				glPushMatrix();
				glTranslatef(thePosition.x(), thePosition.y(), thePosition.z());
				glRotatef(theRotationAngle, theRotationVec.x(), theRotationVec.y(), theRotationVec.z());
				glScalef(theScaling, theScaling, theScaling);
			}

			/// compute the face normal for a given face index
			Vec3f computeFaceNormal(int fi) const;

			/// undo the open gl transformation(popMatrix) for this body
			void glUndoTransformations() { glPopMatrix(); }

			/// render the body in world coordinates
			void render();

			/// gets the name of the mesh
			const std::string& getName() { return m_name; }
			void setName(const std::string& new_name) { m_name = std::string(new_name); }

			void triangulate();

			bool IsTriangulated() { return isTriangulated; }

			void set_all_marks(bool setMarks) { theMesh->set_all_marks(setMarks); }

			BBox3f BoundingBox();

			/// Adds the material to the mashs material repository
			/// and returns the index under what it will be accessible
			int AddMaterial(Shading::Material& material);

			/// Gets a string that represents the mesh with
			/// sketch code
			template<class OutputStream>
			void getSketchRepresentation(OutputStream& os, std::string name)
			{
				os << "def " << name << "{" << std::endl;

				// add a style
				os << "\tspecial|\\tikzstyle{" << name << "style} = [fill=orange!20,draw=orange]|[lay=under]" << std::endl;


				face* f = theMesh->face_source;
				while (f!=NULL) {
					halfedge* h_start = f->m_halfedge;
					halfedge* h = h_start;

					// add the polygon
					os << "\tpolygon[fill style=" << name << "style]";

					do {
						point3f pos = h->origin->position;
						os << "(" << pos.x() << "," << pos.y() << "," << pos.z() << ")";
						h=h->next;
					} while (h != h_start);

					os << std::endl;

					f = f->next_face;
				}

				os << "}" << std::endl;
			}
		};
}
