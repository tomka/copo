 #include <view.h>
#include "ModifierSnowStability.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include "../MeshPool.h"
#include "../Mesh.h"
#include <Model/ILaunchSite.h>
#include <Model/IAngleOfRepose.h>
#include <Model/AORDefault.h>
#include <Model/EdgeGroup.h>
#include <Utilities/Random.h>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <Utilities/math.h>
#include <fltk/run.h>

#ifndef WIN32
#include <unistd.h>
#endif
static int my_sleep( unsigned long _t ) {
#ifdef WIN32
Sleep( _t ); return 0;
#else
return usleep( 1000 * _t );
#endif
}


// to get no conflicts with the numeric_limits max
#undef max

namespace SnowSim {
	namespace Modifiers {

		const char* criteria[2] = {"Iterations","Time"};
		const std::vector<const char*> ModifierSnowStability::m_terminationCriteria(criteria, criteria + 2);

		const char* aor[3] = {"Constant","Two constants", "Linear function"};
		const std::vector<const char*> ModifierSnowStability::m_aor_functions(aor, aor + 3);

		ModifierSnowStability::ModifierSnowStability(World * world)
			: Modifier(350, ModifierSnowStability::getModifierName()),
				m_world(world),
				m_maxIterations(10),
				m_stepwise(false),
				m_averageTimePerIteration(0),
				m_maxTimeInSec(10.0),
				m_maxDistToLS(5.0),
				m_keyForward(Utilities::Keys::SpaceKey),
				m_keyFastForward(Utilities::Keys::LeftCtrlKey),
				m_cbStepwise(5, 10, 170, 20, "Interactive stabilization"),
				m_choise_termination(112,35,60,20, "Terminate by: "),
				m_input_maxTime(130, 63, 40, 20, "Max. time [sec]: "),
				m_input_iterations(130, 63, 40, 20, "Max. iterations [#]: "),
				m_input_maxDistToHitLS(130, 88, 40, 20, "Max. dist. Hit-LS: "),
				m_label_aor(72,108,100,20, "@C#0C4CE0;Angle Of Repose"),
				m_choise_aor(97,130,75,20, "AOR function: "),
				m_aor_constant1(130,150,40,20, "Constant: "),
				m_aor_stable_angle(130,170,40,20, "Stable angle [°]: "),
				m_aor_sep_angle(130,190,40,20, "@s11;Separating angle [°]: "),
				m_aor_constant2(130,210,40,20, "2. Constant: "),
				m_aor_linear_m(50,210,40,20, "m: "),
				m_aor_linear_n(130,210,40,20, "n: "),
				m_aor_val_c1(1.0),
				m_aor_val_c2(0.0),
				m_aor_val_sep_a( 0.5235 ), //  1.0 / 6.0) * PI
				m_aor_val_m(-1.14592),
				m_aor_val_n(1.600002),
				m_aor_val_stable_angle(1.22)
		{
			m_cbStepwise.value(m_stepwise);
			m_cbStepwise.callback(callback_settings,this);

			// Set iterations termination criterion initially
			m_selected_index = 0;

			for (unsigned tc = 0; tc < m_terminationCriteria.size() ; ++tc) {
				fltk::Item * item = new fltk::Item(m_terminationCriteria[tc]);
				//choise_flakeSchema[si].callback(callback_settings,this);
				m_choise_termination.add(item);
			}
			m_choise_termination.focus_index(m_selected_index);
			m_choise_termination.callback(callback_settings,this);

			m_selected_aor_index = 2;
			for (unsigned af = 0; af < m_aor_functions.size() ; ++af) {
				fltk::Item * item = new fltk::Item(m_aor_functions[af]);
				m_choise_aor.add(item);
			}
			m_choise_aor.focus_index(m_selected_aor_index);
			m_choise_aor.callback(callback_settings,this);

			m_aor_constant1.linesize(0.1);
			m_aor_constant1.range(0.0f, 1.0f );
			m_aor_constant1.value( m_aor_val_c1 );
			m_aor_constant1.callback(callback_settings, this);

			m_aor_constant2.linesize(0.1);
			m_aor_constant2.range(0.0f, 1.0f );
			m_aor_constant2.value( m_aor_val_c2 );
			m_aor_constant2.callback(callback_settings, this);

			m_aor_sep_angle.linesize(1.0);
			m_aor_sep_angle.range(0.0f, 90.0f );
			m_aor_sep_angle.value( ::Utilities::Math::RadToDeg(m_aor_val_sep_a) );
			m_aor_sep_angle.callback(callback_settings, this);

			m_aor_stable_angle.linesize(1.0);
			m_aor_stable_angle.range(0.0f, 90.0f );
			m_aor_stable_angle.value( ::Utilities::Math::RadToDeg(m_aor_val_stable_angle) );
			m_aor_stable_angle.callback(callback_settings, this);

			m_aor_linear_m.linesize(0.02);
			m_aor_linear_m.range(-1000.0f, 1000.0f );
			m_aor_linear_m.value( m_aor_val_m );
			m_aor_linear_m.callback(callback_settings, this);

			m_aor_linear_n.linesize(0.05);
			m_aor_linear_n.range(-1000.0f, 1000.0f );
			m_aor_linear_n.value( m_aor_val_n );
			m_aor_linear_n.callback(callback_settings, this);

			m_input_iterations.linesize(1.0);
			m_input_iterations.range(1.0f, std::numeric_limits<int>::max() );
			m_input_iterations.value( m_maxIterations );
			m_input_iterations.callback(callback_settings, this);

			m_input_maxTime.linesize(0.1);
			m_input_maxTime.range(1.0f,std::numeric_limits<float>::max());
			m_input_maxTime.value( m_maxTimeInSec );
			m_input_maxTime.callback(callback_settings, this);

			m_input_maxDistToHitLS.linesize(0.1);
			m_input_maxDistToHitLS.range(1.0f,std::numeric_limits<float>::max());
			m_input_maxDistToHitLS.value( m_maxDistToLS );
			m_input_maxDistToHitLS.callback(callback_settings, this);

			m_aor_widget = new GUI::AORWidget<PrecisionType>(7,237, 162, 110);
			m_aor_widget->SetValues(m_aor_val_c1, m_aor_val_sep_a, m_aor_val_c2, m_aor_val_m, m_aor_val_n);

			end();

			add(m_cbStepwise);
			add(m_choise_termination);
			add(m_input_iterations);
			add(m_input_maxTime);
			add(m_input_maxDistToHitLS);
			add(m_choise_aor);
			add(m_aor_stable_angle);
			add(m_aor_constant1);
			add(m_aor_constant2);
			add(m_aor_sep_angle);
			add(m_aor_linear_m);
			add(m_aor_linear_n);
			add(m_label_aor);
			add(m_aor_widget);

			update_bindings();

			m_keyevent_connection = SnowSim::view::Instance()->KeyPressed.connect(
					boost::bind(&ModifierSnowStability::OnKeyPressed, this, _1));

			m_connection_OnSnowModelChanged
					= world->GetSnowModel()->PropertyChanged.connect(
							boost::bind(&ModifierSnowStability::OnSnowModelChanged, this, _1) );


			// register for key input
			if (!m_stepwise) {
				m_keyevent_connection.block();
				//SnowSim::view::Instance()->KeyPressed += event_handler(this, &ModifierSnowStability::OnKeyPressed);
			}

			//m_aor_widget->hide();

			update_ui();
		}

