#include "../../LaunchSite.h"
#include "../../EdgeGroup.h"

class NaiveVoronoi
{
public:
	NaiveVoronoi(std::vector<EdgeGroup::Drop*>& edgeGroup_border);


	~NaiveVoronoi();

	void addLaunchSite(LaunchSite* site);
	void getClosestSite(float x, float y, LaunchSite* site);
	float getClosestSiteDistance(float x, float y);
	void getNeighbours(float x, float y, std::vector<LaunchSite*>& neighbours);


	void render();

}