//
// "$Id: cgv/registry/registry_entry.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// a registry entry gives access to factories and resources attached to a name
//
// Authored by Stefan Gumhold
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#pragma once
#include <stdarg.h>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include <cgv/config/global.h>
#include <cgv/registry/factory.h>

namespace cgv {
	namespace registry {
		/// type of factory vector
		typedef std::vector<factory*> factory_vector_type;
		/// the registry entry class gives access to different registry entries
		class registry_entry
		{
			/// type of factory map
			typedef std::map<std::string,factory_vector_type> factory_map_type;
			/// keep a factory map
			factory_map_type factory_map;
		public:
			/// construct empty entry
			registry_entry()
			{
			}
			/// find the factory vector for a given destination type name or return 0 if non was found
			const factory_vector_type* find(const char* destination_type_name) const
			{
				factory_map_type::const_iterator i = factory_map.find(destination_type_name);
				if (i == factory_map.end())
					return 0;
				return &((*i).second);
			}
			/// add a factory to the entry
			void add_factory(const char* name, factory* fac)
			{
				factory_map[name].push_back(fac);
			}
			/// remove a factory from the entry
			void remove_factory(const char* name, factory* fac)
			{
				factory_map_type::iterator i = factory_map.find(name);
				if (i == factory_map.end())
					return;
				factory_vector_type& factories = (*i).second;
				std::remove(factories.begin(), factories.end(), fac);
				if (factories.empty())
					factory_map.erase(i);
			}
		};
	}
}
