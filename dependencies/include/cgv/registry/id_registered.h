#pragma once

#include <cgv/registry/id_registrator.h>

namespace cgv {
	namespace registry {
		
/** implements the derived class D such that it belongs to a group G of types, 
	which are tagged by different ids and which can be constructed from G by id.
	G must be derived from the id_registrator template. */
template <class G, class D>
class id_registered : public G
{
public:
	static cgv::type::index_type get_id_static()
	{
		static cgv::type::index_type id = generate_id<D>();
		return id;
	}
	/// overload to provide a unique id for each node type
	virtual cgv::type::index_type get_id() const 
	{
		return get_id_static();
	}
};

	}
}

