#include "SkyGrid.h"
#include <Exceptions/ArgumentException.h>
#include <GL/glew.h>
#include <view.h>
#include <boost/bind.hpp>
#include <fstream>
#include <fltk/file_chooser.h>
#include <fltk/ask.h>

namespace SnowSim {
	namespace Model {

		SkyGrid::SkyGrid(World* world, int height /* = 15 */, int width /* = 15 */)
			: fltk::Group(0,0,100,150),
				m_world(world), m_heightCount(height), m_widthCount(width),
				m_colorSky(blue_color), m_colorHits(violet_color), m_colorCurrentBucket(yellow_color),
				m_debug(false),
				m_currentSkyBucket(0),
				m_totalFlakes(0),
				m_totalRepArea(0.0F),
				m_gui_vinput_widthCount(65,0,20,20, "# Cells in X-Dir:"),
				m_gui_vinput_heightCount(65,20,20,20, "# Cells in Z-Dir:"),
				m_gui_cb_image_path(3,40,40,20,"(none)"),
				m_gui_button_image(50,40,35,18, "load img"),
				m_needs_refresh(false),
				m_use_image_filter(false),
				m_filename(NULL),
				m_filtermap(NULL)

		{
			m_gui_vinput_widthCount.linesize(1.0);
			m_gui_vinput_widthCount.range(1.0f,std::numeric_limits<double>::infinity());
			m_gui_vinput_widthCount.callback(callback_settings, this);

			m_gui_vinput_heightCount.linesize(1.0);
			m_gui_vinput_heightCount.range(1.0f,std::numeric_limits<double>::infinity());
			m_gui_vinput_heightCount.callback(callback_settings, this);

			m_gui_cb_image_path.callback(callback_settings, this);
			m_gui_button_image.callback(callback_add_image, this);

			end();

			add(m_gui_vinput_heightCount);
			add(m_gui_vinput_widthCount);
			add(m_gui_cb_image_path);
			add(m_gui_button_image);

			refresh();
			update_ui();

			ilInit();
			iluInit();
			ilutInit();
		}

		void SkyGrid::callback_add_image()
		{
			const char* filename = fltk::file_chooser("Open new filter image", "*", "");
			if (filename) {
				open(filename);
			}

			update_ui();
		}

		// method for opening an image
		void SkyGrid::open(const char* filepath)
		{
			std::string str_filepath(filepath);

			if (m_filename != NULL) {
				delete m_filename;
			}

			m_filename = new std::string(str_filepath);

			if (m_filtermap != NULL) {
				delete m_filtermap;
				m_filtermap = NULL;
			}

			ILuint ImgId = 0;
			ilGenImages(1, &ImgId);
			ilBindImage(ImgId);

			std::cout << "[Sky] Trying to load image: " << str_filepath << std::endl;

			
#if _WIN32
			std::wstring ws(str_filepath.begin(), str_filepath.end() );
			const wchar_t* fn = ws.c_str();
#else
			const char* fn = str_filepath.c_str();
#endif
			// Load the image from the file name
			if ( ilLoadImage( fn ) ) {
				int size = m_widthCount * m_heightCount;
				unsigned char* charmap = static_cast<unsigned char*>( malloc(size * sizeof(unsigned char)) );

				iluScale(m_widthCount, m_heightCount, 1);
				ilCopyPixels(0, 0, 0, m_widthCount, m_heightCount, 1, IL_LUMINANCE, IL_UNSIGNED_BYTE, charmap);

				m_filtermap = static_cast<float*>( malloc(size * sizeof(float)) );

				//normalize to maximum value of "one"
				unsigned char max_value = 0;
				for (int i=0; i < size; ++i  )
				{
					std::cout << charmap[i] << " ";
					if (charmap[i] > max_value)
						max_value = charmap[i];
				}

				if (max_value > NearlyZero) {
					float divisor = 1.0f / max_value;

					for (int i=0; i < size; ++i)
					{
						m_filtermap[i] = charmap[i] * divisor;
						std::cout << m_filtermap[i] << " ";
					}
				} else {
					for (int i=0; i < size; i=i + sizeof(float)  )
					{
						m_filtermap[i] = 0.0f;
					}
				}

				//cleanup
				if (charmap != NULL)
					delete charmap;

			} else {
				std::string msg("An error occurred while loading the image ");
				msg.append("\"");
				msg.append(filepath);
				msg.append("\"");

				fltk::message(msg.c_str());
				m_filename = NULL;
			}

			ilBindImage(0);
			ilDeleteImage(ImgId);

			m_image_changed = true;

		}

