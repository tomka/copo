#pragma once
#include "Modifier.h"
#include "../polygonal_mesh.h"
#include <Model/World.h>
//#include "../view.h"
#include <fltk/CheckButton.h>
#include <fltk/ValueInput.h>
#include <boost/signal.hpp>

namespace SnowSim {
	namespace Modifiers {

		class ModifierCreateLaunchSites : public Modifier
		{
		private:
			SnowSim::Model::World* world;

			/// checkbutton: launch sites visible
			fltk::CheckButton cbLaunchSitesVisible;
			/// checkbutton: initial launch sites visible
			fltk::CheckButton cbInitLaunchSitesVisible;
			/// checkbutton: launch areas visible
			fltk::CheckButton cbLaunchAreasVisible;
			/// checkbutton: triangulation visible
			fltk::CheckButton cbSnowTriangulationVisible;
			/// checkbutton: triangulation shaded
			fltk::CheckButton cbSnowTriangulationShaded;

			/// launch site resolution input
			fltk::ValueInput input_launchsite_resolution;
			/// maximum amount of launch site per face
			fltk::ValueInput input_max_launchsites_per_face;
			/// iterations of launch site optimizations
			fltk::ValueInput launchsite_optimizations;

			/// static callback for cbEGvisible_cb
			static inline void callback_settings(fltk::Widget *w,void *data)  {
				((ModifierCreateLaunchSites*)data)->callback_settings();
			}
			void callback_settings();
			/// static callback for cbEGvisible_cb
			static inline void callback_ls_optimizations(fltk::Widget *w,void *data)  {
				((ModifierCreateLaunchSites*)data)->callback_ls_optimizations();
			}
			void callback_ls_optimizations();

			/// Handler for change events from the snow model
			void OnSnowModelChanged(SnowModel* sm);
			/// Handler connection
			boost::signals::connection m_connection_OnSnowModelChanged;

		public:
			ModifierCreateLaunchSites(SnowSim::Model::World* world);
			~ModifierCreateLaunchSites(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Generate launch sites";
			}
		};
	}
}