		ModifierSnowStability::~ModifierSnowStability(void) { }

		void ModifierSnowStability::OnSnowModelChanged(SnowModel* sm)
		{

		}

		// update functions objects
		void ModifierSnowStability::update_bindings()
		{
			if (m_selected_index == 0) {
				m_func_continue = boost::bind( &ModifierSnowStability::evalIteratrions, this, _1, _2 );
			} else {
				m_func_continue = boost::bind( &ModifierSnowStability::evalTime, this, _1, _2 );
			}
		}

		void ModifierSnowStability::activate()
		{
			Modifier::activate();
			m_aor_widget->show();
		}

		void ModifierSnowStability::deactivate()
		{
			Modifier::deactivate();
			m_aor_widget->hide();
		}

		void ModifierSnowStability::update_ui()
		{
			if (m_selected_index == 0) {
				m_input_maxTime.hide();
				m_input_iterations.show();
			} else {
				m_input_iterations.hide();
				m_input_maxTime.show();
			}

			m_aor_widget->SetValues(m_aor_val_c1, m_aor_val_sep_a,
					m_aor_val_c2, m_aor_val_m, m_aor_val_n);

			if (m_selected_aor_index == 0) {
				// constant
				m_aor_sep_angle.hide();
				m_aor_constant2.hide();
				m_aor_linear_m.hide();
				m_aor_linear_n.hide();
				m_aor_widget->SetCurrentType(GUI::AORWidget<PrecisionType>::Constant);
			} else if (m_selected_aor_index == 1) {
				// double constant
				m_aor_sep_angle.show();
				m_aor_constant2.show();
				m_aor_linear_m.hide();
				m_aor_linear_n.hide();
				m_aor_widget->SetCurrentType(GUI::AORWidget<PrecisionType>::DuoConstant);
			} else  if (m_selected_aor_index == 2) {
				// linear
				m_aor_sep_angle.show();
				m_aor_constant2.hide();
				m_aor_linear_m.show();
				m_aor_linear_n.show();
				m_aor_widget->SetCurrentType(GUI::AORWidget<PrecisionType>::Linear);
			} else {
				m_aor_sep_angle.hide();
				m_aor_constant1.hide();
				m_aor_constant2.hide();
				m_aor_linear_m.hide();
				m_aor_linear_n.hide();
				m_aor_widget->SetCurrentType(GUI::AORWidget<PrecisionType>::None);
			}
		}


