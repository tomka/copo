#include "CGALAdapter.h"
#include <Exceptions/NotImplementedException.h>
#include <Exceptions/SnowModelException.h>
#include <limits>
#include <Utilities/Intersection/Intersection.h>
#include <types.h>

// usings
using namespace SnowSim;

namespace MeshRepresentations {
	namespace CGALRepresentation {

			/// Constructor
			CGALAdapter::CGALAdapter()
			{
				m_cdt = new CDT();
			}

			/// Destructor
			CGALAdapter::~CGALAdapter()
			{
				delete(m_cdt);
			}

			/// Gets the number of launch sites in this representation.
			unsigned int
			CGALAdapter::NumberLaunchSites()
			{
				return this->m_cdt->number_of_vertices();
			}

			ILaunchSite<PrecisionType>*
			CGALAdapter::Insert(const point3f& position)
			{
				Vertex_handle handle = m_cdt->insert(CGALLaunchSite<>::PointCGAL(position.x(), position.z()));

				LaunchSite* new_ls = &(*handle);
				new_ls->SetBaseHeight( position.y() );

				// Temporary insertion
				m_handleMap.insert(make_pair(new_ls, handle));

				return new_ls;
			}

			std::pair<ILaunchSite<PrecisionType>*, ILaunchSite<PrecisionType>*>
			CGALAdapter::InsertConstraint(point3f& a, point3f& b)
			{
				// insert the (pseudo) launch sites
				Vertex_handle site1 = m_cdt->insert(CGALLaunchSite<>::PointCGAL(a.x(), a.z()));
				Vertex_handle site2 = m_cdt->insert(CGALLaunchSite<>::PointCGAL(b.x(), b.z()));

				// add height information and type
				LaunchSite* ls1 = &(*site1);
				ls1->SetBaseHeight(a.y());
				ls1->SetType(LaunchSite::OnDrop);

				LaunchSite* ls2 = &(*site2);
				ls2->SetBaseHeight(b.y());
				ls2->SetType(LaunchSite::OnDrop);

				// Temporary insertion
				m_handleMap.insert(make_pair(ls1, site1));
				m_handleMap.insert(make_pair(ls2, site2));

				m_cdt->insert_constraint ( site1, site2 );

				return make_pair( ls1, ls2);
			}

			bool
			CGALAdapter::InsertConstraint(ILaunchSite<PrecisionType>* a, ILaunchSite<PrecisionType>* b)
			{
				HandleMap_Iterator h_it1 = m_handleMap.find(a);
				if ( h_it1 == m_handleMap.end() )
					return false;

				HandleMap_Iterator h_it2 = m_handleMap.find(b);
				if ( h_it2 == m_handleMap.end() )
					return false;

				Vertex_handle vh1 = (*h_it1).second;
				Vertex_handle vh2 = (*h_it2).second;

				m_cdt->insert_constraint( vh1, vh2 );

				return true;
			}

			LaunchSite_Pair
			CGALAdapter::GetLaunchSitesFromEdge(Edge_Iterator& e)
			{
				EdgeCDT edge = *e;

				// since a edge of index i is opposite to vertex i
				// we have calculate the vertex index of the
				// starting vertex of the edge
				int v1 = ( edge.second + 1 ) % 3;

				// return the starting  and end vertex of the edge
				return LaunchSite_Pair(
					&*( edge.first->vertex( v1 ) ),
					&*( edge.first->vertex( (v1 + 1) % 3 ) ) );
			}

			bool
			CGALAdapter::Are_there_incident_constraints(LaunchSite_Iterator& site_iterator)
			{
				return m_cdt->are_there_incident_constraints(site_iterator);
			}

			LaunchSite_Circulator
			CGALAdapter::GetNeighbors(LaunchSite_Iterator& site_iterator) const
			{
				return m_cdt->incident_vertices(site_iterator);
			}

			LaunchSite_Circulator
			CGALAdapter::GetNeighbors(ILaunchSite<PrecisionType>* site)
			{
				/* Version 1: Map
				std::map<LaunchSite*, Vertex_handle>::const_iterator it = m_handleMap.find(site);

				if (it == m_handleMap.end())
					throw Exceptions::SnowModelException("Did not find the requested launch site in mesh representation!");

				return m_cdt->incident_vertices( (*it).second );
				*/

				/*	Performance test for v1 against v2 against v3
					I compared the method with a lookup table and the insertion method - 400 points each:

					No optimization:
					Insert: 63,7883ms
					Map: 5,69598ms
					Cast: 3,95972ms

					Optimized:
					Insert: 15,143ms
					Map: 0,129905
					Cast: 0,110629ms
				*/

				/* Version 2: Insert, an insertion is no problem, the point exists and we get back the correct handle

				point3f position = site->GetPosition();
				CGALLaunchSite<>::PointCGAL pnt(position.x(), position.z());

				return m_cdt->incident_vertices( m_cdt->insert( pnt ) );
				*/

				/* Version 3: casts and face */

				return m_cdt->incident_vertices( vertex_cast( static_cast<Vertex*>(site) ) );
			}

