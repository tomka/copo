#pragma once

#include <types.h>
#include <Model/ILaunchSite.h>

namespace SnowSim {
	namespace Utilities {
		namespace Data {

			typedef SnowSim::Model::ILaunchSite<PrecisionType>*		PLaunchSite;

			/// A compare method for sorting the launch sites
			struct f_launchsite_sort
			{
				bool operator()(const PLaunchSite ls1, const PLaunchSite ls2) const
				{
					if(ls1->GetTotalHeight() < ls2->GetTotalHeight()) return true;
					if(ls1->GetTotalHeight() > ls2->GetTotalHeight()) return false;
					return std::less<void*>()(ls1, ls2);
				}
			};
		}
	}
}