		void ModifierSnowStability::OnKeyPressed(SNOW_KEY key)
		{
			// change selected sky bucket if in info mode
			if (m_stepwise) {
				if (key == m_keyForward) {
						//std::cout << "\t->Forwarding to next lower launch site." << std::endl;
				}
			}
		}

		bool ModifierSnowStability::evalIteratrions(int iteration, LS_Set* u1)
		{
			if (iteration >= m_maxIterations)
				return false;

			return true;
		}

		bool ModifierSnowStability::evalTime(int iteration, LS_Set* u1)
		{
			double currentTime = m_timer.runningTimeInSec();

			if (	( currentTime > m_maxTimeInSec )
				||	((currentTime + m_averageTimePerIteration) > m_maxTimeInSec )
				)
			{
				return false;
			}

			return true;
		}

		bool ModifierSnowStability::execute(void)
		{
			SnowSim::Model::SnowModel* snow_model = m_world->GetSnowModel();
			/* check the number of edge groups */
			unsigned int n_egs = snow_model->GetNumberEdgeGroups();
			if (n_egs < 1) {
				throw Exceptions::PreConditionsViolatedException("ModifierSnowStability: Need at least one edge group.");
			}

			boost::thread thrd( boost::bind(&ModifierSnowStability::Stabilize, this) );

			return true;
		}

