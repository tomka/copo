#pragma once

#include "ISky.h"
#include "types.h"
#include <Model/ILaunchSite.h>
#include <vector>
#include "SkyBucket.h"
#include <Utilities/Keys.h>
#include <boost/signal.hpp>

#include <fltk/Group.h>
#include <fltk/ValueInput.h>
#include <fltk/Button.h>
#include <fltk/CheckButton.h>
#include <IL/ilu.h>

class ilImage;

namespace SnowSim {
	namespace Model {

		class World;

		// Represents the sky of the world
		class SkyGrid : public ISky, fltk::Group {

			private:
				World* m_world;
				// the amount of sky buckets in height
				int m_heightCount;
				// the amount of sky buckets in width
				int m_widthCount;
				// the sky bucket container
				std::vector<SkyBucket> m_skybuckets;
				// debugging switch
				bool m_debug;
				// debug key connection
				boost::signals::connection m_debug_connection;

				// currently selected sky bucket
				int m_currentSkyBucket;
				// key press handling
				void OnKeyPressed(SNOW_KEY);

				// the sky color
				point3f m_colorSky;
				// the hit color
				point3f m_colorHits;
				// the selected bucket color
				point3f m_colorCurrentBucket;

				// print information about the currently selected sky bucket
				void PrintInfo();

				// total amount of flakes reached the sky
				int m_totalFlakes;

				// the sum of all representative areas of all flakes reached the sky
				PrecisionType m_totalRepArea;

				// toggle the use of image filter
				bool m_use_image_filter;

				// string for saving image file name
				std::string * m_filename;

				// name changed toggle
				bool m_image_changed;

				// the array of floats that work as a filter
				// for our grid
				float * m_filtermap;



				// GUI Elements

				// ValueInput for changing the width count of the grid
				fltk::ValueInput m_gui_vinput_widthCount;
				// ValueInput for changing the height count of the grid
				fltk::ValueInput m_gui_vinput_heightCount;
				// Checkbutton for showing current image path
				fltk::CheckButton m_gui_cb_image_path;
				// Button for choosing image file input
				fltk::Button m_gui_button_image;

				// updates the ui elements
				void update_ui();

				/// static callback for setting changes
				static inline void callback_settings(fltk::Widget *w,void *data)  {
					((SkyGrid*)data)->callback_settings();
				}
				void callback_settings();

				/// static callback for adding an image
				static inline void callback_add_image(fltk::Widget *w,void *data)  {
					((SkyGrid*)data)->callback_add_image();
				}
				void callback_add_image();

				// method for opening an image
				void open(const char* filepath);

				// a cleanup and reinitialization method
				void refresh();

				// resizes, if possible, the filter image
				void resize_image();

				// bool for signaling that, before the next snow gets added,
				// everything should be cleaned up cleaned up as there was
				// no there before. Even the grid cells are created from scratch.
				bool m_needs_refresh;

			public:
				// Creates a new SkyGrid with the given horizontal and vertical size
				SkyGrid(World* world, int height = 1, int width = 1);

				bool Intersect(const ray3f& r, point2f& intersection);

				// Adds flakes shot
				void AddShootingResult(FlakeSchema::ShootingResult& result, ILaunchSite<PrecisionType>* origin);

				// Renders the contents of the sky buckets
				void RenderHits();

				// Renders the sky bucket grid
				void Render();

				// Enable or disable debugging
				void SetDebugging(bool debug);

				// is the interavtive infor mode enabled?
				bool IsDebugging() { return m_debug; }

				// Reprojects the mass to the launch sites
				void ReprojectMass();

				// Resets the sky to its initial state
				void Reset();

				// implement to return a property widget
				fltk::Widget* GetPropertiesWidget()
				{
					return this;
				}
		};
	}
}