		// resizes, if possible, the filter image
		void SkyGrid::resize_image()
		{
			if (m_filename != NULL)
				open( m_filename->c_str() );
		}

		// a cleanup and reinitialization method
		void SkyGrid::refresh()
		{
			m_skybuckets.clear();

			int nrBuckets = m_heightCount * m_widthCount;
			// fill the sky bucket container
			for (int i = 0; i < nrBuckets; ++i)
				m_skybuckets.push_back(SkyBucket());

			resize_image();

			m_needs_refresh = false;
		}

		void SkyGrid::update_ui()
		{
			m_gui_vinput_widthCount.value(m_widthCount);
			m_gui_vinput_heightCount.value(m_heightCount);

			if (m_use_image_filter == true)
			{
				//m_gui_cb_image_path.deactivate();
				m_gui_button_image.activate();
			}
			else
			{
				//m_gui_cb_image_path.activate();
				m_gui_button_image.deactivate();
			}

			if (m_image_changed) {
				if (m_filename != NULL) {
					m_gui_cb_image_path.label( m_filename->c_str() );
				} else {
					m_gui_cb_image_path.label( "(none)" );
				}
				m_image_changed = false;
			}
		}

		void SkyGrid::callback_settings()
		{
			m_widthCount = static_cast<int>( m_gui_vinput_widthCount.value() );
			m_heightCount = static_cast<int>( m_gui_vinput_heightCount.value() );
			m_use_image_filter = m_gui_cb_image_path.state();

			// a bit rigorous, could be made more efficient
			m_needs_refresh = true;

			update_ui();
		}

		void SkyGrid::Reset()
		{
//			int nrBuckets = m_heightCount * m_widthCount;
//
//			for (int i=0; i < nrBuckets; ++i)
//			{
//				SkyBucket& b = m_skybuckets[i];
//
//				b.m_hit_positions.clear();
//				b.m_origin_hits.clear();
//				b.m_totalArea = 0.0f;
//			}

			refresh();
			update_ui();
		}

		bool SkyGrid::Intersect(const ray3f& r, point2f& intersection)
		{
			// get the parameter for r to get to the intersection point
			float nominator = dot( top_direction, r.direction );

			if (nominator < 0.0001 && nominator > -0.0001)
				return false;

			float t = dot( -top_direction, r.origin - m_world->BoundingBox().max ) / nominator;

			if (/* !neg_t_allowed && */ t < 0.0)
				return false;

			point3f result = r.origin + t * r.direction;

			intersection.x() = result.x();
			intersection.y() = result.z();

			return true;
		}

