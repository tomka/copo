#pragma once

#include <cgv/type/index_type.h>
#include <cgv/registry/id_generator.h>

namespace cgv {
	namespace registry {
		
/** implements the derived class D such that it belongs to a group G of types, 
	which are tagged by different ids. */
template <class G, class D>
class id_holder : public G
{
public:
	typedef cgv::type::index_type index_type;
	/// overload to provide a unique id for each node type
	static index_type get_id_static()
	{
		static index_type id = generate_id();
		return id;
	}
	/// overload to provide a unique id for each node type
	virtual index_type get_id() const 
	{
		return get_id_static();
	}
};

	}
}