		//#pragma optimize("", off)
		bool ModifierSnowStability::Stabilize()
		{
			m_timer.start();

			/*
				Concept:
				1.	Sort all launch sites by absolute height + snow accumulation in list u1
				2.	Go through u1 from heighest to lowest launch site and do stability test
			*/

			/* Implementation: */
			/* 1. Sort by height + snow */

			SnowSim::Model::SnowModel* snow_model = m_world->GetSnowModel();

			unsigned int n_egs = snow_model->GetNumberEdgeGroups();

			/* the number of launch sites */
			unsigned int n_ls = 0;

			/* the number of added launch sites */
			unsigned int n_new_ls = 0;

			/* the number of snow transit over drops to other edge groups */
			unsigned int n_drop_transits = 0;

			/* the number of snow transits into the void */
			unsigned int n_void_transits = 0;

			/* the number of snow transits through the air, falling down, on another site */
			unsigned int n_air_transits = 0;

			/* the number of snow blockings by non-snow objects */
			unsigned int n_blocked_transits = 0;

			/* the number of snow blockings by non-snow objects in the future, if snow movement would have been done */
			unsigned int n_future_blocked_transits = 0;

			/* the current snow density [ (kg / m^3) * 0.001 = (g / cm^3) ] */
			PrecisionType density_g_cm = snow_model->GetDensity() * 0.001;

			/* Sorted set of launch site */
			LS_Set* u1 = new LS_Set();
			LS_Set* u2 = new LS_Set();

			std::cout << "[Modifier] Sorting launch sites by height...";

			#pragma region Sort launch sites

			/* go through all edge groups */
			for (unsigned int i=0; i < n_egs; ++i)
			{
				EdgeGroup* eg = snow_model->GetEdgeGroup(i);
				n_ls += eg->GetNumberLaunchSites();

				EdgeGroup::CDT_VIterator vit = eg->LaunchSites_Begin();
				EdgeGroup::CDT_VIterator end = eg->LaunchSites_End();

				/* go through all launch sites of this edge group */
				/* and add them to the sorted list u1 */
				while (vit != end) {
					PLaunchSite ls = &(*vit);
					u1->insert(ls);
					++vit;
				}
			}

			#pragma endregion

			std::cout << "done" << std::endl << "[Modifier] Stabilizing snow on " << n_egs << " edge groups with a total of "
				<< n_ls << " launch sites." << std::endl;

			/* Create angle of repose evaluator */
			/* Currently for points (1/6 * Pi) and ( 4/9 * Pi) */
			SnowSim::Model::IAngleOfRepose<>* aor_evaluator;
			std::cout << "\t*Stable angle: " << m_aor_val_stable_angle << std::endl
					<< "\t*Stability strategy: ";

			if (m_selected_aor_index == 0) {
				std::cout << "Constant - c: " << m_aor_val_c1 << std::endl;
				// Constant
				aor_evaluator = new SnowSim::Model::AORDefault<PrecisionType>(
						m_aor_val_stable_angle, m_aor_val_c1);
			} else if (m_selected_aor_index == 1) {
				std::cout << "Two constants - c1: " << m_aor_val_c1 << " c2: " << m_aor_val_sep_a << std::endl;
				// Double Constant
				aor_evaluator = new SnowSim::Model::AORDefault<PrecisionType>(
						m_aor_val_stable_angle, m_aor_val_c1, m_aor_val_sep_a, m_aor_val_c2);
			} else {
				std::cout << "Linear function + constant - c1: " << m_aor_val_c1 << " m: " << m_aor_val_m << " n: " << m_aor_val_n << std::endl;
				// Linear + Constant
				// TODO: Funktion noch mal testen ob wirklich im Cosinus-bereich!
				aor_evaluator =
					new SnowSim::Model::AORDefault<PrecisionType>(
							m_aor_val_stable_angle, m_aor_val_c1, m_aor_val_sep_a, m_aor_val_m, m_aor_val_n);
			}

			int stat_totalTests = 0;
			int stat_noSnowMoved = 0;

			BBox3f world_bb = m_world->BoundingBox();

			PrecisionType world_height = world_bb.max.y() - world_bb.min.y();

			Utilities::Random random;

			LS_Set::reverse_iterator lit = u1->rbegin();
			LS_Set::reverse_iterator lit_end = u1->rend();

			int iterations = 0;
			do {
				std::cout << "\tIteration " << iterations + 1 << ":" << std::endl;

				// Performance test -> AspectC++
				Utilities::Timer perf_timer;
				perf_timer.start();

				// Go through all launch sites and compare to their neighbors
				while (lit != lit_end) {

					PLaunchSite ls = *lit;
					PrecisionType ls_area = ls->GetResponsibility();
					PrecisionType ls_area_inv = 1.0 / ls_area;

					#pragma region Optinally stepwise stabilization

					/* PERF: If set so, make stepwise stabilization */
					if (m_stepwise)
					{
						ls->SetHighlighted(true);
						m_world->onChanged();

						while (true)
						{
							//fltk::lock();
							//if ( fltk::get_key_state(m_keyForward) ) {
							if ( fltk::event_key() == m_keyForward ) {
								std::cout << "\t->Forwarding to next lower launch site." << std::endl;

								/* is the the fast forward button pressed? */
								//if ( ! fltk::get_key_state(m_keyFastForward) ) {
								if ( ! (fltk::event_key() == m_keyFastForward) ) {
									/* wait until the key is up again */
									while (true)
									{
										//if ( ! fltk::get_key_state(m_keyForward) ) {
										if ( ! (fltk::event_key() == m_keyForward) ) {
											break;
										}
										my_sleep(10);
									};
								}

								ls->SetHighlighted(false);
								m_world->onChanged();
								//fltk::redraw();

								break;
							}
							//fltk::awake();
							//fltk::unlock();
							my_sleep(10);
						};
					}

					#pragma endregion

					// move the iterator to the next launch site
					// to make continuing easy in other parts
					++lit;

					/* remember current launch site */
					u2->insert(ls);

					// the handling of avalanches over drops is handlet
					// in the neighbor handling
					if (ls->GetType() == ls->OnDrop || ls->GetFace() == NULL)
						continue;

					EdgeGroupFace* egf = ls->GetFace();
					EdgeGroup* eg = egf->GetEdgeGroup();

					EdgeGroup::CDT_VCirculator neighbors = eg->GetNeighbors(ls), done(neighbors);

					PrecisionType ls_snowHeight = ls->GetSnowHeight();
					PrecisionType ls_snowHeight_2 = ls_snowHeight * 0.5;

					if (neighbors != NULL)
					{
						/*  1. Compute AOR between L and all lower N
							2. when large enough AOR:
								do obstacle test (Fearing PhD p. 144), if obstacle:
					 			- if there is a no-snow obstacle in
					 			the way, the avalanche is blocked and (Fearing: (i))
					 			- if there is a VERTICAL snow surface (an edge group boundary)
								in the way, another snow carrying surface is interpenetrating
								between pos_ls and pos_n (Fearing: (ii))
								- if there is no vertical surface B in the way, so there is another
								interpenetrating surface that could potentially receive snow
								(destined for pos_n) - replace LOCALLY pos_n with B (Fearing: (iii))
							2. for all neighbors still in contention shift delta from L
							3. Repeat steps 1 to 3 until no unstable neighbors left
						*/


						// copy all neighbors into a vector
						std::vector<ILaunchSite<PrecisionType>*> vec_neighbors;
						do {
							vec_neighbors.push_back( &(*neighbors) );
						} while (++neighbors != done);

						// should be set to true if a neighbor is added (refinement)
						bool neighbor_added = false;

						std::vector<ILaunchSite<PrecisionType>*>::iterator n_it = vec_neighbors.begin();

						point3f pos_ls = ls->GetPosition();

						// filter higher neighbors
						while (n_it != vec_neighbors.end())
						{
							++stat_totalTests; /*increase status counter */

							PLaunchSite current_n = *n_it;

							point3f pos_n = current_n->GetPosition();

							/* Only consider lower launch sites, get rid of the higher ones */
							if (	(pos_ls.y() < ( pos_n.y() + NearlyZero ) )
								//||	(current_n->GetType() == current_n->OnDrop)
								)
							{
								// neighbor is higher
								n_it = vec_neighbors.erase(n_it);
								continue;
							} else {
								++n_it;
							}
						}

						// now, only lower neighbors are in consideration
						n_it = vec_neighbors.begin();
						std::vector<ILaunchSite<PrecisionType>*>::iterator n_end = vec_neighbors.end();
						PrecisionType neighbot_factor = 1.0 / vec_neighbors.size();

						while (n_it != n_end)
						{
							PLaunchSite current_n = *n_it;

							point3f pos_n = current_n->GetPosition();

							// direction from ls to the neighbor projection to y-height of ls
							Vec3f connection = pos_n - pos_ls;
							Vec3f projXZ_connection(connection);
							projXZ_connection.y() = 0.0;

							// direction from ls to the neighbor
							Vec3f dir1 = normalize( connection );
							Vec3f dir2 = normalize( projXZ_connection );

							PrecisionType angle = acosf( dot(dir1, dir2) );
							/* 1. Compute AOR */
							PrecisionType p = aor_evaluator->GetProbability( angle );

							//debug:
							//std::cout << "angle: " << angle << " prop: " << p << " result: " << random.randomBernoulli(p) << std::endl;

							/* decide if snow should be transfered to neighbor n */
							if ( random.randomBernoulli(p) )
							{
								// snow will not be transfered, since it seems to be stable
								n_it = vec_neighbors.erase(n_it);
								n_end = vec_neighbors.end();
								neighbot_factor = 1.0 / vec_neighbors.size();

								if (n_it == n_end) {
									n_it = vec_neighbors.begin();
								}
								continue;
							}

							/* 2. do obstacle test */
							// !!! If a neighbor has to be added
							// set neighbor_added to true !!!

							// For now check only against parts of the launchsites edge group

							/* 2.1. Check for intersection with edge group surface and the current neighbor*/
							/* PERFORMANCE: Es wäre besser hier nur gegen die Faces zu testen wo ls und n drin sind */
							// FIXME: Eigentlich sollte der Test gegen alle möglichen Edge Groups gehen
							if ( eg->IsIntersectingGround(pos_ls, pos_n) )
							{
								// TODO: Im Moment wird nur weiter gemacht, aber eig. sollte an der Kante eine neue
								// launch site entstehen.
								++n_blocked_transits;
								++n_it;
								continue;
							}

							/* the delta to shift to a lower launch site:
								1. Get the lowest neighbor
								2. Do the snow shift only to him
							*/

							//PrecisionType delta = ( pos_ls.y() - pos_n.y() )  / ( vec_size + 1 );
							PrecisionType proj_length = length(projXZ_connection);
							PrecisionType stable_angle = aor_evaluator->GetStableAngle(angle);

							/* if no angle difference is here, don't do any snow moving */
							if ( (angle - stable_angle) > NearlyZero)
							{
								// calculate the height of the launchsite to reach the stable angle
								//PrecisionType newLSHeight = max( tan(stable_angle) * proj_length, ls_snowHeight_2 + NearlyZero );
								PrecisionType newLSHeight = tan(stable_angle) * proj_length;
								PrecisionType height_diff =  -1 * connection.y();
								PrecisionType newLSHeightDelta =  height_diff - newLSHeight;

								if (newLSHeightDelta > ls_snowHeight) {
									newLSHeightDelta = ls_snowHeight;
									newLSHeight = 0.0;
								}

								// to avoid some float problems that occur when the start ls has a new position with snow height 0
								// (it means the vertex is directly in the plane which leads to an intersection, but we don't
								//  want the intersection right now!)
								newLSHeightDelta -= NearlyZero;

								// calculate the difference in height
								PrecisionType neighbor_delta = newLSHeightDelta * neighbot_factor * ( ls_area / current_n->GetResponsibility() );

								/* create new connection */
								point3f ls_new_pos( pos_ls.x(), pos_ls.y() - newLSHeightDelta, pos_ls.z() );
								point3f n_new_pos( pos_n.x(), pos_n.y() + neighbor_delta, pos_n.z() );

								/* IMPROVEMENT: Check if there would be an surface intersection with new heights
								 * -> continue if there is an intersection and do not modify heights */
								if ( eg->IsIntersectingGround(ls_new_pos, n_new_pos) )
								{
									++n_future_blocked_transits;
									++n_it;
									continue;
								}

								if ( ! (neighbor_delta > NearlyZero) )
								{
									++n_it;
									continue;
								}

								/* add the height (but not to drop sites) */
								if (current_n->GetType() != current_n->OnDrop)
								{
									current_n->AddToSnowHeight(neighbor_delta);

									/* remember the launch neighbor */
									u2->insert(current_n);
								} else {
									/* The snow falls over the edge (drop) */

									/* 1. Test if there is an obstacle nearby */
									point3f intersection;
									point3f pos_n_small_step(pos_n + dir1);
									EdgeGroup* hit_eg = NULL;

									if ( snow_model->IsIntersectingSnow(pos_n, pos_n_small_step, intersection, &hit_eg) )
									{
										if (hit_eg != eg) {
											std::pair<ILaunchSite<PrecisionType>*, PrecisionType> hit
												= hit_eg->GetNearestLaunchSite(intersection);

											++n_drop_transits;

											if ( m_maxDistToLS < hit.second )
											{
												/* the snow has to be added to a new launch site */
												ILaunchSite<PrecisionType>* new_ls
													= hit_eg->AddNewLaunchSite(intersection);

												PrecisionType delta = newLSHeightDelta * ls_area / new_ls->GetResponsibility();

												//std::cerr << delta << " <- ls_area: " << ls_area << " hit_area: " << new_ls->GetResponsibility() <<  std::endl;

												new_ls->AddToSnowHeight(delta);

												++n_new_ls;
											}
											else
											{
												PrecisionType delta = newLSHeightDelta * ls_area / ( hit.first->GetResponsibility() );

												/* the snow has to be added to the found launch site */
												hit.first->AddToSnowHeight(delta);
												u2->insert(hit.first);
											}
										}
									}
									else if ( m_world->Intersect(pos_n, pos_n_small_step, intersection) )
									{
										++n_blocked_transits;
										current_n->AddToSnowHeight(neighbor_delta); // stack the snow up at the border
									}
									else
									{
										// check if the snow avalanche hits another edge group
										// downwards - if it hits the ground and NO edge groups,
										// it will vanish into the void
										point3f pos_2(	pos_n_small_step.x(),
														pos_n_small_step.y() - world_height,
														pos_n_small_step.z());

										if (snow_model->IsIntersectingSnow(
											pos_n_small_step,
											pos_2,
											intersection, &hit_eg) )
										{
											// TODO: Es sollte mehr gemacht werden als nur ein Strahl
											// nach unten zu schicken.

											/* care only about lower hit sites */

											if (intersection.y() < pos_n.y() ) {

												std::pair<ILaunchSite<PrecisionType>*, PrecisionType> hit
													= hit_eg->GetNearestLaunchSite(intersection);

												++n_air_transits;

												if ( m_maxDistToLS < hit.second )
												{
													/* the snow has to be added to a new launch site */
													ILaunchSite<PrecisionType>* new_ls
														= hit_eg->AddNewLaunchSite(intersection);

													PrecisionType delta = newLSHeightDelta * ls_area / new_ls->GetResponsibility();

													//std::cerr << delta << " <= ls_area: " << ls_area
													//	<< " hit_area: " << new_ls->GetResponsibility() <<  std::endl;

													new_ls->AddToSnowHeight(delta);

													++n_new_ls;
												}
												else
												{
													PrecisionType delta = newLSHeightDelta * ls_area / ( hit.first->GetResponsibility() );

													/* the snow has to be added to the found launch site */
													hit.first->AddToSnowHeight(delta);
													u2->insert(hit.first);
												}
											}
										}
										else
										{
											++n_void_transits;
										}
									}
								}

								/* reomve height from origin launch site */
								ls->ReduceSnowHeight(newLSHeightDelta);
								ls_snowHeight = ls->GetSnowHeight();
								ls_snowHeight_2 = ls_snowHeight * 0.5;
								pos_ls = ls->GetPosition();
							}

							/* 3. are there still some neighbors in consideration? */

							// go to the next neighbor (that is still in consideration)
							++n_it;
						}

						#pragma region Test if snow transport affected higher launch sites

						if (neighbor_added) {
							/* Get current neighbors if something has changed */
							neighbors = ls->GetFace()->GetEdgeGroup()->GetNeighbors(ls);
							done = neighbors;
						} else {
							/* reuse old circulator */
							neighbors = done;
						}

						// test if this snow transport affected higher neighbors
						// - do this after the distribution on all neighbors
						do {
							ILaunchSite<PrecisionType>* current_n = &(*neighbors);

							point3f pos_n = current_n->GetPosition();

							// direction from ls to the neighbor
							Vec3f dir1 = normalize(pos_n - pos_ls);

							/* Only consider higher launch sites and non-drop-neighbors */
							if (	(pos_ls.y() < ( pos_n.y() + NearlyZero) )
								&&	(current_n->GetType() != current_n->OnDrop)
								)
							{
								/*
								Vec3f dir2 = normalize( point3f(pos_n.x(), pos_ls.y(), pos_n.z()) - pos_ls );

								PrecisionType angle = acosf( dot(dir1, dir2) );
								// Compute AOR
								PrecisionType p = aor_evaluator->GetProbability(angle);

								*/

								/* decide if snow should be transfered to N*/
								//if ( !random.randomBernoulli(p) ) {
									u2->insert(current_n); // remember for next pass
								//}
							}
						} while (++neighbors != done);

						#pragma endregion
					}
				} // end going through all launch sites top down

				/* update our "unresolved" list */
				delete(u1);
				u1 = u2;
				u2 = new LS_Set();
				lit = u1->rbegin();
				lit_end = u1->rend();

				/* Performance test -> AspectC++ ? */
				perf_timer.stop();
				double takenTime = perf_timer.diffTimeInSec();

				// increase iterations counter
				++iterations;

				//if (iterations == 1)
					m_averageTimePerIteration = takenTime;
				//else
				//	m_averageTimePerIteration = ( takenTime / (iterations - 1) ) + ( takenTime / iterations );

				std::cout << "\t\t-> Performance: Took " << takenTime * 1000 << "ms" << " (Average: " << m_averageTimePerIteration * 1000 << "ms)" << std::endl;

				/* smooth normals */
				snow_model->SmoothNormals();

			} while ( m_func_continue(iterations, u1) && (u1->size() > 0) );

			m_timer.stop();

			std::cout << "\t* Did " << stat_totalTests << " tests with " << stat_totalTests - stat_noSnowMoved
				<< " snow movements to neighbors." << std::endl
				<< "\t* Total time: " << m_timer.diffTimeInMSec() << "msec" << std::endl
				<< "\t* Made " << n_drop_transits << " snow transits over drops to nearby groups" << std::endl
				<< "\t* Created " << n_new_ls << " new launch sites" << std::endl
				<< "\t* Shifted " << n_void_transits << " avalanches into the void" <<std::endl
				<< "\t* Blocked avalanches (by non-snow objcts): " << n_blocked_transits << std::endl
				<< "\t* Future blocked avalanches (by non-snow objcts): " << n_future_blocked_transits << std::endl
				<< "\t* Avalanches that fell down onto launch sites: " << n_air_transits << std::endl;

			delete(aor_evaluator);

			m_world->onChanged();

			return true;
		}

