#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class ArgumentException : public std::logic_error
			{
			public:
			   explicit ArgumentException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~ArgumentException() throw() {}
			};
		}
}