#pragma once

#include "view.h"

#include <cgv/math/linalg/linalg.h>
#include <fltk/Window.h>
#include <fltk/MenuBar.h>
#include <fltk/ItemGroup.h>
#include <fltk/Item.h>
#include <fltk/events.h>
#include <fltk/file_chooser.h>
#include <fltk/Slider.h>
#include <fltk/Color.h>
#include <fltk/FloatInput.h>
#include <fltk/CheckButton.h>
#include <fltk/Button.h>
#include <fltk/ToggleButton.h>
#include <fltk/Choice.h>
#include <fltk/Output.h>
#include <fltk/Divider.h>
#include <fltk/TabGroup.h>
#include <fltk/Browser.h>
#include <fltk/ValueInput.h>
#include <fltk/ReturnButton.h>

#include <IO/MeshReader.h>
#include "Modifiers/Modifiers.h"
#include "types.h"
#include <fstream>
#include <sstream>
#include <strstream>
#include <map>
#include <math.h>
#include <cstdio>
#include "utilities.h"
#include "Model/World.h"
#include "MeshPool.h"
#include <Modifiers/Modifier.h>

namespace SnowSim {
	namespace GUI {

		/**
		* Window is an fltk window.
		**/
		class window : public fltk::Window {
		private:
			enum TabName { Old, Meshes, Modify, General };
			enum Commands { COM_AddObject};

			typedef SnowSim::Modifiers::Modifiers Modifiers;
			typedef SnowSim::Modifiers::Modifier Modifier;

			/// characters for output widget
			const char* outputsOld[5];

			// default mesh
			static const char* m_cStartFile;
			static const char* m_cStartFileName;

			typedef SnowSim::Model::World World;

		protected:
			/// menu widget
			fltk::MenuBar *menu;
			/// opengl display
			view *display;
			/// pointer to base mesh drawable
			SnowSim::polygonal_mesh_drawable* basemesh_drawable;
			/// pointer to modified mesh drawable
			SnowSim::polygonal_mesh_drawable* modifiedmesh_drawable;
			/// pointer to modified mesh drawable backup (mesh before current modifier)
			SnowSim::polygonal_mesh * modifiedmesh_bak;
			SnowSim::polygonal_mesh ** meshToModify;

			/// the currently selected body
			Mesh* currentMesh;

			const char * filename;

			// Does the graphics card support all needed extensions?
			bool m_hasAllExtensions;

			/// modifier list
			std::vector<fltk::Item*> availableModifiers;
			SnowSim::Modifiers::Modifier * currentModifier;

			/// halfedge
			typedef SnowSim::polygonal_mesh::halfedge halfedge;
			/// edge
			typedef SnowSim::polygonal_mesh::edge edge;
			/// vertex
			typedef SnowSim::polygonal_mesh::vertex vertex;
			/// face
			typedef SnowSim::polygonal_mesh::face face;
			/// item for opening
			fltk::Item* add_item;

			/// item for opening
			fltk::Item* open_item;
			/// item for opening
			fltk::Item* reopen_item;
			/// item for closing
			fltk::Item* close_item;
			/// item for saving as
			fltk::Item* save_as_item;
			/// item for saving
			fltk::Item* save_item;
			/// item for quit
			fltk::Item* quit_item;
			/// item for reseting the camera
			fltk::Item* reset_camera_item;
			/// item for saving the camera view
			fltk::Item* save_camera_item;
			/// item for loading a previously saved camera view
			fltk::Item* load_camera_item;
			/// item for exporting as sketch code
			fltk::Item* export_sketch_item;
			/// item for unmark all
			fltk::Item* unmark_all_item;
			/// item for control mesh visible
			fltk::Item* controlmesh_visible_item;
			/// item for fine mesh visible
			fltk::Item* finemesh_visible_item;
			/// item for triangulation
			fltk::Item* triangulate_item;
			/// item for triangulation
			fltk::Item* flip_normals_item;
			/// item for mark all
			fltk::Item* mark_all_item;
			/// save fine mesh
			fltk::Item* save_snowmesh_as_item;

			/// modifier menu:
			/// reexecute modifiers
			fltk::Item* m_menu_mod_reexecute_item;
			/// reset modifiers
			fltk::Item* m_menu_mod_reset_item;

