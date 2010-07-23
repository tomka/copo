#pragma once

#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/is_composed.h>
#include <cgv/type/reflect/interfaces.h>
#include <cgv/type/reflect/type.h>
#include <cgv/registry/id_generator.h>

namespace cgv {
	namespace type {
		namespace reflect {
			namespace detail {
				struct destruct_nothing
				{
					static void call(void* ptr)
					{
					}
				};
				template <typename T>
				struct destruct_composed
				{
					static void call(void* ptr)
					{
						((T*)ptr)->~T();
					}
				};
				/// traits class that allows to call the destructor of a type even if no constructor is available
				template <typename T>
				struct destruct : public 
					ctrl::if_<cond::is_composed<T>, destruct_nothing, destruct_composed<T> >::type
				{
				};
				struct type_id_generator : public cgv::registry::id_generator<type_id_generator>
				{
				};
				/// implements the size_of, random_access and destruct methods
				template <class CL, class D>
				struct type_interface_base_impl : 
					public type_interface
				{
					/**@name handling */
					//@{
					/// modifiable access to array element
					void* access_array_element(void* array_ptr, int element) const
					{
						return &(((CL*) array_ptr)[element]); 
					}
					/// constant access to array element
					const void* access_array_element(const void* array_ptr, int element) const 
					{
						return &(((const CL*) array_ptr)[element]); 
					}
					/// sizeof operator
					size_type size_of() const 
					{
						return sizeof(CL); 
					}
					//@}

					/**@name destruction */
					//@{
					/// call standard destructor only
					void destruct(void* instance_ptr) const 
					{
						detail::destruct<CL>::call(instance_ptr);
					}
					/// call delete operator and destructor
					void delete_destruct(void* instance_ptr) const
					{ 
						delete (CL*) instance_ptr; 
					}
					/// call delete operator for arrays and for each array element the destructor
					void delete_array(void* array_ptr) const 
					{ 
						delete [] (CL*) array_ptr; 
					}
					//@}
				};

				/// implements the static identification methods, depending on the ta_is_singleton parameter
				template <class CL, class D, bool ta_is_singleton>
				struct type_interface_base_static : 
					public type_interface_base_impl<CL,D>,
					public detail::type_id_generator
				{
					/// provide is_singleton parameter for template programming
					static const bool static_is_singleton = ta_is_singleton;
					/// return id of registered type
					int get_id() const 
					{
						static int id = (int)generate_id();
						return id;
					}
					/// access to static instance of type interface
					unions::pointer_union<type_interface> query_type() const
					{
						return static_query_type();
					}
					/// access to static instance of type interface
					static unions::pointer_union<D> static_query_type()
					{
						static D instance;
						return (const D*)(&instance);
					}
					/// return whether the type is a singleton type
					bool is_singleton() const { return true; }
				};

				/// implements the static identification methods, depending on the ta_is_singleton parameter
				template <class CL, class D>
				struct type_interface_base_static<CL,D,false> : 
					public type_interface_base_impl<CL,D>
				{
					/// provide is_singleton parameter for template programming
					static const bool static_is_singleton = false;
					/// non singleton types do not have an id
					int get_id() const                     { return -1; }
					/// non singleton types do not have a static instance of type interface
					unions::pointer_union<type_interface> query_type() const { return static_query_type(); }
					static unions::pointer_union<D> static_query_type() { return new D(); }
					/// return whether the type is a singleton type
					bool is_singleton() const              { return false; }
				};

				/** implements additionally the default constructor related interface */
				template <class CL, class D, bool ta_default_constructible, bool ta_is_singleton>
				struct type_interface_base_def : public type_interface_base_static<CL,D,ta_is_singleton>
				{
					/**@name constructors */
					//@{
					/// check if the reflected type has a default constructor
					bool has_default_constructor() const             { return ta_default_constructible; }
					/// allocate an instance of the reflected type and construct with the default constructor. Return the pointer to the instance.
					void* new_default() const                        { return new CL; }
					/// allocate an array of \c count default constructed instances and return a pointer on the array
					void* new_array(int n) const                     { return new CL[n]; }
					/// default construct an already allocated instance
					void default_construct(void* instance_ptr) const { new (instance_ptr) CL; }
					//@}
				};

