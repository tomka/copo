#pragma once

#include <Exceptions/ArgumentException.h>
#include <Exceptions/NotImplementedException.h>
#include "Mesh.h"
#include "MeshRepresentation/Voronoi/geom2d.h"
#include <Model/ILaunchSite.h>
#include "types.h"
#include "CGALConfig.h"
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Triangulation_2.h>
#include <Model/EdgeGroupFace.h>

// forward declarations
namespace SnowSim
{
	namespace Model {
		class EdgeGroup;
	}
}

using namespace SnowSim::Model;
using namespace SnowSim;

namespace MeshRepresentations {
	namespace CGALRepresentation {

		template < class VertexBase = CGAL::Triangulation_vertex_base_2<K> >
		class CGALLaunchSite :
				public ILaunchSite<PrecisionType>,
				public VertexBase
			{
				public:
					typedef ILaunchSite<PrecisionType>			BaseLS;
					typedef VertexBase							Base;

				public:
					// CGAL rebindung
					// see: http://www.cgal.org/Manual/3.3/doc_html/cgal_manual/TDS_2/Chapter_main.html
					template < typename TDS2 >
					struct Rebind_TDS {
						typedef typename VertexBase::template Rebind_TDS<TDS2>::Other   Vb2;
						typedef CGALLaunchSite<Vb2>										Other;
					};

					// CGAL typedefs
					typedef typename VertexBase::Vertex_handle      Vertex_handle;
					typedef typename VertexBase::Face_handle        Face_handle;
					typedef typename VertexBase::Point              PointCGAL;

					// Own typedefs
					typedef BaseLS::LaunchSiteType					LaunchSiteType;


				private:
					typedef std::vector<point3f*>::iterator		egf_iterator;

				private:
					EdgeGroupFace*	m_face;
					bool			m_initial;
					PrecisionType	m_height; /* total height, needs always to be uptodate (base + snow) */
					PrecisionType	m_baseHeight;
					PrecisionType	m_snowHeight;
					point3f			m_position;
					LaunchSiteType	m_type;
					PrecisionType	m_responsibility_area;
					Vertex_handle	m_handle;
					cgv::math::linalg::fix_col_vec<PrecisionType,3>
									m_normal;

				public:

					#pragma region Construction / Destruction

					CGALLaunchSite()
						:	VertexBase(), m_face(NULL), m_initial(false), m_height(0),
							m_baseHeight(0), m_snowHeight(0)
					{
					}

					CGALLaunchSite(const CGALLaunchSite& ls)
					{
						this->m_face = ls.m_face;
						this->m_initial = ls.m_initial;
						this->m_height = ls.m_height;
						this->m_baseHeight = ls.m_baseHeight;
						this->m_snowHeight = ls.m_snowHeight;
						this->m_position = ls.m_position;
					}

					CGALLaunchSite(const PointCGAL & p) : VertexBase(p) { Init(); }
					CGALLaunchSite(const PointCGAL & p, Face_handle f) : VertexBase(f, p) { Init(); }
					CGALLaunchSite(Face_handle f) : VertexBase(f) { Init(); }

					CGALLaunchSite(EdgeGroupFace * face, bool initial = false, LaunchSiteType type = Normal)
						: m_face(face), m_initial(initial), m_height(0), m_type(type), m_baseHeight(0), m_snowHeight(0)
					{
					}

					CGALLaunchSite(EdgeGroupFace * face, Real height, bool initial = false, LaunchSiteType type = Normal)
						: m_face(face), m_initial(initial), m_height(height), m_type(type), m_baseHeight(0), m_snowHeight(0)
					{
					}

					~CGALLaunchSite(void)
					{
					}

					#pragma endregion

					PrecisionType x()
					{
						return this->point().x();
					}

					PrecisionType y()
					{
						return this->m_height;
					}

					PrecisionType z()
					{
						return this->point().y();
					}

					void SetBaseHeight(PrecisionType height)
					{
						this->m_baseHeight = height;
						this->m_height = m_baseHeight + m_snowHeight;

						if (m_face != NULL)
							m_face->PropagateNewHeight(this);
					}

					PrecisionType GetBaseHeight()
					{
						return this->m_baseHeight;
					}

					PrecisionType GetSnowHeight()
					{
						return this->m_snowHeight;
					}

					void AddToSnowHeight(PrecisionType addition)
					{
						m_height += addition;
						m_snowHeight += addition;

						if (m_face != NULL)
							m_face->PropagateNewHeight(this);
					}

					void ReduceSnowHeight(PrecisionType removal)
					{
						m_height -= removal;
						m_snowHeight -= removal;

						if (m_face != NULL)
							m_face->PropagateNewHeight(this);
					}

					PrecisionType GetTotalHeight()
					{
						return this->m_height;
					}

					// Gets the responsibility area of the launch site
					PrecisionType GetResponsibility()
					{
						return m_responsibility_area;
					}

					// Sets the responsibility area of the launch site
					void SetResponsibility(PrecisionType responsibility)
					{
						m_responsibility_area = responsibility;
					}

					cgv::scene::geometry::point<PrecisionType, 3> GetPosition()
					{
						return cgv::scene::geometry::point<PrecisionType, 3>(x(), m_height, z());
					}

					// Gets the normal of the launch site
					cgv::math::linalg::fix_col_vec<PrecisionType,3> GetNormal()
					{
						return m_normal;
					}

					// Gets the normal of the launch site
					void SetNormal(cgv::math::linalg::fix_col_vec<PrecisionType,3>& n)
					{
						m_normal = n;
					}

					EdgeGroupFace* GetFace()
					{
						return m_face;
					}

					void SetFace(EdgeGroupFace* face)
					{
						if (face == NULL)
							throw Exceptions::ArgumentException("face must not be null");

						m_face = face;
					}

					// Gets if the launch site is an initial one or not
					bool IsInitial()
					{
						return m_initial;
					}

					void SetInitial(bool isInitial)
					{
						m_initial = isInitial;
					}

					LaunchSiteType GetType()
					{
						return m_type;
					}

					void SetType(LaunchSiteType type)
					{
						m_type = type;
					}

				private:
					void Init()
					{

					}
			};
	}
}
