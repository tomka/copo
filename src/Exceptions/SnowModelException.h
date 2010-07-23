#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class SnowModelException : public std::logic_error
			{
			public:
			   explicit SnowModelException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~SnowModelException() throw() {}
			};
		}
}