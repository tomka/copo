#include "IncDelaunay.h"

namespace MeshRepresentations {
	namespace Voronoi {

		IncDelaunay::~IncDelaunay()
		{
			QuadEdge *qp;
			for (LlistPos p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				qp = (QuadEdge *)edges.retrieve(p);
				delete qp;
			}
		}

		void IncDelaunay::init() {
			this->ForceBruteForceLocate = FALSE;
		}

		// Initialize the CDT_Mesh to the triangle defined by the points a, b, c.
		IncDelaunay::IncDelaunay(const Point2d& a, const Point2d& b, const Point2d& c)
		{
			init();
			Point2d *da = new Point2d(a);
			Point2d *db = new Point2d(b);
			Point2d *dc = new Point2d(c);

			Edge* ea = MakeEdge(da, db, TRUE);
   			Edge* eb = MakeEdge(db, dc, TRUE);
			Edge* ec = MakeEdge(dc, da, TRUE);

			// Connect edges together:
			Splice(ea->Sym(), eb);
			Splice(eb->Sym(), ec);
			Splice(ec->Sym(), ea);

			startingEdge = ec;
		}

		// Initialize the CDT_Mesh to the Delaunay triangulation of
		// the quadrilateral defined by the points a, b, c, d.
		// NOTE: quadrilateral is assumed convex.
		IncDelaunay::IncDelaunay(const Point2d& a, const Point2d& b,
				   const Point2d& c, const Point2d& d)
		{
			init();

			Boolean InCircle(const Point2d&, const Point2d&,
							 const Point2d&, const Point2d&);
			Point2d *da = new Point2d(a);
			Point2d *db = new Point2d(b);
			Point2d *dc = new Point2d(c);
			Point2d *dd = new Point2d(d);

			Edge* ea = MakeEdge(da, db, TRUE);
			Edge* eb = MakeEdge(db, dc, TRUE);
			Edge* ec = MakeEdge(dc, dd, TRUE);
			Edge* ed = MakeEdge(dd, da, TRUE);

			// Connect edges together:
			Splice(ea->Sym(), eb);
			Splice(eb->Sym(), ec);
			Splice(ec->Sym(), ed);
			Splice(ed->Sym(), ea);

			// Split into two triangles:
			if (InCircle(c, d, a, b)) {
				// Connect d to b:
				startingEdge = Connect(ec, eb);
			} else {
				// Connect c to a:
				startingEdge = Connect(eb, ea);
			}
		}

		IncDelaunay::IncDelaunay( int numVertices, double *bdryVertices )
		// Initialize the CDT_Mesh to the Delaunay triangulation of the
		// convex polygon defined by the coordinates in the bdryVertices
		// array. The number of vertices (coordinate pairs) is specified
		// via numVertices.
		// NOTE: polygon is assumed convex.
		{
			init();

			Point2d orig, dest;
			Edge *e0, *e1;
			register int i;

			Assert(numVertices >= 3);

			Point2d **verts = new Point2dPtr[numVertices + 1];
			Edge **edges = new EdgePtr[numVertices + 1];

			// Create all vertices:
			for (i = 0; i < numVertices; i++) {
				verts[i] = new Point2d(bdryVertices[2*i], bdryVertices[2*i+1]);
			}
			verts[numVertices] = verts[0];

			// Create all edges:
			for (i = 0; i < numVertices; i++) {
				edges[i] = MakeEdge(verts[i], verts[i+1], TRUE);
			}
			edges[numVertices] = edges[0];

			// Connect edges together:
			for (i = 0; i < numVertices; i++) {
				Splice(edges[i]->Sym(), edges[i+1]);
			}

			// Triangulate:
			Triangulate(edges[0]);

			// Initialize starting edge:
			startingEdge = edges[0];

			delete[] verts;
			delete[] edges;
		}

		// Shorten edge e s.t. its destination becomes x. Connect
		// x to the previous destination of e by a new edge. If e
		// is constrained, x is snapped onto the edge, and the new
		// edge is also marked as constrained.
		void IncDelaunay::SplitEdge(Edge *e, const Point2d& x)
		{
			Point2d *dt;

			if (e->isConstrained()) {
				// snap the point to the edge before splitting:
				dt = new Point2d(snap(x, e->Org2d(), e->Dest2d()));
			} else
				dt = new Point2d(x);

			Edge *t  = e->Lnext();
			Splice(e->Sym(), t); // uncouple destination of e
			e->EndPoints(e->Org(), dt);
			Edge *ne = Connect(e, t);

			#pragma region Old
			/*
			LaunchSite* ref = e->SiteReference();
			if (ref) {
				ref->SetReference(ne->Sym());
			}
			*/
			#pragma endregion

			if (e->isConstrained())
				ne->Constrain();
		}