		//#pragma optimize("", on)

		const char * ModifierSnowStability::getName() {
			return ModifierSnowStability::getModifierName();
		}

		/// callback for settings changes
		void ModifierSnowStability::callback_settings()
		{

			m_connection_OnSnowModelChanged.block();

			bool stepwise = m_cbStepwise.value();

			if ( stepwise && !m_stepwise ) {
				m_stepwise = true;
				m_keyevent_connection.unblock();
			} else if ( !stepwise && m_stepwise ) {
				m_stepwise = false;
				m_keyevent_connection.block();
			}

			m_maxIterations = m_input_iterations.value();
			m_maxTimeInSec = m_input_maxTime.value();
			m_maxDistToLS = m_input_maxDistToHitLS.value();

			if (m_selected_index != m_choise_termination.focus_index()) {
				m_selected_index = m_choise_termination.focus_index();

				update_bindings();
			}

			if (m_selected_aor_index != m_choise_aor.focus_index() ) {
				m_selected_aor_index =  m_choise_aor.focus_index();
			}

			m_aor_val_c1 = m_aor_constant1.value();
			m_aor_val_c2 = m_aor_constant2.value();
			m_aor_val_sep_a = SnowSim::Utilities::Math::DegToRad( m_aor_sep_angle.value() );
			m_aor_val_m = m_aor_linear_m.value();
			m_aor_val_n = m_aor_linear_n.value();
			m_aor_val_stable_angle = SnowSim::Utilities::Math::DegToRad( m_aor_stable_angle.value() );

			update_ui();

			m_connection_OnSnowModelChanged.unblock();

			m_world->onChanged();
		}
	}
}
