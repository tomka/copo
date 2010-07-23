#include <cgv/registry/registry.h>

namespace cgv {
	namespace registry {
		registry_map_type& registry_map()
		{
			static registry_map_type map;
			return map;
		}
		void add_factory(const char* source_type_name, const char* destination_type_name, factory* _factory)
		{
			registry_map_type::iterator i = registry_map().find(std::string(source_type_name));
			if (i == registry_map().end()) {
				registry_entry* entry = new registry_entry();
				entry->add_factory(destination_type_name, _factory);
				registry_map()[source_type_name] = entry;
			}
			else 
				(*i).second->add_factory(destination_type_name, _factory);
		}
		void remove_factory(const char* source_type_name, const char* destination_type_name, factory* _factory)
		{
			registry_map_type::iterator i = registry_map().find(std::string(source_type_name));
			if (i == registry_map().end()) 
				return;
			(*i).second->remove_factory(destination_type_name, _factory);
		}
		registry_entry* find(const char* source_type_name)
		{
			registry_map_type::iterator i = registry_map().find(std::string(source_type_name));
			if (i == registry_map().end())
				return 0;
			return (*i).second;
		}
		const factory_vector_type* find(const char* source_type_name, const char* destination_type_name)
		{
			registry_entry* entry = find(source_type_name);
			if (!entry)
				return 0;
			return entry->find(destination_type_name);
		}
		void* construct(factory* factory_ptr, ...)
		{
			va_list args;
			va_start(args, factory_ptr);
			return factory_ptr->construct(args);
		}
	}
}