		void IncDelaunay::RemoveSite(const Point2d&, Real dist)
		{
			// TODO, Loeschen erstmal nicht implementieren. Ist eine Optimierung
		}

		void IncDelaunay::Triangulate(Edge *first)
		// Triangulates the left face of first, which is assumed to be closed.
		// It is also assumed that all the vertices of that face lie to the
		// left of last (the edge preceeding first).
		// This is NOT intended as a general simple polygon triangulation
		// routine. It is called by InsertEdge in order to restore a
		// triangulation after an edge has been inserted.
		// The routine consists of two loops: the outer loop continues as
		// long as the left face is not a triangle. The inner loop examines
		// successive triplets of vertices, creating a triangle whenever the
		// triplet is counterclockwise.
		{
			Edge *a, *b, *e, *t1, *t2, *last = first->Lprev();

			while (first->Lnext()->Lnext() != last) {
				e = first->Lnext();
				t1 = first;
				while (e != last) {
					t2 = e->Lnext();
					if (t2 == last && t1 == first)
						break;
					if (LeftOf(e->Dest2d(), t1)) {
						if (t1 == first)
							t1 = first = Connect(e, t1)->Sym();
						else
							t1 = Connect(e, t1)->Sym();
						a = t1->Oprev(), b = t1->Dnext();
						FixEdge(a);
						FixEdge(b);
						e = t2;
					} else {
						t1 = e;
						e = t2;
					}
				}
			}
			a = last->Lnext(), b = last->Lprev();
			FixEdge(a);
			FixEdge(b);
			FixEdge(last);
		}

