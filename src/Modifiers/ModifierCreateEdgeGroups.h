#pragma once
#include "Modifier.h"
#include "../polygonal_mesh.h"
#include <Model/World.h>
//#include "../view.h"
#include <fltk/CheckButton.h>
#include <fltk/ValueInput.h>
#include <fltk/Button.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierCreateEdgeGroups : public Modifier
		{
		private:
			SnowSim::Model::World* world;

			/// checkbutton: edge groups visible
			fltk::CheckButton cbEGvisible;
			/// checkbutton: drops visible
			fltk::CheckButton cbDropsVisible;
			/// checkbutton: should explicitely checked for oerlaps?
			fltk::CheckButton cbDropsExplicitOverlapTest;
			/// checkbutton: launch areas visible
			fltk::CheckButton cbProjectedEGVisible;
			/// checkbutton: edge group faces visible
			fltk::CheckButton cbEdgeGroupFacesVisible;
			/// checkbutton: edge group faces visible
			fltk::CheckButton cbRememberEGFColors;
			/// button: change egf colors
			fltk::Button bChangeColors;
			/// checkbutton: normals of scnow surfaces visible
			fltk::CheckButton cbNormalsVisible;

			/// static callback for setting changes
			static inline void callback_settings(fltk::Widget *w,void *data)  {
				((ModifierCreateEdgeGroups*)data)->callback_settings();
			}
			void callback_settings();

			/// static callback for color chang
			static inline void callback_colorchange(fltk::Widget *w,void *data)  {
				((ModifierCreateEdgeGroups*)data)->callback_colorchange();
			}
			void callback_colorchange();

			void update_ui();

			typedef SnowSim::Model::World World;

			/// Handler for change events from the snow model
			void OnSnowModelChanged(SnowModel* sm);
			/// Handler connection
			boost::signals::connection m_connection_OnSnowModelChanged;

			/// flag if modifier has been executed
			bool m_has_been_executed;

		public:
			ModifierCreateEdgeGroups(SnowSim::Model::World* world);
			~ModifierCreateEdgeGroups(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Generate Edge Groups";
			}
		};
	}
}