		void SkyGrid::AddShootingResult(FlakeSchema::ShootingResult& shooting, ILaunchSite<PrecisionType>* origin)
		{
			if (origin == NULL)
				throw Exceptions::ArgumentException("origin must not be null.");

			if (m_needs_refresh == true)
				refresh();

			int size = shooting.hits.size();

			PrecisionType total_area = 0.0F;

			point3f min = m_world->BoundingBox().min;
			point3f max = m_world->BoundingBox().max;

			PrecisionType w = max.x() - min.x();
			PrecisionType h = max.z() - min.z();

			// get dimensions of one bucket
			PrecisionType x_dim = w / m_widthCount;
			PrecisionType y_dim = h / m_heightCount;

			// calculate the area of one bucket
			PrecisionType bucket_area = x_dim * y_dim;

			for (int i = 0; i < size; ++i) {

				PrecisionType x = shooting.hits[i].x() - min.x();
				if (x < NearlyZero) x = NearlyZero;
				else if (x > w) x = w;

				int x_pos = abs( static_cast<int>( x / x_dim ) );

				// for the y position y has to be used for the hit cooridnate,
				// since 2d-vectors only care about x and y
				PrecisionType y = shooting.hits[i].y() - min.z();
				if (y < NearlyZero) y = NearlyZero;
				else if (y > h) y = h;

				int y_pos = abs( static_cast<int>( y / y_dim ) );

				int pos = x_pos + y_pos * m_widthCount;
				if (pos >= m_skybuckets.size()) pos = m_skybuckets.size() - 1;

				SkyBucket& _bucket = m_skybuckets[pos];
				SkyBucket::MapType& _map = _bucket.m_origin_hits;

				// remember the hit position
				_bucket.m_hit_positions.push_back(shooting.hits[i]);

				PrecisionType area = origin->GetResponsibility() / shooting.batch_size;

				// is the flakes area greater then the bucket_area?
				if (area > bucket_area) {
					// has this origin already been added?
					SkyBucket::MapType::iterator it = _map.find(origin);
					if ( it == _map.end() ) {
						// calculate the distributed area for the surrounding buckets
						// TODO: 1. get amount of neighbor buckets
						/*int neighbors = 0;
						if (x_pos > 0) neighbors += 2;
						if (x_pos < (m_widthCount - 1) ) neighbors += 2;
						if (y_pos > 0) neighbors += 2;
						if (y_pos < (m_heightCount - 1) ) neighbors += 2;*/

						// TODO: Use correct amount of neighbors and distribute over them
						PrecisionType dist_area = area * ( 1.0F / 9.0F );
						_map.insert(SkyBucket::PairType(origin, LSInfo(1, dist_area)));
						_bucket.m_totalArea += dist_area;
					} else {
						it->second.total_flakes++;
						it->second.total_area += area;
						_bucket.m_totalArea += area;
					}
				} else {
					// has this origin already been added?
					SkyBucket::MapType::iterator it = _map.find(origin);
					if ( it == _map.end() ) {
						_map.insert(SkyBucket::PairType(origin, LSInfo(1, area)));
					} else {
						it->second.total_flakes++;
						it->second.total_area += area;
					}

					_bucket.m_totalArea += area;
				}

				// add the flake to the total area
				total_area += area;
			}

			// "count" the flakes
			this->m_totalFlakes += size;
			this->m_totalRepArea += total_area;
		}

		//#pragma optimize( "", off )
		// Reprojects the mass to the launch sites
		void SkyGrid::ReprojectMass()
		{
			SnowModel* sm = m_world->GetSnowModel();

			point3f min = m_world->BoundingBox().min;
			point3f max = m_world->BoundingBox().max;

			PrecisionType w = max.x() - min.x();
			PrecisionType h = max.z() - min.z();

			// get dimensions of one bucket
			PrecisionType x_dim = w / m_widthCount;
			PrecisionType y_dim = h / m_heightCount;

			// calculate the area of one bucket
			PrecisionType bucket_area = x_dim * y_dim;

			// convert standard density [kg/m^3] to [g/cm^3]
			PrecisionType density = sm->GetDensity()
				* ( 1000.0f /* for kg -> g */ )
				* ( 1.0f / 1000000.0f /* for m^3 -> cm^3 */ );

			// the mass of one bucket in [g]
			PrecisionType bucket_mass = density /* [g/cm^3] */
				* bucket_area /* [cm^2] */ * sm->MaxSnowHeight() /* [cm] */;

			int size = m_skybuckets.size();

			if (m_use_image_filter &&  m_filtermap != NULL) {
				// go through all buckets
				for (int i=0; i < size; ++i) {
					SkyBucket& _bucket = m_skybuckets[i];

					float filter = m_filtermap[i];

					// calculate a mass per area value [g / cm^2]
					PrecisionType mass_per_bucket_unit_area =
						( bucket_mass /* [g] */ ) / ( _bucket.m_totalArea /* [cm^2] */ );

					SkyBucket::MapType::iterator it = _bucket.m_origin_hits.begin();

					while (it != _bucket.m_origin_hits.end())
					{
						SkyBucket::PairType current = *it;

						PrecisionType ls_mass_addition = mass_per_bucket_unit_area * current.second.total_area;

						if ( !(filter < NearlyZero) )
						{
							current.first->AddToSnowHeight(filter *
								ls_mass_addition
								/
								( current.first->GetResponsibility() * density ) );
						}

						++it;
					}
				}
			} else {
				// go through all buckets
				for (int i=0; i < size; ++i) {
					SkyBucket& _bucket = m_skybuckets[i];

					// calculate a mass per area value [g / cm^2]
					PrecisionType mass_per_bucket_unit_area =
						( bucket_mass /* [g] */ ) / ( _bucket.m_totalArea /* [cm^2] */ );

					SkyBucket::MapType::iterator it = _bucket.m_origin_hits.begin();

					while (it != _bucket.m_origin_hits.end())
					{
						SkyBucket::PairType current = *it;

						PrecisionType ls_mass_addition = mass_per_bucket_unit_area * current.second.total_area;

						current.first->AddToSnowHeight(
							ls_mass_addition
							/
							( current.first->GetResponsibility() * density ) );

						++it;
					}
				}
			}

			this->m_world->UpdateBoundingBox();
		}
		//#pragma optimize( "", on )

