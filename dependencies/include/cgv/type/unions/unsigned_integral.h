#pragma

#include <string>
#include <strstream>
#include <cgv/type/index_type.h>
#include <cgv/type/kind.h>

namespace cgv {
	namespace type {
		namespace unions {

struct unsigned_integral
{
	union {
		unsigned char uc;
		uint16 us;
		uint32 ui;
		uint64 ul;
	};
	template <typename T>
	void set(kind t, const T& v) 
	{
		if (t == UCHAR)  uc = v; 
		if (t == UINT16) us = v;
		if (t == UINT32) ui = v;
		if (t == UINT64) ul = v;
	}
	int64 to_integral(kind t) const 
	{
		if (t == UCHAR)  return uc;
		if (t == UINT16) return us;
		if (t == UINT32) return ui;
		if (t == UINT64) return ul;
		return 0;
	}
	std::string to_string(kind t) const
	{
		std::strstream ss;
		if (t == UCHAR) ss << uc;
		if (t == UINT16) ss << us;
		if (t == UINT32) ss << ui;
		if (t == UINT64) ss << ul;
		return std::string(ss.str(), ss.pcount());
	}
	unsigned char&  operator = (const unsigned char&  _uc) { uc = _uc; return uc; }
	uint16& operator = (const uint16& _us) { us = _us; return us; }
	uint32& operator = (const uint32& _ui) { ui = _ui; return ui; }
	uint64& operator = (const uint64& _ul) { ul = _ul; return ul; }
};

		}
	}
}
