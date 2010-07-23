#pragma once

#include <cgv/type/reflect/descriptors.h>

namespace cgv {
	namespace type {
		namespace reflect {

/** @defgroup interf_grp Abstract Interfaces Classes

	The interfaces describe variable information and methods that
	are used to reflect types.

	@{
*/
class call_interface;
class constructor_interface;
class base_interface;
class type_interface;
class template_interface;

class interface_base
{
	virtual       call_interface* get_call_interface()       { return 0; }
	virtual const call_interface* get_call_interface() const { return 0; }
	bool       is_call_interface() const                     { return get_call_interface() != 0; }

	virtual       constructor_interface* get_constructor_interface()       { return 0; }
	virtual const constructor_interface* get_constructor_interface() const { return 0; }
	bool       is_constructor_interface() const                     { return get_constructor_interface() != 0; }

	virtual       base_interface* get_base_interface()       { return 0; }
	virtual const base_interface* get_base_interface() const { return 0; }
	bool       is_base_interface() const                     { return get_base_interface() != 0; }

	virtual       type_interface* get_type_interface()       { return 0; }
	virtual const type_interface* get_type_interface() const { return 0; }
	bool       is_type_interface() const                     { return get_type_interface() != 0; }

	virtual       template_interface* get_template_interface()       { return 0; }
	virtual const template_interface* get_template_interface() const { return 0; }
	bool       is_template_interface() const                         { return get_template_interface() != 0; }
};

/** this interface is shared by the function and method interfaces. */
class call_interface : public interface_base
{
public:
	const call_interface* get_call_interface() const { return this; }
	      call_interface* get_call_interface()       { return this; }
	/// return a type interface for the calling structure, that holds a member for each functor argument and as last member one for the return value if this is not of type void.
	virtual const type_interface* get_calling_structure_type() const = 0;
	/// call the functor with an instance of the calling structure type
	virtual void call(void* calling_structure_instance) const = 0;
};

/** this interface is provided for non standard constructors. */
class constructor_interface
{
public:
	const constructor_interface* get_constructor_interface() const { return this; }
	      constructor_interface* get_constructor_interface()       { return this; }
	/// return a type interface for the calling structure, that holds a member for each functor argument and as last member one for the return value if this is not of type void.
	virtual const type_interface* get_calling_structure_type() const = 0;
	/// call the constructor with an instance of the calling structure type and return a newly allocated and constructed instance
	virtual void* new_construct(void* calling_structure_instance) const = 0;
	/// call the constructor on an already allocated instance with an instance of the calling structure
	virtual void call(void* instance_ptr, void* calling_structure_instance) const = 0;
	/// reconstruct the constructor arguments for a given instance and return true if this was possible. The default implementation returns \c false.
	virtual bool construct_calling_structure(void* instance_ptr) const { return false; }
};

/** the base interface encapsulates the cast from the parent to the base class.
    This is only necessary for virtual base classes. Otherwise the base_descriptor
	handles base classes. */
class base_interface
{
public:
	const base_interface* get_base_interface() const { return this; }
	      base_interface* get_base_interface()       { return this; }
	/// cast a pointer to an instance of the derived class into a pointer to an instance of the base class
	virtual void* cast(void* derived_instance_ptr) const { return 0; }
	/// cast a const pointer to an instance of the derived class into a const pointer to an instance of the base class
	virtual const void* cast(const void* derived_instance_ptr) const { return 0; }
	/// return whether the base class is virtually inherited
	virtual bool is_virtual() const = 0;
};

/** the template interface is very similar to the call interface. It allows to construct
    a type interface for a given set of template parameters. The parameters are described
	in a compound struct type, of which the type interface is provided via the
	get_parameter_structure_type method. */
class template_interface
{
public:
	const template_interface* get_template_interface() const { return this; }
	      template_interface* get_template_interface()       { return this; }
	/// construct a type interface for the given instance of the parameter structure
	virtual const type_interface* get_type(void* parameter_structure_instance) { return 0; }
	/// return a type descriptor for a type that can store the template parameters
	virtual const type_interface* get_parameter_structure_type() const = 0;
	/// type of name range
	typedef cgv::storage::collection::vector<parameter_descriptor> parameter_range;
	/// return a range of parameter descriptors
	virtual const parameter_range* get_parameter_range() const = 0;

	/**@name namespaces */
	//@{
	/// type of name range
	typedef cgv::storage::collection::vector<const char*> name_range;
	/// access to list of namespaces. The default implementation returns 0.
	virtual const name_range* get_namespaces() const { return 0; }
	//@}
	
};

/** abstract interface that allows to work with arbitrary types. */
class type_interface
{
public:
	const type_interface* get_type_interface() const { return this; }
	      type_interface* get_type_interface()       { return this; }
	/**@name identification */
	//@{
	/** There are singleton types:

		TYPENAME, BOOL, STRING
		
		sets of singletons that are unique per represented type:
		
		ENUM, STRUCT, CLASS, UNION

		and non singleton types that have attributes that can specify the
		type further, such as range bounds etc.

		CHAR, UCHAR, WCHAR,
		INT16, UINT16, INT32, UINT32, INT64, UINT64,
		FLOAT, DOUBLE,
		FUNCTION_POINTER,
		POINTER, REFERENCE
		MEMBER_POINTER, METHOD_POINTER

		The latter type kinds do not allocate their own id, nor should they be registered. These
		type kinds do not need to specify their type names, although the number types do.

		There is no function type. Functions are handled as instances to which function pointers
		point. The type is described through the function pointer.

		*/
	virtual kind get_kind() const = 0;
	/// return id of registered type
	virtual int get_id() const = 0;
	/// return the version as a pair of two ints
	virtual std::pair<int,int> get_version() const { return std::pair<int,int>(1,1); }
	/// return name of type, function, method or member pointee; empty for type constructors like pointer, array or reference
	virtual const char* get_name() const = 0;
	/// return whether the type is a singleton type
	virtual bool is_singleton() const = 0;
	/// return a pointer union to the static instance of the type interface of a singleton types or a newly allocated instance of the type interface for the other types
	virtual unions::pointer_union<type_interface> query_type() const = 0;
	/// for parameterized types, return a pointer to the template interface, otherwise return 0 pointer
	virtual const template_interface* get_template_interface() const { return 0; }
	//@}

