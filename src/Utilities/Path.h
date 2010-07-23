#pragma once

#include <string>
#include "Strings.h"

namespace SnowSim {
	namespace Utilities {

		struct Path {

			/// Returns the path of a filepath string.
			/// e. g. if path is "c:\a\b\c.txt" then "c:\a\b" will be returned
			static std::string GetPathWithoutFileName(const std::string& path)
			{
				string::size_type cutAt = path.find_last_of("\\");

				if ( cutAt == path.npos ) {
					cutAt = path.find_last_of("/");

					if ( cutAt == path.npos )
						return string("");
				}

				return path.substr(0, cutAt);
			}

			/// Returns the filename of a filepath string.
			/// e. g. if path is "c:\a\b\c.txt" then "c.txt" will be returned
			static std::string GetFileName(const std::string& path)
			{
				vector<string> s_arr;
				SnowSim::Utilities::Strings::StringSplit(path, '\\', s_arr);

				if (s_arr.size() == 1) {
					s_arr.clear();
					Utilities::Strings::StringSplit(path, '/', s_arr);
				}

				return s_arr[s_arr.size() - 1];
			}
		};
	}
}