		Edge* IncDelaunay::InsertSiteAt(Edge *e, const Point2d& x, Real dist) {
			if (e == NIL(Edge))
				return e;

			// lies x on the origin of e?
			if (coincide(x, e->Org2d(), dist))
				return e; // yes, success

			// lies x on the destination of e?
			if (coincide(x, e->Dest2d(), dist))
				return e->Sym(); // yes, success

			// is there a left face beside e?
			Boolean hasLeft = hasLeftFace(e);
			// is there a right face beside e?
			Boolean hasRight = hasRightFace(e);

			// has the edge faces at all?
			if (!hasLeft && !hasRight) {
				Warning("InsertSite: edge does not have any faces");
				return NIL(Edge); // no, error
			}

			// x should be inside a face adjacent to e:
			// so, lies x on e?
			Boolean onEdge = OnEdge(x, e);
			// is x in the left face or in the right face of e?
			Boolean insideLeft  = hasLeft && (onEdge || LeftOf(x, e)) &&
								  RightOf(x, e->Onext()) && RightOf(x, e->Dprev());
			Boolean insideRight = hasRight && (onEdge || RightOf(x, e)) &&
								  LeftOf(x, e->Oprev()) && LeftOf(x, e->Dnext());

			// is x is not in left and not in the right face?
			if (!insideLeft && !insideRight) {
				Warning("InsertSite: point not in a face adjacent to edge");
				return NIL(Edge); // not an any face, error
			}

			// TOMTODO: Was genau wird hier getestet?
			// lies x on the origin point of the cw edge (=before) us?
			if (insideLeft && coincide(x, e->Onext()->Dest2d(), dist))
				return e->Lprev(); // yes, success - previous edge
			//
			if (insideRight && coincide(x, e->Oprev()->Dest2d(), dist))
				return e->Dnext(); // yes, success

			// save the launch site reference, in case e gets changed
			ILaunchSite<PrecisionType>* ref = e->SiteReference();
			ILaunchSite<PrecisionType>* symref = ref = e->Sym()->SiteReference();
			Point2d symOrigin = Point2d(e->Org2d()[0], e->Org2d()[1]);

			// Now we know, x lies within the quadrilateral (= Viereck ;)) whose diagonal is e
			// (or a triangle, if e only has one adjacent face). We also know x
			// is not one of e's endpoints.
			if (onEdge) {
				// x lies on e

				// snap x to e, and check for coincidence:
				Point2d xx = snap(x, e->Org2d(), e->Dest2d());

				// TOMTODO/TOMASK: Warum n�ssen wir nochmal testen hier?
				// am Anfang wurde doch schon getestet?
				if (coincide(xx, e->Org2d(), dist))
					return e; // yes, success - x lies on the origin of e
				if (coincide(xx, e->Dest2d(), dist))
					return e->Sym(); // yes, success - x lies on the destination of e, so return symetric edge


				MeshChangedEventArgs args = MeshChangedEventArgs();

				// does the edge lies between two faces?
				if (hasRight && hasLeft) {
					// yes, has two faces
					Edge *a, *b, *c, *d;

					// 2 trianlges: left side - e c d, right side - e-Sym() a b
					a = e->Oprev(); // next cw edge, starting at origin e
					b = e->Dnext(); // next ccw edge, rotating around destination of e
					c = e->Lnext(); // next edge around left face after e
					d = e->Lprev(); // previous edge around face before e

					// shorten e: connect e->Org to x and x to e->Dest
					SplitEdge(e, x);
					// connect (the now shortened) e with the origin of the prvious edge (d)
					Connect(e, e->Lprev());
					// connect next edge around right face to the origin of (the now shortened) e symatric
					Connect(e->Oprev(), e->Sym());

					FixEdge(a);
					FixEdge(b);
					FixEdge(c);
					FixEdge(d);

					// For LaunchSites:
					// Removed: original e (is nececarry!) only if e-ref != 0
					// Added:	edge with origin of
					//			x - the new point

					// since e has still the same origin as before
					// we only need to regard the reference of the inverse
					// (since the edge got shortened)
					if (symref) {
						symref->SetReference(Locate(symOrigin));
					}
					args.addNewPoint(e->Sym()); // Sollte das nicht sowieso gemacht werden und muss hier nicht nochma extra hin?

				} else {
					// has only one face
					if (hasRight)
						e = e->Sym();
					// get next edge around left face
					Edge *c = e->Lnext();
					// get prvious face around left face
					Edge *d = e->Lprev();
					// split the edge
					SplitEdge(e, x); // remove e, connect e->Org to x and x to e->Dest
					Connect(e, e->Lprev());
					FixEdge(c);
					FixEdge(d);
				}
				startingEdge = e->Sym();

				Changed(args);

				return startingEdge;
			}

			// x is not on e, should be in face to the left of e:
			if (!insideLeft) {
				Warning("InsertSite: point is not to the left of edge");
				return NIL(Edge);
			}

			// x should be strictly inside the left face:
			if (OnEdge(x, e->Onext()) || OnEdge(x, e->Dprev())) {
				Warning("InsertSite: point is not strictly inside face");
				return NIL(Edge);
			}

			// Now, hopefully, x is strictly inside the left face of e,
			// so everything should proceed smoothly from this point on.

   			Point2d* first = e->Org();
   			// make a new edge
			Edge* base = MakeEdge(e->Org(), new Point2d(x));
			// connect origins of the new edge and e
			Splice(base, e);
			// use this edge as the starting point next time:
			startingEdge = base->Sym();
			// go through all vertices of this face
			// and connect each one to the new point
			do {
				base = Connect(e, base->Sym());
				e = base->Oprev();
			} while (e->Dprev() != startingEdge);

			// Examine suspect edges to ensure that the Delaunay condition
			// is satisfied.
   			do {
				Edge* t = e->Oprev();
				if (!e->isConstrained() &&
					InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x)) {
						Swap(e);
						e = e->Oprev();
					}
				else if (e->Lprev() == startingEdge) {  // no more suspect edges
					Changed(MeshChangedEventArgs(ref));
					return startingEdge;
				}
				else  // pop a suspect edge
					e = e->Onext()->Lprev();
			} while (TRUE);

			Changed(MeshChangedEventArgs(ref));
		}

		Edge* IncDelaunay::MakeEdge(Boolean constrained)
		{
			QuadEdge *qe = new QuadEdge(constrained);
			qe->p = edges.insert(edges.first(), qe);
			return qe->edges();
		}