			/// modifier browser
			fltk::Browser* modifier_browser;
			/// modifier choice
			fltk::Choice * modifierChoice;
			/// modifier pause toggle button
			fltk::ToggleButton * m_tbtnModifierPause;
			/// re execute stack button
			fltk::Button * executeModifiersButton;
			/// mesh choice
			fltk::Choice* mesh_choice;
			/// the button for adding a body
			fltk::Button* body_add_button;
			/// the button for removing a button
			fltk::Button* body_remove_button;

			/// the check box for marking a body as convex
			fltk::CheckButton* body_convex_checkbox;

			/// the label for the translation
			fltk::Group* body_translation_group;

			/// the value input for setting the x translation of the current body
			fltk::ValueInput* body_trans_x;
			/// the value input for setting the y translation of the current body
			fltk::ValueInput* body_trans_y;
			/// the value input for setting the z translation of the current body
			fltk::ValueInput* body_trans_z;

			/// the label for the rotation
			fltk::Group* body_rotation_group;

			/// the value input for setting the x rotation axis of the current body
			fltk::ValueInput* body_rot_x;
			/// the value input for setting the y rotation axis of the current body
			fltk::ValueInput* body_rot_y;
			/// the value input for setting the z rotation axis of the current body
			fltk::ValueInput* body_rot_z;
			/// the value input for setting the rotation angle of the current body
			fltk::ValueInput* body_rot_angle;
			/// the value input for setting the scaling factor of the current body
			fltk::ValueInput* body_scaling;
			/// the check button for en/disable rendering vertices of the current body
			fltk::CheckButton* body_render_vertices;
			/// the check button for en/disable rendering edges of the current body
			fltk::CheckButton* body_render_edges;
			/// the check button for en/disable rendering faces of the current body
			fltk::CheckButton* body_render_faces;
			/// the check button for en/disable rendering face normals of the current mesh
			fltk::CheckButton* body_render_normals;
			/// the check button for en/disable rendering vertex normals of the current mesh
			fltk::CheckButton* body_render_v_normals;
			/// the check button for en/disable rendering the bounding box of the current mesh
			fltk::CheckButton* body_render_boundingbox;
			/// the check button for en/disable rendering the bounding box of the world
			fltk::CheckButton* world_render_boundingbox;
			/// the check button for en/disable rendering the axes of the world
			fltk::CheckButton* world_render_axes;

			//
			// add body dialog
			//

			/// the add body dialog window
			fltk::Window *add_body_dialog;
			/// the body name input field in the add body dialog
			fltk::Input *add_body_dialog_body_name;
			/// the file name input field in the add body dialog
			fltk::Input *add_body_dialog_file_name;
			/// the choose file button in the add body dialog
			fltk::Button *add_body_dialog_choose_file_button;
			/// the cancel button in the add body dialog
			fltk::Button *add_body_dialog_cancel_button;
			/// the checkbox for deciding if normals should be recomputed
			fltk::CheckButton *add_body_dialog_recompute_normals;
			/// the create body button field in the add body dialog
			fltk::ReturnButton *add_body_dialog_create_body_button;


			//
			// export sketch dialog
			//

			/// the export sketch dialog window
			fltk::Window *export_sketch_dialog;
			/// the export file name in the export sketch dialog
			fltk::Input *export_sketch_dialog_file_name;
			/// the choose file button in the export sketch dialog
			fltk::Button *export_sketch_dialog_choose_file_button;
			/// the cancel button in the export sketch dialog
			fltk::Button *export_sketch_dialog_cancel_button;
			/// the disable culling checkbox in the export sketch dialig
			fltk::CheckButton *export_sketch_dialog_disable_cull_button;
			/// the export button in the export sketch dialog
			fltk::ReturnButton *export_sketch_dialog_export_button;

			/// whether model is modified
			bool modified;
			/// indicates if there is still an unexecuted modifier on the stack
			bool m_modifier_pending;
			/// the pending modifier (if any)
			Modifier* m_pending_modifier;

			/// the tab panel
			fltk::TabGroup * tabs;
			std::vector<fltk::Group *> * tabPages;

			std::map<TabName, int> * tabMap;

			// Snow stuff
			World * world;

