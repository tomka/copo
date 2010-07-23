#pragma once

#include <string>
#include <cgv/type/unions/signed_integral.h>
#include <cgv/type/unions/unsigned_integral.h>

namespace cgv {
	namespace type {
		namespace unions {

struct integral 
{
	union {
		signed_integral si;
		unsigned_integral ui;
	};

	template <typename T>
	void set(kind k, const T& v) 
	{
		if (is_signed_integral(k))  si.set(k, v); 
		if (is_unsigned_integral(k))  ui.set(k, v); 
	}

	int64 to_integral(kind k) const 
	{
		if (is_signed_integral(k))  return si.to_integral(k); 
		if (is_unsigned_integral(k)) return ui.to_integral(k); 
		return 0;
	}
	std::string to_string(kind k) const
	{
		if (is_signed_integral(k))  return si.to_string(k); 
		if (is_unsigned_integral(k)) return ui.to_string(k); 
		return std::string();
	}
	char&  operator = (const char&  _c) { return si = _c; }
	int16& operator = (const int16& _s) { return si = _s; }
	int32&   operator = (const int32&   _i) { return si = _i; }
	int64&  operator = (const int64&  _l) { return si = _l; }
	unsigned char&  operator = (const unsigned char&  _uc) { return ui = _uc; }
	uint16& operator = (const uint16& _us) { return ui = _us; }
	uint32&   operator = (const uint32&   _ui) { return ui = _ui; }
	uint64&  operator = (const uint64&  _ul) { return ui = _ul; }
};
		}
	}
}