		Edge* IncDelaunay::MakeEdge(Point2d *a, Point2d *b, Boolean constrained)
		{
			Edge *e = MakeEdge(constrained);
			e->EndPoints(a, b);
			return e;
		}

		void IncDelaunay::InsertEdge(const Point2d& a, const Point2d& b)
		// Inserts a constraining edge into a CDT.
		{
			Edge *ea, *eb;
			Point2d aa, bb;
			if (ea = InsertSite(a))
				aa = ea->Org2d();
			if (eb = InsertSite(b))
				bb = eb->Org2d();
			if (ea == NIL(Edge) || eb == NIL(Edge)) {
				Warning("InsertEdge: could not insert endpoints of edge");
				return;
			}

			startingEdge = ea;
			if ((ea = Locate(aa)) == NIL(Edge)) {
				Warning("InsertEdge: could not locate an endpoint");
				return;
			}
			if (!(aa == ea->Org2d()))
				ea = ea->Sym();
			if (!(aa == ea->Org2d())) {
				Warning("InsertEdge: point a is not an endpoint of ea");
				return;
			}

			if (aa == bb) {
				Warning("InsertEdge: both ends map to same vertex");
				return;
			}

			// aa and bb are vertices in the CDT_Mesh; our goal is to connect
			// them by a sequence of one or more edges.

			Line ab(aa, bb);
			Vector2d dd = bb - aa;
			Edge *t, *ne;
			Point2d *last = eb->Org();

			while (ea->Org() != last) {

				// Set ea to the first edge to the right of (or aligned with)
				// the segment (a,b), by moving ccw around the origin of ea:
				t = ea;
				do {
					if ((ea->Dest2d() == ab && ((ea->Dest2d() - aa)|dd) > 0))
						break;
					if (ea->Onext()->Dest2d() == ab &&
						((ea->Onext()->Dest2d() - aa)|dd) > 0) {
							ea = ea->Onext();
							break;
						}
					if (!cw(ea->Dest2d(), bb, aa) && cw(ea->Onext()->Dest2d(), bb, aa))
						break;
					ea = ea->Onext();
					if (ea == t) {
						Warning("InsertEdge: infinite loop");
						return;
					}
				} while (TRUE);

				// check to see if an edge is already there:
				if (ea->Dest2d() == ab) {
					ea->Constrain();
					aa = ea->Dest2d();
					if (aa == bb)
						return;
					ab.set(aa, bb);
					dd = bb - aa;
					ea = ea->Sym()->Onext();
					continue;
				}

				t = ea;

				while (TRUE) {
					if (t->Lnext()->Dest2d() == ab) {
						if (t->Lnext()->Lnext()->Lnext() == ea) {
							// edge is already there
							t->Lnext()->Lnext()->Constrain();
							ea = t->Lnext()->Sym();
							break;
						} else {
							// need a new edge
							ne = Connect(t->Lnext(), ea); ne->Constrain();
							ea = t->Lnext()->Sym();
							Triangulate(ne->Lnext());
							Triangulate(ne->Oprev());
							break;
						}
					}
					if (t->Lnext()->Dest2d() < ab) {
						// edges cross
						if (!t->Lnext()->isConstrained()) {
							DeleteEdge(t->Lnext());
						} else {
							// the crossing edge is also constrained
							// compute and insert the intersection
							Point2d x = Intersect(t->Lnext(), ab);
							// split t->Lnext() into two at x
							SplitEdge(t->Lnext(), x);
							// connect to the new vertex:
							ne = Connect(t->Lnext(), ea);
							ne->Constrain();
							ea = t->Lnext()->Sym();
							Triangulate(ne->Lnext());
							Triangulate(ne->Oprev());
							break;
						}
					} else {
						t = t->Lnext();
					}
				}
			}
		}

