#pragma once

#include <string>
#include <cgv/storage/collection/vector.h>
#include <cgv/type/reflect/array_bound.h>
#include <cgv/type/unions/fundamental.h>
#include <cgv/type/unions/pointer_union.h>

namespace cgv {
	namespace type {
		namespace reflect {

class type_interface;
class call_interface;
class base_interface;
class constructor_interface;

/** @defgroup descr_grp Descriptor Classes

	The descriptors are data fields that store the information
	necessary to describe different kinds of types

	@{
*/

/** entry of enumerator range consists of value name pair */
class enumerator_descriptor
{
protected:
	int value;
	std::string name;
public:
	/// construct
	enumerator_descriptor(int _value, const std::string& _name) : value(_value), name(_name) {}
	/// return value
	int get_value() const { return value; }
	/// return name
	const char* get_name() const { return name.c_str(); }
};

/** descriptor of enum types */
class enum_descriptor
{
public:
	/// type of enumerator range
	typedef cgv::storage::collection::vector<enumerator_descriptor> enumerator_range;
	class enum_iterator
	{
		const std::string& def;
		std::size_t pos;
		int val;
	public:
		enum_iterator(const std::string& d) : def(d), pos(0), val(-1) {}
		void   restart()   { pos = 0; val = -1; }
		bool   has_name()   { return pos < (int) def.size(); }
		std::string get_name()
		{
			++val;
			std::size_t opos = pos;
			pos = def.find_first_of(',', opos);
			if (pos == std::string::npos) pos = def.size();
			std::size_t epos = def.find_first_of('=', opos);
			if ((epos != std::string::npos) && (epos < pos)) {
				val = atoi(def.substr(epos+1, pos-epos-1).c_str());
				++pos;
				return def.substr(opos, epos-opos);
			}
			else {
				++pos;
				return def.substr(opos, pos-opos-1);
			}			
		}
		int    get_value()  { return val; }
	};
protected:
	enumerator_range enum_range;
public:
	enum_descriptor(const std::string& def)
	{
		enum_iterator i(def);
		while (i.has_name()) {
			std::string n = i.get_name();
			(*this) << enumerator_descriptor(i.get_value(), n);
		}
	}
	/// append another enumerator to the range
	enum_descriptor& operator << (const enumerator_descriptor& ed)
	{
		enum_range.push_back(ed);
		return *this;
	}
	/// access to enumerator range
	const enumerator_range& get_enumerator_range() const { return enum_range; }
};


/** descriptor of pointer, reference and array types */
class pointer_descriptor
{
protected:
	bool cnst;
	bool array;
	array_bound lwb, hib;
	unions::pointer_union<type_interface> type;
public:
	/// construct from pointed to type and existance of const modifier 
	pointer_descriptor(unions::pointer_union<type_interface> _type, bool _cnst = false) : cnst(_cnst), type(_type), array(false) {}
	/// interface that allows to modify the type pointed to
	type_interface* T() { return type.ref(); }
	/// set the const modifier
	void set_const(bool value = true) { cnst = value; }
	/// set the array modifier
	void set_array(bool value = true) { array = value; }
    /// set the lower bound of the array
    pointer_descriptor& set_lwb(const array_bound& ab) { lwb = ab; array = true; return *this; }
    pointer_descriptor& set_lwb(int32 bd)              { lwb = array_bound(bd); array = true; return *this; }
    /// set the higher bound of the array
    pointer_descriptor& set_hib(const array_bound& ab) { hib = ab; array = true; return *this; }
    pointer_descriptor& set_hib(int bd)                { hib = array_bound(bd); array = true; return *this; }

