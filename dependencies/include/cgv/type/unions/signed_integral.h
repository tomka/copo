#pragma

#include <string>
#include <strstream>
#include <cgv/type/kind.h>
#include <cgv/type/index_type.h>

namespace cgv {
	namespace type {
		namespace unions {

struct signed_integral
{
	union {
		char c;
		int16 s;
		int32 i;
		int64 l;
	};
	template <typename T>
	void set(kind k, const T& v) 
	{
		if (k == CHAR)  c = v; 
		if (k == INT16) s = v;
		if (k == INT32)   i = v;
		if (k == INT64)  l = v;
	}
	int64 to_integral(kind k) const 
	{
		if (k == CHAR)  return c;
		if (k == INT16) return s;
		if (k == INT32) return i;
		if (k == INT64) return l;
		return 0;
	}
	std::string to_string(kind k) const
	{
		if (k == CHAR) return std::string(&c,1);
		std::strstream ss;
		if (k == INT16) ss << s;
		if (k == INT32) ss << i;
		if (k == INT64) ss << l;
		return std::string(ss.str(), ss.pcount());
	}
	char&  operator = (const char&  _c) { c = _c; return c; }
	int16& operator = (const int16& _s) { s = _s; return s; }
	int32& operator = (const int32& _i) { i = _i; return i; }
	int64& operator = (const int64& _l) { l = _l; return l; }
};

		}
	}
}
