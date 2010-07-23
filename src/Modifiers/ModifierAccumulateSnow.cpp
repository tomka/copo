#include "ModifierAccumulateSnow.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include "../MeshPool.h"
#include "../Mesh.h"
#include <Model/ILaunchSite.h>
#include <Model/EdgeGroup.h>
#include <Model/Flake_Schema_Straight.h>
#include <Model/Flake_Schema_DynStraight.h>
#include <Model/Flake_Schema_Small.h>
#include <Model/Flake_Schema_Normal.h>
#include <Utilities/Timer.h>
#include <boost/bind.hpp>

// to get no conflicts with the numeric_limits max
#undef max

namespace SnowSim {
	namespace Modifiers {

		const char * ModifierAccumulateSnow::m_flake_schemata[4] = {"Straight","Angle Variation","Small Helix","Normal Helix"};

		ModifierAccumulateSnow::ModifierAccumulateSnow(World * world)
			: Modifier(330, ModifierAccumulateSnow::getModifierName()),
				selected_index(2),
				choise_flakeSchema(70,10,100,20, "@s11;Flake type: "),
				input_batch_size(130, 40, 40, 20, "Batch size: "),
				input_maxSnowHeight(130, 65, 40, 20, "@s10;Max. snow height [cm]: "),
				input_snowDensity(130, 90, 40, 20, "@s10;Sn. Density [kg/m^3]: "),
				m_cbRenderSkyBuckets(5,115,120,20,"Render sky buckets"),
				m_cbRenderSkyHits(5,135,120,20,"Render sky hits"),
				m_cbInteractiveInfo(5, 155, 120,20, "Interactive info"),
				m_cbRefineMesh(5,175,120,20, "Refine snow mesh"),
				m_pgSkyWidget(0,212,200,50, "@C#0C4CE0;Sky settings"),
				m_pgFlakeWidget(0,290,200,20, ""),
				m_world(world),
				batch_size(10),
				m_refine_snow_mesh(false),
				m_refinemen_difference(0.5f)
		{
				selected_index = m_world->GetSnowModel()->GetFlakeSchema()->GetType();

				for (unsigned si = 0; si < 4; si++) {
					fltk::Item * item = new fltk::Item(m_flake_schemata[si]);
					//choise_flakeSchema[si].callback(callback_settings,this);
					choise_flakeSchema.add(item);
				}
				choise_flakeSchema.focus_index(selected_index);
				choise_flakeSchema.callback(callback_settings,this);

				// create the flake widget
				create_flake_widget();

				input_batch_size.linesize(1.0);
				input_batch_size.range(1.0f, std::numeric_limits<int>::max() );
				input_batch_size.value( batch_size );
				input_batch_size.callback(callback_settings, this);

				input_maxSnowHeight.linesize(0.1);
				input_maxSnowHeight.range(1.0f,std::numeric_limits<float>::max());
				input_maxSnowHeight.value( m_world->GetSnowModel()->MaxSnowHeight() );
				input_maxSnowHeight.callback(callback_settings, this);

				input_snowDensity.linesize(1.0);
				input_snowDensity.range(1.0f,std::numeric_limits<int>::max());
				input_snowDensity.value( world->GetSnowModel()->GetDensity() );
				input_snowDensity.callback(callback_settings, this);

				m_cbRenderSkyBuckets.callback(callback_settings,this);
				m_cbRenderSkyBuckets.value(m_world->GetRenderSky());

				m_cbRenderSkyHits.callback(callback_settings,this);
				m_cbRenderSkyHits.value(m_world->GetRenderSkyHits());

				m_cbInteractiveInfo.callback(callback_settings, this);
				m_cbInteractiveInfo.value(m_world->GetSky()->IsDebugging());

				m_cbRefineMesh.callback(callback_settings, this);
				m_cbRefineMesh.value(m_refine_snow_mesh);
				m_cbRefineMesh.deactivate();

				m_pgSkyWidget.add( this->m_world->GetSky()->GetPropertiesWidget() );

				// Add GUI elements
				add(choise_flakeSchema);
				add(input_batch_size);
				add(input_maxSnowHeight);
				add(input_snowDensity);
				add(m_cbRenderSkyBuckets);
				add(m_cbRenderSkyHits);
				add(m_cbInteractiveInfo);
				add(m_cbRefineMesh);
				add(m_pgSkyWidget);
				add(m_pgFlakeWidget);

				SnowModel* sm = world->GetSnowModel();

				sm->SnowTriangulationVisible(true);
				sm->SnowTriangulationShaded(true);
				m_connection_OnSnowModelChanged
					= sm->PropertyChanged.connect( boost::bind(&ModifierAccumulateSnow::OnSnowModelChanged, this, _1) );
		}

