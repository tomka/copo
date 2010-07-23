#pragma once
#include <vector>
#include <Model/ILaunchSite.h>
#include "geom2d.h"
#include <types.h>

using namespace SnowSim::Model;

namespace MeshRepresentations {
	namespace Voronoi {

		class Edge;

		class MeshChangedEventArgs {
			private:
				std::vector<const ILaunchSite<PrecisionType>*> m_toBeRemoved;
				std::vector<const Edge*> m_toBeAdded;

			public:
				MeshChangedEventArgs();
				MeshChangedEventArgs(ILaunchSite<PrecisionType>* toRemove);
				MeshChangedEventArgs(ILaunchSite<PrecisionType>* toRemove, Edge* toAdd);
				unsigned int getCountToBeRomved();
				unsigned int getCountToBeAdded();
				void addLaunchSiteToRemove(ILaunchSite<PrecisionType>* toRemove);
				void addNewPoint(const Edge* toAdd);
		};
	}
}
