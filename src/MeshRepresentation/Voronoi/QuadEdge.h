#pragma once
#include "../../types.h"
#include "geom2d.h"
#include "dllist.h"
#include <Utilities/Events/events.h>
#include <Model/ILaunchSite.h>

using namespace zx;
using namespace SnowSim::Model;

namespace MeshRepresentations {
	namespace Voronoi {

		class QuadEdge;
		class CDT_Mesh;
		class MeshChangedEventArgs;

		#define UNVISITED 0
		#define VISITED   1

		class Edge {
			friend class QuadEdge;
			friend class CDT_Mesh;
			friend class IncDelaunay;
			friend void Splice(Edge*, Edge*);

		private:
			int num, mark;
			Edge *next;
			Point2d *data;
			ILaunchSite<PrecisionType>* sitereference;

		public:
			Edge(ILaunchSite<PrecisionType>* ls = NULL);
			Edge* Rot();
			Edge* invRot();
			Edge* Sym();
			Edge* Onext();
			Edge* Oprev();
			Edge* Dnext();
			Edge* Dprev();
			Edge* Lnext();
			Edge* Lprev();
			Edge* Rnext();
			Edge* Rprev();
			Point2d* Org();
			Point2d* Dest();
			const Point2d& Org2d() const;
			const Point2d& Dest2d() const;
			void EndPoints(Point2d*, Point2d*);
			void Constrain();
			Boolean isConstrained();
			Boolean isPartOfBorderTriangle();
			QuadEdge* QEdge() const    { return (QuadEdge *)(this - num); }
			ILaunchSite<PrecisionType>* SiteReference() { return sitereference; }
			void SetSiteReference(ILaunchSite<PrecisionType> * ref);
		};

		class QuadEdge {
			friend class CDT_Mesh;
			friend class IncDelaunay;
		  private:
			Edge e[4];
			Boolean c;
			LlistPos p;
		  public:
			QuadEdge(Boolean);
			Edge *edges()           { return e; }
			Boolean isConstrained() { return c; }
			void Constrain()        { c = TRUE; }
			~QuadEdge();
		};

		class CDT_Mesh {
		  private:
			Edge *startingEdge;
			Llist edges;
			void DeleteEdge(Edge*);
			Edge *MakeEdge(Boolean);
			Edge *MakeEdge(Point2d*, Point2d*, Boolean);
			Edge *Connect(Edge*, Edge*);
			Edge *BruteForceLocate(const Point2d&);
			void SplitEdge(Edge*, const Point2d&);
			void Triangulate(Edge*);
			void MarkEdges(Edge*);
		  public:
			CDT_Mesh(const Point2d&, const Point2d&, const Point2d&);
			CDT_Mesh(const Point2d&, const Point2d&, const Point2d&, const Point2d&);
			CDT_Mesh(int numVertices, double *bdryVertices);

			event<MeshChangedEventArgs&> Changed;

			Edge *InsertSite(const Point2d&, Real dist = EPS);
			Edge *InsertSiteAt(Edge *e, const Point2d& x, Real dist = EPS); // TODO: Maybe is here a friend function better
			void  InsertEdge(const Point2d&, const Point2d&);
			Edge *Locate(const Point2d&);
			int  numEdges() const { return edges.length(); }
			void ApplyVertices( void (*f)( void *, Point2d * ), void * );
			void ApplyVertices( void (*f)( void *, Edge * ), void * );
			void ApplyEdges( void (*f)( void *, Edge *, Boolean ), void * );
			~CDT_Mesh();
		};

		Boolean InCircle(const Point2d& a, const Point2d& b,
						 const Point2d& c, const Point2d& d);

		Point2d CircumCenter(const Point2d& a, const Point2d& b, const Point2d& c);

		/// Project x on the line between a and b
		Point2d snap(const Point2d& x, const Point2d& a, const Point2d& b);

		// Returns TRUE if the point x is strictly to the right of e
		Boolean RightOf(const Point2d& x, Edge* e);

		// Returns TRUE if the point x is strictly to the left of e
		Boolean LeftOf(const Point2d& x, Edge* e);

		// A simple, but recursive way of doing things.
		// Flips e, if necessary, in which case calls itself
		// recursively on the two edges that were to the right
		// of e before it was flipped, to propagate the change.
		// returns true if edge had to be fixed
		bool FixEdge(Edge *e);

		// A simple, but recursive way of doing things.
		// Flips e, if necessary, in which case calls itself
		// recursively on the two edges that were to the right
		// of e before it was flipped, to propagate the change.
		// The args paremeter will be filled with relevant data
		bool FixEdge(Edge *e, MeshChangedEventArgs& args);

		// Returns TRUE if the points a and b are closer than dist to each other.
		// This is useful for creating nicer CDT_Meshes, by preventing points from
		// being too close to each other.
		static Boolean coincide(const Point2d& a, const Point2d& b, Real dist)
		{
			Vector2d d = a - b;

			if (fabs(d[0]) > dist || fabs(d[1]) > dist)
				return FALSE;

			return ((d|d) <= dist*dist);
		}

		// Returns TRUE if there is a triangular face to the left of e.
		Boolean hasLeftFace(Edge *e);

		// Returns TRUE if there is a triangular face to the right of e.
		Boolean hasRightFace(Edge *e);

		// A predicate that determines if the point x is on the edge e.
		// The point is considered on if it is in the EPS-neighborhood
		// of the edge.
		Boolean OnEdge(const Point2d& x, Edge* e);

		// Essentially turns edge e counterclockwise inside its enclosing
		// quadrilateral. The data pointers are modified accordingly.
		void Swap(Edge* e);

		// Essentially turns edge e counterclockwise inside its enclosing
		// quadrilateral. The data pointers are modified accordingly.
		void Swap(Edge* e, MeshChangedEventArgs& args);

		// Returns TRUE if the points a, b, c are in a clockwise order
		Boolean cw(const Point2d& a, const Point2d& b, const Point2d& c);

		// Returns the intersection point between e and l (assumes it exists).
		Point2d Intersect(Edge *e, const Line& l);

		// This operator affects the two edge rings around the origins of a and b,
		// and, independently, the two edge rings around the left faces of a and b.
		// In each case, (i) if the two rings are distinct, Splice will combine
		// them into one; (ii) if the two are the same ring, Splice will break it
		// into two separate pieces.
		// Thus, Splice can be used both to attach the two edges together, and
		// to break them apart. See Guibas and Stolfi (1985) p.96 for more details
		// and illustrations.
		void Splice(Edge* a, Edge* b);
	}
}
