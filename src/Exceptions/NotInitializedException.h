#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class NotInitializedException : public std::logic_error
			{
			public:
				explicit NotInitializedException()
					: std::logic_error(std::string("Not initialized!")) {}

				explicit NotInitializedException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~NotInitializedException() throw() {}
			};
		}
}