#pragma once

#include <Model/ILaunchSite.h>
#include "IMeshRepresentation.h"
#include <utility>

using namespace SnowSim::Model;

namespace MeshRepresentations {


	/// Defines an interface for mesh representations
	/// suitable for the snow simulation.
	template<
		typename T,
		class TLaunchSite,
		class TIterator,
		class TVertexCirculator,
		class TEdgeIterator,
		class TFaceIterator,
		class TFaceCirculator>
	class MeshRepresentation
		: public IMeshRepresentation<T, TLaunchSite, TIterator, TVertexCirculator,
			TEdgeIterator, TFaceIterator, TFaceCirculator>
	{
		public:
			typedef IMeshRepresentation<T, TLaunchSite, TIterator, TVertexCirculator, TEdgeIterator, TFaceIterator, TFaceCirculator>
				Base;
			typedef typename Base::LaunchSite_Iterator		LaunchSite_Iterator;
			typedef typename Base::LaunchSite_Circulator	LaunchSite_Circulator;
			typedef typename Base::Edge_Iterator			Edge_Iterator;
			typedef typename Base::LaunchSite_Pair			LaunchSite_Pair;
			typedef typename Base::Face_Iterator			Face_Iterator;
			typedef typename Base::Face_Circulator			Face_Circulator;
			typedef typename Base::LaunchSite_Tripple		LaunchSite_Tripple;

			/// Gets the number of launch sites in this representation.
			virtual unsigned int NumberLaunchSites() = 0;

			virtual ILaunchSite<T>*
			Insert(const point3f& position) = 0;

			virtual LaunchSite_Pair
			InsertConstraint(point3f& a, point3f& b) = 0;

			virtual bool
			InsertConstraint(ILaunchSite<T>* a, ILaunchSite<T>* b) = 0;

			virtual std::pair<ILaunchSite<T>*, T>
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
			GetNeighbors(ILaunchSite<T>* site) = 0;

			virtual Face_Circulator
			GetNeighborFaces(LaunchSite_Iterator& site_iterator) const = 0;

			/// Removes the constraining edge between ls1 and ls2. Returns true if this
			/// has been successfully done, false otherwise (also if there is no constraint
			/// at all).
			virtual bool RemoveConstraint(ILaunchSite<T>* ls1, ILaunchSite<T>* ls2) = 0;

			// remove

			virtual bool
			IsIntersecting(point3f& a, point3f& b, PrecisionType& t) = 0;

			/// Tests if the the vector from v1 to v2 is an edge of the triangulation
			/// and writes the third point to out_v3 if so
			virtual bool
			IsEdge(ILaunchSite<T>* v1, ILaunchSite<T>* v2, ILaunchSite<T>** out_v3) = 0;
	};
};
