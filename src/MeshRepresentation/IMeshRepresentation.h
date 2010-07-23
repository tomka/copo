#pragma once

#include <Model/ILaunchSite.h>
#include <types.h>
#include <utility>
#include <vector>

using namespace SnowSim::Model;

namespace MeshRepresentations {

	/// Defines an interface for mesh representations
	/// suitable for the snow simulation.
	template<typename TPrecision, class TLaunchSite,
		class TVertexIterator, class TVertexCirculator, class TEdgeIterator,
		class TFaceIterator, class TFaceCirculator>
	class IMeshRepresentation
	{
		public:

			typedef TVertexIterator				LaunchSite_Iterator;
			typedef TVertexCirculator			LaunchSite_Circulator;
			typedef TEdgeIterator				Edge_Iterator;
			typedef TFaceIterator				Face_Iterator;
			typedef TFaceCirculator				Face_Circulator;
			typedef ILaunchSite<TPrecision>*	LSPtr;
			typedef std::pair<LSPtr, LSPtr>		LaunchSite_Pair;

			struct LaunchSite_Tripple
			{
				LSPtr first;
				LSPtr second;
				LSPtr third;
			};

			/////////////////
			// Information
			/////////////////

			/// Gets the number of launch sites in this representation.
			virtual unsigned int
			NumberLaunchSites() = 0;

			// Get the nearest launch site and the distance to it
			virtual std::pair<ILaunchSite<TPrecision>*, TPrecision>
			GetNearestLaunchSite(point3f& position) = 0;

			virtual bool
			Are_there_incident_constraints(LaunchSite_Iterator& site_iterator) = 0;

			// circulates over all neighbours of the given launch site iterator
			virtual LaunchSite_Circulator
			GetNeighbors(LaunchSite_Iterator& site_iterator) const = 0;

			// circulates over all neighbours of the given launch site
			virtual LaunchSite_Circulator
			GetNeighbors(ILaunchSite<TPrecision>* site) = 0;

			// circulates over all neighbor faces of the given launch site
			virtual Face_Circulator
			GetNeighborFaces(LaunchSite_Iterator& site_iterator) const = 0;

			// Returns true iff e is a constrained edge
			virtual bool
			IsConstrained(Edge_Iterator& e) = 0;

			// Gets start and end launch site of an edge
			virtual LaunchSite_Pair
			GetLaunchSitesFromEdge(Edge_Iterator& e) = 0;

			// Gets the three corners of a delaunay face
			virtual LaunchSite_Tripple
			GetLaunchSitesFromFace(Face_Iterator& f) = 0;

			// Gets the three corners of a delaunay face
			virtual LaunchSite_Tripple
			GetLaunchSitesFromFace(Face_Circulator& f) = 0;

			/////////////////
			// Modification
			/////////////////

			// Inserts a new launch site in the mesh representation
			virtual ILaunchSite<TPrecision>*
			Insert(const point3f& position) = 0;

			// Inserts a constraint into the mesh representation
			/// Launch sites are newly created
			virtual LaunchSite_Pair
			InsertConstraint(point3f& a, point3f& b) = 0;

			// Inserts a constraint into the mesh representation
			/// Launch sites are not newly created
			virtual bool
			InsertConstraint(ILaunchSite<TPrecision>* a, ILaunchSite<TPrecision>* b) = 0;

			// Computes responsibility area for every launch site
			virtual void
			CreateLaunchSiteResponsibilities() = 0;

			/// Tests if the the vector from v1 to v2 is an edge of the triangulation
			/// and writes the third point to out_v3 if so
			virtual bool
			IsEdge(LSPtr v1, LSPtr v2, LSPtr* out_v3) = 0;

			/// Removes the constraining edge between ls1 and ls2. Returns true if this
			/// has been successfully done, false otherwise (also if there is no constraint
			/// at all).
			virtual bool RemoveConstraint(ILaunchSite<TPrecision>* ls1, ILaunchSite<TPrecision>* ls2) = 0;

			// remove

			/////////////////
			// Traversal
			/////////////////

			// Gets a launch site iterator positioned at the beginning
			virtual LaunchSite_Iterator
			LaunchSites_Begin() = 0;

			// Gets a past-the-end launch site iterator
			virtual LaunchSite_Iterator
			LaunchSites_End() = 0;

			// Gets an edge iterator, positioned at an arbitrary finite edge
			virtual Edge_Iterator
			Edges_Begin() = 0;

			// Gets an past-the-end edge iterator
			virtual Edge_Iterator
			Edges_End() = 0;

			// Gets a face iterator, positioned at an arbitrary face
			virtual Face_Iterator
			Faces_Begin() = 0;

			// Gets an past-the-end face iterator
			virtual Face_Iterator
			Faces_End() = 0;

			// Tests if the segment from a to b intersects the mesh representation
			// Moreover is t set to the parameter for the parameterized line from a through b.
			virtual bool
			IsIntersecting(point3f& a, point3f& b, PrecisionType& t) = 0;
	};
};