			Vertex_handle CGALAdapter::vertex_cast(Vertex* v)
			{
				 Face_handle c = v->face();
				 if (&*(c->vertex(0)) == v) return c->vertex(0);
				 if (&*(c->vertex(1)) == v) return c->vertex(1);
				 if (&*(c->vertex(2)) == v) return c->vertex(2);
				 assert( &*(c->vertex(3)) == v );
				 return c->vertex(3);
			}

			void
			CGALAdapter::CreateLaunchSiteResponsibilities()
			{
				// go through all launch sites
				for (Finite_vertices_iterator vit = m_cdt->finite_vertices_begin();
						vit != m_cdt->finite_vertices_end(); ++vit)
				{
					//std::cout << "\tLaunch site " << vit->point() << std::endl;
					PrecisionType area = 0;
					ILaunchSite<PrecisionType>& site = *vit;

					// ignore vertices that are connected to constrains (e. g. are on a drop)
					if ( site.GetType() == LaunchSite::OnDrop ) {
						site.SetResponsibility(1.0);
						continue;
					}

					point3f pos_site = site.GetPosition();

					//
					// calculate area
					//
					LaunchSite_Circulator vcirc = m_cdt->incident_vertices(vit);
					LaunchSite_Circulator done(vcirc);

					// go through all neighbors
					if (vcirc != 0) {
						do {
							// the responsibility area of the launchsite "site"
							// is calculated by the sum of the area of the surrounding
							// triangles. Each triangle area is divided by the amount
							// of real launch sites being vertices of the triangle.

							// get the other two launch sites
							ILaunchSite<PrecisionType>& n1 = *vcirc;
							ILaunchSite<PrecisionType>& n2 = *(++vcirc);

							// calculate nominator by counting normal launch site neighbors
							int nominator = 1;
							if (n1.GetType() != LaunchSite::OnDrop)
							{
								++nominator;
							}
							if (n2.GetType() != LaunchSite::OnDrop)
							{
								++nominator;
							}

							// calculate triangle area
							// from: http://softsurfer.com/Archive/algorithm_0101/algorithm_0101.htm
							//float subarea = ( 0.5F * length(
							//	cross(n1.GetPosition() - site.GetPosition(), n2.GetPosition() - site.GetPosition()))
							//	);
							point3f pos_n1 = n1.GetPosition();
							point3f pos_n2 = n2.GetPosition();

							float subarea = 0.5F * (
									( pos_n2.z() - pos_site.z() ) * ( pos_n1.x() - pos_site.x() )
								-	( pos_n2.x() - pos_site.x() ) * ( pos_n1.z() - pos_site.z() ) );

							area += subarea / nominator;

							//std::cout << "\t\t" << vcirc->point() << std::endl;
						} while(vcirc != done);

						// set responsibility
						site.SetResponsibility(area);
					}

				}
			}

			std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
			CGALAdapter::GetNearestLaunchSite(point3f& position)
			{
				// TODO:
				// use cdt.get_boundary_of_conflicts ( Point p, OutputItBoundaryEdges eit, Face_handle start)
				// (See mailing list)

				PointCDT cgal_pos(position.x(), position.z());

				Face_handle handle = m_cdt->locate(cgal_pos);

				std::vector<EdgeCDT> conflict_boundary;

				// get conflict boundary
				m_cdt->get_boundary_of_conflicts(cgal_pos, std::back_inserter(conflict_boundary), handle);

				// currently minimal distance
				PrecisionType min_distance = std::numeric_limits<PrecisionType>::infinity();

				// the return launch site
				ILaunchSite<PrecisionType>* nearest_ls = NULL;

				for (	std::vector<EdgeCDT>::iterator cb_it = conflict_boundary.begin();
						cb_it != conflict_boundary.end();
						++cb_it )
				{
					EdgeCDT edge = *cb_it;

					// since a face of index i is opposite to vertex i
					// we have calculate the vertex index
					int v1 = ( edge.second + 1 ) % 3;

					// get the starting vertex of the edge
					Vertex_handle vertex1 = edge.first->vertex(v1);
					// and determine the distance to the query point
					VectorCGAL vec1(vertex1->point(), cgal_pos);
					PrecisionType distance = vec1.squared_length();

					// have we got a new minimum?
					if (distance < min_distance) {
						min_distance = distance;
						nearest_ls = &*vertex1;
					}
				}

				return std::pair<ILaunchSite<PrecisionType>*, PrecisionType>(nearest_ls, min_distance);
			}

