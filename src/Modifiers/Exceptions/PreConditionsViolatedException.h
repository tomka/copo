#pragma once
#include <stdexcept>

namespace SnowSim {
	namespace Modifiers {
		namespace Exceptions {

			class PreConditionsViolatedException : public std::logic_error
			{
			public:
			   explicit PreConditionsViolatedException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~PreConditionsViolatedException() throw() {}
			};
		}
	}
}