			/// update the gui after changing the body
			void update_mesh_ui();

			void onWorldChanged(World* world);



			// Handler for the addidtion of modifiers
			void OnModifierAdded( Modifier* modifier );

			bool addModifier(Modifiers modifier);

			void save_heightmap();

			// Handler for a cleared modifier stack event
			void OnModifierStackCleared( int previous_size );

		public:

			#pragma region Construction & Destruction

			/// constructor
			window(int x, int y, int width, int height);

			/// destructor
			~window();

			#pragma endregion

			#pragma region GUI updates

			/// update label of window
			void update_label();

			#pragma endregion

			#pragma region Mesh management

			/// Adds a new mes to the scene
			void AddMesh(std::string file_name, std::string display_name, bool recompute_normals);

			#pragma endregion

			#pragma region GUI callbacks

			///callback for the body_choice
			static inline void callback_mesh_choice(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_mesh_choice();
			}
			void callback_mesh_choice();

			/// callback for the remove_button
			static inline void callback_remove_button(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_remove_button();
			}
			void callback_remove_button();

			/// callback for the add_button
			static inline void callback_add_button(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_add_button();
			}
			void callback_add_button();

			/// callback for the choose_file_button in the add_body_dialog
			static inline void callback_add_body_choose_file(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_add_body_choose_file();

			}
			void callback_add_body_choose_file();

			/// static callback for the cancel_button in the add_body_dialog
			static inline void callback_add_body_cancel(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_add_body_cancel();

			}
			/// Callback for the cancel_button in the add_body_dialog
			/// Currently only hides the add_body_dialog again
			void callback_add_body_cancel();

			/// callback for the create_button in the add_body_dialog
			static inline void callback_add_body_create(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_add_body_create();

			}
			void callback_add_body_create();

			/// callback for body_trans_x/y/z input
			static inline void callback_body_trans(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_body_trans();
			}

			void callback_body_trans();

			/// callback for body_rot_x/y/z/angle input
			static inline void callback_body_rot(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_body_rot();
			}
			void callback_body_rot();

			/// callback for body_scaling input
			static inline void callback_body_scaling(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_body_scaling();
			}
			void callback_body_scaling();

			/// callback for body_render_vertices/edges/faces input
			static inline void callback_body_render(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_body_render();
			}
			void callback_body_render();

			/// callback for world_render
			static inline void callback_world_render(::fltk::Widget *w,void *data)
			{
				window* gui=(window*)data;
				gui->callback_world_render();
			}
			void callback_world_render();


			/// callback for unmark all
			static inline void unmark_all_cb(fltk::Widget *w,void *data)  {
				((window*)data)->unmark_all_cb();
			}
			void unmark_all_cb();

			/// callback for mark all
			static inline void mark_all_cb(fltk::Widget *w,void *data)  {
				((window*)data)->mark_all_cb();
			}
			void mark_all_cb();

			/// static callback for the modifierchooser
			static inline void executeStackButton_cb(fltk::Widget *w,void *data) {
				((window*)data)->executeStackButton_cb();
			}
			/// callback for the modifierchooser
			void executeStackButton_cb();

			/// static callback for the modifierchooser
			static inline void modifierchoice_cb(fltk::Widget *w,void *data) {
				((window*)data)->modifierchoice_cb();
			}
			/// callback for the modifierchooser
			void modifierchoice_cb();

			/// static callback for the modifier toggle button
			static inline void modifierpause_cb(fltk::Widget *w,void *data) {
				((window*)data)->modifierpause_cb();
			}
			/// callback for the modifier toggle button
			void modifierpause_cb();

			/// static callback for the modifierbrowser click
			static inline void modifierbrowser_cb(fltk::Widget *w,void *data) {
				((window*)data)->modifierbrowser_cb();
			}
			/// callback for the modifierbrowser click
			void modifierbrowser_cb();

			/// static callback for opening a file
			static inline void open_cb(fltk::Widget *w,void *data) {
				((window*)data)->open_cb();
			}
			/// callback for opening a file
			void open_cb();

			/// static callback for opening a file
			static inline void reopen_cb(fltk::Widget *w,void *data) {
				((window*)data)->reopen_cb();
			}
			/// callback for opening a file
			void reopen_cb();

