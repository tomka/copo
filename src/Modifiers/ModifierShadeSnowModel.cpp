#include "ModifierShadeSnowModel.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include "Exceptions/NotSupportedException.h"
#include <MeshPool.h>
#include <Utilities/Noise.h>
#include <Shading/Texture.h>
#include <fltk/ask.h>
#include <fltk/file_chooser.h>
#include <Shading/GLSL_Shader.h>

// to get no conflicts with the numeric_limits max
#undef max

namespace SnowSim {
	namespace Modifiers {

		ModifierShadeSnowModel::ModifierShadeSnowModel(World * world)
			:	Modifier(350, ModifierShadeSnowModel::getModifierName() ),
				m_world(world),
				m_inputDustingThreshold(130, 10, 40, 20, "@s10;Dusting threshold [cm]"),
				m_inputPerlinOctaves(130, 35, 40, 20, "Perlin octaves"),
				m_inputPerlinPersistence(130, 60, 40, 20, "Perlin persistence"),
				m_inputPerlinZoom(130, 85, 40, 20, "Perlin zoom"),
				m_buttonSetTexture(10, 110, 155, 18, "Set dust texture"),
				m_inputSnowCulling(130, 292, 40, 18, "@s10;Hide snow below [cm]"),
				m_buttonReclalcLSNormals(10, 313, 155, 18, "Recalc. snow normals"),
				m_cbUseShaders(7, 337, 14, 14, ""),
				m_buttonEditShaders(30, 335, 135, 18, "Edit shaders")
		{
			end();

			// GUI
			this->m_wNoise = new SnowSim::GUI::NoiseWidget<PrecisionType>(10, 133, 155, 155);

			m_inputDustingThreshold.linesize(0.05);
			m_inputDustingThreshold.range(0.0f,std::numeric_limits<float>::max());
			m_inputDustingThreshold.callback(callback_settings, this);

			m_inputPerlinOctaves.linesize(1.0);
			m_inputPerlinOctaves.range(1.0f, std::numeric_limits<int>::max() );
			m_inputPerlinOctaves.callback(callback_settings, this);

			m_inputPerlinPersistence.linesize(0.1);
			m_inputPerlinPersistence.range(0.0f,std::numeric_limits<float>::max());
			m_inputPerlinPersistence.callback(callback_settings, this);

			m_inputPerlinZoom.linesize(0.1);
			m_inputPerlinZoom.range(0.05f,std::numeric_limits<float>::max());
			m_inputPerlinZoom.callback(callback_settings, this);

			m_buttonSetTexture.callback(SetTextureButton_cb, this);
			m_buttonReclalcLSNormals.callback(RecalcNormalsButton_cb, this);
			m_buttonEditShaders.callback(EditShadersButton_cb, this);

			m_inputSnowCulling.linesize(0.001);
			m_inputSnowCulling.range(0.0f,std::numeric_limits<float>::max());
			m_inputSnowCulling.callback(callback_settings, this);

			m_cbUseShaders.callback(callback_settings, this);

			add(m_inputDustingThreshold);
			add(m_inputPerlinOctaves);
			add(m_inputPerlinPersistence);
			add(m_inputPerlinZoom);
			add(m_buttonSetTexture);
			add(m_wNoise);
			add(m_inputSnowCulling);
			add(m_buttonReclalcLSNormals);
			add(m_cbUseShaders);
			add(m_buttonEditShaders);

			// Create the Add Body Dialog
			edit_shaders_dialog = new fltk::Window(480,100, "Edit shaders");
			edit_shaders_dialog->begin();
				edit_shaders_dialog_vertex_name = new fltk::Input(115,10,280,24,"Vertex Shader:");
				edit_shaders_dialog_fragment_name = new fltk::Input(115,40,280,24,"Fragment Shader:");
				edit_shaders_dialog_choose_vert_file_button = new fltk::Button(400,10,70, 24,"Load File");
				edit_shaders_dialog_choose_vert_file_button->callback(LoadVertexShader_cb, this);
				edit_shaders_dialog_choose_frag_file_button = new fltk::Button(400,40,70, 24,"Load File");
				edit_shaders_dialog_choose_frag_file_button->callback(LoadFragmentShader_cb, this);
				edit_shaders_dialog_use_vertex_shader = new fltk::CheckButton(10,70,150,24,"Use Vertex shader");
				edit_shaders_dialog_use_fragment_shader = new fltk::CheckButton(165,70,150,24,"Use Fragment shader");
				edit_shaders_dialog_cancel_button = new fltk::ReturnButton(325,70,70,24,"Cancel");
				edit_shaders_dialog_cancel_button->callback(CloseEditShaders_cb, this);
				edit_shaders_dialog_load_ok_button = new fltk::ReturnButton(400,70,70,24,"Load");
				edit_shaders_dialog_load_ok_button->callback(LoadShaders_cb, this);
			edit_shaders_dialog->end();

			update_ui();
		}