		Edge* IncDelaunay::Locate(const Point2d& x)
		// The goals of this routine are as follows:
		// - if x coincides with one or more vertices in the CDT_Mesh, return an edge
		//   whose origin is one such vertex;
		// - otherwise, return the edge nearest to x oriented ccw around a face
		//   containing x (up to numerical round-off);
		// In the event of failure, we pass the search to BruteForceLocate.
		{
		if (this->ForceBruteForceLocate)
				return BruteForceLocate(x);

			Edge* e = startingEdge;
			int iterations = 0;

			while (TRUE) {
				iterations++;

				if (iterations > numEdges()) {
					Warning("Locate: infinite loop");
					return BruteForceLocate(x);
				}

				// first, examine the endpoints for coincidence:
				if (x == e->Org2d())
					return startingEdge = e;
				if (x == e->Dest2d())
					return startingEdge = e->Sym();

				// x isn't one of the endpoints.
				// orient e s.t. x is not to its right:
				if (RightOf(x, e))
					e = e->Sym();

				// x is not to the right of e.
				// now we need to test if x is in the left face of e:
				if (hasLeftFace(e)) {

					// does x coincide with the third vertex?
					if (x == e->Lprev()->Org2d())
						return startingEdge = e->Lprev();

					if (LeftOf(x, e->Onext())) {
						e = e->Onext();
						continue;
					}
					if (LeftOf(x, e->Dprev())) {
						e = e->Dprev();
						continue;
					}

					// x is inside the left face of e!
					// return the edge nearest to x:
					Line l1(e->Org2d(), e->Dest2d());
					Line l2(e->Dest2d(), e->Lprev()->Org2d());
					Line l3(e->Lprev()->Org2d(), e->Org2d());
					switch (_imin(fabs(l1.eval(x)), fabs(l2.eval(x)), fabs(l3.eval(x)))) {
					  case 0:
						startingEdge = e; break;
					  case 1:
						startingEdge = e->Lnext(); break;
					  case 2:
						startingEdge = e->Lprev(); break;
					}
					return startingEdge;
				}

				// there is no left face!
				if (OnEdge(x, e))
					return startingEdge = e->Sym();

				// ok, let's try to get closer using the edges of the right face
				if (RightOf(x, e->Oprev())) {
					e = e->Oprev()->Sym();
					continue;
				}
				if (RightOf(x, e->Dnext())) {
					e = e->Dnext()->Sym();
					continue;
				}

				Warning("Locate: I'm stuck and can't move on - trying brute force search");
				return BruteForceLocate(x);
			}
		}

