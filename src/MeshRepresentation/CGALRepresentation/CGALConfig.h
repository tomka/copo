#pragma once

//#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <types.h>

namespace MeshRepresentations {
	namespace CGALRepresentation {

		//struct K : CGAL::Exact_predicates_inexact_constructions_kernel {};
		struct K : CGAL::Simple_cartesian< PrecisionType > { };

	}
}