		ModifierShadeSnowModel::~ModifierShadeSnowModel(void) { }

		bool ModifierShadeSnowModel::execute(void) {

			return true;
		}

		// update user interface
		void ModifierShadeSnowModel::update_ui()
		{
			m_inputDustingThreshold.value( m_world->GetSnowModel()->GetFlakeDustingThreshold() );
			m_inputPerlinOctaves.value( m_wNoise->GetPerlinOctaves() );
			m_inputPerlinPersistence.value( m_wNoise->GetPerlinPersistence() );
			m_inputPerlinZoom.value( m_wNoise->GetPerlinZoom() );

			SnowModel* sm = m_world->GetSnowModel();

			m_inputSnowCulling.value ( sm->GetSnowClippingThreshold() );

			m_cbUseShaders.state( sm->UseSnowShader() );

			if ( m_cbUseShaders.state() == false )
			{
				m_buttonEditShaders.deactivate();
			} else {
				m_buttonEditShaders.activate();
			}
		}

		const char * ModifierShadeSnowModel::getName() {
			return ModifierShadeSnowModel::getModifierName();
		}

		void ModifierShadeSnowModel::callback_settings()
		{
			m_wNoise->SetPerlinOctaves( m_inputPerlinOctaves.value() );
			m_wNoise->SetPerlinPersistence( m_inputPerlinPersistence.value() );
			m_wNoise->SetPerlinZoom( m_inputPerlinZoom.value() );
			m_world->GetSnowModel()->SetFlakeDustingThreshold( m_inputDustingThreshold.value() );

			try
			{
				m_world->GetSnowModel()->UseSnowShader( m_cbUseShaders.state() );
			}
			catch (SnowSim::Exceptions::NotSupportedException& ex)
			{
				std::cerr << std::endl << "[Error] " << ex.what() << std::endl
					<< "\t->Feature has been disabled." << std::endl;
			}

			m_world->GetSnowModel()->SetSnowClippingThreshold( m_inputSnowCulling.value() );

			m_world->onChanged();
			update_ui();
		}

		void ModifierShadeSnowModel::activate()
		{
			Modifier::activate();
			m_wNoise->show();
		}

		void ModifierShadeSnowModel::deactivate()
		{
			Modifier::deactivate();
			m_wNoise->hide();
		}

		void ModifierShadeSnowModel::EditShadersButton_cb()
		{
			SnowModel *sm =	m_world->GetSnowModel();
			Shading::Shader* shader = sm->GetSnowShader();
			if ( shader != NULL )
			{
				edit_shaders_dialog_vertex_name->value( shader->GetVertexProgramPath().c_str() );
				edit_shaders_dialog_fragment_name->value( shader->GetFragmentProgramPath().c_str() );
			} else
			{
				edit_shaders_dialog_vertex_name->value( "" );
				edit_shaders_dialog_fragment_name->value( "" );
			}
			edit_shaders_dialog->show();
		}

