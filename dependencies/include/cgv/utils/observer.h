#pragma once

/**
* How to use observer pattern?
*
* You have a value which should be observed by multiple observers (gui, renderview,...)
* 
* The first thing to do is to create an instance of observable.
*
* struct value : observable
* {
*   float value;
* } fops;
* 
*
* Now you can register observers with the add_observer method.
*
* f_ops.add_observer(view1);
* f_ops.add_observer(view2);
*
* After changing the value of f a call to invalidate() informs all registered
* observers to update themselves.
*
* fops.value = 9.1f;
* fops.value.invalidate();
* 
* To create an own observer type,e.g. a view-widget, just derive from cgv::util::observer and implement the update method.
*/

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

namespace cgv{
	namespace utils{


class observable;


/**
* All concrete observers have to implement this simple interface.
*/
class observer
{
public:
	virtual void update()=0;

};



/**
* An observable is a kind of registry to manage observers for a given type T.
* You can add and remove observers, access the observed value and inform all
* registered observes about changes via an invalidate() call.
*/
class observable 
{
	std::vector<observer*> observers;
	typedef  std::vector<observer*>::iterator obs_iter;

public:

	
	///notify all registered observers about changes (this calls update)
	void invalidate()
	{
		
		for(obs_iter it= observers.begin();it!=observers.end();it++)
		{
			(*it)->update();
		}
	}
	

	///register given observer o to be notified when changes take place
	void add_observer(observer* o)
	{
		observers.push_back(o);
	}

	///deregister given observer o, no further notification are done to the given observer
	void remove_observer(observer* o)
	{
		obs_iter it = std::find(observers.begin(),observers.end(),o);
		if(it != observers.end())
			observers.erase(it);
		
	}

};
	}
}


