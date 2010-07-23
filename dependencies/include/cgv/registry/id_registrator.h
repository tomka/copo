#pragma once

#include <vector>
#include <cgv/type/index_type.h>

namespace cgv {
	namespace registry {

/** the id registrator is used to implement a new group of polymorphic classes 
	that are all derived from the derived class D and tagged with different ids.
	The registrator provides a method to construct an instance of each registered
	class through their ids. The constructed instance is passed by a pointer to
	type D. For this to work the registered classes must implement the DefaultConstructible 
	concept. */
template <class D>
class id_registrator
{
protected:
	struct base_factory
	{
		virtual D* construct() const = 0;
	};
	template <typename T>
	struct factory : public base_factory
	{
		D* construct() const 
		{
			return new T;
		}
	};
	/// access to unique factory vector
	static std::vector<base_factory*>& ref_factories()
	{
		static std::vector<base_factory*> factories;
		return factories;
	}
	/// this method registers a factors and generates a unique id for it
	static cgv::type::index_type register_factory(base_factory* f)
	{
		cgv::type::index_type id = (cgv::type::index_type) ref_factories().size();
		ref_factories().push_back(f);
		return id;
	}
	/// templated version of registration method
	template <typename T>
	static cgv::type::index_type generate_id()
	{
		return register_factory(new factory<T>);
	}
public:
	/// overload to provide a unique id for each derived class
	virtual cgv::type::index_type get_id() const = 0;
	/// construct group member from id
	static D* construct(cgv::type::index_type id)
	{
		return ref_factories()[id]->construct();
	}
};

	}
}

