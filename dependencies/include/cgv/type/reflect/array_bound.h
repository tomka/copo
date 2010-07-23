#pragma once

#include <cgv/type/kind.h>
#include <cgv/type/index_type.h>

namespace cgv {
	namespace type {
		namespace reflect {

/** the bound class is used to specify the upper and lower bound of an array range. */
class array_bound
{
protected:
	kind my_kind;
	union {
		int32 value;
		int32 pointer_offset;
	};
	/// offset that is added to the bound before returned
	int32 value_offset;
public:
	/// standard constructor
	array_bound() : my_kind(INT32), value(0), value_offset(0) {}
	/// construct a bound from a constant integer
	array_bound(int32 _value) : my_kind(INT32), value(_value), value_offset(0) {}
	/// construct a bound from a pointer
	array_bound(int32 _pointer_offset, int32 _value_offset) : 
		my_kind(MEMBER_POINTER), pointer_offset(_pointer_offset), value_offset(_value_offset) {}
	/// copy constructor
	array_bound(const array_bound& ab) : my_kind(ab.my_kind), value_offset(ab.value_offset) 
	{
		if (my_kind == MEMBER_POINTER)
			pointer_offset = ab.pointer_offset;
		else
			value = ab.value;
	}
	/// access to offset
	int32 get_offset() const { return value_offset; }
	/// set the offset
	void set_offset(int off) { value_offset = off; }
	/// check if the bound is a pointer
	bool is_member_pointer() const  { return my_kind == MEMBER_POINTER; }
	/// return the offset necessary to be added to a this pointer to arrive at the bound
	int32 get_pointer_offset() const { return is_member_pointer() ? pointer_offset : 0; }
	/// return the value of the bound
	int get_bound(const void* instance_ptr = 0) const 
	{
		if (is_member_pointer())
			return *((int32*)(((const char*) instance_ptr)+pointer_offset))+value_offset;
		else
			return value;
	}
};

		}
	}
}