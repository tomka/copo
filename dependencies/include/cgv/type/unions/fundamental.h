#pragma once

#include <string>
#include <cgv/type/unions/number.h>

namespace cgv {
	namespace type {
		namespace unions {

struct fundamental
{
	union {
		number n;
		bool b;
		wchar_t w;
	};

	template <typename T>
	void set(kind k, const T& v) 
	{
		if (is_number(k))   n.set(k, v); 
		if (k == BOOL) b = v;
	}

	std::string to_string(kind k) const 
	{
		if (is_number(k))  return n.to_string(k); 
		if (k == BOOL) return b?"true":"false";
		if (k == TYPENAME) return "EMPTY";
		return std::string();
	}
	char&  operator = (const char&  _c) { return n = _c; }
	int16& operator = (const int16& _s) { return n = _s; }
	int32&   operator = (const int32& _i) { return n = _i; }
	int64&  operator = (const int64& _l) { return n = _l; }
	unsigned char&  operator = (const unsigned char&  _uc) { return n = _uc; }
	uint16& operator = (const uint16& _us) { return n = _us; }
	uint32&   operator = (const uint32& _ui) { return n = _ui; }
	uint64&  operator = (const uint64& _ul) { return n = _ul; }
	float& operator  = (const float& _f)  { return n = _f; }
	double& operator = (const double& _d) { return n = _d; }
	bool& operator  = (const bool& _b)  { return b = _b; }
	wchar_t& operator  = (const wchar_t& _w)  { return w = _w; }
};

struct any_fundamental 
{
	kind storage_type;
	fundamental v;

	any_fundamental()				 { storage_type = TYPENAME; v = (int)0; }
	any_fundamental(const char&  _c) { storage_type = CHAR; v = _c; }
	any_fundamental(const int16& _s) { storage_type = INT16; v = _s; }
	any_fundamental(const int32&   _i) { storage_type = INT32; v = _i; }
	any_fundamental(const int64&  _l) { storage_type = INT64;  v = _l; }
	any_fundamental(const unsigned char&  _uc) { storage_type = UCHAR; v = _uc; }
	any_fundamental(const uint16& _us) { storage_type = UINT16; v = _us; }
	any_fundamental(const uint32&   _ui) { storage_type = UINT32; v = _ui; }
	any_fundamental(const uint64&  _ul) { storage_type = UINT64; v = _ul; }
	any_fundamental(const float& _f)  { storage_type = FLOAT; v = _f; }
	any_fundamental(const double& _d) { storage_type = DOUBLE; v = _d; }
	any_fundamental(const bool& _b)   { storage_type = BOOL; v = _b; }
	any_fundamental(const wchar_t& _w)   { storage_type = WCHAR; v = _w; }

	std::string to_string() const { return v.to_string(storage_type); }

	char&  operator = (const char&  _c) { storage_type = CHAR; return v = _c; }
	int16& operator = (const int16& _s) { storage_type = INT16; return v = _s; }
	int32&   operator = (const int32& _i) { storage_type = INT32; return v = _i; }
	int64&  operator = (const int64& _l) { storage_type = INT64;  v = _l; }
	unsigned char&  operator = (const unsigned char&  _uc) { storage_type = UCHAR; return v = _uc; }
	uint16& operator = (const uint16& _us) { storage_type = UINT16; return v = _us; }
	uint32&   operator = (const uint32& _ui) { storage_type = UINT32; return v = _ui; }
	uint64&  operator = (const uint64& _ul) { storage_type = UINT64; return v = _ul; }
	float& operator  = (const float& _f)  { storage_type = FLOAT; return v = _f; }
	double& operator = (const double& _d) { storage_type = DOUBLE; return v = _d; }
	bool& operator  = (const bool& _b)  { storage_type = BOOL; return v = _b; }
	wchar_t& operator  = (const wchar_t& _w)  { storage_type = WCHAR; return v = _w; }

	bool empty() const { return storage_type == TYPENAME; }
};
		}
	}
}