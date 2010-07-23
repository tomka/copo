#pragma once

#include <map>
#include <vector>
#include <types.h>
#include <string>

namespace SnowSim {
	namespace Model {

		class SkyGrid;

		class LSInfo
		{
			public:
				int total_flakes;
				PrecisionType total_area;

				LSInfo(int flakes, PrecisionType area)
					: total_flakes(flakes), total_area(area)
				{

				}
		};



		class SkyBucket {

			public:
				typedef std::map<ILaunchSite<PrecisionType>*, LSInfo> MapType;
				typedef std::pair<ILaunchSite<PrecisionType>*, LSInfo> PairType;

				friend class SkyGrid;

			private:

				// a map of lunach sites and the hits of flakes received from it
				MapType m_origin_hits;
				// a vector of hits, for rendering purposes
				std::vector<point2f> m_hit_positions;
				// the total area reached this bucket
				PrecisionType m_totalArea;

			public:

				SkyBucket()
				{
					m_totalArea = 0.0;
				}
		};
	}
}
