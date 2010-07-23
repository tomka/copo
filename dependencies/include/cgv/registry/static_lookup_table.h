#pragma once

#include <vector>
#include <cgv/registry/id_registrator.h>

namespace cgv {
	namespace registry {
		
/**  */
template <typename T, class derived>
class static_lookup_table
{
	static std::vector<T*>& ref_lookup_table()
	{
		static std::vector<T*> lt;
		return lt;
	}
public:
	static void enter(cgv::type::index_type id, T* p)
	{
		while ((cgv::type::index_type)ref_lookup_table().size() < id+1)
			ref_lookup_table().push_back(0);
		ref_lookup_table()[id] = p;
	}
	static T* lookup(cgv::type::index_type id)
	{
		if (id >= (cgv::type::index_type) ref_lookup_table().size())
			return 0;
		return ref_lookup_table()[id];
	}
};

	}
}

