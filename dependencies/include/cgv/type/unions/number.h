#pragma once

#include <cgv/type/unions/integral.h>
#include <cgv/type/unions/floating.h>

namespace cgv {
	namespace type {
		namespace unions {

struct number
{
	union {
		integral i;
		floating f;
	};

	template <typename T>
	void set(kind k, const T& v) 
	{
		if (is_integral(k))   i.set(k, v); 
		if (is_floating(k)) f.set(k, v); 
	}

	double to_double(kind k) const 
	{
		if (is_integral(k))  return (double)i.to_integral(k); 
		if (is_floating(k)) return f.to_double(k); 
		return 0;
	}
	std::string to_string(kind k) const
	{
		if (is_integral(k))  return i.to_string(k); 
		if (is_floating(k)) return f.to_string(k); 
		return std::string();
	}
	char&  operator = (const char&  _c) { return i = _c; }
	int16& operator = (const int16& _s) { return i = _s; }
	int32&   operator = (const int32& _i) { return i = _i; }
	int64&  operator = (const int64& _l) { return i = _l; }
	unsigned char&  operator = (const unsigned char&  _uc) { return i = _uc; }
	uint16& operator = (const uint16& _us) { return i = _us; }
	uint32&   operator = (const uint32&   _ui) { return i = _ui; }
	uint64&  operator = (const uint64&  _ul) { return i = _ul; }
	float& operator  = (const float& _f)  { return f = _f; }
	double& operator = (const double& _d) { return f = _d; }
};

struct any_number
{
	kind storage_type;
	number n;
	any_number(const char&  _c) { storage_type = CHAR;  n = _c; }
	any_number(const int16& _s) { storage_type = INT16;  n = _s; }
	any_number(const int32&   _i) { storage_type = INT32;  n = _i; }
	any_number(const int64&  _l) { storage_type = INT64;  n = _l; }
	any_number(const unsigned char&  _uc) { storage_type = UCHAR;  n = _uc; }
	any_number(const uint16& _us) { storage_type = UINT16;  n = _us; }
	any_number(const uint32&   _ui) { storage_type = UINT32;  n = _ui; }
	any_number(const uint64&  _ul) { storage_type = UINT64;  n = _ul; }
	any_number(const float& _f)  { storage_type = FLOAT;  n = _f; }
	any_number(const double& _d) { storage_type = DOUBLE;  n = _d; }

	std::string to_string() const { return n.to_string(storage_type); }
	double to_double() const { return n.to_double(storage_type); }

	char&  operator = (const char&  _c) { storage_type = CHAR; return n = _c; }
	int16& operator = (const int16& _s) { storage_type = INT16; return n = _s; }
	int32&   operator = (const int32&   _i) { storage_type = INT32; return n = _i; }
	int64&  operator = (const int64&  _l) { storage_type = INT64; return n = _l; }
	unsigned char&  operator = (const unsigned char&  _uc) { storage_type = UCHAR; return n = _uc; }
	uint16& operator = (const uint16& _us) { storage_type = UINT16; return n = _us; }
	uint32&   operator = (const uint32&   _ui) { storage_type = UINT32; return n = _ui; }
	uint64&  operator = (const uint64&  _ul) { storage_type = UINT64; return n = _ul; }
	float& operator  = (const float& _f)  { storage_type = FLOAT; return n = _f; }
	double& operator = (const double& _d) { storage_type = DOUBLE; return n = _d; }
};

		}
	}
}