		/// laod vertex shader callback
		void ModifierShadeSnowModel::LoadVertexShader_cb()
		{
			const char* filename = fltk::file_chooser("Open GLSL vertex shader file", "*", "");
			edit_shaders_dialog_vertex_name->value( filename );
		}


		/// laod vertex shader callback
		void ModifierShadeSnowModel::LoadFragmentShader_cb()
		{
			const char* filename = fltk::file_chooser("Open GLSL fragment shader file", "*", "");
			edit_shaders_dialog_fragment_name->value( filename );
		}

		/// cancel shader changes callback
		void ModifierShadeSnowModel::CloseEditShaders_cb()
		{
			edit_shaders_dialog->hide();
		}

		// load new shader configuration
		void ModifierShadeSnowModel::LoadShaders_cb()
		{
			std::cout << "[Modifier] Changing shader configuration...";

			SnowModel *sm =	m_world->GetSnowModel();

			Shading::Shader* shader = new Shading::GLSL_Shader();

			const char* vertex_path = edit_shaders_dialog_vertex_name->value();
			const char* fragment_path = edit_shaders_dialog_fragment_name->value();

			if ( edit_shaders_dialog_use_vertex_shader->state() == true ) {
				shader->unloadShaders();
				shader->load_vertex_shader( vertex_path );

				if ( edit_shaders_dialog_use_fragment_shader->state() == true ) {
						shader->load_fragment_shader( fragment_path );
				}
			} else if ( edit_shaders_dialog_use_fragment_shader->state() == true ) {
				shader->unloadShaders();
				shader->load_fragment_shader( fragment_path );
			}

			sm->SetSnowShader( shader );

			CloseEditShaders_cb();
			std::cout << "ready" << std::endl;
		}

		/// callback for the set recalc normals button
		void ModifierShadeSnowModel::RecalcNormalsButton_cb()
		{
			try {
				recalc_launchsite_normals();
			} catch (SnowSim::Modifiers::Exceptions::PreConditionsViolatedException &ex) {
				std::string msg("Precondition violated: ");
				msg.append(ex.what());

				fltk::message(msg.c_str());
			}
		}

		void ModifierShadeSnowModel::recalc_launchsite_normals()
		{
			SnowSim::Model::SnowModel* snow_model = m_world->GetSnowModel();

			unsigned int n_egs = snow_model->GetNumberEdgeGroups();
			if (n_egs < 1) {
				throw Exceptions::PreConditionsViolatedException("Recalculate Snow normals: Need at least one edge group.");
			}
			FlakeSchema::Flake_Schema<>* flake_schema = snow_model->GetFlakeSchema();

			std::cout << "[Modifier] Recalculating snow normals..." << std::endl;
			std::cout << "\t*Progress: ";

			int percent_step = static_cast<int>(n_egs * 0.1f);
			int step = 10;

			if (percent_step < 1)
			{
				step = static_cast<int>(100.0f / n_egs);
				percent_step = 1;
			}

			unsigned int percent_count = 0;
			int percent = 0;
			unsigned int eg_count = 0;

			SnowSim::Utilities::Timer t;

			t.start();

			// go through all edge groups
			//#pragma omp parallel for
			for (int i=0; i < n_egs; ++i)
			{
				EdgeGroup* current_eg = snow_model->GetEdgeGroup(i);
				int n_launchsites = current_eg->GetNumberLaunchSites();

				// go through all launch sites on the current edge group
				for (EdgeGroup::CDT_VIterator vit = current_eg->LaunchSites_Begin();
						vit != current_eg->LaunchSites_End(); ++vit)
				{
					// TOMTODO: Im Moment wird die WICHTIGKEIT (Importance) ignoriert,
					// da sie noch nicht implementiert ist.
					// get current launch site
					ILaunchSite<PrecisionType>* current_ls = &(*vit);

					//// ignore vertices that are connected to constrains (e. g. are on a drop)
					//if ( current_eg->Are_there_incident_constraints(vit) )
					//	continue;

					/// computes and returns the unit normal of the face
					fix_col_vec<PrecisionType, 3> n(0.0f, 0.0f, 0.0f);

				 	EdgeGroup::CDT_VCirculator neighbor = current_eg->GetNeighbors(current_ls), done(neighbor);
				 	ILaunchSite<PrecisionType> *n1, *n2 = &(*neighbor);

				 	point3f own_pos = current_ls->GetPosition();

				 	do {
				 		n1 = n2;
				 		++neighbor;
				 		n2 = &(*neighbor);

				 		point3f vec_n1(n1->GetPosition() - own_pos);
				 		point3f vec_n2(n2->GetPosition() - own_pos);

				 		n +=cross( vec_n2, vec_n1 );
				 	} while (neighbor != done);

				 	n = (1 / length( n )) * n;
				 	current_ls->SetNormal( n );
				}

				if (eg_count > percent_count)
				{
					//#pragma omp critical(eg_count_ls_creation)
					{
						// to make sure that an parallel thread does not
						// does the increment, too - we check again.
						if ( eg_count > percent_count ) {
							std::cout << " " << percent;
							std::flush(std::cout);
							percent_count += percent_step;
							percent += step;
						}
					}
				}

				//#pragma omp atomic
				++eg_count;
			}

			std::cout << "...ready" << std::endl;

		}

