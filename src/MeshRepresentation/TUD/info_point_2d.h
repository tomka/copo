#pragma once

#include "point_2d.h"

/// type of a point defining the location of a vertex
template <typename coord_type, typename info_type>
class info_point_2d
	: public point_2d<coord_type>
{
	private:
		info_type* m_info;

	public:
		info_point_2d(coord_type x = 0, coord_type y = 0, info_type* info = NULL)
			: point_2d<coord_type>(x, y),
				m_info(info)
		{
		}

		info_type* get_info()
		{
			return m_info;
		}

		void set_info(info_type* info)
		{
			m_info = info;
		}
};
