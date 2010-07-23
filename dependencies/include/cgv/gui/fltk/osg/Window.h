//
// "$Id: cgv/gui/fltk/osg/Window.h 4910 2006-08-02 10:00:00Z sgumhold $"
//
// fltk based window to show osg scenes. This is based on
// -*-c++-*- osgFltkWindow - Copyright (C) 2006  Court Cutting
//
// Authored by Stefan Gumhold
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#pragma once
#pragma warning (disable:4311)
#include <fltk/GlWindow.h>
#include <fltk/gl.h>
#include <Producer/Trackball>
#include <osg/Node>
#include <osg/Timer>
#include <osg/ref_ptr>
#include <osg/FrameStamp>
#include <osgUtil/SceneView>
#include <osg/Switch>
#include <cgv/config/global.h>

#include <cgv/gui/fltk/osg/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

	/** The Window class interfaces osg to the fltk GlWindow and passes on some
	    of the events. Ultimately one needs to implement the osgGA interface.
	*/
	class CGV_API Window : public ::fltk::GlWindow 
	{
	public:
		/// construct from size and label name
		Window(int x,int y,int w,int h,const char *l=0);
		/// overload the draw method
		void draw();
		/// pass events on to scene graph
		int handle(int event);
		/// access to current view matrix
		::osg::Matrixd getViewMatrix();
		void resetTrackball();
		void setUpViewer(unsigned int options);
		inline const int xPos(void){return _x;}
		inline const int yPos(void){return _y;}
		inline const int winWidth(void){return _w;}
		inline const int winHeight(void){return _h;}
		/// set the root node for the view scene
		void setSceneData(::osg::Node *scene); 
		/// access to root node of current 
		::osg::Node* getLoadedModel() {return _loadedModel.get();}

		::osg::Group* create_scene(::osg::Node* node);


		bool getWireFrameOn(){	return _wireframe_on;	}
		void setWireFrameOn(bool on);

		bool getGridOn(){return _grid_on;}
		void setGridOn(bool on)
		{
			_grid_on = on;
			::osg::Group *root  = (::osg::Group*)_sceneView->getSceneData();
			if(on)
				((::osg::Switch*)root->getChild(0))->setAllChildrenOn();
			else
				((::osg::Switch*)root->getChild(0))->setAllChildrenOff();


		}

	private:
		float                               _mx, _my;
		float                               _mx_buttonPress, _my_buttonPress;
		unsigned int                        _mbutton;
		bool                                _mouseMovingOnPreviousRelease;
		::osg::ref_ptr<Producer::Trackball>	_trackBall;    
		bool                                _done;
		int _x;
		int _y;
		int _w;
		int _h;
		::osg::ref_ptr<osgUtil::SceneView> _sceneView;
		::osg::ref_ptr< ::osg::FrameStamp > frameStamp;
		::osg::Matrix viewMatrix;
		::osg::Timer_t start_tick;
		unsigned int frameNum;
		::osg::ref_ptr< ::osg::Node > _loadedModel;
		bool	_wireframe_on;
		bool	_grid_on;
	};
			}
		}
	}
}

#include <cgv/config/lib_end.h>