	/// return, whether pointer is const
	bool is_const() const                  { return cnst; }
	/// return, whether pointer points to array
	bool is_array() const  { return array; }
	/// return the lower bound
	const array_bound& get_lwab() const { return lwb; }
	int				   get_lwb() const  { return lwb.get_bound(); }
	/// return the higher bound
	const array_bound& get_hiab() const { return hib; }
	int                get_hib() const  { return hib.get_bound(); }
	/// return pointer to the type interface that describes the type pointed to by the pointer type
	const type_interface* get_type() const { return type.operator->(); }
};


/** descriptor of function arguments */
class argument_descriptor
{
protected:
	const type_interface* type;
	const char* name;
public:
	/// construct
	argument_descriptor(const type_interface* _type, const char* _name) : type(_type), name(_name) {}
	/// return value
	const type_interface* get_type() const { return type; }
	/// return name
	const char* get_name() const { return name; }
};

/** descriptor of void functor types that include void functions, void methods and constructors. */
class void_functor_descriptor
{
public:
	/// type of argument range
	typedef cgv::storage::collection::vector<argument_descriptor> argument_range;
protected:
	const call_interface* call;
	argument_range args_range;
public:
	/// standard constructor for functor interface
	void_functor_descriptor(const call_interface* _call) : call(_call) {}
	/// append another enumerator to the range
	void_functor_descriptor& argument(const type_interface* type, const char* name = "")
	{
		args_range.push_back(argument_descriptor(type, name));
	}
	/// access to enumerator range
	const argument_range& get_argument_range() const { return args_range; }
	/// return a call interface
	const call_interface* get_call_interface() const { return call; }
};

/** descriptor of functor types that include function and method pointers. */
class functor_descriptor : public void_functor_descriptor
{
protected:
	const char* return_name;
	const type_interface* return_type;
public:
	/// standard constructor for functor interface
	functor_descriptor(const call_interface* _call) : void_functor_descriptor(_call),
		return_type(0), return_name("") {}
	/// set the return type
	functor_descriptor& set_return_name(const char* _return_name)           { return_name = _return_name; return *this; }
	functor_descriptor& set_return_type(const type_interface* _return_type) { return_type = _return_type; return *this; }
	/// access to the return type
	const char*           get_return_name() const { return return_name; }
	const type_interface* get_return_type() const { return return_type; }
};

/** descriptor of function pointer types */
class function_pointer_descriptor : public functor_descriptor
{
public:
	function_pointer_descriptor(const call_interface* _call) : functor_descriptor(_call) {}
};

/** descriptor of elements, i.e. members or methods, of classes, structs or unions */
class compound_element_descriptor
{
protected:
	bool cnst;
	const type_interface* parent_type;
	const char* name;
	const char* comment;
	int group_index;
public:
	compound_element_descriptor() : cnst(false), parent_type(0), name(0), comment(0), group_index(0) {}
	/// construct
	compound_element_descriptor(
		const char* _name, 
		const type_interface* _parent_type, 
		bool _cnst, 
		const char* _comment, 
		int _group_index = 0) : name(_name), parent_type(_parent_type), cnst(_cnst), comment(_comment), group_index(_group_index) {}
	/// return pointer to the type interface that describes the type of the class that holds the member
	const type_interface* get_parent_type() const { return parent_type; }
	/// return, whether element refers to const this pointer
	bool is_const() const { return cnst; }
	/// return the name of the described element
	const char* get_name() const { return name; }
	/// return the comment, that describes the element
	const char* get_comment() const { return comment; }
	/// return the group index, which is used for unions and further sub structuring of structs and classes
	int get_group_index() const { return group_index; }
};

/** */
class number_descriptor
{
protected:
	enum bound_type { NO_BOUND, CONST_BOUND, MEMBER_POINTER_BOUND };
	unions::any_number lwb, hib;
public:
	number_descriptor() : lwb(0), hib(0) {}
};

/** descriptor of member pointer types */
class member_descriptor : public compound_element_descriptor
{
protected:
	unions::pointer_union<type_interface> member_type;
	size_type offset;
public:
	member_descriptor() : member_type(0), offset(0) {}
	/// construct from pointed to type and existance of const modifier 
	member_descriptor(
		const char* _name, 
		unions::pointer_union<type_interface> _member_type, 		
		const type_interface* _parent_type, 			  
		size_type _offset, 
		const char* _comment, 
		int _group_index = 0) : 
		compound_element_descriptor(_name, _parent_type, false, _comment, _group_index), 
		member_type(_member_type), offset(_offset) {}
	/// return pointer to the type interface that describes the type of the member
	type_interface* T() { return member_type.ref(); }
	/// return the offset to be added to the this pointer in order to reach the member
	size_type get_offset() const { return offset; }
	/// return pointer to the type interface that describes the type of the member
	const type_interface* get_member_type() const { return member_type.operator->(); }
};

/** descriptor of non standard constructors */
class constructor_descriptor : public compound_element_descriptor, public void_functor_descriptor
{
protected:
	const constructor_interface* constructor;
public:
	/// construct from pointed to type and existance of const modifier 
	constructor_descriptor(
		const constructor_interface* _constructor, 
		const type_interface* _parent_type, 				   
		const char* _comment, 
		int _group_index = 0) : 
		void_functor_descriptor(0),
		compound_element_descriptor("constructor", _parent_type, false, _comment, _group_index),
		constructor(_constructor) {}
};

/** descriptor of method pointer types */
class method_descriptor : public compound_element_descriptor, public functor_descriptor
{
public:
	/// construct from pointed to type and existance of const modifier 
	method_descriptor(
		const char* _name, 
		const call_interface* _call, 
		const type_interface* _parent_type, 
		bool _cnst, 
		const char* _comment, 
		int _group_index = 0) : 
		functor_descriptor(_call),
		compound_element_descriptor(_name, _parent_type, _cnst, _comment, _group_index) {}
};

/** descriptor for template parameter */
class parameter_descriptor
{
protected:
	unions::pointer_union<type_interface> template_type;
	unions::any_fundamental default_value;
	const type_interface* default_type;
public:
	/// construct from value
	parameter_descriptor(
		unions::pointer_union<type_interface> _template_type, 
		unions::any_fundamental _default_value = unions::any_fundamental()) :
		template_type(_template_type), default_value(_default_value), default_type(0) {}
	/// construct from template type and value
	parameter_descriptor(const type_interface* _default_type = 0) : template_type(0), default_type(_default_type) {}
	/// return pointer to the type interface that describes the type of the template parameter
	type_interface* T() { return template_type.ref(); }
	/// return, whether the parameter is \c typename 
	bool is_typename() const { return template_type == 0; }
	/// return whether a default value is available
	bool has_default_value() const { return !default_value.empty(); }
	/// return the default value, which is empty, if no default value is given
	const unions::any_fundamental& get_default_value() const { return default_value; }
	/// return whether a default type is available
	bool has_default_type() const { return default_type != 0; }
	/// return a default type is given
	const type_interface* get_default_type() const { return default_type; }
	/// return the type of the template parameter
	const type_interface* get_type() const { return template_type.cref(); }
};

/** descriptor for base classes */
class base_descriptor
{
protected:
	const type_interface* type;
	const base_interface* base;
public:
	/// construct from type of base class, offset to by added to instance pointer and bool whether base class is virtual
	base_descriptor(const type_interface* _type, const base_interface* _base) : 
	  type(_type), base(_base) {}
	/// return the type of the base class
	const type_interface* get_type() const { return type; }
	/// return the base interface
	const base_interface* get_base() const { return base; }
};

/** descriptor of a compound type */
class compound_descriptor
{
public:
	/// type of member range
	typedef cgv::storage::collection::vector<member_descriptor> member_range;
	/// type of method range
	typedef cgv::storage::collection::vector<method_descriptor> method_range;
	/// type of template base range
	typedef cgv::storage::collection::vector<base_descriptor> base_range;
	/// type of template parameter range
	typedef cgv::storage::collection::vector<parameter_descriptor> parameter_range;
	/// type of constructor range
	typedef cgv::storage::collection::vector<constructor_descriptor> constructor_range;
protected:
	/// my own type interface
	const type_interface* type;
	/// range of members
	member_range mb_range;
	/// range of non standard constructors
	constructor_range co_range;
	/// range of methods
	method_range mt_range;
	/// range of base classes
	base_range bs_range;
public:
	/// construct from type interface of this compound type
	compound_descriptor(const type_interface* _type) : type(_type) {}
	/// add a member
	compound_descriptor& operator << (const member_descriptor& md)
	{
		mb_range.push_back(md);
		return *this;
	}
	/// add method
	compound_descriptor& operator << (const constructor_descriptor& co)
	{
		co_range.push_back(co);
		return *this;
	}
	/// add method
	compound_descriptor& operator << (const method_descriptor& md)
	{
		mt_range.push_back(md);
		return *this;
	}
	/// add base class
	compound_descriptor& operator << (const base_descriptor& bd)
	{
		bs_range.push_back(bd);
		return *this;
	}
	/// access to range of members
	const member_range&    get_member_range() const    { return mb_range; }
	/// access to range of non standard constructors
	const constructor_range& get_constructor_range() const { return co_range; }
	/// access to range of methods
	const method_range&    get_method_range() const    { return mt_range; }
	/// access to range of base classes
	const base_range&      get_base_range() const      { return bs_range; }
};

/** descriptor of a template */
class template_descriptor
{
public:
	/// type of template parameter range
	typedef cgv::storage::collection::vector<parameter_descriptor> parameter_range;
protected:
	/// range of template parameters
	parameter_range par_range;
public:
	/// add template parameter value
	compound_descriptor& operator << (const parameter_descriptor& pd)
	{
		par_range.push_back(pd);
	}
	/// access to range of template parameters
	const parameter_range& get_parameter_range() const { return par_range; }
};

/** @} */
		}
	}
}