		Edge* IncDelaunay::BruteForceLocate(const Point2d& x)
		// Same as Locate, but uses a brute force exhaustive search.
		{
			Edge *e;

			for (LlistPos p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {

				e = ((QuadEdge *)edges.retrieve(p))->edges();
				// first, examine the endpoints for coincidence:
				if (x == e->Org2d())
					return startingEdge = e;
				if (x == e->Dest2d())
					return startingEdge = e->Sym();

				// x isn't one of the endpoints.
				// orient e s.t. x is not to its right:
				if (RightOf(x, e))
					e = e->Sym();

				// x is not to the right of e.
				// now we need to test if x is in the left face of e:
				if (hasLeftFace(e)) {

					// does x coincide with the third vertex?
					if (x == e->Lprev()->Org2d())
						return startingEdge = e->Lprev();

					if (!RightOf(x, e->Lprev()) && !RightOf(x, e->Lnext())) {
						// x is in this face!
						// return the edge nearest to x:
						Line l1(e->Org2d(), e->Dest2d());
						Line l2(e->Dest2d(), e->Lprev()->Org2d());
						Line l3(e->Lprev()->Org2d(), e->Org2d());
						switch(_imin(fabs(l1.eval(x)), fabs(l2.eval(x)), fabs(l3.eval(x)))) {
						  case 0:
							startingEdge = e; break;
						  case 1:
							startingEdge = e->Lnext(); break;
						  case 2:
							startingEdge = e->Lprev(); break;
						}
						return startingEdge;
					} else
						continue;
				}

				// there is no left face!
				if (OnEdge(x, e))
					return startingEdge = e->Sym();
				else
					continue;

			}

			// if we're here, it must be because we failed :-{
			Warning("Locate: Could not locate using brute force");
			return NIL(Edge);
		}

		Edge *IncDelaunay::InsertSite(const Point2d& x, Real dist /* = EPS */)
		// Inserts a new site into a CDT. This is basically the Guibas-Stolfi
		// incremental site insertion algorithm, except that is does not flip
		// constraining edges (in fact, it does not even test them.)
		// Returns NIL(Edge) if insertion has failed; otherwise, returns an edge
		// whose origin is the new site.
		{
			// get the nearest edge to x
			Edge *e = Locate(x);

			// insert x at e
			return InsertSiteAt(e, x, dist);

		}

		// Add a new edge e connecting the destination of a to the
		// origin of b, in such a way that all three have the same
		// left face after the connection is complete.
		// Additionally, the data pointers of the new edge are set.
		Edge* IncDelaunay::Connect(Edge* a, Edge* b)
		{
			// make a new edge connecting destination of a
			// and origin of b
			Edge* e = MakeEdge(a->Dest(), b->Org());
			Splice(e, a->Lnext());
			Splice(e->Sym(), b);
			return e;
		}

		void IncDelaunay::DeleteEdge(Edge* e)
		{
			// Make sure the starting edge does not get deleted:
			if (startingEdge->QEdge() == e->QEdge()) {
				Warning("IncDelaunay::DeleteEdge: attempting to delete starting edge");
				// try to recover:
				startingEdge = (e != e->Onext()) ? e->Onext() : e->Dnext();
				Assert(startingEdge->QEdge() != e->QEdge());
			}

			// remove edge from the edge list:
			QuadEdge *qe = e->QEdge();
			edges.remove(edges.prev(qe->p));
			delete qe;
		}

		inline void IncDelaunay::MarkEdges(Edge *e)
		{
			register Edge *t = e;
			do {
				t->mark = VISITED;
				t = t->Onext();
			} while (t != e);
		}

		void IncDelaunay::ApplyEdges( void (*f)( void *arg, Edge *edge, Boolean ), void *arg )
		// Applies the function f to every edge in the edge-list. The function
		// takes application dependent arguments pointed to by arg, a pointer
		// to the edge, and a flag that specifies whether or not the edge is
		// constrained. This last argument allows the application to handle
		// constrained edges differently.
		{
			register QuadEdge *qp;
			register LlistPos p;

			for (p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				qp = (QuadEdge *)edges.retrieve(p);
				f(arg, qp->edges(), qp->isConstrained());
			}
		}

		void IncDelaunay::ApplyVertices( void (*f)( void *arg, Point2d *vertex ), void *arg )
		// Applies the function f to every vertex in the CDT_Mesh. This is done
		// similarly to ApplyEdges, but care must be taken to apply only once
		// to each vertex. Every time f is applied to a vertex, all edges
		// originating at that vertex are marked as VISITED. When we traverse
		// the edge-list we only apply f to the origins of UNVISITED edges.
		// After the traversal is complete, we go over the edges once more to
		// reset the marks.
		{
			register Edge *e;
			register LlistPos p;

			for (p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				e = ((QuadEdge *)edges.retrieve(p))->edges();
				if (e->mark == UNVISITED) {
					f(arg, e->Org());
					MarkEdges(e);
				}
				if (e->Sym()->mark == UNVISITED) {
					f(arg, e->Sym()->Org());
					MarkEdges(e->Sym());
				}
			}

			for (p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				e = ((QuadEdge *)edges.retrieve(p))->edges();
				e->mark = e->Sym()->mark = UNVISITED;
			}
		}

		void IncDelaunay::ApplyVertices( void (*f)( void *arg, Edge *edge ), void *arg )
		// Applies the function f to every vertex in the CDT_Mesh. This is done
		// similarly to ApplyEdges, but care must be taken to apply only once
		// to each vertex. Every time f is applied to a vertex, all edges
		// originating at that vertex are marked as VISITED. When we traverse
		// the edge-list we only apply f to the origins of UNVISITED edges.
		// After the traversal is complete, we go over the edges once more to
		// reset the marks.
		{
			register Edge *e;
			register LlistPos p;

			for (p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				e = ((QuadEdge *)edges.retrieve(p))->edges();
				if (e->mark == UNVISITED) {
					f(arg, e);
					MarkEdges(e);
				}
				if (e->Sym()->mark == UNVISITED) {
					f(arg, e->Sym());
					MarkEdges(e->Sym());
				}
			}

			for (p = edges.first(); !edges.isEnd(p); p = edges.next(p)) {
				e = ((QuadEdge *)edges.retrieve(p))->edges();
				e->mark = e->Sym()->mark = UNVISITED;
			}
		}

		SnowSim::Model::ILaunchSite<PrecisionType>* IncDelaunay::GetLaunchSite(unsigned int index)
		{
			return NULL;
		}
	}
}
