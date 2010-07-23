#pragma once

#include "QuadEdge.h"
#include <Utilities/Events/events.h>
#include "MeshChangedEventArgs.h"
#include "../MeshRepresentation.h"
#include <Model/ILaunchSite.h>
#include <Model/ILaunchSite.h>
#include <Exceptions/NotImplementedException.h>
#include <boost/signal.hpp>

namespace MeshRepresentations {
	namespace Voronoi {

		// The following typedefs are necessary to avoid stupid warnings
		// on some compilers:
		typedef Point2d*	Point2dPtr;
		typedef Edge*		EdgePtr;
		typedef float		PrecisionType;

		class IncDelaunay : public MeshRepresentations::MeshRepresentation<PrecisionType> {
		  private:
			Edge *startingEdge;
			Llist edges;
			void DeleteEdge(Edge*);
			Edge *MakeEdge(Boolean = FALSE);
			Edge *MakeEdge(Point2d*, Point2d*, Boolean = FALSE);
			Edge *Connect(Edge*, Edge*);
			Edge *BruteForceLocate(const Point2d&);
			void SplitEdge(Edge*, const Point2d&);
			void Triangulate(Edge*);
			void MarkEdges(Edge*);
			Boolean ForceBruteForceLocate;
			void init();
		  public:
			IncDelaunay(const Point2d&, const Point2d&, const Point2d&);
			IncDelaunay(const Point2d&, const Point2d&, const Point2d&, const Point2d&);
			IncDelaunay(int numVertices, double *bdryVertices);
			~IncDelaunay();

			boost::signal<void (MeshChangedEventArgs&)> Changed;

			Edge *InsertSite(const Point2d&, Real dist = EPS);
			Edge *InsertSiteAt(Edge *e, const Point2d& x, Real dist = EPS); // TODO: Maybe is here a friend function better
			void  InsertEdge(const Point2d&, const Point2d&);
			Edge *Locate(const Point2d&);
			int  numEdges() const { return edges.length(); }
			void ApplyVertices( void (*f)( void *, Point2d * ), void * );
			void ApplyVertices( void (*f)( void *, Edge * ), void * );
			void ApplyEdges( void (*f)( void *, Edge *, Boolean ), void * );

			// Remove a site from the delaunay triangulation
			// One can only remove sites if there are at least
			// four in the triangulation
			void RemoveSite(const Point2d&, Real dist = EPS);

			#pragma region IMeshRepresentation Implementation

			unsigned int NumberLaunchSites()
			{
				return numEdges();
			}

			SnowSim::Model::ILaunchSite<PrecisionType>* GetLaunchSite(unsigned int index);

			void Insert(ILaunchSite<PrecisionType>* site)
			{
				throw SnowSim::Exceptions::NotImplementedException();
			}


			ILaunchSite<PrecisionType>* GetNearestLaunchSite(point3f position)
			{
				throw SnowSim::Exceptions::NotImplementedException();
			}

			#pragma endregion
		};

	}
}