		// Enable or disable debugging
		void SkyGrid::SetDebugging(bool debug)
		{
			m_debug = debug;

			// register for key input
			if (!m_debug_connection.connected()) {
				m_debug_connection = SnowSim::view::Instance()->KeyPressed.connect(
					boost::bind(&SkyGrid::OnKeyPressed, this, _1));
			}

			if (m_debug) {
				m_debug_connection.unblock();
				//SnowSim::view::Instance()->KeyPressed += event_handler(this, &SkyGrid::OnKeyPressed);
			} else {
				m_debug_connection.block();
				//SnowSim::view::Instance()->KeyPressed -= event_handler(this, &SkyGrid::OnKeyPressed);
			}
		}

		void SkyGrid::OnKeyPressed(SNOW_KEY key)
		{
			bool update = false;
			bool printinfo = false;

			// change selected sky bucket if in info mode
			if (m_debug) {
				switch (key) {
					case Utilities::Keys::LeftKey:
						if (m_currentSkyBucket > 0
							&& ( (m_currentSkyBucket % m_widthCount) > 0 ))
						{
							update = true;
							m_currentSkyBucket--;
						}
						printinfo = true;
						break;
					case Utilities::Keys::UpKey:
						if ( m_currentSkyBucket >= m_widthCount )
						{
							update = true;
							m_currentSkyBucket -= m_widthCount;
						}
						printinfo = true;
						break;
					case Utilities::Keys::RightKey:
						if (((m_currentSkyBucket % m_widthCount) != (m_widthCount - 1) )
							&& ( m_currentSkyBucket < (m_widthCount * m_heightCount) ) )
						{
							update = true;
							m_currentSkyBucket++;
						}
						printinfo = true;
						break;
					case Utilities::Keys::DownKey:
						if ( (m_currentSkyBucket / m_widthCount) < (m_heightCount - 1) )
						{
							update = true;
							m_currentSkyBucket += m_widthCount;
						}
						printinfo = true;
						break;
					default:
						return;

				}

				if (printinfo) {
					// print the new info
					PrintInfo();
				}

				if (update) {
					// redraw
					SnowSim::view::Instance()->redraw();
				}
			}
		}

		void SkyGrid::PrintInfo()
		{
			SkyBucket& bucket = m_skybuckets[m_currentSkyBucket];

			int totalHits = bucket.m_origin_hits.size();

			// print general information
			std::cout << "[SkyGrid] Currently selected sky bucket information:" << std::endl
				<< "\tID: " << m_currentSkyBucket << std::endl
				<< "\tTotal Hits: " << totalHits << std::endl;

			if (totalHits > 0) {
				std::cout << "\tOrigins / Hits:" << std::endl;

				SkyBucket::MapType::iterator o_it = bucket.m_origin_hits.begin();

				// print origin / hits information
				while (o_it != bucket.m_origin_hits.end()) {
					SkyBucket::PairType p = *o_it;
					point3f pos = p.first->GetPosition();

					std::cout << "\t\t Pos: (" << pos.x() << ", " << pos.y()
						<< ", " << pos.z() << ") / " << "#Flakes: " << p.second.total_flakes << " #Area: " << p.second.total_area << std::endl;

					++o_it;
				}
			}
		}

		#pragma region Rendering

