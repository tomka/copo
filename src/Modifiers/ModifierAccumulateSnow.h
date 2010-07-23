#pragma once
#include "Modifier.h"
#include "../polygonal_mesh.h"
#include <Model/World.h>
//#include <view.h>
#include <fltk/ValueInput.h>
#include <fltk/CheckButton.h>
#include <fltk/Choice.h>
#include <fltk/Item.h>
#include <fltk/Widget.h>
#include <fltk/PackedGroup.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierAccumulateSnow : public Modifier
		{
		private:
			/// The world
			SnowSim::Model::World* m_world;

			/// default batch size of shot flakes per launch site
			int batch_size;

			/// selected index in subdivision scheme list
			int selected_index;

			// should snow mesh be refined?
			bool m_refine_snow_mesh;

			// the refinement_difference: Is the exposure
			// difference between two launch sites higher
			// than this. A new launch sites will be creates
			float m_refinemen_difference;

			/// flake schema choise
			fltk::Choice choise_flakeSchema;
			/// input: batch size
			fltk::ValueInput input_batch_size;
			/// input: max. snow height
			fltk::ValueInput input_maxSnowHeight;
			/// input: snow density
			fltk::ValueInput input_snowDensity;
			/// checkbutton: Render sky buckets
			fltk::CheckButton m_cbRenderSkyBuckets;
			/// checkbutton: Render hits of snow flakes in the sky
			fltk::CheckButton m_cbRenderSkyHits;
			/// checkbutton: Make interactive info available
			fltk::CheckButton m_cbInteractiveInfo;
			/// checkbutton: Should the mesh be refined?
			fltk::CheckButton m_cbRefineMesh;

			/// the container group for the flake widget
			fltk::PackedGroup m_pgFlakeWidget;

			/// the container group for the sky widget
			fltk::PackedGroup m_pgSkyWidget;

			/// Names for the flake schemata
			static const char * m_flake_schemata[4];

			/// static callback for cbEGvisible_cb
			static inline void callback_settings(fltk::Widget *w,void *data)  {
				((ModifierAccumulateSnow*)data)->callback_settings();
			}
			void callback_settings();

			void create_flake_widget();

			/// Handler for change events from the snow model
			void OnSnowModelChanged(SnowModel* sm);
			/// Handler connection
			boost::signals::connection m_connection_OnSnowModelChanged;

		public:
			ModifierAccumulateSnow(SnowSim::Model::World* world);
			~ModifierAccumulateSnow(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Accumulate/Refine Snow";
			}
		};
	}
}
