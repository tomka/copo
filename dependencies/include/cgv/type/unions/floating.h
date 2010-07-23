#pragma

#include <string>
#include <strstream>
#include <cgv/type/kind.h>

namespace cgv {
	namespace type {
		namespace unions {

struct floating
{
	union {
		float f;
		double d;
	};
	template <typename T>
	void set(kind k, const T& v) 
	{
		if (k == FLOAT) f = v; 
		if (k == DOUBLE) d = v;
	}
	double to_double(kind k) const 
	{
		if (k == FLOAT) return f;
		if (k == DOUBLE) return d;
		return 0;
	}
	std::string to_string(kind k) const
	{
		std::strstream ss;
		if (k == FLOAT)  ss << f;
		if (k == DOUBLE) ss << d;
		return std::string(ss.str(), ss.pcount());
	}
	float& operator  = (const float& _f)  { f = _f; return f; }
	double& operator = (const double& _d) { d = _d; return d; }
};

		}
	}
}
