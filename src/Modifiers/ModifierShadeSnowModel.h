#pragma once
#include "Modifier.h"
#include <Model/World.h>
#include <GUI/NoiseWidget.h>
#include <types.h>
#include <fltk/ValueInput.h>
#include <fltk/Button.h>
#include <boost/signal.hpp>
#include <fltk/ScrollGroup.h>
#include <fltk/Window.h>
#include <fltk/Input.h>
#include <fltk/ReturnButton.h>
#include <fltk/CheckButton.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierShadeSnowModel : public Modifier
		{
			typedef SnowSim::Model::World			World;

			private:
				SnowSim::Model::World * m_world;
				/// a control to display a perlin noise sample
				SnowSim::GUI::NoiseWidget<PrecisionType>* m_wNoise;
				/// input: perlin noise octaves
				fltk::ValueInput m_inputPerlinOctaves;
				/// input: perlin noise persistence
				fltk::ValueInput m_inputPerlinPersistence;
				/// input: perlin noise zoom
				fltk::ValueInput m_inputPerlinZoom;
				/// input: flake dusting threshold
				fltk::ValueInput m_inputDustingThreshold;
				/// button: update the texture
				fltk::Button m_buttonSetTexture;
				/// button: recalculate launch site normals
				fltk::Button m_buttonReclalcLSNormals;
				/// button: edit shaders
				fltk::Button m_buttonEditShaders;
				/// checkbutton: use shaders at all
				fltk::CheckButton m_cbUseShaders;
				/// input: snow culling
				fltk::ValueInput m_inputSnowCulling;

				/// Shader editing dialog

				/// the shader editing dialog window
				fltk::Window *edit_shaders_dialog;
				/// the vertex shader file input field in the edit shaders dialog
				fltk::Input *edit_shaders_dialog_vertex_name;
				/// the fragment shader input field in the edit shaders dialog
				fltk::Input *edit_shaders_dialog_fragment_name;
				/// the choose vertex file button in the edit shaders dialog
				fltk::Button *edit_shaders_dialog_choose_vert_file_button;
				/// the choose fragment file button in the edit shaders dialog
				fltk::Button *edit_shaders_dialog_choose_frag_file_button;
				/// checkbox: should vertex shader be used at all?
				fltk::CheckButton *edit_shaders_dialog_use_vertex_shader;
				/// checkbox: should fragment shader be used at all?
				fltk::CheckButton *edit_shaders_dialog_use_fragment_shader;
				/// the cancel button in the edit shaders dialog
				fltk::ReturnButton *edit_shaders_dialog_cancel_button;
				/// the ok button in the edit shaders dialog
				fltk::ReturnButton *edit_shaders_dialog_load_ok_button;

				/// static callback for the set texture button
				static inline void callback_settings(fltk::Widget *w,void *data)  {
					((ModifierShadeSnowModel*)data)->callback_settings();
				}
				void callback_settings();

				/// callback for the set recalc normals button
				static inline void RecalcNormalsButton_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->RecalcNormalsButton_cb();
				}

				/// callback for the set recalc normals button
				void RecalcNormalsButton_cb();

				/// callback for the edit shader button
				static inline void EditShadersButton_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->EditShadersButton_cb();
				}

				/// callback for the edit shader button
				void EditShadersButton_cb();

				/// callback for the set texture butto
				static inline void SetTextureButton_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->SetTextureButton_cb();
				}

				/// callback for the set texture button
				void SetTextureButton_cb();

				/// recalculate the launch site normals
				void recalc_launchsite_normals();


				/// laod vertex shader callback
				static inline void LoadVertexShader_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->LoadVertexShader_cb();
				}

				/// laod vertex shader callback
				void LoadVertexShader_cb();

				/// laod fragment shader callback
				static inline void LoadFragmentShader_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->LoadFragmentShader_cb();
				}

				/// laod vertex shader callback
				void LoadFragmentShader_cb();

				/// laod fragment shader callback
				static inline void LoadShaders_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->LoadShaders_cb();
				}

				/// laod vertex shader callback
				void LoadShaders_cb();

				/// laod fragment shader callback
				static inline void CloseEditShaders_cb(fltk::Widget *w,void *data) {
					((ModifierShadeSnowModel*)data)->CloseEditShaders_cb();
				}

				/// laod vertex shader callback
				void CloseEditShaders_cb();

				// update user interface
				void update_ui();


			public:
				ModifierShadeSnowModel(SnowSim::Model::World * world);
				~ModifierShadeSnowModel(void);

				bool execute(void);
				const char * getName();

				void activate();
				void deactivate();

				static const char * getModifierName() {
					return "Shade snow model";
				}
		};

	}
}
