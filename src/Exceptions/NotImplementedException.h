#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class NotImplementedException : public std::logic_error
			{
			public:
				explicit NotImplementedException()
					: std::logic_error(std::string("Not implemented!")) {}

				explicit NotImplementedException(const std::string& what)
				  : std::logic_error(what) {}

			   virtual ~NotImplementedException() throw() {}
			};
		}
}