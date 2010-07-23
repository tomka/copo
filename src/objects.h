#pragma once
#ifdef WIN32
#include <windows.h>
#pragma warning (disable:4311)
#endif

#include "types.h"
#include <fltk/gl.h>

void render_arrow(const point3f& from, const point3f& to, float stretch=1,
				  float rad_stretch=1, int nr_phi=10, bool normals=true);
void render_arrow(const point3f& from, float radius, const point3f& to,
				  float head_rad_scale, float head_aspect, int nr_phi, bool normals);