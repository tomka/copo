/*
 * TUDAdapter.cpp
 *
 *  Created on: Aug 22, 2009
 *      Author: tom
 */

#include "TUDAdapter.h"

namespace MeshRepresentations {
	namespace TUDRepresentation {

		TUDAdapter::TUDAdapter()
			: 	tm(NULL),
				tm_bak(NULL),
				hierarchy_factor(32)
		{
			init_mesh();
		}

		TUDAdapter::~TUDAdapter()
		{
			// TODO Auto-generated destructor stub
		}

		void TUDAdapter::init_mesh()
		{
			if (tm)
				delete tm;
			if (tm_bak)
				delete tm_bak;
			tm_bak = 0;

			tm = new tm_type();
			tm->set_hierarchy_factor(hierarchy_factor);
		}

		/* Abstract class implementations */

		/// Gets the number of launch sites in this representation.
		unsigned int TUDAdapter::NumberLaunchSites()
		{
			return tm->get_nr_vertices();
		}

		ILaunchSite<PrecisionType>*
		TUDAdapter::Insert(const point3f& position)
		{
			ILaunchSite<PrecisionType>* new_ls = new LaunchSite( position.x(), position.z() );
			tm_type::point_type new_v(position.x(), position.z(), new_ls);
			tm->add_point( new_v );
			tm->insert_vertex( tm->get_nr_vertices() );

			return new_ls;
		}

		LaunchSite_Pair
		TUDAdapter::InsertConstraint(point3f& a, point3f& b)
		{
			// Seems not to be available in the TUD implementation
		}

		std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
		TUDAdapter::GetNearestLaunchSite(point3f& position)
		{
			unsigned int vi = tm->find_nearest_neighbor( tm_type::point_type(position.x(), position.y() ) );
			tm_type::point_type& p = tm->p_of_vi(vi);
		}

	}
}
