#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class MeshPoolException : public std::logic_error
			{
			public:
			   explicit MeshPoolException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~MeshPoolException() throw() {}
			};
		}
}