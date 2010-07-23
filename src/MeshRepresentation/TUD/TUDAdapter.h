/*
 * TUDAdapter.h
 *
 *  Created on: Aug 22, 2009
 *      Author: tom
 */

#ifndef TUDADAPTER_H_
#define TUDADAPTER_H_

#include <MeshRepresentation/MeshRepresentation.h>
#include "delaunay_mesh_with_hierarchy.h"
#include "delaunay_mesh_with_hierarchy.cxx"
#include "TUDLaunchSite.h"
#include "types.h"
#include <map>
#include <utility>
#include "info_point_2d.h"

namespace MeshRepresentations {
	namespace TUDRepresentation {

		class TUD_Vertex_iterator;
		class TUD_Finite_vertices_iterator;
		class TUD_Edge_iterator;
		class TUD_Vertex_circulator;
		class TUD_Face_iterator;
		class TUD_Face_circulator;

		typedef TUD_Vertex_iterator				Vertex_iterator;
		typedef TUD_Finite_vertices_iterator	Finite_vertices_iterator;
		typedef TUD_Edge_iterator				Edge_iterator;
		typedef TUD_Vertex_circulator			Vertex_circulator;
		typedef TUD_Face_iterator				Face_iterator;
		typedef TUD_Face_circulator				Face_circulator;

		typedef TUDLaunchSite<PrecisionType>	LaunchSite;

		typedef MeshRepresentations::MeshRepresentation<PrecisionType,
					LaunchSite,	Finite_vertices_iterator, Vertex_circulator,
					Edge_iterator, Face_iterator, Face_circulator>			MeshRepBase;

		typedef MeshRepBase::LaunchSite_Iterator							LaunchSite_Iterator;
		typedef	MeshRepBase::LaunchSite_Circulator							LaunchSite_Circulator;
		typedef MeshRepBase::Edge_Iterator									Edge_Iterator;
		typedef MeshRepBase::LaunchSite_Pair								LaunchSite_Pair;
		typedef MeshRepBase::Face_Iterator									Face_Iterator;
		typedef MeshRepBase::Face_Circulator								Face_Circulator;
		typedef MeshRepBase::LaunchSite_Tripple								LaunchSite_Tripple;

		class TUDAdapter
			: public MeshRepBase
		{
			protected:

				typedef info_point_2d<double, ILaunchSite<PrecisionType> >	info_p_type;
				typedef ext_corner_connectivity								c_connectivity_type;
				typedef mesh_geometry<double, info_p_type>					m_geometry_type;
				typedef triangle_mesh<m_geometry_type, c_connectivity_type>	triangle_m_type;
				typedef delaunay_mesh<triangle_m_type>						delaunay_m_type;
				typedef delaunay_mesh_with_hierarchy<delaunay_m_type>		tm_type;

			private:
				/// the hierarchy factor for the triangulation
				unsigned int hierarchy_factor;
				/// triangle mesh
				tm_type *tm;
				tm_type *tm_bak;

			private:
				void init_mesh();

			public:
				TUDAdapter();
				virtual ~TUDAdapter();

				/* Abstract class implementations */

				/// Gets the number of launch sites in this representation.
				unsigned int NumberLaunchSites();

				ILaunchSite<PrecisionType>*
				Insert(const point3f& position);

				virtual LaunchSite_Pair
				InsertConstraint(point3f& a, point3f& b) = 0;

				virtual std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
				GetNearestLaunchSite(point3f& position) = 0;

				virtual bool
				Are_there_incident_constraints(LaunchSite_Iterator& site_iterator) = 0;

				virtual LaunchSite_Iterator
				LaunchSites_Begin() = 0;

				virtual LaunchSite_Iterator
				LaunchSites_End() = 0;

				virtual void
				CreateLaunchSiteResponsibilities() = 0;

				virtual LaunchSite_Circulator
				GetNeighbors(LaunchSite_Iterator& site_iterator) const = 0;

				virtual Edge_Iterator
				Edges_Begin() = 0;

				virtual Edge_Iterator
				Edges_End() = 0;

				virtual bool
				IsConstrained(Edge_Iterator& e) = 0;

				virtual LaunchSite_Pair
				GetLaunchSitesFromEdge(Edge_Iterator& e) = 0;

				virtual Face_Iterator
				Faces_Begin() = 0;

				virtual Face_Iterator
				Faces_End() = 0;

				virtual LaunchSite_Tripple
				GetLaunchSitesFromFace(Face_Iterator& f) = 0;

				virtual LaunchSite_Tripple
				GetLaunchSitesFromFace(Face_Circulator& f) = 0;

				virtual LaunchSite_Circulator
				GetNeighbors(ILaunchSite<tm_type>* site) = 0;

				virtual Face_Circulator
				GetNeighborFaces(LaunchSite_Iterator& site_iterator) const = 0;

				virtual bool
				IsIntersecting(point3f& a, point3f& b, PrecisionType& t) = 0;

				/// Tests if the the vector from v1 to v2 is an edge of the triangulation
				/// and writes the third point to out_v3 if so
				virtual bool
				IsEdge(LSPtr v1, LSPtr v2, LSPtr out_v3) = 0;
		};
	}
}

#endif /* TUDADAPTER_H_ */