			/// opens a file with given filename
			void open(const char* filename);

			static inline void save_heightmap_cb(fltk::Widget *w,void *data)
			{
				((window*)data)->save_heightmap();
			}

			/// static callback for saving a file
			static inline void save_finemesh_as_cb(fltk::Widget *w,void *data) {
				((window*)data)->save_finemesh_as_cb();
			}
			/// callback for saving a file
			void save_finemesh_as_cb();

			/// static callback for saving a file
			static inline void save_as_cb(fltk::Widget *w,void *data) {
				((window*)data)->save_as_cb();
			}
			/// callback for saving a file
			void save_as_cb();

			/// static callback for saving a file
			static inline void save_cb(fltk::Widget *w,void *data) {
				((window*)data)->save_cb();
			}
			/// callback for saving a file
			void save_cb();

			/// static callback for saving a file
			static inline void export_sketch_cb(fltk::Widget *w,void *data) {
				((window*)data)->export_sketch_cb();
			}
			/// callback for saving a file
			void export_sketch_cb();

			/// static callback for saving a file
			static inline void export_snow_sketch_cb(fltk::Widget *w,void *data) {
				((window*)data)->export_snow_sketch_cb();
			}
			/// callback for saving a file
			void export_snow_sketch_cb();

			/// static callback for closing a file
			static inline void close_cb(fltk::Widget *w,void *data) {
				((window*)data)->close_cb();
			}
			/// callback for closing a file
			void close_cb();

			/// static callback for the menu entry quit
			static inline void quit_cb(fltk::Widget*, void* data) {
				((window*)data)->quit_cb();
			}
			/// callback for the menu entry quit
			void quit_cb() {
				this->destroy();
			}
			/// static callback to reset the camera
			static inline void reset_camera_cb(fltk::Widget *w,void *data)  {
				((window*)data)->reset_camera_cb();
			}
			/// callback to reset the camera
			void reset_camera_cb() {
				display->reset_camera();
			}

			/// static callback to reset the camera
			static inline void save_camera_cb(fltk::Widget *w,void *data)  {
				((window*)data)->save_camera_cb();
			}
			/// callback to reset the camera
			void save_camera_cb();

			/// static callback to reset the camera
			static inline void load_camera_cb(fltk::Widget *w,void *data)  {
				((window*)data)->load_camera_cb();
			}
			/// callback to reset the camera
			void load_camera_cb();

			/// static callback of the subdivision drawable
			static inline void drawable_cb(fltk::Widget *w,void *data)  {
				((window*)data)->drawable_cb();
			}
			/// callback of the subdivision drawavle
			void drawable_cb() {
				display->redraw();
			}
			/// static callback for the display
			static inline void display_cb(fltk::Widget *w,void *data)  {
				((window*)data)->display_cb();
			}
			/// callback for the display
			void display_cb();

			/// static callback for hide
			static inline void hide_control_mesh_cb(fltk::Widget *w,void *data)  {
				((window*)data)->hide_control_mesh_cb();
			}
			/// callback for hide
			void hide_control_mesh_cb();

			/// static callback for hide
			static inline void reset_modifiers_cb(fltk::Widget *w,void *data)  {
				((window*)data)->reset_modifiers_cb();
			}
			/// callback for hide
			void reset_modifiers_cb();

			/// static callback for hide
			static inline void reexecute_modifiers_cb(fltk::Widget *w,void *data)  {
				((window*)data)->reexecute_modifiers_cb();
			}
			/// callback for hide
			void reexecute_modifiers_cb();

			/// static callback for hide
			static inline void hide_fine_mesh_cb(fltk::Widget *w,void *data)  {
				((window*)data)->hide_fine_mesh_cb();
			}
			/// callback for hide
			void hide_fine_mesh_cb();

			/// static callback for triangulate
			static inline void triangulate_cb(fltk::Widget *w,void *data)  {
				((window*)data)->triangulate_cb();
			}
			/// callback for triangulate
			void triangulate_cb();

			/// static callback for flip normals of current mesh
			static inline void flip_notmals_cb(fltk::Widget *w,void *data)  {
				((window*)data)->flip_notmals_cb();
			}
			/// callback for triangulate
			void flip_notmals_cb();

			#pragma endregion
		};
	}
}
