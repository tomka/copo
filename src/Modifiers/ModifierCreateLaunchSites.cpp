#include "ModifierCreateLaunchSites.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include <boost/bind.hpp>

#undef max

namespace SnowSim {
	namespace Modifiers {

		ModifierCreateLaunchSites::ModifierCreateLaunchSites(SnowSim::Model::World* theWorld)
			: world(theWorld), Modifier(250, ModifierCreateLaunchSites::getModifierName()),
				cbLaunchSitesVisible(5, 10, 170, 20, "Launch sites visible"),
				cbInitLaunchSitesVisible(5, 30, 170, 20, "Initial launch sites visible"),
				cbLaunchAreasVisible(5, 50, 170, 20, "Launch Areas visible"),
				cbSnowTriangulationVisible(5,70, 170, 20, "Snow triangulation visible"),
				cbSnowTriangulationShaded(5,90, 170, 20, "Snow triangulation shaded"),
				input_launchsite_resolution(132, 160, 40, 20, "Launch site resolution: "),
				input_max_launchsites_per_face(132, 140, 40, 20, "Max. launch s./face: "),
				launchsite_optimizations(132, 180, 40, 20, "Site optimizations: ")
		{
				SnowModel* sm = theWorld->GetSnowModel();

				cbLaunchSitesVisible.value(sm->InitialLaunchSitesVisible());
				cbLaunchSitesVisible.callback(callback_settings,this);

				cbInitLaunchSitesVisible.value(sm->LaunchSitesVisible());
				cbInitLaunchSitesVisible.callback(callback_settings,this);

				cbLaunchAreasVisible.value(sm->LaunchAreasVisible());
				cbLaunchAreasVisible.callback(callback_settings,this);

				cbSnowTriangulationVisible.value(sm->SnowTriangulationVisible());
				cbSnowTriangulationVisible.callback(callback_settings,this);

				cbSnowTriangulationShaded.value(sm->SnowTriangulationVisible());
				cbSnowTriangulationShaded.callback(callback_settings,this);

				if ( ! cbSnowTriangulationVisible.value() )
					cbSnowTriangulationShaded.deactivate();

				input_launchsite_resolution.linesize(0.1);
				input_launchsite_resolution.range(1.0f,std::numeric_limits<double>::infinity());
				input_launchsite_resolution.value(sm->LaunchSiteResoltution());
				input_launchsite_resolution.callback(callback_settings, this);

				input_max_launchsites_per_face.linesize(1.0);
				input_max_launchsites_per_face.range(1.0f,std::numeric_limits<double>::infinity());
				input_max_launchsites_per_face.value(sm->MaxLaunchSitesPerFace());
				input_max_launchsites_per_face.callback(callback_settings, this);

				launchsite_optimizations.linesize(1.0);
				launchsite_optimizations.range(0.0f,std::numeric_limits<int>::max());
				launchsite_optimizations.value(sm->GetLaunchSiteOptimizations());
				launchsite_optimizations.callback(callback_ls_optimizations, this);

				end();

				add(cbLaunchSitesVisible);
				add(cbInitLaunchSitesVisible);
				add(cbLaunchAreasVisible);
				add(cbSnowTriangulationVisible);
				add(input_launchsite_resolution);
				add(input_max_launchsites_per_face);
				add(launchsite_optimizations);
				add(cbSnowTriangulationShaded);

				m_connection_OnSnowModelChanged
					= sm->PropertyChanged.connect( boost::bind(&ModifierCreateLaunchSites::OnSnowModelChanged, this, _1) );

				// we do not want to see basic triangulation with dropy only.
				cbSnowTriangulationShaded.value(true);
				world->GetSnowModel()->EdgeGoupFacesVisible(false);
				world->GetSnowModel()->LaunchSitesVisible(true);
		}

		ModifierCreateLaunchSites::~ModifierCreateLaunchSites(void)
		{
			m_connection_OnSnowModelChanged.disconnect();
		}

		/// Handler for change events from the snow model
		void ModifierCreateLaunchSites::OnSnowModelChanged(SnowModel* sm)
		{
			cbLaunchSitesVisible.value(sm->InitialLaunchSitesVisible());
			cbInitLaunchSitesVisible.value(sm->LaunchSitesVisible());
			cbLaunchAreasVisible.value(sm->LaunchAreasVisible());
			cbSnowTriangulationVisible.value(sm->SnowTriangulationVisible());
			cbSnowTriangulationShaded.value(sm->SnowTriangulationVisible());

			if ( ! cbSnowTriangulationVisible.value() )
				cbSnowTriangulationShaded.deactivate();

			input_launchsite_resolution.value(sm->LaunchSiteResoltution());
			input_max_launchsites_per_face.value(sm->MaxLaunchSitesPerFace());
			launchsite_optimizations.value(sm->GetLaunchSiteOptimizations());
		}

		bool ModifierCreateLaunchSites::execute(void) {
			if (!world->GetBaseMeshs()->isTriangulated())
				throw Exceptions::PreConditionsViolatedException("Mesh has to be triangulated");
			if (!world->GetSnowModel()->edgeGroupsGenerated())
				throw Exceptions::PreConditionsViolatedException("Edge groups have to be generated first.");

			world->GetSnowModel()->generateLaunchSites();

			// rewrite the current settings to the model,
			// to handle re execute inconsistencies
			callback_settings();

			return true;
		}

		const char * ModifierCreateLaunchSites::getName() {
			return ModifierCreateLaunchSites::getModifierName();
		}

		void ModifierCreateLaunchSites::callback_ls_optimizations()
		{
			SnowSim::Model::SnowModel * snow_model = this->world->GetSnowModel();

			int opts = (int)launchsite_optimizations.value();

			snow_model->SetLaunchSiteOptimizations(opts);
		}

		/// callback for cbEGvisible_cb
		void ModifierCreateLaunchSites::callback_settings() {
			SnowSim::Model::SnowModel * snow_model = this->world->GetSnowModel();

			m_connection_OnSnowModelChanged.block();

			if (input_launchsite_resolution.value() < 1.0) input_launchsite_resolution.value(1.0);
			if (input_max_launchsites_per_face.value() < 1.0) input_max_launchsites_per_face.value(1.0);

			snow_model->LaunchSitesVisible(cbLaunchSitesVisible.value());
			snow_model->InitialLaunchSitesVisible(cbInitLaunchSitesVisible.value());
			snow_model->LaunchAreasVisible(cbLaunchAreasVisible.value());
			snow_model->SnowTriangulationVisible(cbSnowTriangulationVisible.value());
			snow_model->LaunchSiteResoltution((float)input_launchsite_resolution.value());
			snow_model->MaxLaunchSitesPerFace((int)input_max_launchsites_per_face.value());
			snow_model->SnowTriangulationShaded(cbSnowTriangulationShaded.value());

			if ( cbSnowTriangulationVisible.value() )
				cbSnowTriangulationShaded.activate();
			else
				cbSnowTriangulationShaded.deactivate();

			m_connection_OnSnowModelChanged.unblock();

			this->world->onChanged();
		}
	}
}