	/**@name constructors */
	//@{
	/// check if the reflected type has a default constructor
	virtual bool has_default_constructor() const = 0;
	/// check if an instance of the reflected type is default constructed. The default implementation returns \c false.
	virtual bool is_default_constructed(void* instance) const { return false; }
	/// allocate an instance of the reflected type and construct with the default constructor. Return the pointer to the instance.
	virtual void* new_default() const = 0;
	/// allocate an array of \c count default constructed instances and return a pointer on the array
	virtual void* new_array(int count) const = 0;
	/// default construct an already allocated instance
	virtual void default_construct(void* instance_ptr) const = 0;
	/// check if the reflected type has a copy constructor
	virtual bool has_copy_constructor() const = 0;
	/// check if an instance of the reflected type is a copy of another instance. The default implementation returns \c false.
	virtual bool is_copy_constructed(const void* instance_ptr) const { return false; }
	/// construct a new instance of the reflected type as copy of the given instance
	virtual void* new_copy(const void* instance_ptr) const = 0;
	/// construct an already allocated instance of the reflected type as the copy of a second instance
	virtual void copy_construct(void* instance_1_ptr, const void* instance_2_ptr) const = 0;
	//@}

	/**@name destruction */
	//@{
	/// call standard destructor only
	virtual void destruct(void* instance_ptr) const = 0;
	/// call delete operator and destructor
	virtual void delete_destruct(void* instance_ptr) const = 0;
	/// call delete operator for arrays and for each array element the destructor
	virtual void delete_array(void* array_ptr) const = 0;
	//@}
	
	/**@name handling */
	//@{
	/// modifiable access to array element
	virtual void* access_array_element(void* array_ptr, int element) const = 0;
	/// constant access to array element
	virtual const void* access_array_element(const void* array_ptr, int element) const = 0;
	/// sizeof operator
	virtual size_type size_of() const = 0;
	/// check if array elements are packed without empty space in between
	bool array_is_packed() const 
	{
		char dummy;
		void* ptr = &dummy; 
		return (char*)access_array_element(ptr,1)-(char*)ptr == size_of(); 
	}
	//@}

	/**@name namespaces */
	//@{
	/// type of name range
	typedef cgv::storage::collection::vector<const char*> name_range;
	/// access to list of namespaces. The default implementation returns 0.
	virtual const name_range* get_namespaces() const { return 0; }
	//@}
	
	/**@name descriptors of special kind of this type */
	//@{
	/// return the enum descriptor if get_kind() returns ENUM
	virtual const enum_descriptor*      get_enum_descriptor() const { return 0; }
	/// return the function descriptor if get_kind() returns FUNCTION_POINTER
	virtual const function_pointer_descriptor*  get_function_pointer_descriptor() const { return 0; }
	/// return the pointer descriptor if get_kind() returns POINTER, REFERENCE or ARRAY
	virtual const pointer_descriptor*   get_pointer_descriptor() const { return 0; }
	/// return the number descriptor if get_kind() returns a fundamental number type
	virtual const number_descriptor*    get_number_descriptor() const { return 0; }
	/// return the compound descriptor if get_kind() returns STRUCT, CLASS or UNION
	virtual const compound_descriptor*  get_compound_descriptor() const { return 0; }
	/// return the descriptor for a member pointer if get_kind() returns MEMBER_POINTER
//	virtual const member_pointer_descriptor*    get_member_pointer_descriptor() const { return 0; }
	/// return the method descriptor if get_kind() returns METHOD_POINTER
//	virtual const method_pointer_descriptor*    get_method_pointer_descriptor() const { return 0; }
	//@}

	/**@name modification interface for descriptors of special kind of this type */
	//@{
	/// return the enum descriptor if get_kind() returns ENUM
	virtual enum_descriptor*              E() { return 0; }
	/// return the function descriptor if get_kind() returns FUNCTION
	virtual function_pointer_descriptor*  F() { return 0; }
	/// return the pointer descriptor if get_kind() returns POINTER, REFERENCE or ARRAY
	virtual pointer_descriptor*           P() { return 0; }
	/// return the number descriptor if get_kind() returns a fundamental number type
	virtual number_descriptor*            N() { return 0; }
	/// return the compound descriptor if get_kind() returns STRUCT, CLASS or UNION
	virtual compound_descriptor*          C() { return 0; }
	/// return the descriptor for a member pointer if get_kind() returns MEMBER
//	virtual member_pointer_descriptor*    MBP() const { return 0; }
	/// return the method descriptor if get_kind() returns METHOD
//	virtual method_pointer_descriptor*    MTP() const { return 0; }
	//@}
};


/** @} */

		}
	}
}