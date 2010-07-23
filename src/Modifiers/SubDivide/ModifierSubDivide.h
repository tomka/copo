#pragma once
#include "../Modifier.h"
#include <Model/World.h>
#include "subdiv_scheme.h"
#include <fltk/Choice.h>
#include <fltk/Slider.h>
#include <fltk/Item.h>
#include <fltk/Output.h>
#include <fltk/CheckButton.h>
#include <vector>

namespace SnowSim {
	namespace Modifiers {
		/// characters for output widget
		class ModifierSubDivide : public Modifier
		{
			typedef SnowSim::Model::World	World;

		private:
			static const char * outputs[5];

			/// checkbutton for b_auto_subdivide
			fltk::CheckButton *checkbutton;
			/// auto subdivide
			bool b_auto_subdivide;
			/// slider widget
			fltk::Slider *slider;
			/// chice widget
			fltk::Choice *choice;
			/// subdivision scheme list
			std::vector<subdiv_scheme*> subdiv_schemes;
			/// number of subdivision steps
			int subdiv_steps;
			/// selected index in subdivision scheme list
			int selected_index;
			/// output widget
			fltk::Output *output;
			/// button widget
			fltk::Button *button;
			/// pointer to fine mesh
			SnowSim::Model::World * world;

			/// Callbacks
			static inline void slider_cb(fltk::Widget *q,void *data);
			void slider_cb();
			static inline void scheme_cb(fltk::Widget *w,void *data);
			void scheme_cb();
			static inline void choice_cb(fltk::Widget *w,void *data);
			void choice_cb();
			static inline void button_cb(fltk::Widget *w,void *data);
			void button_cb();
			static inline void checkbutton_cb(fltk::Widget *w,void *data);
			void checkbutton_cb();

		public:
			ModifierSubDivide(SnowSim::Model::World * world);
			~ModifierSubDivide(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Subdivide";
			}
		};

	}
}
