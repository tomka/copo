#pragma once

#include <cgv/type/index_type.h>

namespace cgv {
	namespace registry {

/** the id generator template is used to implement a new group of classes that are 
    tagged with different ids. */
template <class D>
class id_generator
{
public:
	typedef cgv::type::index_type index_type;
	/// overload to provide a unique id for each derived class
	virtual index_type get_id() const = 0;
protected:
	/// this method generates unique ids
	static index_type generate_id()
	{
		static index_type last_id = -1;
		return ++last_id;
	}
};

	}
}

