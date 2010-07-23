#ifndef GEOM2D_H
#define GEOM2D_H

#include <math.h>
#include <stdlib.h>
#include <iostream>
#include "common.h"

#define EPS 1e-6
#define _X 0
#define _Y 1
#define _Z 2

typedef Lreal Real;
using namespace std;

namespace MeshRepresentations {
	namespace Voronoi {

		enum PointType {PointType_Border, PointType_LaunchSite};

		class Vector2d {
		private:
			Real x, y;
			PointType m_type;
		public:
			Vector2d(PointType type = PointType_Border);
			Vector2d(Real a, Real b, PointType type = PointType_Border);

			Vector2d(const Vector2d &v);
			Real& operator[](int i);
			const Real& operator[](int i) const;

			inline Real norm() const;
			inline Real normalize();
			Boolean operator==(const Vector2d&) const;
			inline Vector2d operator+(const Vector2d&) const;
			inline Vector2d operator-(const Vector2d&) const;

			// dot product (cannot overload the . operator)
			Real operator|(const Vector2d&) const;

			PointType Type();

			friend Vector2d operator*(Real, const Vector2d&);
			friend Vector2d operator/(const Vector2d&, Real);

			// another dot product
			friend Real dot(const Vector2d&, const Vector2d&);
			friend istream& operator>>(istream&, Vector2d&);
			friend ostream& operator<<(ostream&, const Vector2d&);
		};

		typedef Vector2d Point2d;

		inline Vector2d operator*(Real c, const Vector2d& v);
		Vector2d operator/(const Vector2d& v, Real c);
		inline Real dot(const Vector2d& u, const Vector2d& v);


		class Line {

		public:
			Line();

			// Computes the normalized line equation through the points p and q.
			Line(const Point2d&, const Point2d&);
			void set(const Point2d&, const Point2d&);

			// Plugs point p into the line equation.
			inline Real eval(const Point2d&) const;

			// Returns -1, 0, or 1, if p is to the left of, on,
			// or right of the line, respectively.
			int classify(const Point2d&) const;

			// Returns the intersection of the line with the segment (p1,p2)
			Point2d intersect(const Point2d&, const Point2d&) const;
			int clip(const Point2d&, const Point2d&, Point2d& result) const;

		private:
			Real a, b, c;
		};

		// Returns TRUE if point is on the line (actually, on the EPS-slab
		// around the line).
		Boolean operator==(const Point2d& point, const Line& line);

		// Returns TRUE if point is to the left of the line (left to
		// the EPS-slab around the line).
		Boolean operator<(const Point2d& point, const Line& line);

		inline ostream& operator<<(ostream& os, const Vector2d& v);

		inline istream& operator>>(istream& is, Vector2d& v);

	}
}

#endif
