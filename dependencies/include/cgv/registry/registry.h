//
// "$Id: cgv/registry/registry.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// methods that allow to access the global registry
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
#include <map>
#include <string>
#include <cgv/config/global.h>
#include <cgv/registry/factory.h>
#include <cgv/registry/registry_entry.h>

#include <cgv/registry/lib_begin.h>

namespace cgv {
	namespace registry {
		/// type of registry map
		typedef std::map<std::string,registry_entry*> registry_map_type;
		/// reference to global registry map
		extern CGV_API registry_map_type& registry_map();
		/// add a factory for a source type, that generates instances of the destination type
		extern CGV_API void add_factory(const char* source_type_name, const char* destination_type_name, factory* _factory);
		/// remove a factory for a source type, that generates instances of the destination type
		extern CGV_API void remove_factory(const char* source_type_name, const char* destination_type_name, factory* _factory);
		/// find a registry entry by the source type name
		extern CGV_API registry_entry* find(const char* source_type_name);
		/// find the factory vector for a pair of source and destination type names or return 0 if non was found
		extern CGV_API const factory_vector_type* find(const char* source_type_name, const char* destination_type_name);
		/// construct a new object from the given factory and a list of arguments for the constructor of the destination type
		extern CGV_API void* construct(factory* factory_ptr, ...);
	}
}


#include <cgv/config/lib_end.h>

