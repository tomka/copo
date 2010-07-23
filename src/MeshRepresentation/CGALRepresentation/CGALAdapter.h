#pragma once
//#pragma warning( disable : 4396 )

#include "CGALConfig.h"
#include <MeshRepresentation/MeshRepresentation.h>
#include "CGALLaunchSite.h"
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include "types.h"
#include <map>
#include <utility>

namespace MeshRepresentations {
	namespace CGALRepresentation {

		typedef CGAL::Constrained_triangulation_face_base_2<K>					Fb;
		typedef CGAL::Exact_predicates_tag										Itag;

		typedef CGAL::Triangulation_data_structure_2 < CGALLaunchSite<>, Fb >	Tds;
		typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds, Itag>		CDT;

		typedef CDT::Vertex_handle		Vertex_handle;
		typedef CDT::Face_handle		Face_handle;
		typedef CDT::Point				PointCDT;
		typedef CDT::Edge				EdgeCDT;
		typedef CDT::Vertex				Vertex;
		typedef K::Vector_2				VectorCGAL;

		typedef CDT::Vertex_iterator			Vertex_iterator;
		typedef CDT::Finite_vertices_iterator	Finite_vertices_iterator;
		typedef CDT::Edge_iterator				Edge_iterator;
		typedef CDT::Vertex_circulator			Vertex_circulator;
		typedef CDT::Finite_faces_iterator		Face_iterator;
		typedef CDT::Face_circulator			Face_circulator;

		typedef CGALLaunchSite<>		LaunchSite;
		typedef MeshRepresentations::MeshRepresentation<PrecisionType, LaunchSite,
			Finite_vertices_iterator, Vertex_circulator, Edge_iterator, Face_iterator, Face_circulator>
																			MeshRepBase;
		typedef MeshRepBase::LaunchSite_Iterator							LaunchSite_Iterator;
		typedef	MeshRepBase::LaunchSite_Circulator							LaunchSite_Circulator;
		typedef MeshRepBase::Edge_Iterator									Edge_Iterator;
		typedef MeshRepBase::LaunchSite_Pair								LaunchSite_Pair;
		typedef MeshRepBase::Face_Iterator									Face_Iterator;
		typedef MeshRepBase::Face_Circulator								Face_Circulator;
		typedef MeshRepBase::LaunchSite_Tripple								LaunchSite_Tripple;

		class CGALAdapter :
			public MeshRepBase
		{
			private:
				/// the constrained delaunay triangulation
				CDT* m_cdt;

				typedef ILaunchSite<PrecisionType>				LaunchSite;
				typedef std::map<LaunchSite*, Vertex_handle>	HandleMapType;
				typedef HandleMapType::iterator					HandleMap_Iterator;

				HandleMapType		m_handleMap;

				// a helper method to cast LaunchSites to Vertex_handles
				Vertex_handle vertex_cast(Vertex* v);

			public:

				CGALAdapter();

				~CGALAdapter();

				/// Gets the number of launch sites in this representation.
				unsigned int
				NumberLaunchSites();

				ILaunchSite<PrecisionType>*
				Insert(const point3f& position);

				LaunchSite_Pair
				InsertConstraint(point3f& a, point3f& b);

				std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
				GetNearestLaunchSite(point3f& position);

				bool
				Are_there_incident_constraints(LaunchSite_Iterator& site_iterator);

				LaunchSite_Iterator
				LaunchSites_Begin();

				LaunchSite_Iterator
				LaunchSites_End();

				void CreateLaunchSiteResponsibilities();

				LaunchSite_Circulator
				GetNeighbors(LaunchSite_Iterator& site_iterator) const;

				Edge_Iterator
				Edges_Begin();

				Edge_Iterator
				Edges_End();

				bool
				IsConstrained(Edge_Iterator& e);

				bool
				InsertConstraint(ILaunchSite<PrecisionType>* a, ILaunchSite<PrecisionType>* b);

				LaunchSite_Pair
				GetLaunchSitesFromEdge(Edge_Iterator& e);

				Face_Iterator
				Faces_Begin();

				Face_Iterator
				Faces_End();

				LaunchSite_Tripple
				GetLaunchSitesFromFace(Face_Iterator& f);

				LaunchSite_Tripple
				GetLaunchSitesFromFace(Face_Circulator& f);

				LaunchSite_Circulator
				GetNeighbors(ILaunchSite<PrecisionType>* site);

				Face_Circulator
				GetNeighborFaces(LaunchSite_Iterator& site_iterator) const;

				/// Removes the constraining edge between ls1 and ls2. Returns true if this
				/// has been successfully done, false otherwise (also if there is no constraint
				/// at all).
				bool RemoveConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2);

				bool
				IsIntersecting(point3f& a, point3f& b, PrecisionType& t);

				/// Tests if the the vector from v1 to v2 is an edge of the triangulation
				/// and writes the third point to out_v3 if so
				bool
				IsEdge(ILaunchSite<PrecisionType>* v1, ILaunchSite<PrecisionType>* v2, ILaunchSite<PrecisionType>** out_v3);
		};
	}
}
