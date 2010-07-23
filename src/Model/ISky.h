#pragma once

#include <cgv/scene/geometry/ray.h>
#include <types.h>
#include <Model/ILaunchSite.h>
#include <Model/Flake_Schema.h>
#include <fltk/Widget.h>

namespace SnowSim {
	namespace Model {

		// Represents the sky of the world
		class ISky {

			public:
				// Gets the intersection with a ray and the sky.
				virtual bool Intersect(const ray3f& r, point2f& intersection) = 0;

				// Adds some flakes belonging to a specified origin to the sky.
				virtual void AddShootingResult(FlakeSchema::ShootingResult& shooting, ILaunchSite<PrecisionType>* origin) = 0;

				// Renders the snowflake hits into the sky.
				virtual void RenderHits() = 0;

				// Renders the sky.
				virtual void Render() = 0;

				// Enable sky bucket debugging
				virtual void SetDebugging(bool debug) = 0;

				// is the interavtive infor mode enabled?
				virtual bool IsDebugging() = 0;

				// Reprojects the mass to the launch sites
				virtual void ReprojectMass() = 0;

				// Resets the sky to its initial state
				virtual void Reset() = 0;

				// Could be implemented to return a property widget
				virtual fltk::Widget* GetPropertiesWidget()
				{
					return NULL;
				}
		};
	}
}
