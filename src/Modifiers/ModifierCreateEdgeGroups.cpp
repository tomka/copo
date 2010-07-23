#include "ModifierCreateEdgeGroups.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include <boost/bind.hpp>

namespace SnowSim {
	namespace Modifiers {

		ModifierCreateEdgeGroups::ModifierCreateEdgeGroups(World* theWorld)
			: Modifier(250, ModifierCreateEdgeGroups::getModifierName()),
				world(theWorld),
				cbEGvisible(5,10,170,20,"Edge groups visible"),
				cbDropsVisible(5, 30, 170, 20, "Drops visible"),
				cbDropsExplicitOverlapTest(5, 50, 170, 20, "Explicit check for overlaps"),
				cbProjectedEGVisible(5, 70, 170, 20, "Projection visible"),
				cbEdgeGroupFacesVisible(5,90, 170, 20, "Edge goup faces visible"),
				cbRememberEGFColors(23, 110, 120, 20, "Remember colors"),
				bChangeColors(145, 113, 28, 14, "new"),
				cbNormalsVisible(5, 130, 120, 20, "Snow normals visible"),
				m_has_been_executed(false)
		{
				cbEGvisible.callback(callback_settings,this);
				cbProjectedEGVisible.callback(callback_settings,this);
				cbDropsVisible.callback(callback_settings,this);
				cbDropsExplicitOverlapTest.callback(callback_settings,this);
				cbEdgeGroupFacesVisible.callback(callback_settings,this);
				cbNormalsVisible.callback(callback_settings, this);
				cbRememberEGFColors.callback(callback_settings, this);
				bChangeColors.callback(callback_colorchange, this);

				end();

				add(cbEGvisible);
				add(cbProjectedEGVisible);
				add(cbDropsVisible);
				add(cbDropsExplicitOverlapTest);
				add(cbEdgeGroupFacesVisible);
				add(cbRememberEGFColors);
				add(cbNormalsVisible);
				add(bChangeColors);

				m_connection_OnSnowModelChanged
					= world->GetSnowModel()->PropertyChanged.connect( boost::bind(&ModifierCreateEdgeGroups::OnSnowModelChanged, this, _1) );

				update_ui();
		}

		ModifierCreateEdgeGroups::~ModifierCreateEdgeGroups(void) { }

		void ModifierCreateEdgeGroups::OnSnowModelChanged(SnowModel* sm)
		{
			update_ui();
		}

		bool ModifierCreateEdgeGroups::execute(void) {
			if (!world->GetBaseMeshs()->isTriangulated())
				throw Exceptions::PreConditionsViolatedException("Mesh has to be triangulated");

			world->GetSnowModel()->generateEdgeGroups();

			// rewrite the current settings to the model,
			// to handle re execute inconsistensies
			callback_settings();

			if ( !m_has_been_executed )
			{
				// we do not want to see basic triangulation with dropy only.
				world->GetSnowModel()->SnowTriangulationVisible(false);
				world->GetSnowModel()->EdgeGoupFacesVisible(true);
				world->GetSnowModel()->ProjectedEdgeGroupsVisible(false);

				// we do not want to see basic triangulation with dropy only.
				cbEdgeGroupFacesVisible.value(true);

				m_has_been_executed = true;

				world->onChanged();
			}

			return true;
		}

		const char * ModifierCreateEdgeGroups::getName() {
			return ModifierCreateEdgeGroups::getModifierName();
		}

		void ModifierCreateEdgeGroups::callback_colorchange() {
			world->GetSnowModel()->SetRandomEGFColors();
			this->world->onChanged();
		}

		void ModifierCreateEdgeGroups::update_ui() {
			SnowSim::Model::SnowModel* sm = world->GetSnowModel();

			cbEGvisible.value(sm->EdgeGroupsVisible());
			cbProjectedEGVisible.value(sm->ProjectedEdgeGroupsVisible());
			cbDropsVisible.value(sm->DropsVisible());
			cbDropsExplicitOverlapTest.value(sm->DoExplicitOverlapCheck());
			cbEdgeGroupFacesVisible.value(sm->EdgeGoupFacesVisible());
			cbRememberEGFColors.value(sm->RememberEGFColors());
			cbNormalsVisible.value(sm->SnowNormalsVisible());

			if (cbEGvisible.value()) {
				cbProjectedEGVisible.activate();
				cbDropsVisible.activate();
				cbDropsExplicitOverlapTest.activate();
				cbEdgeGroupFacesVisible.activate();
			} else {
				cbProjectedEGVisible.deactivate();
				cbDropsVisible.deactivate();
				cbDropsExplicitOverlapTest.deactivate();
				cbEdgeGroupFacesVisible.deactivate();
			}

			if (cbEdgeGroupFacesVisible.value()) {
				cbRememberEGFColors.activate();
			} else {
				cbRememberEGFColors.deactivate();
			}
		}

		/// callback for cbEGvisible_cb
		void ModifierCreateEdgeGroups::callback_settings() {
			SnowSim::Model::SnowModel* snow_model = world->GetSnowModel();

			m_connection_OnSnowModelChanged.block();

			snow_model->EdgeGroupsVisible(cbEGvisible.value());
			snow_model->ProjectedEdgeGroupsVisible(cbProjectedEGVisible.value());
			snow_model->DropsVisible(cbDropsVisible.value());
			snow_model->DoExplicitOverlapCheck(cbDropsExplicitOverlapTest.value());
			snow_model->EdgeGoupFacesVisible(cbEdgeGroupFacesVisible.value());
			snow_model->RememberEGFColors(cbRememberEGFColors.value());
			snow_model->SnowNormalsVisible( cbNormalsVisible.value() );

			m_connection_OnSnowModelChanged.unblock();

			update_ui();
			this->world->onChanged();
		}
	}
}
