#pragma once

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/** A quaternion |x y z w|. Does not take care of normalization and zero quaternions!*/
template<class T>
class quaternion {

protected:

	T v[4];

public:

	/// Resets quaternion to |0 0 0 1|.
	void reset() {
		v[0] = (T)0;
		v[1] = (T)0;
		v[2] = (T)0;
		v[3] = (T)1;
	}

	/// Constructs unit quaternion |0 0 0 1|.
	quaternion() {
		reset();
	}

	/// Copy constructor.
	quaternion(const quaternion<T> &q) {
		for(int i=0; i<4; i++) v[i] = q(i);
	}

	/// Constructs quaternion from 3x3 rotation matrix. 
	quaternion(const cgv::math::linalg::fix_sqr_mat<T,3>& r)	{
		T tr = r(0,0) + r(1,1) + r(2,2);
		if (tr > (T)FLT_EPSILON) {
			T s = sqrt(tr + (T)1);
			v[3] = (T)0.5*s;
			s = (T)0.5/s;
			v[0] = (r(1,2)-r(2,1))*s;
			v[1] = (r(2,0)-r(0,2))*s;
			v[2] = (r(0,1)-r(1,0))*s;
		} else {
			int i = 0;
			if (r(1,1) > r(0,0)) i = 1;
			if (r(2,2) > r(i,i)) i = 2;
			int j = (i+1)%3;
			int k = (j+1)%3;
			T s = sqrt((r(i,i)-(r(j,j)+r(k,k))) + (T)1);
			v[i] = (T)0.5*s;
			s = (T)0.5/s;
			v[3] = (r(j,k)-r(k,j))*s;
			v[j] = (r(i,j)+r(j,i))*s;
			v[k] = (r(i,k)+r(k,i))*s;
		}
	}

	/// Constructs quaternion from 4x4 rotation matrix. 
	quaternion(const cgv::math::linalg::fix_sqr_mat<T,4>& r)	{
		T tr = r(0,0) + r(1,1) + r(2,2);
		if (tr > (T)FLT_EPSILON) {
			T s = sqrt(tr + (T)1);
			v[3] = (T)0.5*s;
			s = (T)0.5/s;
			v[0] = (r(1,2)-r(2,1))*s;
			v[1] = (r(2,0)-r(0,2))*s;
			v[2] = (r(0,1)-r(1,0))*s;
		} else {
			int i = 0;
			if (r(1,1) > r(0,0)) i = 1;
			if (r(2,2) > r(i,i)) i = 2;
			int j = (i+1)%3;
			int k = (j+1)%3;
			T s = sqrt((r(i,i)-(r(j,j)+r(k,k))) + (T)1);
			v[i] = (T)0.5*s;
			s = (T)0.5/s;
			v[3] = (r(j,k)-r(k,j))*s;
			v[j] = (r(i,j)+r(j,i))*s;
			v[k] = (r(i,k)+r(k,i))*s;
		}
	}

	/// Constructs quaternion from rotation axis and angle. 
	quaternion(const cgv::math::linalg::fix_col_vec<T,3>& axis, T angle) {
		cgv::math::linalg::fix_col_vec<T,3> a = cgv::math::linalg::normalize(axis);
		angle /= (T)2;
		T s = sin(angle);
		T c = cos(angle);
		v[0] = a.x() * s;
		v[1] = a.y() * s;
		v[2] = a.z() * s;
		v[3] = c;
	}

	/// Assign operator.
	quaternion<T>& operator = (const quaternion<T> &q) {
		for (int i = 0; i < 4; i++) v[i] = q(i);
		return *this;
	}

	/// Checks if all components are the same.
	bool operator == (const quaternion<T> &q) const {
		for(int i = 0; i < 4; i++) 
			if (v[i] != q(i)) return false;
		return true;
	}

	/// Checks if not all components are the same.
	bool operator != (const quaternion<T> &q) const {
		return !(*this == q);
	}

	/// Access to component.
	inline T& operator()(const int i) {
		return v[i];
	}

	/// Access to component of const quaternion.
	inline const T& operator()(const int i) const {
		return v[i];
	}
	
	/// Returns the rotations angle.
	T angle() const {
		return acos(v[3]) * (T)2;
	}

	/// Returns the rotation axis.
	cgv::math::linalg::fix_col_vec<T,3> axis() const {
		T sin_a = sqrt((T)1 - v[3] * v[3]);
		if (fabs(sin_a) < FLT_EPSILON) sin_a = (T)1;
		cgv::math::linalg::fix_col_vec<T,3> r;
		r.set(v[0] / sin_a, v[1] / sin_a, v[2] / sin_a);
		return r;
	}

