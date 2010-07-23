#include "ModifierSubDivide.h"
#include "subdiv_loop.h"
#include "subdiv_butterfly.h"
#include "subdiv_catmullclark.h"
#include "subdiv_face_cutting_quad.h"
#include "subdiv_kobbelt.h"

namespace SnowSim {
	namespace Modifiers {

		const char * ModifierSubDivide::outputs[5] = {"no","1","2","3","4"};

		ModifierSubDivide::ModifierSubDivide(
			World * world)
			: Modifier(300, "Subdivide")
			{

			this->world = world;
			this->subdiv_steps = 1;
			this->b_auto_subdivide = false;

			/// Subdivision schemes
			subdiv_schemes.push_back(new subdiv_face_cutting_triangle	(5,150,170,180));
			subdiv_schemes.push_back(new subdiv_butterfly				(5,150,170,180));
			subdiv_schemes.push_back(new subdiv_loop					(5,150,170,180));
			subdiv_schemes.push_back(new subdiv_face_cutting_quad		(5,150,170,180));
			subdiv_schemes.push_back(new subdiv_catmullclark			(5,150,170,180));
			subdiv_schemes.push_back(new subdiv_kobbelt					(5,150,170,180));

			/// GUI
			selected_index = 2;
			this->begin();

			fltk::Item * item;
			choice = new fltk::Choice(5,10,170,20);
			for (unsigned si = 0; si < subdiv_schemes.size(); si++) {
				item = new fltk::Item(subdiv_schemes[si]->get_name());
				subdiv_schemes[si]->callback(scheme_cb,this);
				choice->add(item);
			}
			choice->focus_index(selected_index);
			choice->callback(choice_cb,this);

			output = new fltk::Output(5,40,30,20);
			output->value(outputs[subdiv_steps]);

			slider = new fltk::Slider(35,40,140,20,"# steps (mousewheel)");
			slider->range(0,4);
			slider->step(1);
			slider->value(subdiv_steps);
			slider->type(fltk::Slider::TICK_ABOVE);
			slider->callback(slider_cb,this);

			checkbutton = new fltk::CheckButton(5,85,20,20,"auto");
			checkbutton->value(b_auto_subdivide);
			checkbutton->callback(checkbutton_cb,this);
			checkbutton->deactivate();

			button = new fltk::Button(50,85,120,20,"subdivide");
			button->callback(button_cb,this);

			this->add(subdiv_schemes[selected_index]);

			this->end();
		}

		ModifierSubDivide::~ModifierSubDivide(void) {
			for (unsigned si = 0; si < subdiv_schemes.size(); si++) {
				delete subdiv_schemes[si];
			}
			delete slider;
			delete choice;
			delete output;
			delete button;
			delete checkbutton;

		}

		bool ModifierSubDivide::execute(void) {
			button_cb();
			return true;
		}

		const char * ModifierSubDivide::getName() {
			return ModifierSubDivide::getModifierName();
		}

		/// static callback for b_auto_subdivide
		inline void ModifierSubDivide::checkbutton_cb(fltk::Widget *w,void *data)  {
			((ModifierSubDivide*)data)->checkbutton_cb();
		}
		/// callback for button
		void ModifierSubDivide::checkbutton_cb() {
			b_auto_subdivide = checkbutton->value();
			slider_cb();
		}

		/// static callback for slider
		inline void ModifierSubDivide::slider_cb(fltk::Widget *q, void *data) {
			((ModifierSubDivide*)data)->slider_cb();
		}
		/// callback for slider
		void ModifierSubDivide::slider_cb() {
			subdiv_steps = (int)slider->value();
			output->value(outputs[subdiv_steps]);
			if (!b_auto_subdivide) {
				return;
			}
			button_cb();
		}

		/// static callback for button
		inline void ModifierSubDivide::button_cb(fltk::Widget *w,void *data)  {
			((ModifierSubDivide*)data)->button_cb();
		}

		/// callback for button
		void ModifierSubDivide::button_cb() {
			MeshPool* pool = world->GetBaseMeshs();

			for (unsigned int i=0; i < pool->getNrMeshs(); i++) {
				subdiv_schemes[selected_index]->subdivide(pool->getMesh(i)->mesh(), subdiv_steps);
			}
		}

		/// static callback for the selected subdivision scheme
		inline void ModifierSubDivide::scheme_cb(fltk::Widget *w,void *data)  {
			((ModifierSubDivide*)data)->scheme_cb();
		}
		/// callback for the selected subdivision scheme
		void ModifierSubDivide::scheme_cb() {
			slider_cb();
		}

		/// static callback for the chooser
		inline void ModifierSubDivide::choice_cb(fltk::Widget *w,void *data) {
			((ModifierSubDivide*)data)->choice_cb();
		}
		/// callback for the chooser
		void ModifierSubDivide::choice_cb() {
			this->remove(subdiv_schemes[selected_index]);
			selected_index = choice->focus_index();
			this->add(subdiv_schemes[selected_index]);

			//fltk::Widget * selectedTab = tabs->selected_child();
			//if (selectedTab) selectedTab->draw();

			slider_cb();
			this->redraw();
		}
	}
}