		ModifierAccumulateSnow::~ModifierAccumulateSnow(void)
		{
			m_connection_OnSnowModelChanged.disconnect();
		}

		/// Handler for change events from the snow model
		void ModifierAccumulateSnow::OnSnowModelChanged(SnowModel* sm)
		{
			selected_index =sm->GetFlakeSchema()->GetType();
			choise_flakeSchema.focus_index(selected_index);
			input_maxSnowHeight.value(sm->MaxSnowHeight() );
			input_snowDensity.value( sm->GetDensity() );
		}

		void ModifierAccumulateSnow::create_flake_widget()
		{
			m_pgFlakeWidget.clear();

			FlakeSchema::Flake_Schema<PrecisionType>* fs = this->m_world->GetSnowModel()->GetFlakeSchema();

			fltk::Widget* w = fs->GetPropertiesWidget();

			if (w == NULL) {
				m_pgFlakeWidget.label("");
			} else {
				m_pgFlakeWidget.label("@C#0C4CE0;Flake Schema");
				m_pgFlakeWidget.add(w);
				m_pgFlakeWidget.layout();
			}

		}

		bool ModifierAccumulateSnow::execute(void) {

			MeshPool* pool = m_world->GetBaseMeshs();

			// Anstatt alles im Snow Model zu machen,
			// lieber alles hier implementieren.
			// world->GetSnowModel()->accumulateSnow();

			/*
			 1.	Von LaunchSite eine Liste von Schneeflocken bekommen
					(Default-Wert eintsllbar mchen und je niedriger die WICHTIGKEIT, desto weniger)
			 2.	Jede Schneeflocke nach oben tanzen lassen und mit jedem Teilvektor
					des Schneeflugs (haha ;)) einen INTERSECTION-Test gegen alle Faces der Welt
					und gegen alle Top- und Side-Polygone der Schneeoberfl�che (evtl. Range-Tree);
					wenn getroffen: abbruch des Flugs, n�chste Flocke
			 3.	Kommt die Flocke im Himmel an (Boundingbox-Top) wird dieser einer Zelle im Himmelsgrid
					zugeordnet (sky buckets);
					KONZEPT: jeder sky bucket hat gr��e s_area; Die repr�sentierte Fl�che der
					Ursprungs-Launchsite wird dem Bucket hinzugef�gt. Ist die von der Flocke repr�sentierte
					Fl�che (= Repr. Fl�che der LaunchSite / Anzahl Schneeflocken in diesem Batch ) gr��er,
					werden entsprechende Anteile an benachbarte Zellen gegeben.
					IMPLEMENTATION: Jeder sky bucket hat eine Liste, sortiert nach LaunchSites mit einer
					zugeordneten Fl�che wie viel er insgesamt von dieser LaunchSite "bekommen" hat. Vielleicht
					kann man auch eine Flake-Struktur nehmen welche die ganzen Tests macht, die aktuelle LaunchSite
					und die Wiederholung (sowie aktuellen Anzahl) speichert.
			 4.	Netzverfeinerung.
			 5.	Ist diese Schneeakkumulationsphase beendet wird jeder LaunchSite eine neue Masse von den Buckets
					in welche eine Flocke der Site traf, zugeordnet:
					Diese ist proportional zu der Summe alle repr�sentativen Fl�chen aller Flocken die dieser LaunchSite
					entsprungen sind und einen Bucket getroffen haben. Es wird vorher die Gesamtfl�che ausgerechnet die im
					Himmel "angekommen" ist und gleichm��ig auf alle buckets verteilt
					U. a. damit wird dann die SCHNEEH�HE berechnet. Au�erdem spielt noch die Schnee-DICHTE mit rein.
			 6.	Es ist manchmal besser die gewollte Schneeh�he in mehrere "Teilh�hen" aufzuteilen.
			 7.	Je nach ZEIT f�r eine Phase, kann es sein dass manche LaunchSites gar nicht zum Schuss kamen und
					daher werden sie n�chstes mal bevorzugt und eine alte Version (sofern Vorhanden) der Schneeflocken-
					Information wird im Himmel behalten bis eine neue kommt und diese ersetzt
			*/


			// also, die Implementation
			SnowSim::Model::SnowModel* snow_model = m_world->GetSnowModel();
			SnowSim::Model::ISky* sky = m_world->GetSky();

			// Reset the sky
			sky->Reset();

			unsigned int n_egs = snow_model->GetNumberEdgeGroups();
			if (n_egs < 1) {
				throw Exceptions::PreConditionsViolatedException("ModifierAccumulateSnow: Need at least one edge group.");
			}
			FlakeSchema::Flake_Schema<>* flake_schema = snow_model->GetFlakeSchema();

			std::cout << "[Modifier] Accumulating snow on " << n_egs << " edge groups" << std::endl;
			std::cout << "\t*Using flake schema \"" << flake_schema->getName() << "\"" << std::endl;
			std::cout << "\t*Progress: ";

			std::flush(std::cout);

			int percent_step = static_cast<int>(n_egs * 0.1f);
			int step = 10;

			if (percent_step < 1)
			{
				step = static_cast<int>(100.0f / n_egs);
				percent_step = 1;
			}

			unsigned int percent_count = 0;
			int percent = 0;
			unsigned int eg_count = 0;

			SnowSim::Utilities::Timer t;

			t.start();

			// go through all edge groups
			//#pragma omp parallel for
			for (int i=0; i < n_egs; ++i)
			{
				EdgeGroup* current_eg = snow_model->GetEdgeGroup(i);
				int n_launchsites = current_eg->GetNumberLaunchSites();

				// go through all launch sites on the current edge group
				for (EdgeGroup::CDT_VIterator vit = current_eg->LaunchSites_Begin();
						vit != current_eg->LaunchSites_End(); ++vit)
				{
					// TOMTODO: Im Moment wird die WICHTIGKEIT (Importance) ignoriert,
					// da sie noch nicht implementiert ist.
					// get current launch site
					ILaunchSite<PrecisionType>* current_ls = &(*vit);

					// ignore vertices that are connected to constrains (e. g. are on a drop)
					if ( current_eg->Are_there_incident_constraints(vit) )
						continue;

					/* 1. & 2.: Get snow flakes starting at this launch site and shoot them towards sky */
					FlakeSchema::ShootingResult shooting_result;

					// calculate used batch size, importance should be used here
					int actual_batch_size = batch_size;
					flake_schema->shootFlakes(current_ls, actual_batch_size, shooting_result);

					// remember the occlusion factor
					int hits = shooting_result.hits.size();

					if (hits == 0) {
						current_ls->SetExposure(0.0f);
					} else if (hits == shooting_result.batch_size) {
						current_ls->SetExposure(1.0f);
					} else {
						current_ls->SetExposure(
								static_cast<float>(shooting_result.batch_size) / static_cast<float>(hits) );
					}

					/* 3. Add shooting result to the sky */
					sky->AddShootingResult(shooting_result, current_ls);
				}

				if (eg_count > percent_count)
				{
					//#pragma omp critical(eg_count_ls_creation)
					{
						// to make sure that an parallel thread does not
						// does the increment, too - we check again.
						if ( eg_count > percent_count ) {
							std::cout << " " << percent;
							std::flush(std::cout);
							percent_count += percent_step;
							percent += step;
						}
					}
				}

				//#pragma omp atomic
				++eg_count;

			}

			std::cout << "...ready" << std::endl << "\t*Reprojecting snow mass...";

			/* 5. Reproject mass */
			sky->ReprojectMass();

			if ( m_refine_snow_mesh )
			{
				// for practical reasons (it's only a prototype...)
				// do the mesh refinement after reorojection

				/* 4. Refine mesh:
				 * see Fearing PhD (p. 143) for refinement constraints
				 *
				*/

				std::cout << "ready" << std::endl << "\t*Refining snow mesh...";

				percent_count = 0;
				percent = 0;
				eg_count = 0;

				// go through all edge groups
				#pragma omp parallel for
				for (int i=0; i < n_egs; ++i)
				{
					EdgeGroup* current_eg = snow_model->GetEdgeGroup(i);
					int n_launchsites = current_eg->GetNumberLaunchSites();

					// go through all launch sites on the current edge group
					for (EdgeGroup::CDT_VIterator vit = current_eg->LaunchSites_Begin();
							vit != current_eg->LaunchSites_End(); ++vit)
					{
						// TOMTODO: Im Moment wird die WICHTIGKEIT (Importance) ignoriert,
						// da sie noch nicht implementiert ist.
						// get current launch site
						ILaunchSite<PrecisionType>* current_ls = &(*vit);

						// ignore vertices that are connected to constrains (e. g. are on a drop)
						if ( current_eg->Are_there_incident_constraints(vit) )
							continue;

						// get exposure
						float exposure = current_ls->GetExposure();

						// check against neighbor exposures
						EdgeGroup::CDT_VCirculator nbrs = current_eg->GetNeighbors(current_ls), done(nbrs);

						do {
							ILaunchSite<PrecisionType>* current_nbr = &(*nbrs);

							float diff = abs(current_nbr->GetExposure() - exposure);

							if (diff > m_refinemen_difference) {
								// TODO: Handle refine cases!

								//std::cout << "Refine!" << std::endl;
							} else if ( diff < NearlyZero )
							{
								// TODO: Handle delete cases!

								//std::cout << "delete!" << std::endl;
							}

							++nbrs;
						} while (nbrs != done);
					}

					if (eg_count > percent_count)
					{
						#pragma omp critical(eg_count_ls_creation)
						{
							// to make sure that an parallel thread does not
							// does the increment, too - we check again.
							if ( eg_count > percent_count ) {
								std::cout << " " << percent;
								std::flush(std::cout);
								percent_count += percent_step;
								percent += step;
							}
						}
					}

					#pragma omp atomic
					++eg_count;

				}

				std::cout << "..."; // only for eye candy
			}

			t.stop();
			std::cout << "ready" << std::endl << "\t*Time: " << t.diffTimeInMSec() << "msec" << std::endl;

			return true;
		}

