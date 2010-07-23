#include "geom2d.h"

namespace MeshRepresentations {
	namespace Voronoi {

		// Vector2d:

		Vector2d::Vector2d(PointType type)
		{
			x = 0; y = 0; m_type = type;
		}

		Vector2d::Vector2d(Real a, Real b, PointType type)
		{
			x = a; y = b; m_type = type;
		}

		Vector2d::Vector2d(const Vector2d &v)
		{
			*this = v;
		}

		Real Vector2d::norm() const
		{
			return sqrt(x * x + y * y);
		}

		Real Vector2d::normalize()
		{
			Real len = norm();

			if (len == 0.0)
				cerr << "Vector2d::normalize: Division by 0\n";
			else {
				x /= len;
				y /= len;
			}
			return len;
		}

		Real& Vector2d::operator[](int i)
		{
			return ((Real *)this)[i];
		}

		const Real& Vector2d::operator[](int i) const
		{
			return ((Real *)this)[i];
		}

		Vector2d Vector2d::operator+(const Vector2d& v) const
		{
			return Vector2d(x + v.x, y + v.y);
		}

		Vector2d Vector2d::operator-(const Vector2d& v) const
		{
			return Vector2d(x - v.x, y - v.y);
		}

		Boolean Vector2d::operator==(const Vector2d& v) const
		{
			return (*this - v).norm() <= EPS;
		}

		Real Vector2d::operator|(const Vector2d& v) const
		{
			return x * v.x + y * v.y;
		}

		Vector2d operator*(Real c, const Vector2d& v)
		{
			return Vector2d(c * v.x, c * v.y);
		}

		Vector2d operator/(const Vector2d& v, Real c)
		{
			return Vector2d(v.x / c, v.y / c);
		}

		Real dot(const Vector2d& u, const Vector2d& v)
		{
			return u.x * v.x + u.y * v.y;
		}

		ostream& operator<<(ostream& os, const Vector2d& v)
		{
			os << '(' << v.x << ", " << v.y << ')';
			return os;
		}

		istream& operator>>(istream& is, Vector2d& v)
		{
			is >> v.x >> v.y;
			return is;
		}

		PointType Vector2d::Type() {
			return m_type;
		}

		// Line:

		Line::Line() { }

		Line::Line(const Point2d& p, const Point2d& q)
		{
			Vector2d t = q - p;
			Real len = t.norm();

			a =   t[_Y] / len;
			b = - t[_X] / len;
			// c = -(a*p[X] + b*p[Y]);

			// less efficient, but more robust -- seth.
			c = -0.5 * ((a*p[_X] + b*p[_Y]) + (a*q[_X] + b*q[_Y]));
		}

		void Line::set(const Point2d& p, const Point2d& q)
		{
			*this = Line(p, q);
		}

		Real Line::eval(const Point2d& p) const
		{
			return (a * p[0] + b* p[1] + c);
		}

		Point2d Line::intersect(const Point2d& p1, const Point2d& p2) const
		{
				// assumes that segment (p1,p2) crosses the line
				Vector2d d = p2 - p1;
				Real t = - eval(p1) / (a*d[_X] + b*d[_Y]);
				return (p1 + t*d);
		}

		int Line::clip(const Point2d& p1, const Point2d& p2, Point2d& result) const {
			Line line = Line(p1, p2);
			
			Real det = a*line.b - line.a*b;

			if(abs(det) > EPS) {
				result[0] = (line.b*c - b*line.c)/det;
				result[1] = (a*line.c - line.a*c)/det;

				//if ( MIN(p1[0],p2[0]) <= result[0] && result[0] <= MAX(p1[0], p2[0])
				//	&& MIN(p1[1],p2[1]) <= result[1] && result[1] <= MAX(p1[1], p2[1])) 
				return 1;
			} // else lines are parallel
			return 0;
		}

		inline int Line::classify(const Point2d& p) const
		{
			Real d = eval(p);
			return (d < -EPS) ? -1 : (d > EPS ? 1 : 0);
		}

		Boolean operator==(const Point2d& point, const Line& line)
		{
			Real tmp = line.eval(point);
			return(ABS(tmp) <= EPS);
		}

		Boolean operator<(const Point2d& point, const Line& line)
		{
			return (line.eval(point) < -EPS);
		}
	}
}