	/// Converts quaternion to 3x3 rotation matrix. 
	cgv::math::linalg::fix_sqr_mat<T,3> matrix_33() const {
		T s = (T)2/sqr_length(*this);
		T xs = v[0]*s,		ys = v[1]*s,	zs = v[2]*s,
		  wx = v[3]*xs,		wy = v[3]*ys,	wz = v[3]*zs,
		  xx = v[0]*xs,		xy = v[0]*ys,	xz = v[0]*zs,
		  yy = v[1]*ys,		yz = v[1]*zs,	zz = v[2]*zs;
		cgv::math::linalg::fix_sqr_mat<T,3> r;
		r(0,0) = (T)1-(yy+zz);	r(0,1) = xy+wz;			r(0,2) = xz-wy;
		r(1,0) = xy-wz;			r(1,1) = (T)1-(xx+zz);	r(1,2) = yz+wx;
		r(2,0) = xz+wy;			r(2,1) = yz-wx;			r(2,2) = (T)1-(xx+yy);
		return r;
	}

	/// Converts quaternion to 4x4 rotation matrix. 
	cgv::math::linalg::fix_sqr_mat<T,4> matrix_44() const {
		T s = (T)2/sqr_length(*this);
		T xs = v[0]*s,		ys = v[1]*s,	zs = v[2]*s,
		  wx = v[3]*xs,		wy = v[3]*ys,	wz = v[3]*zs,
		  xx = v[0]*xs,		xy = v[0]*ys,	xz = v[0]*zs,
		  yy = v[1]*ys,		yz = v[1]*zs,	zz = v[2]*zs;
		cgv::math::linalg::fix_sqr_mat<T,4> r;
		r(0,0) = (T)1-(yy+zz);	r(0,1) = xy+wz;			r(0,2) = xz-wy;			r(0,3) = (T)0;
		r(1,0) = xy-wz;			r(1,1) = (T)1-(xx+zz);	r(1,2) = yz+wx;			r(1,3) = (T)0;
		r(2,0) = xz+wy;			r(2,1) = yz-wx;			r(2,2) = (T)1-(xx+yy);	r(2,3) = (T)0;
		r(3,0) = (T)0;			r(3,1) = (T)0;			r(3,2) = (T)0;			r(3,3) = (T)1;
		return r;
	}

	/// Linear interpolation of quaternions. If b_avoid_long_way is set, the interpolation happens along the short arc. 
	quaternion<T> affin(const quaternion<T>& to, T t, bool b_avoid_long_way = false) const {
		if (t <= (T)0) return (*this);
		if (t >= (T)1) return to;
		quaternion<T> qt;
		quaternion<T> q = to;
		if (b_avoid_long_way && (sqr_length((*this)+q) < sqr_length((*this)-q))) q = -q;
		T cosom = dot(*this, q);
		if (((T)1+cosom) > (T)FLT_EPSILON) {
			T sclp;
			T sclq;
			if (((T)1-cosom) > (T)FLT_EPSILON) {
				T omega = acos(cosom);
				T sinom = sin(omega);
				sclp = sin(((T)1-t)*omega)/sinom;
				sclq = sin(t*omega)/sinom;
			} else {
				sclp = (T)1-t;
				sclq = t;
			}
			for (int i=0; i<4; i++) 
				qt(i) = sclp*v[i]+sclq*q(i);
		} else {
			qt(0) = -v[1];
			qt(1) = v[0];
			qt(2) = -v[3];
			qt(3) = v[2];
			T sclp = sin(((T)1-t)*(T)1.570796326794895);
			T sclq = sin(t*(T)1.570796326794895);
			for (int i=0; i<4; i++) 
				qt(i) = sclp*v[i]+sclq*qt(i);
		}
		return qt;
	}

	/// Returns imaginary part.
	cgv::math::linalg::fix_col_vec<T,3> im() const {
		cgv::math::linalg::fix_col_vec<T,3> i;
		i(0) = v[0];
		i(1) = v[1];
		i(2) = v[2];
		return i;
	}

	/// Returns the real part.
	inline T re() const {
		return v[3];
	}

	/// Rotates vector v. 
	cgv::math::linalg::fix_col_vec<T,3> rotate(const cgv::math::linalg::fix_col_vec<T,3>& v) const {
		cgv::math::linalg::fix_col_vec<T,3> i = im();
		return (re()*re()- dot(i, i)) * v + (T)2*(dot(i,v)*i + re()*cross(i,v));
	}

	/// Rotates frame m.
	cgv::math::linalg::fix_sqr_mat<T,3> rotate(const cgv::math::linalg::fix_sqr_mat<T,3>& m) const {
		cgv::math::linalg::fix_col_vec<T,3> x, y, z;
		x.set(m(0,0), m(1,0), m(2,0));
		y.set(m(0,1), m(1,1), m(2,1));
		z.set(m(0,2), m(1,2), m(2,2));
		x = rotate(x);
		y = rotate(y);
		z = rotate(z);
		cgv::math::linalg::fix_sqr_mat<T,3> r;
		r(0,0) = x(0); r(0,1) = y(0); r(0,2) = z(0);
		r(1,0) = x(1); r(1,1) = y(1); r(1,2) = z(1);
		r(2,0) = x(2); r(2,1) = y(2); r(2,2) = z(2);
		return r;
	}

};