			LaunchSite_Iterator CGALAdapter::LaunchSites_Begin()
			{
				return m_cdt->finite_vertices_begin();
			}

			LaunchSite_Iterator CGALAdapter::LaunchSites_End()
			{
				return m_cdt->finite_vertices_end();
			}

			Edge_Iterator CGALAdapter::Edges_Begin()
			{
				return m_cdt->finite_edges_begin();
			}

			Edge_Iterator CGALAdapter::Edges_End()
			{
				return m_cdt->finite_edges_end();
			}

			Face_Iterator CGALAdapter::Faces_Begin()
			{
				return m_cdt->finite_faces_begin();
			}

			Face_Iterator CGALAdapter::Faces_End()
			{
				return m_cdt->finite_faces_end();
			}

			Face_Circulator CGALAdapter::GetNeighborFaces(LaunchSite_Iterator& site_iterator) const
			{
				return m_cdt->incident_faces(site_iterator);
			}

			bool CGALAdapter::IsConstrained(Edge_Iterator& e)
			{
				return m_cdt->is_constrained(*e);
			}

			LaunchSite_Tripple CGALAdapter::GetLaunchSitesFromFace(Face_Iterator& f)
			{
				LaunchSite_Tripple face;

				face.first = &*((*f).vertex(0));
				face.second = &*((*f).vertex(1));
				face.third = &*((*f).vertex(2));

				return face;
			}

			LaunchSite_Tripple CGALAdapter::GetLaunchSitesFromFace(Face_Circulator& f)
			{
				LaunchSite_Tripple face;

				face.first = &*((*f).vertex(0));
				face.second = &*((*f).vertex(1));
				face.third = &*((*f).vertex(2));

				return face;
			}

			bool CGALAdapter::IsIntersecting(point3f& a, point3f& b, PrecisionType& t)
			{
				Face_Iterator	f_it = m_cdt->finite_faces_begin(),
								f_it_end = m_cdt->finite_faces_end();

				while ( f_it != f_it_end )
				{
					LaunchSite_Tripple face;

					typedef ILaunchSite<PrecisionType>& PLS;
					PLS ls0 = (*((*f_it).vertex(0)));
					PLS ls1 = (*((*f_it).vertex(1)));
					PLS ls2 = (*((*f_it).vertex(2)));

					point3f first = ls0.GetPosition();
					point3f second = ls1.GetPosition();
					point3f third = ls2.GetPosition();

					if ( SnowSim::Utilities::Intersect::intersect(
							first, third - first, second - first,
							a, b,
							true, false, t) )
					{
						if (	( ls0.GetType() != LaunchSite::OnDrop )
							||	( ls1.GetType() != LaunchSite::OnDrop )
							|| 	( ls2.GetType() != LaunchSite::OnDrop ) )
							{
								return true;
							}
					}

					++f_it;
				};

				return false;
			}

			/// Removes the constraining edge between ls1 and ls2. Returns true if this
			/// has been successfully done, false otherwise (also if there is no constraint
			/// at all).
			bool CGALAdapter::RemoveConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2)
			{
				Face_handle handle;

				HandleMap_Iterator h_it1 = m_handleMap.find(ls1);
				if ( h_it1 == m_handleMap.end() )
					return false;

				HandleMap_Iterator h_it2 = m_handleMap.find(ls2);
				if ( h_it2 == m_handleMap.end() )
					return false;

				Vertex_handle vh1 = (*h_it1).second;
				Vertex_handle vh2 = (*h_it2).second;

				int third_vertex;
				bool result = m_cdt->is_edge( vh1 , vh2, handle, third_vertex);

				if (result)
				{
					m_cdt->remove_constrained_edge(handle, third_vertex );
				}

				return result;
			}

			/// Tests if the the vector from v1 to v2 is an edge of the triangulation
			/// and writes the third point to out_v3 if so
			bool
			CGALAdapter::IsEdge(LaunchSite* v1, LaunchSite* v2, LaunchSite** out_v3)
			{
				Face_handle handle;

				HandleMap_Iterator h_it1 = m_handleMap.find(v1);
				if ( h_it1 == m_handleMap.end() )
					return false;

				HandleMap_Iterator h_it2 = m_handleMap.find(v2);
				if ( h_it2 == m_handleMap.end() )
					return false;

				Vertex_handle vh1 = (*h_it1).second;
				Vertex_handle vh2 = (*h_it2).second;

				int third_vertex;
				bool result = m_cdt->is_edge( vh1 , vh2, handle, third_vertex);

				if (result)
				{
					// since a edge of index i is opposite to vertex i
					// "third_vertex" is the correct vertex
					Vertex_handle v = handle->vertex( third_vertex );
					*out_v3 = &(*v);
				}

				return result;
			}
	}
}
