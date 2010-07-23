#include "objects.h"

/// render arrow
void render_arrow(const point3f& from, const point3f& to, float stretch, float rad_stretch, int nr_phi, bool normals) {
	stretch *= 25.0f;
	float radius = rad_stretch*length(to-from)/stretch;
	float scale  = 3.0f;
	float aspect = 1.5f;
	render_arrow(from, radius, to, scale, aspect, nr_phi, normals);
}
/// render arrow
void render_arrow(const point3f& from, float radius, const point3f& to, float head_rad_scale, float head_aspect, int nr_phi, bool normals) {
	float head_radius = head_rad_scale*radius;
	float head_width  = head_aspect*head_radius;
	// get carthesian system
	point3f z = to - from;
	float l = length(z);
	z = normalize(z);
	point3f tmp(1.0f,0.0f,0.0f);
	if (fabs(z(1))+fabs(z(2)) < 0.1) {
		tmp(0) = 0;
		tmp(1) = 1;
	}
	point3f x;
	point3f y;
	x = cross(tmp, z);
	x = normalize(x);
	y = cross(z, x);
	float delta_phi = 2.0f * PI / nr_phi;
	glBegin(GL_TRIANGLE_FAN);
	if (normals) {
		glNormal3fv(-z);
	}
	glVertex3fv(from);
	point3f x1;
	point3f y1;
	point3f v;
	double phi = 0;
	int iphi;
	for (iphi=0; iphi<=nr_phi; iphi++, phi += delta_phi) {
		x1 = x;
		y1 = y;
		x1 = (float)(sin(phi)*radius) * x1;
		y1 = (float)(cos(phi)*radius) * y1;
		v = from;
		v += x1 + y1;
		//((v = from) += x1) += y1;
		glVertex3fv(v);
	}
	glEnd();
	// produce cylinder
	glBegin(GL_QUAD_STRIP);
	phi = 0;
	point3f mid = from;
	mid = (1.0f-(1.0f-head_width/l)) * mid + (1.0f-head_width/l) * to;
	for (iphi=0; iphi<=nr_phi; iphi++, phi += delta_phi) {
		x1 = x;
		y1 = y;
		x1 = (float)(sin(phi)*radius) * x1;
		y1 = (float)(cos(phi)*radius) * y1;
		x1 += y1;
		if (normals) {
			y1 = x1;
			y1 = normalize(y1);
			glNormal3fv(y1);
		}
		(v = from) += x1;
		glVertex3fv(v);
		(v = mid) += x1;
		glVertex3fv(v);
	}
	glEnd();
	// produce fan
	glBegin(GL_TRIANGLES);
	point3f x2;
	point3f y2;
	phi = 2 * PI;
	point3f dz = to;
	dz -= mid;
	for (iphi=0; iphi<=nr_phi; iphi++, phi -= delta_phi) {
		x1 = x;
		y1 = y;
		x1 = (float)(sin(phi)*head_radius) * x1;
		y1 = (float)(cos(phi)*head_radius) * y1;
		x1 += y1;
		if (normals) {
			y1 = dz-x1;
			y1 = cross(y1,cross(x1,y1));
			y1 = normalize(y1);
		}
		(v = mid) += x1;
		if (iphi != 0) {
			if (normals) {
				glNormal3fv(y2);
			}
			glVertex3fv(x2);
			if (normals) {
				glNormal3fv(y1);
			}
			glVertex3fv(v);
			if (normals) {
				y2 = cross(v-x2,to-x2);
				y2 = normalize(y2);
				glNormal3fv(y2);
			}
			glVertex3fv(to);
		}
		x2 = v;
		if (normals) {
			y2 = y1;
		}
	}
	glEnd();
	// produce disc with hole
	glBegin(GL_QUAD_STRIP);
	if (normals) {
		glNormal3fv(-z);
	}
	phi = 0;
	for (iphi=0; iphi<=nr_phi; iphi++, phi += delta_phi) {
		x1 = x;
		y1 = y;
		x1 = (float)(sin(phi)*radius) * x1;
		y1 = (float)(cos(phi)*radius) * y1;
		x1 += y1;
		(v = mid) += x1;
		glVertex3fv(v);
		x1 = head_rad_scale * x1;
		(v = mid) += x1;
		glVertex3fv(v);
	}
	glEnd();
}