				template <class CL, class D, bool ta_is_singleton>
				struct type_interface_base_def<CL,D,false,ta_is_singleton> : public type_interface_base_static<CL,D,ta_is_singleton>
				{
					/**@name constructors */
					//@{
					/// check if the reflected type has a default constructor
					bool has_default_constructor() const             { return false; }
					/// allocate an instance of the reflected type and construct with the default constructor. Return the pointer to the instance.
					void* new_default() const                        { return 0; }
					/// allocate an array of \c count default constructed instances and return a pointer on the array
					void* new_array(int n) const                     { return 0; }
					/// default construct an already allocated instance
					void default_construct(void* instance_ptr) const { }
					//@}
				};
				/** abstract interface that allows to work with arbitrary types. */
				template <class D>
				struct compound_type_interface_base : public type_interface
				{
				public:
					/// return what kind of type it is
					kind get_kind() const { return STRUCT; }
					/// return id of registered type
					int get_id() const { return -1; }
					/// return name of type, function, method or member pointee; empty for type constructors like pointer, array or reference
					const char* get_name() const { return "compound_type_interface_base"; }
					/// return whether the type is a singleton type
					virtual bool is_singleton() const { return true; }
					/// return a pointer union to the static instance of the type interface of a singleton types or a newly allocated instance of the type interface for the other types
					virtual unions::pointer_union<type_interface> query_type() const { return unions::pointer_union<type_interface>(D::static_query_type()); }
					///
					static const D* static_query_type()
					{
						static D i;
						return &i;
					}
					//@}

					/**@name constructors */
					//@{
					/// check if the reflected type has a default constructor
					bool has_default_constructor() const { return true; }
					/// allocate an instance of the reflected type and construct with the default constructor. Return the pointer to the instance.
					void* new_default() const { void* mem = new char[size_of()]; default_construct(mem); return mem; }
					/// allocate an array of \c count default constructed instances and return a pointer on the array
					void* new_array(int count) const 
					{ 
						char* mem = new char[count*size_of()]; 
						for (int i=0; i<count; ++i) default_construct(mem + size_of()*i); 
						return mem; 
					}
					/// check if the reflected type has a copy constructor
					virtual bool has_copy_constructor() const { return false; }
					/// construct a new instance of the reflected type as copy of the given instance
					virtual void* new_copy(const void* instance_ptr) const { return 0; }
					/// construct an already allocated instance of the reflected type as the copy of a second instance
					virtual void copy_construct(void* instance_1_ptr, const void* instance_2_ptr) const { }
					//@}

					/**@name destruction */
					//@{
					/// call delete operator and destructor
					virtual void delete_destruct(void* instance_ptr) const
					{
						destruct(instance_ptr);
						delete [] ((char*)instance_ptr);
					}
					/// call delete operator for arrays and for each array element the destructor
					virtual void delete_array(void* array_ptr) const
					{
						delete [] ((char*)array_ptr);
					}
					//@}
					
					/**@name handling */
					//@{
					/// modifiable access to array element
					void* access_array_element(void* array_ptr, int element) const
					{
						return (char*&)array_ptr + element*size_of();
					}
					/// constant access to array element
					const void* access_array_element(const void* array_ptr, int element) const
					{
						return (const char*&)array_ptr + element*size_of();
					}
					//@}
				};
			}


/** \defgroup conv_grp Convenience Templates

	templated types that facilitate the implementation of the
	type reflection interface.

	@{
*/


/** This is a convenience base class for type interfaces that defines 
	the constructor, destruction and handling methods. The first template 
	argument is the reflected class. The second template argument should be set 
	to false for a class that does not have a default constructor. The 
	second template argument is true by default. The third template 
	argument should be set to false for a class that does not have a 
	copy constructor. The third template argument is also true by default. 
	*/
template <class CL, bool ta_default_constructible = true, bool ta_copy_constructible = true, bool ta_is_singleton = true>
struct type_interface_base : public detail::type_interface_base_def<CL,type<CL>,ta_default_constructible,ta_is_singleton>
{
	/**@name constructors */
	//@{
	/// check if the reflected type has a copy constructor
	bool has_copy_constructor() const				 { return ta_default_constructible; }
	/// construct a new instance of the reflected type as copy of the given instance
	void* new_copy(const void* instance_ptr) const         
	{
		return new CL(*((CL*)instance_ptr)); 
	}
	/// construct an already allocated instance of the reflected type as the copy of a second instance
	void copy_construct(void* instance_1_ptr, const void* instance_2_ptr) const { new (instance_1_ptr) CL(*((CL*)instance_2_ptr)); }
	//@}
};

template <class CL, bool ta_default_constructible, bool ta_is_singleton>
struct type_interface_base<CL,ta_default_constructible,false,ta_is_singleton> :
	public detail::type_interface_base_def<CL,type<CL>,ta_default_constructible,ta_is_singleton>
{
	/**@name constructors */
	//@{
	/// check if the reflected type has a copy constructor
	bool has_copy_constructor() const				 { return false; }
	/// construct a new instance of the reflected type as copy of the given instance
	void* new_copy(const void* instance_ptr) const         { return 0; }
	/// construct an already allocated instance of the reflected type as the copy of a second instance
	void copy_construct(void* instance_1_ptr, const void* instance_2_ptr) const { }
	//@}
};


/** @} */

		}
	}
}