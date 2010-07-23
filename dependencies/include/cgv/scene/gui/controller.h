#pragma once

namespace cgv{
	namespace scene{
		namespace gui{

class controller
{
public:

	virtual void reshape(int width, int height){}
	virtual void mouse_down(int x, int y,int button){} 
	virtual void mouse_move(int x, int y){}
	virtual void mouse_up(int x, int y,int button) {}
	virtual void key_down(unsigned int key){}
	virtual void key_up( unsigned int key){}
	virtual void mouse_wheel(int dy){}
};
		}
	}
}