/*
 * ModifierAddAdditionalBorderPoints.cpp
 *
 *  Created on: Sep 6, 2009
 *      Author: tom
 */

#include "ModifierAddAdditionalBorderPoints.h"
#include <Model/EdgeGroup.h>
#include "Exceptions/PreConditionsViolatedException.h"

namespace SnowSim {
	namespace Modifiers {
		ModifierAddAdditionalBorderPoints::ModifierAddAdditionalBorderPoints(World * world)
			:	Modifier(330, ModifierAddAdditionalBorderPoints::getModifierName()),
				m_world(world)
		{
		}

		ModifierAddAdditionalBorderPoints::~ModifierAddAdditionalBorderPoints()
		{
		}

		const char * ModifierAddAdditionalBorderPoints::getName() {
			return ModifierAddAdditionalBorderPoints::getModifierName();
		}

		struct DropSplitter
		{
			typedef ILaunchSite<PrecisionType>* pls;

			point2f new_point;
			PrecisionType new_height;
			pls start_launchsite;
			pls end_launchsite;

			DropSplitter(point2f p, PrecisionType height, pls s, pls e)
				: new_point(p), new_height(height), start_launchsite(s), end_launchsite(e)
			{}
		};

		/*
		 * Go through all the border triangles of the triangulation and split
		 * them in halfs.
		 */
		bool ModifierAddAdditionalBorderPoints::execute(void)
		{
			// 1. Iterate through all border triangles
			SnowModel* sm = m_world->GetSnowModel();

			unsigned int n_egs = sm->GetNumberEdgeGroups();
			if (n_egs < 1) {
				throw Exceptions::PreConditionsViolatedException("ModifierAccumulateSnow: Need at least one edge group.");
			}

			// go through all edge groups
			for (unsigned int i=0; i < n_egs; ++i)
			{
				EdgeGroup* current_eg = sm->GetEdgeGroup(i);
				unsigned int n_launchsites = current_eg->GetNumberLaunchSites();

				typedef EdgeGroup::Drop_Iterator 	Drop_Iterator;
				typedef ILaunchSite<PrecisionType>	LaunchSite;

				Drop_Iterator 	ds_it = current_eg->DropSources_Begin(),
								ds_end = current_eg->DropSources_End();


				std::vector<DropSplitter> vec_new_points;

				// iterate through the drop sources
				while ( ds_it != ds_end )
				{
					// iterate through all the actual drops
					EdgeGroup::Drop* current_ds, *done;
					current_ds = *ds_it;
					done = current_ds;

					do
					{
						ILaunchSite<PrecisionType>* ls_start = 	current_ds->start_launchsite;
						ILaunchSite<PrecisionType>* ls_end = 	current_ds->end_launchsite;

						// get third point of triangle left of the drop
						ILaunchSite<PrecisionType>* ls_mid;

						if ( current_eg->IsEdge(ls_start, ls_end, &ls_mid) )
						{
							// calculate projected point
							point3f pos_start = ls_start->GetPosition();
							point2f proj_pos_start = point2f( pos_start.x(), pos_start.z() );

							point3f pos_end = ls_end->GetPosition();
							point3f pos_mid = ls_mid->GetPosition();

							point3f dir_s_m = pos_mid - pos_start;
							point3f dir_s_e = pos_end - pos_start;
							point3f dirn_s_e = normalize( dir_s_e );
							point2f proj_dir_s_e = point2f(dir_s_e.x(), dir_s_e.z());
							point2f proj_dirn_s_e = point2f(dirn_s_e.x(), dirn_s_e.z());

							point3f mid = pos_start + pos_end;
							mid = 0.5f * mid;

							PrecisionType height = mid.y();
							point2f new_pt(mid.x(), mid.z());
							vec_new_points.push_back( DropSplitter(new_pt, height, ls_start, ls_end) );

//							PrecisionType proj_length =  dot(
//									point2f(dir_s_m.x(), dir_s_m.z()),
//									proj_dirn_s_e );

//							point2f new_pt = proj_pos_start + proj_length * proj_dirn_s_e;

//							PrecisionType ratio = proj_length / length( proj_dir_s_e);

//							if ( ratio > NearlyZero ) {
//								if ( ratio < (1.0 - NearlyZero) ) {
//									// calculate new height
//									PrecisionType height_diff = pos_end.y() - pos_start.y();
//									PrecisionType height = pos_start.y() + ( height_diff * ratio );
//
//									vec_new_points.push_back( DropSplitter(new_pt, height, ls_start, ls_end) );
//								} else {
//									// look into next constraint, currently ignored
//								}
//							} else
//							{
//								// look backwards,  currently ignored
//							}
						}

						current_ds = current_ds->next;
					} while ( current_ds != done );

					++ds_it;
				}

				// remove old constrained, add new ones
				std::vector<DropSplitter>::iterator split_it = vec_new_points.begin();

				while ( split_it != vec_new_points.end() )
				{
					DropSplitter ds = *split_it;

					// remove old constraint
					bool result = current_eg->RemoveConstraint(ds.start_launchsite, ds.end_launchsite);

					if (result == true)
					{
						// add two new ones
						ILaunchSite<PrecisionType>* new_ls
							= current_eg->Insert( point3f( ds.new_point.x(), 0.0, ds.new_point.y() ) );

						new_ls->SetBaseHeight(ds.new_height);
						new_ls->SetType( LaunchSite::OnDrop );

						//current_eg->InsertConstraint(ds.start_prev_constraint, ds.)
						result = current_eg->InsertConstraint( ds.start_launchsite , new_ls );
						result &= current_eg->InsertConstraint( new_ls, ds.end_launchsite );

						if (result == false)
						{
							throw std::logic_error("A constraint could not be inserted!");
						}
					}

					++split_it;
				}
			}
		}

	}
}