		/// callback for the modifyerchooser
		void ModifierShadeSnowModel::SetTextureButton_cb()
		{
			try {

				SnowSim::Shading::Texture texture;

				int dim = 128;
				int bytes_per_pixel = 1;
				texture.internal_format = GL_ALPHA8;
				texture.width = dim;
				texture.height = dim;
				texture.depth = dim;
				texture.level = 0;
				texture.border = 0;
				texture.format = GL_ALPHA;
				texture.type = GL_UNSIGNED_BYTE;

				std::cout << "[Modifier] Generating new perlin noise dust texture (" << dim << "x" << dim << "x" << dim << " [px])" << std::endl;

				// ask for enough memory for the texels and make sure we got it before proceeding
				unsigned char * texels = (unsigned char *) malloc(dim * dim * dim * bytes_per_pixel);
				if (texels == NULL)
					throw std::runtime_error(std::string("Snow model: not enough memory for texture creation."));
				int s, t, r;

				float inv_zoom = m_wNoise->GetPerlinZoom();
				int octaves = m_wNoise->GetPerlinOctaves();
				float persistence = m_wNoise->GetPerlinPersistence();


				for (s = 0; s < texture.width; ++s) {
					for (t = 0; t < texture.height; ++t) {
						for (r = 0; r < texture.depth; ++r) {
							int pos = bytes_per_pixel * (s + t*texture.width + r*texture.width*texture.height );

							/* get the perlin noise value for the current texel; [-1, 1] */
							float perlin_value
									= Utilities::Noise::PerlinNoise_3D<float>(s * inv_zoom , t * inv_zoom, r * inv_zoom,
										octaves, persistence);

							/* scale it to 0...255 */
							perlin_value = (perlin_value + 1.0f) * 125.5f;

							if (perlin_value < NearlyZero)
								perlin_value = NearlyZero;
							else if (perlin_value > 255.0f)
								perlin_value = 255.0f - NearlyZero;

							texels[pos] = static_cast<unsigned char>(perlin_value);
						}
					}
				}

				texture.pixels = texels;

				m_world->GetSnowModel()->SetFlakeDustingTexture(texture);
				m_world->onChanged();

			}
			catch (SnowSim::Exceptions::NotSupportedException& ex) {
				m_world->GetSnowModel()->SetFlakeDusting(false);
				std::cerr << std::endl << "[Error] " << ex.what() << std::endl
					<< "\t->Feature has been disabled." << std::endl;
			}
		}

	}
}