/// Conjugates quaternion. Inverse for unit quaternion.
template <typename T>
quaternion<T> conjugate(const quaternion<T>& p) {
	quaternion<T> r;
	r(0) = -p(0);
	r(1) = -p(1);
	r(2) = -p(2);
	r(3) = p(3);
	return r;
}

/// Inverts the quaternion.
template <typename T>
quaternion<T> invert(const quaternion<T>& p) {
	return normalize(conjugate(p));
}

/// Returns the length.
template <typename T>
T length(const quaternion<T>& p) {
	T s = (T)0;
	for (int i = 0; i<4; i++) s += p(i)*p(i);
	s = sqrt(s);
	return s;
}

/// Returns the square length.
template <typename T>
T sqr_length(const quaternion<T>& p) {
	T s = (T)0;
	for (int i = 0; i<4; i++) s += p(i)*p(i);
	return s;
}

/// Normalizes quaternion.
template <typename T>
quaternion<T> normalize(const quaternion<T>& p) {
	T l = (T)1/length(p);
	quaternion<T> r;
	r(0) = l*p(0);
	r(1) = l*p(1);
	r(2) = l*p(2);
	r(3) = l*p(3);
	return r;
}

/// Negates quaternion.
template <typename T>
quaternion<T> operator - (const quaternion<T>& q) {
	quaternion<T> r;
	r(0) = -q(0);
	r(1) = -q(1);
	r(2) = -q(2);
	r(3) = -q(3);
	return r;
}

/// Adds a quaternion.
template <typename T>
quaternion<T> operator + (const quaternion<T>& p, const quaternion<T>& q) {
	quaternion<T> r;
	r(0) = p(0)+q(0);
	r(1) = p(1)+q(1);
	r(2) = p(2)+q(2);
	r(3) = p(3)+q(3);
	return r;
}

/// Dot product.
template <typename T>
T dot(const quaternion<T>& p, const quaternion<T>& q) {
	T r;
	r = p(0)*q(0) + p(1)*q(1) + p(2)*q(2) + p(3)*q(3);
	return r;
}

/// Subtracts a quaternion.
template <typename T>
quaternion<T> operator - (const quaternion<T>& p, const quaternion<T>& q) {
	quaternion<T> r;
	r(0) = p(0)-q(0);
	r(1) = p(1)-q(1);
	r(2) = p(2)-q(2);
	r(3) = p(3)-q(3);
	return r;
}

/// Prints out quaternion.
template<class T>
std::ostream& operator << (std::ostream& out, const quaternion<T>& q) {
	out << "|" << q(0) << " " << q(1) << " " << q(2) << " " << q(3) << "|";
	return out;
}

/// Concatenates quaternions. 
template<class T>
quaternion<T> operator * (const quaternion<T>& p, const quaternion<T>& q) {
	/* Watt version
	quaternion<T> r;
	r(0) = p(3)*q(0) + p(0)*q(3) + p(1)*q(2) - p(2)*q(1);
	r(1) = p(3)*q(1) + p(1)*q(3) + p(2)*q(0) - p(0)*q(2);
	r(2) = p(3)*q(2) + p(2)*q(3) + p(0)*q(1) - p(1)*q(0);
	r(3) = p(3)*q(3) - p(0)*q(0) - p(1)*q(1) - p(2)*q(2);
	return r;
	*/
	// Fastmul-Alg. siehe Seidel-Paper p.4 
	T s[9];
	s[0] = (p(2) - p(1)) * (q(1) - q(2));
	s[1] = (p(3) + p(0)) * (q(3) + q(0));
	s[2] = (p(3) - p(0)) * (q(1) + q(2));
	s[3] = (p(2) + p(1)) * (q(3) - q(0));
	s[4] = (p(2) - p(0)) * (q(0) - q(1));
	s[5] = (p(2) + p(0)) * (q(0) + q(1));
	s[6] = (p(3) + p(1)) * (q(3) - q(2));
	s[7] = (p(3) - p(1)) * (q(3) + q(2));
	s[8] = s[5] + s[6] + s[7];
	T t = (s[4] + s[8]) / (T)2;
	quaternion<T> r;
	r(0) = s[1] + t - s[8];
	r(1) = s[2] + t - s[7];
	r(2) = s[3] + t - s[6];
	r(3) = s[0] + t - s[5];
	return r;
}

/// Concatenates quaternions. 
template<class T>
quaternion<T> operator *= (quaternion<T>& p, const quaternion<T>& q) {
	p = q*p;
	return p;
}

		}
	}
}