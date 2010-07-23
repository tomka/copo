#pragma once

namespace cgv {
	namespace type {
		namespace detail
		{
			inline static void* get_memory_reference() {
				static char memory_location;
				return &memory_location;
			}
		}
		/** the default initializer struct has a static method get() that 
			returns a value of type T that can be used as a default 
			initializer in a function or method declaration. In case
			of non reference types T, a default constructed instance
			of T is returned. For references, a reference to a not
			initialized singelton memory location is returned, which 
			is not allowed to be dereferenced. */
		template <typename T>
		struct default_initializer
		{
			inline static T get() { return T(); }
		};
		template <typename T>
		struct default_initializer<T&>
		{
			inline static T& get() { return *((T*) detail::get_memory_reference()); } 
		};
		template <typename T>
		struct default_initializer<T*>
		{
			inline static T* get() { return 0; } 
		};
	}
}