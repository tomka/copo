#include <cgv/gui/fltk/osg/Window.h>
#include <fltk/events.h>
#include <fltk/run.h>
#include <fltk/file_chooser.h>
#include <osg/Node>
#include <osgUtil/GLObjectsVisitor>
#include <cgv/gui/fltk/osg/CoordinateGrid.h>
#include <osg/PolygonMode>
#include <osg/StateSet>
#include <osg/Switch>
#include <osg/ClearNode>
#include <osg/CameraNode>
#include <osg/Transform>

namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {

using namespace ::fltk;

	Window::Window(int x,int y,int w,int h,const char *l)
		: GlWindow(x,y,w,h,l)
	{
		_x = x;
		_y = y;
		_w = w;
		_h = h;
		_mx = 0.0f;
		_my = 0.0f;
		_mbutton = 0;
		_done = false;
		_sceneView = NULL;
		_mouseMovingOnPreviousRelease = false;
		_trackBall = new Producer::Trackball;
		resetTrackball();
		_loadedModel = NULL;
		setUpViewer(0);
		setSceneData(NULL); 
	}

	void Window::draw() 
	{
		if(!valid())
		{	// set up projection, viewport, etc.
			// window size is w() and h()
			// valid() will be turned on by FLTK after draw() returns
			// update the viewport dimensions, in case the window has been resized.
			_w = w();
			_h = h();
			_sceneView->setViewport(0,0,_w,_h);
	// COURT - fix next line - do later!!
			_sceneView->getProjectionMatrix().makePerspective(50.0f,((double)_w)/((double)_h),1.0f,10000.0f);
			// set the view
			_sceneView->setViewMatrix(viewMatrix);
			::osg::Vec4 backgroundColor = _sceneView->getClearColor();	// Default is an off blue color
			backgroundColor.set(0.1f,0.1f,0.1f,1.0f);
			_sceneView->setClearColor(backgroundColor);
		}
		// main loop (note, window toolkits which take control over the main loop
		// will require a window redraw callback containing the code below.)
		// set up the frame stamp for current frame to record the current time and frame number so that animtion code can advance correctly
		frameStamp = new ::osg::FrameStamp;
		frameStamp->setReferenceTime(::osg::Timer::instance()->delta_s(start_tick,::osg::Timer::instance()->tick()));
		frameStamp->setFrameNumber(frameNum++);
		// pass frame stamp to the SceneView so that the update, cull and draw traversals all use the same FrameStamp
		_sceneView->setFrameStamp(frameStamp.get());
		// set the view
		_sceneView->setViewMatrix(getViewMatrix());
		// do the update traversal the scene graph - such as updating animations
		_sceneView->update();
		// do the cull traversal, collect all objects in the view frustum into a sorted set of rendering bins
		_sceneView->cull();
		// draw the rendering bins.
		_sceneView->draw();
	}

	int Window::handle(int event)
	{
		switch(event){
			case PUSH:
			{
				// mouse down event
				// position in Fl::event_x() and Fl::event_y()
				_mx = ((event_x()<<1)/(float)_w)-1.0f;
				_my = 1.0f - ((event_y()<<1)/(float)_h);
				_mbutton |= (1<<(event_button()-1));
				_mx_buttonPress = _mx;
				_my_buttonPress = _my;
				redraw();
				return 1;
			}
			case DRAG:
				// mouse moved while down event
				_mx = ((event_x()<<1)/(float)_w)-1.0f;
				_my = 1.0f - ((event_y()<<1)/(float)_h);
				redraw();
				return 1;
			case RELEASE:
				// mouse release event
				_mx = ((event_x()<<1)/(float)_w)-1.0f;
				_my = 1.0f - ((event_y()<<1)/(float)_h);
				_mbutton &= ~(1<<(event_button()-1));
				if (_mx==_mx_buttonPress && _my_buttonPress==_my)
				{
					if (!_mouseMovingOnPreviousRelease)
					{
						// button press and release without moving so assume this means
						// the users wants to pick.
						;
					}
					_mouseMovingOnPreviousRelease = false;
				}
				else
				{
					_mouseMovingOnPreviousRelease = true;
				}
				redraw();
				return 1;
			case FOCUS:
			case UNFOCUS:
				//return 1 if you want keyboard events, 0 otherwise
				return 1;
			case KEY:
				//keypress, key is in Fl::event_key(), ascii in Fl::event_text()
				//return 1 if you understand/use the keyboard event, 0 otherwise
				return 1;
			case SHORTCUT:
				//shortcut, key is in Fl::event_key(), ascii in Fl::event_text()
				//return 1 if you understand/use the keyboard event, 0 otherwise
				return 1;
			default:
				// pass other events to the base class
				return GlWindow::handle(event);
		}
	}

	void Window::setUpViewer(unsigned int options)
	{ 
		// create the view of the scene.
		_sceneView = new osgUtil::SceneView;
		_sceneView->setDefaults();
		_wireframe_on=false;
		_grid_on=true;
	}

	::osg::Group* Window::create_scene(::osg::Node* node)
	{
		::osg::Group * scene = new ::osg::Group();
		
	
		if(node != NULL)
		{			
			::osg::Switch* switchgrid = new ::osg::Switch();
		
			scene->addChild(switchgrid);
			switchgrid->addChild(new cgv::gui::fltk::osg::CoordinateGrid(2*node->getBound().radius()));
			//switchgrid->addChild(new cgv::gui::fltk::osg::CoordinateGrid());
			switchgrid->setAllChildrenOn();

			::osg::ClearNode * clearnode = new ::osg::ClearNode();
			clearnode->setClearColor(::osg::Vec4f(0.8f,0.79f,0.79f,1));
			scene->addChild(clearnode);		
			
		/*	::osg::CameraNode* camera = new ::osg::CameraNode;
			camera->setReferenceFrame(::osg::Transform::ABSOLUTE_RF);
            camera->setProjectionMatrixAsOrtho2D(0,w(),0,h());
			camera->setViewMatrix(::osg::Matrix::identity());
            camera->setClearMask(GL_DEPTH_BUFFER_BIT);

			::osg::Geode* geodes = new ::osg::Geode();
			::osg::Sphere* shpere = new ::osg::Sphere(::osg::Vec3(200,100,0),30);
			::osg::ShapeDrawable* shapeDraw = new ::osg::ShapeDrawable(shpere);
			geodes->addDrawable(shapeDraw);
            camera->addChild(geodes);
			camera->getOrCreateStateSet()->setMode(GL_LIGHTING,::osg::StateAttribute::OFF);
			scene->addChild(camera);*/	

			scene->addChild(node);
		}
		else
		{
			::osg::Switch* switchgrid = new ::osg::Switch();
		
			scene->addChild(switchgrid);
			//switchgrid->addChild(new cgv::gui::fltk::osg::CoordinateGrid(2*node->getBound().radius()));
			switchgrid->addChild(new cgv::gui::fltk::osg::CoordinateGrid());
			switchgrid->setAllChildrenOn();

			::osg::ClearNode * clearnode = new ::osg::ClearNode();
			clearnode->setClearColor(::osg::Vec4f(0.8f,0.79f,0.79f,1));
			scene->addChild(clearnode);
		}

		return scene;
	}

	void Window::setSceneData( ::osg::Node *scenedata ) 
	{
		if(_loadedModel!=scenedata)
			invalidate();

		::osg::Group* scene = create_scene(scenedata);
		

		_sceneView->setSceneData(scene);
		_loadedModel =scenedata;

	

	
		{
			//hack to work around the not solved display list issue 
			osgUtil::GLObjectsVisitor v(osgUtil::GLObjectsVisitor::SWITCH_OFF_DISPLAY_LISTS);
			scene->accept(v);
		}

		resetTrackball();
		// record the timer tick at the start of rendering.   
		start_tick = ::osg::Timer::instance()->tick();
		frameNum = 0;
	}

	

	::osg::Matrixd Window::getViewMatrix()
	{
		_trackBall->input( _mx, _my, _mbutton );
		return ::osg::Matrixd(_trackBall->getMatrix().ptr());
	}

	void Window::resetTrackball()
	{
		if(_sceneView==NULL)
			return;
		::osg::Node* scene = _sceneView->getSceneData();
		if (scene)
		{
			const ::osg::BoundingSphere& bs = scene->getBound();
			if (bs.valid())
			{
				_trackBall->reset();
				_trackBall->setOrientation( Producer::Trackball::Z_UP );
				_trackBall->rotate( -0.5, 0,1,0); 
				_trackBall->rotate( 0.5, 1,0,0); 
				_trackBall->setDistance(bs.radius()*2.0f);
				_trackBall->translate(-bs.center().x(),-bs.center().y(),-bs.center().z());
			}
		}
	}

	void Window::setWireFrameOn(bool on)
	{
		_wireframe_on = on;
		if(_loadedModel != NULL)
		{
		
			::osg::StateSet *state = _loadedModel->getOrCreateStateSet();
			::osg::PolygonMode *polyModeObj;
 
			polyModeObj = dynamic_cast< ::osg::PolygonMode* >
				( state->getAttribute( ::osg::StateAttribute::POLYGONMODE ));
 
			if ( !polyModeObj ) 
			{
				polyModeObj = new ::osg::PolygonMode;
				state->setAttribute( polyModeObj );    
			}
 
			if(_wireframe_on)
				polyModeObj->setMode(  ::osg::PolygonMode::FRONT_AND_BACK, ::osg::PolygonMode::LINE);
			else
				polyModeObj->setMode(  ::osg::PolygonMode::FRONT_AND_BACK, ::osg::PolygonMode::FILL);
	
		}
		

	}


			}
		}
	}
}
