#pragma once

namespace cgv {
	namespace type {
		namespace unions {

	/** the pointer union can hold a const or non const pointer. */
	template <typename T>
	class pointer_union
	{
		bool cnst;
		union {
			T* ptr;
			const T* const_ptr;
		};
	public:
		/// construction from int constructs a non const null pointer, independent of the integer argument
		pointer_union(int) : cnst(false), ptr(0) {}
		/// construct from non const pointer
		pointer_union(T* p) : cnst(false), ptr(p) {}
		/// construct from const pointer
		pointer_union(const T* p) : cnst(true), const_ptr(p) {}
		/// parameterized copy constructor that includes a static type cast
		template <typename S>
		pointer_union(const pointer_union<S>& pu) : cnst(pu.is_const())
		{
			if (cnst)
				const_ptr = static_cast<const T*>(pu.cref());
			else
				ptr = static_cast<T*>(pu.ref());
		}
		/// parameterized assignment calls copy constructor
		template <typename S>
		pointer_union<T>& operator = (const pointer_union<S>& pu) 
		{ 
			new (this) pointer_union<T>(pu); 
			return *this; 
		}
		/// assign to const pointer
		pointer_union<T>& operator = (const T* p) { cnst = true; const_ptr = p; return *this; }
		/// assign to non const pointer
		pointer_union<T>& operator = (T* p) { cnst = false; ptr = p; return *this; }
		/// call delete operator in case of non const pointer only
		void delete_ptr() 
		{
			if (!cnst) {
				delete ptr; 
				ptr = 0;
			}
			else
				const_ptr = 0;
		}
		/// return non const pointer if a non const pointer is represented or 0 pointer if a const pointer is represented
		T* ref() const { return cnst ? 0 : ptr; }
		/// return const pointer
		const T* cref() const { return cnst ? const_ptr : ptr; }
		/// convert to non const pointer. If const pointer is represented, conversion results in 0 pointer
		operator T* () { return cnst ? 0 : ptr; }
		/// convert to const pointer
		operator const T* () { return cnst ? const_ptr : ptr; }
		/// return whether a const pointer is represented
		bool is_const() const { return cnst; }
		/// only const version of -> operator is provided, as automatic selection between a const and non const version does not work
		const T* operator ->() const { return is_const() ? const_ptr : ptr; }
		/// only const version of * operator is provided, as automatic selection between a const and non const version does not work
		const T& operator *() const { return is_const() ? *const_ptr : *ptr; }
		/// compare with const pointer
		bool operator == (const T* p) const { return (is_const() ? const_ptr : ptr) == p; }
		/// compare with pointer
		bool operator == (T* p) const { return (is_const() ? const_ptr : ptr) == p; }
		/// compare with int to allow comparison to 0 pointer
		bool operator == (int) const { return (is_const() ? const_ptr : ptr) == 0; }
	};
		}
	}
}