		const char * ModifierAccumulateSnow::getName() {
			return ModifierAccumulateSnow::getModifierName();
		}

		/// callback for settings changes
		void ModifierAccumulateSnow::callback_settings() {
			SnowModel * snow_model = m_world->GetSnowModel();

			m_connection_OnSnowModelChanged.block();

			if (input_batch_size.value() < 1.0) input_batch_size.value(1.0);
			if (input_maxSnowHeight.value() < 0.0) input_maxSnowHeight.value(0.0);
			if (input_snowDensity.value() < 1.0) input_snowDensity.value(1.0);

			batch_size = (int)( input_batch_size.value() );
			snow_model->MaxSnowHeight( static_cast<PrecisionType>(input_maxSnowHeight.value()) );
			snow_model->SetDensity( static_cast<PrecisionType>(input_snowDensity.value()) );
			m_world->SetRenderSky(m_cbRenderSkyBuckets.value());
			m_world->SetRenderSkyHits(m_cbRenderSkyHits.value());
			m_world->GetSky()->SetDebugging(m_cbInteractiveInfo.value());

			m_refine_snow_mesh = m_cbRefineMesh.state();

			if (selected_index != choise_flakeSchema.focus_index()) {
				selected_index = choise_flakeSchema.focus_index();

				// create new flake schema
				switch (selected_index)
				{
				case 0:
					snow_model->SetFlakeSchema(new FlakeSchema::Flake_Schema_Straight<PrecisionType>(snow_model, m_world));
					break;
				case 1:
					snow_model->SetFlakeSchema(new FlakeSchema::Flake_Schema_DynStraight<PrecisionType>(snow_model, m_world));
					break;
				case 2:
					snow_model->SetFlakeSchema(new FlakeSchema::Flake_Schema_Small<PrecisionType>(snow_model, m_world));
					break;
				case 3:
					snow_model->SetFlakeSchema(new FlakeSchema::Flake_Schema_Normal<PrecisionType>(snow_model, m_world));
					break;
				default:
					break;
				}

				create_flake_widget();
			}

			m_connection_OnSnowModelChanged.unblock();

			m_world->onChanged();
		}
	}
}
