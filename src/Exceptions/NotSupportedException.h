#pragma once
#include <stdexcept>

namespace SnowSim {
		namespace Exceptions {

			class NotSupportedException : public std::runtime_error
			{
			public:
				explicit NotSupportedException()
					: std::runtime_error("Not supported!") {}

				explicit NotSupportedException(const std::string& what)
					: std::runtime_error(what) {}

			   virtual ~NotSupportedException() throw() {}
			};
		}
}
