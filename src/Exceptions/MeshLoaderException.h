#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class MeshLoaderException : public std::logic_error
			{
			public:
			   explicit MeshLoaderException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~MeshLoaderException() throw() {}
			};
		}
}