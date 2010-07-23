#include "MeshChangedEventArgs.h"

namespace MeshRepresentations {
	namespace Voronoi {
		
		MeshChangedEventArgs::MeshChangedEventArgs() { }
		
		MeshChangedEventArgs::MeshChangedEventArgs(LaunchSite* toRemove)
		{
			if (toRemove != NULL)
				this->m_toBeRemoved.push_back(toRemove);
		}
		
		MeshChangedEventArgs::MeshChangedEventArgs(LaunchSite* toRemove, Edge* toAdd)
		{
			if (toRemove != NULL)
				this->m_toBeRemoved.push_back(toRemove);
			if (toAdd != NULL)
				this->m_toBeAdded.push_back(toAdd);
		}

		unsigned int MeshChangedEventArgs::getCountToBeAdded() 
		{
			return this->m_toBeAdded.size();
		}
		
		unsigned int MeshChangedEventArgs::getCountToBeRomved()
		{
			return this->m_toBeRemoved.size();
		}
		
		void MeshChangedEventArgs::addLaunchSiteToRemove(LaunchSite* toRemove)
		{
			if (toRemove != NULL)
				this->m_toBeRemoved.push_back(toRemove);
		}
		
		void MeshChangedEventArgs::addNewPoint(const Edge* toAdd)
		{
			this->m_toBeAdded.push_back(toAdd);
		}
	}
}