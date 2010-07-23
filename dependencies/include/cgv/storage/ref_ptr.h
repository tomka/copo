#pragma once

#include <cgv/defines/assert.h>
#include <cgv/type/index_type.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/cond/has_virtual_destructor.h>
#include <cgv/storage/ref_counted.h>

namespace cgv {
	namespace storage {
		namespace RefPtr
		{
			template <typename T, bool is_ref_counted = false>
			class ref_ptr
			{
				/// struct to store the pointer with a count
				struct counter_type {
					counter_type(T* p = 0, cgv::type::index_type c = 1) : ptr(p), count(c) {}
					T*                    ptr;
					cgv::type::index_type count;
				}* counter;
			protected:
				/// increment the count
				void acquire(const ref_ptr<T,is_ref_counted>& r) throw()
				{ 
					counter = r.counter;
					if (counter) 
						++counter->count;
				}
				/// decrement the count, delete if it is 0
				void release()
				{
					if (counter) {
						if (--counter->count == 0) {
							delete counter->ptr;
							delete counter;
						}
						counter = 0;
					}
				}
				cgv::type::index_type get_count() const
				{
					 return counter ? counter->count : 0;
				}
			public:
				/// construct reference counted pointer
				explicit ref_ptr(T* p = 0) : counter(0) 
				{
					if (p) 
						counter = new counter_type(p); 
				}
				/// return the pointer itself
				T*	ref () const					   
				{ 
					return counter->ptr; 
				}
			};
			template <typename T>
			class ref_ptr<T,true>
			{
				T* ptr;
			protected:
				/// increment the count
				void acquire(const ref_ptr<T,true>& r)
				{
					ptr = r.ptr;
					if (ptr)
						ptr->set_ref_count(ptr->get_ref_count()+1);
				}
				void release() 
				{ 
					if (ptr) {
						cgv::type::index_type count = ptr->get_ref_count();
						if (count <= 1)
							delete ptr;
						else
							ptr->set_ref_count(count-1);
						ptr = 0;
					}
				}
				cgv::type::index_type get_count() const
				{
					 return ptr ? ptr->get_ref_count() : 0;
				}
			public:
				/// construct reference counted pointer
				explicit ref_ptr(T* p = 0) : ptr(p) 
				{ 
					if (ptr)
						ptr->set_ref_count(ptr->get_ref_count()+1);
				}
				/// return the pointer itself
				T*	ref () const					   
				{ 
					return ptr; 
				}
			};

		}

/** reference counted pointer, which can work together with types that implement
    the RefCounted concept but also with types that don't implement this concept.
	In the case of RefCounted types, the reference count provided in the type is
	used, otherwise reference count is allocated. For RefCounted types the access
	operator costs one pointer lookup, for other types, it costs two! */
template <class T>
class ref_ptr : public RefPtr::ref_ptr<T, cgv::storage::RefCounted::traits<T>::implements::value>
{
public:
	typedef typename RefPtr::ref_ptr<T, cgv::storage::RefCounted::traits<T>::implements::value> base_type;
	/// construct reference counted pointer
	explicit ref_ptr(T* p = 0) : base_type(p) {}
	/// destruct reference counted pointer
    ~ref_ptr()                             { release(); }
	/// copy constructor
    ref_ptr(const ref_ptr<T>& r)           { acquire(r); }
	/// allow to copy ref_ptr<S> to a ref_ptr<T> if T is a base class of S and if T has a virtual destructor
	template <typename S>
    ref_ptr(const ref_ptr<S>& s)           { 
		/// ref_ptr conversion only valid if T is base of S
		CGV_DEFINES_ASSERT_TRUE(type::cond::is_base_of<T,S>);
		/// and T has a virtual destructor
		CGV_DEFINES_ASSERT_TRUE(type::cond::has_virtual_destructor<T>);
		const ref_ptr<T>* r = (const ref_ptr<T>*) &s;
		acquire(*r); 
	}
	/// assignment
    ref_ptr<T>& operator = (const ref_ptr<T>& r) { if (this != &r) { release(); acquire(r); } return *this; }
	/// allow to assign ref_ptr<S> to a ref_ptr<T> if T is a base class of S and if T has a virtual destructor
	template <typename S>
	ref_ptr<T>& operator = (const ref_ptr<S>& r) {
		/// ref_ptr conversion only valid if T is base of S
		CGV_DEFINES_ASSERT_TRUE(type::cond::is_base_of<T,S>);
		/// and T has a virtual destructor
		CGV_DEFINES_ASSERT_TRUE(type::cond::has_virtual_destructor<T>);
		*this = (const ref_ptr<T>&)r;
		return *this;
	}
	/// conversion to pointer
	operator T*() const { return ref(); }
	/// access
    T& operator*()  const                  { return *ref();}
	/// access
    T* operator->() const                  { return ref();}
	/// compare by pointer
	bool operator == (const ref_ptr<T>& r) const { return ref() == r.ref(); }
	/// check if this is the only reference
    bool unique()   const                  { return get_count() <= 1; }
	/// check if pointer is not yet set
	bool empty()    const                  { return get_count() == 0; }
	/// set to null pointer
	void clear()                           { release(); }
};

	}
}