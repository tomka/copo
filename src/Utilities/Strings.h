#pragma once

#include <string>
#include <vector>

namespace SnowSim {
	namespace Utilities {
		struct Strings {

			static void StringSplit(std::string str, char delim, std::vector<std::string>& results)
			{
				int cutAt;
				while( (cutAt = str.find_first_of(delim)) != str.npos )
				{
					if(cutAt > 0)
					{
						results.push_back(str.substr(0,cutAt));
					}

					str = str.substr(cutAt+1);
				}

				if(str.length() > 0)
				{
					results.push_back(str);
				}
			}

			template<class T>
			static void findandreplace( T& source, const T& find, const T& replace )
			{
				size_t j;
				for (;(j = source.find( find )) != T::npos;)
				{
					source.replace( j, find.length(), replace );
				}
			}


		};
	}
}