		void SkyGrid::Render()
		{
			#pragma region Render sky buckets

			// OpenGL evaluation from:
			// http://www.opengl.org/resources/code/samples/mjktips/grid/index.html

			point3f min = m_world->BoundingBox().min;
			point3f max = m_world->BoundingBox().max;

			glLineWidth(1.0f);

			// define control points
			GLfloat grid2x2[2][2][3] = {
				{{min.x(), max.y(), min.z()}, {min.x(), max.y(), max.z()}},
				{{max.x(), max.y(), min.z()}, {max.x(), max.y(), max.z()}}
			  };

			// set the color
			glColor3f(m_colorSky[0], m_colorSky[1], m_colorSky[2]);

			glEnable(GL_MAP2_VERTEX_3);
			glMap2f(GL_MAP2_VERTEX_3,
				0.0, 1.0,  /* U ranges 0..1 */
				3,         /* U stride, 3 floats per coord */
				2,         /* U is 2nd order, ie. linear */
				0.0, 1.0,  /* V ranges 0..1 */
				2 * 3,     /* V stride, row is 2 coords, 3 floats per coord */
				2,         /* V is 2nd order, ie linear */
				(GLfloat*)(grid2x2) );  /* control points */

			// tell OpenGL to iterate across the full 0.0 to 1.0 range
			// setup above with m_widthCount rows and m_heightCount columns
			glMapGrid2f(
				m_heightCount, 0.0, 1.0,
				m_widthCount, 0.0, 1.0);

			glEvalMesh2(GL_LINE,
				0, m_heightCount,   /* Starting at 0 mesh 5 steps (rows). */
				0, m_widthCount);  /* Starting at 0 mesh 6 steps (columns). */

			#pragma endregion

			// render interactive info stuff
			if (m_debug) {

				#pragma region Sky bucket visual debug

				glEnable(GL_POLYGON_OFFSET_FILL);
				glPolygonOffset(5.0f, 1.0f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				// set the color
				glColor3f(m_colorCurrentBucket[0], m_colorCurrentBucket[1], m_colorCurrentBucket[2]);

				glPushName((GLuint)m_currentSkyBucket);

				glBegin(GL_POLYGON);
					glNormal3fv(top_direction);

					PrecisionType width = abs(max.x() - min.x()) / this->m_widthCount;
					PrecisionType height = abs(max.z() - min.z()) / this->m_heightCount;

					PrecisionType minX = min.x() + (m_currentSkyBucket % m_widthCount) * width;
					PrecisionType minZ = min.z() + (m_currentSkyBucket / m_widthCount) * height;
					PrecisionType y = max.y();

					glVertex3f(minX, y, minZ);
					glVertex3f(minX + width, y, minZ);
					glVertex3f(minX + width, y, minZ + height);
					glVertex3f(minX, y, minZ + height);

				glEnd();

				glPopName();

				glDisable(GL_LIGHTING);
				glDisable(GL_LIGHT0);
				glPolygonOffset(0.0, 0.0);
				glDisable(GL_POLYGON_OFFSET_FILL);

				#pragma endregion
			}
		}

		void SkyGrid::RenderHits()
		{
			// set to true if a file export of
			// the points should be made
			bool export_to_file = false;

			PrecisionType sky_height = m_world->BoundingBox().max[1];
			int size = m_skybuckets.size();

			glPointSize(3.0f);

			glBegin(GL_POINTS);
				glColor3f(m_colorHits[0], m_colorHits[1], m_colorHits[2]);

				// go through all buckets
				for (int i=0; i < size; ++i) {
					SkyBucket& _bucket = m_skybuckets[i];
					int hits = _bucket.m_hit_positions.size();

					// go through all hits
					for (int b=0; b < hits; ++b) {
						glVertex3f(_bucket.m_hit_positions[b][0], sky_height, _bucket.m_hit_positions[b][1]);
					}
				}
			glEnd();

			if (export_to_file) {
				std:ofstream os;

				os.open("hits.pts", ifstream::in);
				os.close();

				// write only if file does not exist
				if ( os.fail() ) {
					// reset the failbit
					os.clear(ios::failbit);
					os.open("hits.pts", ifstream::out);

					if (os.is_open())
					{
						// go through all buckets
						for (int i=0; i < size; ++i) {
							SkyBucket& _bucket = m_skybuckets[i];
							int hits = _bucket.m_hit_positions.size();

							// go through all hits
							for (int b=0; b < hits; ++b) {
								os << _bucket.m_hit_positions[b][0] << " " << _bucket.m_hit_positions[b][1] << std::endl;
							}
						}
					}

					os.close();
				}
			}
		}

		#pragma endregion
	}
}
