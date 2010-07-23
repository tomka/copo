//
// osg node representing a coordinate grid
//
// Authored by Sören König
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#pragma once
#include <osg/Object>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/MatrixTransform>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/StateSet>
#include <osgText/Text>


namespace cgv {
	namespace gui {
		namespace fltk {
			namespace osg {






class CoordinateGrid : public ::osg::Group
{
    public:
        CoordinateGrid(int size = 200,int d=20) 
		{
			
			_grid = new ::osg::Geometry();
			_geode1 = new ::osg::Geode();
			_geode2 = new ::osg::Geode();
			_geode3 = new ::osg::Geode();
			_geode4 = new ::osg::Geode();
			_axis = new ::osg::Group();
			_axis->setName("coordinate axes");
			_geode1->setName("XY grid");
		
			
			::osg::Vec3Array* coords = new ::osg::Vec3Array(d*4+4);
	
	 
			 for(int i = 0; i <=d;i++)
			 {
				 (*coords)[2*i]= ::osg::Vec3f(-size/2.0,-size/2.0+i*size/(float)d,0);
				 (*coords)[2*i+1]= ::osg::Vec3f(size/2.0,-size/2.0+i*size/(float)d,0); 
			 }

			 for(int i = 0; i <=d;i++)
			 {
				 (*coords)[d*2+2+2*i]= ::osg::Vec3f(-size/2.0+i*size/(float)d,-size/2.0,0);
				 (*coords)[d*2+2+2*i+1]= ::osg::Vec3f(-size/2.0+i*size/(float)d,size/2.0,0); 
			 }
    
			_grid->setVertexArray(coords);

			

    
			::osg::Vec4Array* colours = new ::osg::Vec4Array(1);
			(*colours)[0].set(0.2f,0.2f,0.2f,1.0f);
			_grid->setColorArray(colours);
			_grid->setColorBinding(::osg::Geometry::BIND_OVERALL);

			_grid->addPrimitiveSet(new ::osg::DrawArrays(::osg::PrimitiveSet::LINES,0,coords->size()));
			_grid->setUseDisplayList(false);
			_grid->setSupportsDisplayList(false);
			
			
			
			
	 
	  
			::osg::StateSet* stateset = _grid->getOrCreateStateSet();
			
			stateset->setMode(GL_LIGHTING,::osg::StateAttribute::PROTECTED|
				::osg::StateAttribute::OFF);
			 
			_geode1->addDrawable(_grid);
			

			::osg::Cone* cone = new ::osg::Cone(::osg::Vec3(0,0,size/5.0),size/35.0,size/20.0);
			::osg::ShapeDrawable* shapeDraw1 = new ::osg::ShapeDrawable(cone);
			shapeDraw1->setColor(::osg::Vec4f(0,0,1,1));
			_geode2->addDrawable(shapeDraw1);
			::osg::Cylinder* cylinder = new ::osg::Cylinder(::osg::Vec3(0,0,size/10.0),size/90.0,size/5.0);
			::osg::ShapeDrawable* shapeDraw2 = new ::osg::ShapeDrawable(cylinder);
			shapeDraw2->setColor(::osg::Vec4f(0,0,1,1));
			_geode2->addDrawable(shapeDraw2);

			::osgText::Text* text = new ::osgText::Text;
			text->setFont("fonts\\arial.ttf");
			text->setFontResolution(100,100);
			text->setCharacterSize(size/10.0);
			text->setColor(::osg::Vec4f(0,0,1,1));
			text->setPosition(::osg::Vec3f(0,0,size/3.5));
			text->setAxisAlignment(osgText::Text::SCREEN);
			text->setAlignment(::osgText::Text::CENTER_CENTER);
			text->setText("z");
  			_geode2->addDrawable(text);
			
			_geode2->setName("Z axis");

			
			::osg::ShapeDrawable* shapeDraw3 = new ::osg::ShapeDrawable(cone);
			shapeDraw3->setColor(::osg::Vec4f(1,0,0,1));
			_geode3->addDrawable(shapeDraw3);
			::osg::ShapeDrawable* shapeDraw4 = new ::osg::ShapeDrawable(cylinder);
			shapeDraw4->setColor(::osg::Vec4f(1,0,0,1));
			_geode3->addDrawable(shapeDraw4);
			::osgText::Text* text2 = new ::osgText::Text;
			text2->setFont("fonts\\arial.ttf");
			text2->setFontResolution(100,100);
			text2->setCharacterSize(size/10.0);
			text2->setColor(::osg::Vec4f(1,0,0,1));
			text2->setPosition(::osg::Vec3f(0,0,size/3.5));
			text2->setAxisAlignment(osgText::Text::SCREEN);
			text2->setAlignment(::osgText::Text::CENTER_CENTER);
			text2->setText("x");
			_geode3->addDrawable(text2);
			_geode3->setName("X axis");

			::osg::ShapeDrawable* shapeDraw5 = new ::osg::ShapeDrawable(cone);
			shapeDraw5->setColor(::osg::Vec4f(0,1,0,1));
			_geode4->addDrawable(shapeDraw5);
			::osg::ShapeDrawable* shapeDraw6 = new ::osg::ShapeDrawable(cylinder);
			shapeDraw6->setColor(::osg::Vec4f(0,1,0,1));
			_geode4->addDrawable(shapeDraw6);
			::osgText::Text* text3 = new ::osgText::Text;
			text3->setFont("fonts\\arial.ttf");
			text3->setFontResolution(100,100);
			text3->setCharacterSize(size/10.0);
			text3->setColor(::osg::Vec4f(0,1,0,1));
			text3->setPosition(::osg::Vec3f(0,0,size/3.5));
			text3->setAxisAlignment(osgText::Text::SCREEN);
			text3->setAlignment(::osgText::Text::CENTER_CENTER);
			text3->setText("y");
			_geode4->addDrawable(text3);
			_geode4->setName("Y axis");
			

			::osg::Sphere* shpere = new ::osg::Sphere(::osg::Vec3(0,0,0),size/90.0);
			::osg::ShapeDrawable* shapeDraw7 = new ::osg::ShapeDrawable(shpere);
			_geode1->addDrawable(shapeDraw7);
			

			
			::osg::MatrixTransform* p1 = new ::osg::MatrixTransform;
			p1->setMatrix(::osg::Matrix::rotate(::osg::inDegrees(90.0f),0.0f,1.0f,0.0f));
			 ::osg::MatrixTransform* p2 = new ::osg::MatrixTransform;
			 p2->setMatrix(::osg::Matrix::rotate(::osg::inDegrees(-90.0f),1.0f,0.0f,0.0f));
    
			_axis->addChild(p1);
			_axis->addChild(p2);
			_axis->addChild(_geode2); 
			p1->addChild(_geode3); 
			p2->addChild(_geode4); 

			addChild(_geode1);
			addChild(_axis);
			
			
	  
		}

        /** Copy constructor using CopyOp to manage deep vs shallow copy.
		CoordinateGrid(const CoordinateGrid& cg,const ::osg::CopyOp& copyop=::osg::CopyOp::SHALLOW_COPY):
		:osg::Group(cg,copyop)
		{}
      
		META_Object(cgvlib, CoordinateGrid);*/

    protected:
		virtual ~CoordinateGrid(){}
		::osg::Geometry *_grid;
		::osg::Geode *_geode1,*_geode2,*_geode3,*_geode4;
		::osg::Group *_axis;

		
	   

		
        
};


			}
		}
	}
}
