#include "window.h"
#include <fltk/ask.h>
#include <MainControler.h>
#include <Exceptions/NotImplementedException.h>
#include <iostream>
#include <fstream>
#include <fltk/ask.h>
#include <boost/bind.hpp>
#include <iostream>
#include <Utilities/Strings.h>
#include <fltk/ScrollGroup.h>

namespace SnowSim {
	namespace GUI {

		const char* window::m_cStartFileName( "Test" );
		const char* window::m_cStartFile( "../../obj/warncone.obj" ); // NULL for no mesh
		//const char* window::m_cStartFile( "/home/tom/nickzucc.obj" );
		//const char* window::m_cStartFile( NULL );
		/*
			"../obj/vierwuerfelmitRot.obj"
			"../obj/standard2.obj"
			"../obj/torus_no_interior.obj"
			"../obj/standard2_plate.obj"
			"../obj/standard2.obj"
			"../obj/Hydrant2.obj"
			"../obj/warncone.obj"
			"../obj/boxtorus_plane.obj"
			"../obj/ueberhang.obj"
			"cube1x1x1"
			NULL
		*/

		/// constructor
		window::window(int x, int y, int width, int height)
			: 	fltk::Window(x,y,width,height, "Snow accumulation simulation", true),
				m_modifier_pending(false),
				m_pending_modifier(0)
			{
				// get the world
				world = MainControler::GetInstance()->GetWorld();

				// register handler for world changes
				world->Changed.connect(
						boost::bind(&window::onWorldChanged, this, _1));

				// register handler for the addition of modifiers
				MainControler::GetInstance()->ModifierAdded.connect(
						boost::bind(&window::OnModifierAdded, this, _1));

				MainControler::GetInstance()->ModifierStackCleared.connect(
						boost::bind(&window::OnModifierStackCleared, this, _1));

				outputsOld[0] = "no";
				outputsOld[1] = "1";
				outputsOld[2] = "2";
				outputsOld[3] = "3";
				outputsOld[4] = "4";

				basemesh_drawable = NULL;
				modifiedmesh_drawable = NULL;
				currentModifier = NULL;
				meshToModify = NULL;
				modifiedmesh_bak = NULL;

				//fltk::Item* item;
				//item = new fltk::Item(SnowSim::Modifiers::ModifierAddObject::getName());
				//subdiv_schemes[si]->callback(scheme_cb,this);

				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierTriangulate::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierSubDivide::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierCreateEdgeGroups::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierCreateLaunchSites::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierAccumulateSnow::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierSnowStability::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierRegularStorm::getModifierName()));
				availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierShadeSnowModel::getModifierName()));
				//availableModifiers.push_back(new fltk::Item(SnowSim::Modifiers::ModifierAddAdditionalBorderPoints::getModifierName()));

				begin();

				fltk::ItemGroup* group;

				#pragma region GUI - Menu

				menu = new fltk::MenuBar(0,0,width,24);
				menu->begin();

					group = new fltk::ItemGroup("&File");
					group->begin();
						open_item = new fltk::Item("&Open",NULL,open_cb,this);
						reopen_item = new fltk::Item("&Reopen",NULL,reopen_cb,this);
						new fltk::Divider();
						close_item = new fltk::Item("&Close",NULL,close_cb,this);
						new fltk::Divider();
						save_item = new fltk::Item("&Save",NULL,save_cb,this);
						save_as_item = new fltk::Item("Save &as",NULL,save_as_cb,this);
						save_snowmesh_as_item = new fltk::Item("Save &snow mesh as",NULL,save_finemesh_as_cb,this);
						new fltk::Divider();
						export_sketch_item = new fltk::Item("Export mesh as Sketch",NULL,export_sketch_cb, this);
						export_sketch_item = new fltk::Item("Export snow as Sketch",NULL,export_snow_sketch_cb, this);
						new fltk::Divider();
						quit_item = new fltk::Item("&Quit",NULL,quit_cb,this);
					group->end();

					group = new fltk::ItemGroup("&Edit");
					group->begin();
						mark_all_item = new fltk::Item("&Mark all",NULL,mark_all_cb,this);
						unmark_all_item = new fltk::Item("&Unmark all",NULL,unmark_all_cb,this);
						new fltk::Divider();
						triangulate_item = new fltk::Item("&Triangulate",NULL,triangulate_cb,this);
						new fltk::Divider();
						flip_normals_item= new fltk::Item("F&lip Normals",NULL,flip_notmals_cb,this);
					group->end();

					group = new fltk::ItemGroup("&View");
					group->begin();
						reset_camera_item = new fltk::Item("&Reset camera",fltk::COMMAND+'r',reset_camera_cb,this);
						save_camera_item = new fltk::Item("&Save camera",fltk::COMMAND+'u',save_camera_cb,this);
						load_camera_item = new fltk::Item("&Load camera",fltk::COMMAND+'i',load_camera_cb,this);

						new fltk::Divider();

						controlmesh_visible_item = new fltk::Item("Toggle control mesh visibility",fltk::COMMAND+'x',hide_control_mesh_cb,this);
						finemesh_visible_item = new fltk::Item("Toggle fine mesh visibility",fltk::COMMAND+'y',hide_fine_mesh_cb,this);
					group->end();

					group = new fltk::ItemGroup("&Modifiers");
					group->begin();
						m_menu_mod_reexecute_item = new fltk::Item("&Reexecute modifiers",NULL,reexecute_modifiers_cb,this);
						m_menu_mod_reset_item = new fltk::Item("&Reset modifiers",NULL,reset_modifiers_cb,this);
					group->end();

				menu->end();

				#pragma endregion

				#pragma region GUI

				display = view::CreateInstance(0,24,width-200,height-24, world);
				display->callback(display_cb,this);

				fltk::Group * currentTab;
				tabPages = new std::vector<fltk::Group *>();

				fltk::ScrollGroup * sg;
				sg= new fltk::ScrollGroup(width-190,24,190,height-24, NULL, true);
					tabs = new fltk::TabGroup(0,0,185,578, NULL, true);
					tabMap = new std::map<TabName, int>();
						// tab one (Meshes)
						currentTab = new fltk::Group(0,20,185,558, "Meshes", true);
							tabPages->push_back(currentTab);
							tabMap->insert( std::make_pair( Meshes, 0 ));

							sg = new fltk::ScrollGroup(1,1,183,550, NULL, true);
								mesh_choice = new fltk::Choice(45,10,133,20, "Mesh: ");
								mesh_choice->add("None");

								body_add_button = new fltk::Button(5,40,85,20, "Add Mesh");
								body_remove_button = new fltk::Button(95,40,85,20, "Remove Mesh");
								body_convex_checkbox = new fltk::CheckButton(5, 70, 80, 20, "Body is convex");
								body_translation_group = new fltk::Group(35,115,1,1,"Translation:",false);
								body_trans_x = new fltk::ValueInput(20,125,40,20, "X:");
								body_trans_y = new fltk::ValueInput(80,125,40,20, "Y:");
								body_trans_z = new fltk::ValueInput(140,125,40,20, "Z:");
								body_rotation_group = new fltk::Group(30,170,1,1,"Rotation:",false);
								body_rot_x = new fltk::ValueInput(20,180,40,20, "X:");
								body_rot_y = new fltk::ValueInput(80,180,40,20, "Y:");
								body_rot_z = new fltk::ValueInput(140,180,40,20, "Z:");
								body_rot_angle = new fltk::ValueInput(43,210,40,20,"Angle:");
								body_scaling = new fltk::ValueInput(83,240,40,20, "Scaling factor:");
								body_render_vertices = new fltk::CheckButton(5,280,140,20, "Render Vertices");
								body_render_edges = new fltk::CheckButton(5, 300, 155, 20, "Render Edges");
								body_render_faces = new fltk::CheckButton(5, 320, 155, 20, "Render Faces");
								body_render_normals = new fltk::CheckButton(5, 340, 155, 20, "Render Face Normals");
								body_render_v_normals = new fltk::CheckButton(5, 360, 165, 20, "Render Vertex Normals");
								body_render_boundingbox = new fltk::CheckButton(5, 380, 155, 20, "Render Boundingbox");
							sg->end();
						currentTab->end();

						// tab two (Modify)
						currentTab = new fltk::Group(0,20,185,558, "Modify", true);
							tabPages->push_back(currentTab);

							sg = new fltk::ScrollGroup(1,1,183,550, NULL, true);
								modifierChoice = new fltk::Choice(5,10,151,20);
								tabMap->insert( std::make_pair( Modify, 1 ));
								for (unsigned si = 0; si < availableModifiers.size(); si++) {
									modifierChoice->add(availableModifiers.at(si));
								}
								//modifyerChoice->focus_index(selected_index);
								modifierChoice->callback(modifierchoice_cb,this);

								m_tbtnModifierPause = new fltk::ToggleButton(161, 10, 15, 20, "P");
								m_tbtnModifierPause->callback(modifierpause_cb, this);
								m_tbtnModifierPause->buttoncolor(fltk::color(0,200,0));

								modifier_browser = new fltk::Browser(5, 40, 170, 100);
								modifier_browser->callback(modifierbrowser_cb, this);

								executeModifiersButton = new fltk::Button(5, 145, 170, 20, "Reexecute");
								executeModifiersButton->callback(executeStackButton_cb, this);
							sg->end();
						currentTab->end();

						currentTab = new fltk::Group(0,20,185,558, "General", true);
							tabPages->push_back(currentTab);

							sg = new fltk::ScrollGroup(1,1,183,550, NULL, true);
								world_render_boundingbox = new fltk::CheckButton(5, 10, 150, 20, "Render Boundingbox");
								world_render_boundingbox->callback(callback_world_render,this);
								world_render_axes = new fltk::CheckButton(5, 30, 150, 20, "Render Axes");
								world_render_axes->callback(callback_world_render,this);
							sg->end();
						currentTab->end();

						// ending the tabs
						tabs->value(0);
					tabs->end();
				sg->end();

			end();

			// Create the Add Body Dialog
			add_body_dialog = new fltk::Window(400,100, "Add a new Body");
			add_body_dialog->begin();
				add_body_dialog_body_name = new fltk::Input(80,10,200,24,"Body Name:");
				add_body_dialog_file_name = new fltk::Input(80,40,310,24,"Mesh File:");
				add_body_dialog_choose_file_button = new fltk::Button(290,10,100, 24,"Choose File");
				add_body_dialog_choose_file_button->callback(callback_add_body_choose_file, this);
				add_body_dialog_recompute_normals = new fltk::CheckButton(10,70,140,24,"Recompute normals");
				add_body_dialog_cancel_button = new fltk::Button(180,70,100,24,"Cancel");
				add_body_dialog_cancel_button->callback(callback_add_body_cancel, this);
				add_body_dialog_create_body_button = new fltk::ReturnButton(290,70,100,24,"Create Body");
				add_body_dialog_create_body_button->callback(callback_add_body_create, this);
			add_body_dialog->end();

			body_add_button->callback(callback_add_button, this);
			body_remove_button->callback(callback_remove_button, this);

			body_trans_x->callback(callback_body_trans, this);
			body_trans_y->callback(callback_body_trans, this);
			body_trans_z->callback(callback_body_trans, this);
			body_trans_x->linesize(0.1);
			body_trans_y->linesize(0.1);
			body_trans_z->linesize(0.1);
			body_trans_x->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
			body_trans_y->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
			body_trans_z->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());

			body_rot_x->callback(callback_body_rot, this);
			body_rot_y->callback(callback_body_rot, this);
			body_rot_z->callback(callback_body_rot, this);
			body_rot_angle->callback(callback_body_rot, this);
			body_rot_x->linesize(0.1);
			body_rot_y->linesize(0.1);
			body_rot_z->linesize(0.1);
			body_rot_angle->linesize(5.0);
			body_rot_x->range(-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity());
			body_rot_y->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
			body_rot_z->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
			body_rot_angle->range(-360, 360);

			body_scaling->callback(callback_body_scaling, this);
			body_scaling->linesize(0.1);
			body_scaling->range(-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());

			body_render_vertices->callback(callback_body_render, this);
			body_render_edges->callback(callback_body_render, this);
			body_render_faces->callback(callback_body_render, this);
			body_render_normals->callback(callback_body_render, this);
			body_render_v_normals->callback(callback_body_render, this);
			body_render_boundingbox->callback(callback_body_render, this);

			/// Add default mesh
			if (m_cStartFile != NULL) {
				try
				{
					AddMesh( std::string(m_cStartFile), std::string(m_cStartFileName), false );
					mesh_choice->value(mesh_choice->children()-1);
				}
				catch (exception& e)
				{
					std::cout << "[Error] Could not load mesh \"" << m_cStartFile << "\": " << e.what() << std::endl;
				}
			}

			mesh_choice->when(fltk::WHEN_RELEASE);
			mesh_choice->callback(callback_mesh_choice, this);

			#pragma endregion

				modified = false;
				update_mesh_ui();

				update_label();

				resizable(display);

				// Temp. automatic start modifiers

//				MainControler::GetInstance()->AddModifier(
//						SnowSim::Modifiers::MOD_Triangulate);
//				MainControler::GetInstance()->AddModifier(
//						SnowSim::Modifiers::MOD_CreateEdgeGroups);
//				MainControler::GetInstance()->AddModifier(
//						SnowSim::Modifiers::MOD_CreateLaunchSites);
			}

		/// destructor
		window::~window() {
			close_cb();
			delete display;
			delete menu;
		}

		/// callback to reset the camera
		void window::save_camera_cb() {
			std::ofstream view_file;
			try
			{
				view_file.open("view.cfg");

				if (view_file) {
					cgv::math::linalg::fix_col_vec<float, 4> target
						= display->get_target();

					view_file << display->get_azimut() << " " << display->get_polar() << " "
						<< display->get_distance() << " " << target.x() << " " << target.y()
						<< " " << target.z() << " " << target.w() << std::endl;
					view_file.close();

					std::cout << "[Window] View saved" << std::endl;
				}
			}
			catch (std::exception& e)
			{
				std::cerr << "[Window] Error while saving view: " << e.what() << std::endl;
				if (view_file)
					view_file.close();
			}
			catch (...)
			{
				if (view_file)
					view_file.close();
			}
		}

		/// callback for hide
		void window::reexecute_modifiers_cb()
		{
			executeStackButton_cb();
		}

		/// callback for hide
		void window::reset_modifiers_cb()
		{
			MainControler::GetInstance()->ResetModifierStack();
		}

		/// callback to reset the camera
		void window::load_camera_cb() {
			std::ifstream view_file;
			try
			{
				view_file.open("view.cfg");

				if (view_file) {
					char line[4000];
					view_file.getline(&line[0],4000,'\n');
					if( !(view_file.eof()) && !(view_file.fail()) )
					{
						std::istringstream ins;
						ins.str(line);
						float a,p,d,x,y,z,w;

						ins >> a >> p >> d >> x >> y >> z >> w;

						cgv::math::linalg::fix_col_vec<float, 4> target(x, y, z, w);

						display->set_view(a, p, d, target);

						std::cout << "[Window] View loaded" << std::endl;
					}

					view_file.close();
				}
			}
			catch (std::exception& e)
			{
				std::cerr << "[Window] Error while loading view: " << e.what() << std::endl;
				if (view_file)
					view_file.close();
			}
			catch (...)
			{
				if (view_file)
					view_file.close();
			}
		}

		/// Adds a new mesh to the scene
		void window::AddMesh(std::string file_path, std::string display_name, bool recompute_normals)
		{
			// add the body name to the body choice widget and add the new body to the body pool
			Mesh* newMesh = new Mesh( file_path.c_str(), recompute_normals );
			newMesh->setName(display_name);

			MainControler::GetInstance()->GetMeshPool().addMesh( newMesh );

			mesh_choice->add( display_name.c_str() );
			this->filename = file_path.c_str();
		}

		bool window::addModifier(Modifiers modifier)
		{
			try
			{
				if (m_modifier_pending) {
					fltk::message("The last modifier added needs to be executed first.");
					return false;
				}

				m_modifier_pending = m_tbtnModifierPause->state();

				bool execute_modifier = !m_modifier_pending;

				return MainControler::GetInstance()->AddModifier(modifier, execute_modifier);
			}
			catch (exception &ex)
			{
				fltk::message(ex.what());
				return false;
			}
		}

		void window::OnModifierAdded(Modifier* modifier)
		{
			// Remember the modifier if is "paused"
			if (m_modifier_pending) {
				m_pending_modifier = modifier;
			}

			int modifyTabIndex = tabMap->find(Modify)->second;
			if (currentModifier) {
				currentModifier->deactivate();
				tabPages->at(modifyTabIndex)->remove(currentModifier);
			}

			tabPages->at(modifyTabIndex)->add(modifier);

			std::stringstream s;
			unsigned int modNumber = modifier_browser->size() + 1;
			s << modNumber << ". " << modifier->getName();

			modifier_browser->add( s.str().c_str() );
			modifier_browser->select(modifier_browser->size() - 1);

			currentModifier = modifier;

			update_label();
			display->redraw();
			modified = true;
		}

		/// callback for the modifierchooser
		void window::executeStackButton_cb()
		{
			if (m_modifier_pending) {
				m_modifier_pending = !m_pending_modifier->execute();
			} else {
				MainControler::GetInstance()->ExecuteModifierStack();
			}

			update_label();
			tabs->redraw();
			display->redraw();
		}

		// Handler for a cleared modifier stack event
		void window::OnModifierStackCleared( int previous_size )
		{
			if (currentModifier != NULL) {
				currentModifier->hide();
				currentModifier->deactivate();
				currentModifier = NULL;
			}

			modifier_browser->clear();

			//int modifyTabIndex = this->tabMap->find(Modify)->second;
			//tabPages->at(modifyTabIndex)->clear();

			update_label();
			display->redraw();
		}

		/// callback for the modifierchooser
		void window::modifierchoice_cb()
		{
			// Maybe we should rely on a map which maps Modifiers to ListElements, so we are not bound to the same order all the time
			addModifier((Modifiers) modifierChoice->focus_index());
		}

		void window::modifierpause_cb()
		{
			if (m_tbtnModifierPause->state()) {
				m_tbtnModifierPause->buttoncolor(fltk::color(255,140,0));
			} else {
				m_tbtnModifierPause->buttoncolor(fltk::color(0,200,0));
			}
		}

		/// callback for the modifierbrowser click
		void window::modifierbrowser_cb() {

			// Maybe we should rely on a map which maps Modifiers to ListElements,
			// so we are not bound to the same order all the time
			//executeModifier((Modifiers) modifierChoice->focus_index());

			int selectedItem = modifier_browser->value();
			int modifyTabIndex = this->tabMap->find(Modify)->second;

			MainControler* mc = MainControler::GetInstance();

			if (currentModifier != NULL) {
				currentModifier->hide();
				currentModifier->deactivate();
			}
			if (selectedItem > -1) {
				if ( tabPages->at(modifyTabIndex)->find(mc->GetModifier(selectedItem))
					== tabPages->at(modifyTabIndex)->children() )
				{
					tabPages->at(modifyTabIndex)->add(mc->GetModifier(selectedItem));
				}
				currentModifier = mc->GetModifier( selectedItem );
				currentModifier->show();
				currentModifier->activate();
			}
			tabs->redraw();
		}

		/// opens a file with given filename
		void window::open(const char* filename) {
			if (filename) {
				try
				{
					AddMesh( std::string(filename), std::string(filename), false );
					mesh_choice->value(mesh_choice->children() - 1);
					reset_camera_cb();
					update_label();
					update_mesh_ui();
				}
				catch (exception& e)
				{
					std::cout << "[Error] Could not load mesh \"" << m_cStartFile << "\": " << e.what() << std::endl;
				}
			}
		}

		/// callback for opening a file
		void window::open_cb() {
			const char* filename = fltk::file_chooser("Open new mesh", "*.obj", this->filename);
			if (filename) {
				open(filename);
			}
		}

		/// callback for opening a file
		void window::reopen_cb() {
			if (filename) {
				open(filename);
			}
		}

		/// callback for saving a file
		void window::save_finemesh_as_cb() {
			if (currentMesh != NULL) {

				try {
				std::string obj_file_name(this->filename);
				obj_file_name.append("_snow");

				const char* filename = fltk::file_chooser("Export snow in obj format", "*.obj", obj_file_name.c_str());
				if (filename) {
					std::cout << "[Sketch Output] exporting snow to " << filename << "...";

					std::ofstream output(filename, std::ios::out);
					world->GetSnowModel()->getObjRepresentation(output);

					output.close();

					std::cout << "ready" << std::endl;
				}
				}
				catch (...)
				{
					std::cerr << "[IO] An error occured and the exporting was stopped." << std::endl;
				}
			}
		}

		/// callback for saving a file
		void window::save_as_cb() {
			const char* filename = fltk::file_chooser("Save control mesh", "*.obj", this->filename);
			if (filename) {
				std::cout << "saving control mesh to " << filename << "...";
				if (!basemesh_drawable->mesh->save(filename)) {
					std::cout << "Could not save file: "<< filename << "!" << std::endl;
				} else {
					std::cout << "ready" << std::endl << std::endl;
					this->filename = filename;
					modified = false;
					update_label();
				}
			}
		}

		/// callback for saving a file
		void window::save_cb() {
			if (filename) {
				std::cout << "saving control mesh to " << filename << "...";
				if (!basemesh_drawable->mesh->save(filename)) {
					std::cout << "Could not save file: "<< filename << "!" << std::endl;
				} else {
					std::cout << "ready" << std::endl << std::endl;
					this->filename = filename;
					modified = false;
					update_label();
				}
			}
		}

		/// callback for saving a file
		void window::export_sketch_cb()
		{
			if (currentMesh != NULL) {
				const char* filename = fltk::file_chooser("Export scene as sketch drawing", "*.sk", this->filename);
				if (filename) {
					std::cout << "[Sketch Output] exporting mesh to " << filename << "...";

					std::ofstream output(filename, std::ios::out);

					std::string name(currentMesh->getName());
					Utilities::Strings::findandreplace(name, std::string(" "), std::string("_"));
					currentMesh->getSketchRepresentation(output, name);

					output.close();

					std::cout << "ready" << std::endl;
				}
			}
		}

		/// callback for saving a file
		void window::export_snow_sketch_cb()
		{
			if (currentMesh != NULL) {
				const char* filename = fltk::file_chooser("Export scene as sketch drawing", "*.sk", this->filename);
				if (filename) {
					std::cout << "[Sketch Output] exporting snow to " << filename << "...";

					std::ofstream output(filename, std::ios::out);
					world->GetSnowModel()->getSketchRepresentation(output);

					output.close();

					std::cout << "ready" << std::endl;
				}
			}
		}

		 /// callback for closing a file
		void window::close_cb()
		{
			int meshTabIndex = tabMap->find(Meshes)->second;

			tabPages->at(meshTabIndex)->remove(basemesh_drawable);
			delete basemesh_drawable;
			basemesh_drawable = NULL;

			tabPages->at(meshTabIndex)->remove(modifiedmesh_drawable);
			delete modifiedmesh_drawable;
			modifiedmesh_drawable = NULL;

			display->unregister_drawables();
			filename = NULL;
			modified = false;
			update_label();
		}

		/// callback for hide
		void window::hide_fine_mesh_cb() {
			if (modifiedmesh_drawable != NULL)
			{
				bool vis = modifiedmesh_drawable->get_visible();
				modifiedmesh_drawable->set_visible(!vis);
				update_label();
			}
		}

		/// callback for triangulate
		void window::triangulate_cb() {
			if (addModifier(Modifiers::MOD_Triangulate)) {
			//slider_cb();
			modified = true;
			update_label();
			}
		}

		/// callback for the display
		void window::display_cb() {
			//slider_cb();
			modified = true;
			update_label();
		}

		void window::save_heightmap()
		{
			try
			{
				throw Exceptions::NotImplementedException();

				const char* filename = fltk::file_chooser("Save heightmap", "*.tif",0);
				if (filename)
				{
					std::cout << "saving snow heightmap...";
					display->orthographic_screenshot(filename);
				}
			}
			catch (exception &e)
			{
				fltk::message(e.what());

				// Does not compile on linux:
				//MessageBoxA(NULL, e.what(), "An error occured", MB_OK);
			}
		}

		void window::callback_mesh_choice() {
			// update the mesh user interface
			update_mesh_ui();
			update_label();
		}

		void window::onWorldChanged(World* world) {
			redraw();
		}

		void window::update_mesh_ui() {
			int selected = mesh_choice->focus_index();
			if (0 == selected)
			{
				// "None" was selected as body
				currentMesh = 0;

				body_remove_button->deactivate();
				body_convex_checkbox->deactivate();
				body_translation_group->deactivate();
				body_trans_x->deactivate();
				body_trans_y->deactivate();
				body_trans_z->deactivate();
				body_rotation_group->deactivate();
				body_rot_x->deactivate();
				body_rot_y->deactivate();
				body_rot_z->deactivate();
				body_rot_angle->deactivate();
				body_scaling->deactivate();
				body_render_vertices->deactivate();
				body_render_edges->deactivate();
				body_render_faces->deactivate();
				body_render_normals->deactivate();
				body_render_v_normals->deactivate();
				body_render_boundingbox->deactivate();

			} else {
				// Fill the gui with the information from the selected body
				currentMesh = MainControler::GetInstance()->GetMeshPool().getMesh(selected-1);

				body_remove_button->activate();
				body_convex_checkbox->activate();
				body_convex_checkbox->value(currentMesh->convex());
				body_translation_group->activate();
				body_trans_x->activate();
				body_trans_x->value(currentMesh->position().x());
				body_trans_y->activate();
				body_trans_y->value(currentMesh->position().y());
				body_trans_z->activate();
				body_trans_z->value(currentMesh->position().z());
				body_rotation_group->activate();
				body_rot_x->activate();
				body_rot_x->value(currentMesh->rotationVector().x());
				body_rot_y->activate();
				body_rot_y->value(currentMesh->rotationVector().y());
				body_rot_z->activate();
				body_rot_z->value(currentMesh->rotationVector().z());
				body_rot_angle->activate();
				body_rot_angle->value(currentMesh->rotationAngle());
				body_scaling->activate();
				body_scaling->value(currentMesh->scaling());
				body_scaling->linesize(body_scaling->value() * 0.1f);
				body_render_vertices->activate();
				body_render_vertices->value(currentMesh->renderVertices());
				body_render_edges->activate();
				body_render_edges->value(currentMesh->renderEdges());
				body_render_faces->activate();
				body_render_faces->value(currentMesh->renderFaces());
				body_render_normals->activate();
				body_render_normals->value(currentMesh->renderNormals());
				body_render_v_normals->activate();
				body_render_v_normals->value(currentMesh->renderVertexNormals());
				body_render_boundingbox->activate();
				body_render_boundingbox->value(currentMesh->renderBoundingBox());

				// If no valid mesh is available offer to delete the body
				if (!currentMesh->meshLoaded())
				{
					if (fltk::ask("There is no valid mesh available for this body!\nDo you want to delete this body?"))
					{
						callback_remove_button();
					}
				}
			}
			// update the statistic fields and redraw the gui
			//updateStatistics();
			redraw();
		}

		/// callback for the remove_button
		void window::callback_remove_button()
		{
			// if not "None" selected, remove the selected body
			int selected = mesh_choice->focus_index();
			if (0 != selected)
			{
				MainControler::GetInstance()->GetMeshPool().removeMesh(selected-1);
				mesh_choice->remove(selected);
				mesh_choice->value(selected-1);
				mesh_choice->redraw();
				update_mesh_ui();
			}
			display->redraw();
		}

		/// callback for the add_button
		void window::callback_add_button()
		{
			// Initialize the body name to "Body N"
			std::stringstream sstream;
			std::string bodyName("Mesh ");
			sstream << (mesh_choice->size());
			bodyName += sstream.str();
			std::cout << bodyName;
			add_body_dialog_body_name->replace(0, add_body_dialog_body_name->size(), bodyName.c_str(), static_cast<int>(strlen(bodyName.c_str()) ) );

			// show the add body dialog
			add_body_dialog->show();
		}

		/// callback for the choose_file_button in the add_body_dialog
		void window::callback_add_body_choose_file()
		{
			// open a file chooser dialog and put the result into the file name input field
			const char* c = fltk::file_chooser("Open obj-Mesh", "*.obj", "");
			if (c!= NULL)
			{
				add_body_dialog_file_name->replace(0, add_body_dialog_file_name->size(), c, (int)strlen(c));
			}
		}

		/// callback for the cancel_button in the add_body_dialog
		/// Currently only hides the add_body_dialog again
		void window::callback_add_body_cancel()
		{
			add_body_dialog->hide();
		}

		/// callback for the create_button in the add_body_dialog
		void window::callback_add_body_create()
		{
			try
			{
				AddMesh( std::string(add_body_dialog_file_name->text()), std::string(add_body_dialog_body_name->text()),
						add_body_dialog_recompute_normals->state() );

				add_body_dialog->hide();

				mesh_choice->value(mesh_choice->children() - 1);
				update_mesh_ui();
			}
			catch (exception& e)
			{
				std::cout << "[Error] Could not load mesh: " << e.what() << std::endl;
			}
		}

		/// callback for body_trans_x/y/z input
		void window::callback_body_trans()
		{
			// change the position of the current body and search for contacts
			if (0 != currentMesh)
			{
				currentMesh->setPosition(point3f((float)body_trans_x->value()
												,(float)body_trans_y->value()
												,(float)body_trans_z->value()));

				display->redraw();
			}
		}

		/// callback for body_rot_x/y/z/angle input
		void window::callback_body_rot()
		{
			// change the rotation of the current body and search for contacts
			if (0 != currentMesh)
			{
				currentMesh->setRotationVector(point3f((float)body_rot_x->value()
												,(float)body_rot_y->value()
												,(float)body_rot_z->value()));
				currentMesh->setRotationAngle((float)body_rot_angle->value());

				display->redraw();
			}
		}

		/// callback for body_scaling input
		void window::callback_body_scaling()
		{
			// change the scaling of the current body and search for contacts
			if (0 != currentMesh)
			{
				currentMesh->setScaling((float)body_scaling->value());
				// scale by 10 percent with the buttons
				body_scaling->linesize((float)body_scaling->value() * 0.1f);
				display->redraw();
			}
		}

		/// callback for body_render_vertices/edges/faces input
		void window::callback_body_render()
		{
			// set whether vertices, edges and faces are rendered for the current body
			if (0 != currentMesh)
			{
				currentMesh->setRenderVertices(body_render_vertices->value());
				currentMesh->setRenderEdges(body_render_edges->value());
				currentMesh->setRenderFaces(body_render_faces->value());
				currentMesh->setRenderFaceNormals(body_render_normals->value());
				currentMesh->setRenderVertexNormals(body_render_v_normals->value());
				currentMesh->setRenderBoundingBox(body_render_boundingbox->value());
				display->redraw();
			}
		}

		void window::callback_world_render() {
			if (world) {
				world->setRenderBoundingBox(world_render_boundingbox->value());
				world->SetRenderAxes(world_render_axes->value());
				world->onChanged();
			}
		}

		/// callback for hide
		void window::hide_control_mesh_cb() {
			//bool vis = basemesh_drawable->get_visible();
			//basemesh_drawable->set_visible(!vis);
			//update_label();
		}

		/// callback for mark all
		void window::mark_all_cb() {
			if (currentMesh) {
				currentMesh->set_all_marks(true);
				//slider_cb();
				modified = true;
				update_label();
			}
		}

		/// callback for unmark all
		void window::unmark_all_cb() {
			if (currentMesh) {
				currentMesh->set_all_marks(false);
				//slider_cb();
				modified = true;
				update_label();
			}
		}


		/// update label of window
		void window::update_label() {
			std::stringstream s;
			s << "SnowSim";
			//if (currentMesh) {
			//	s << " - " << currentMesh->;
			//}
			if (modified) {
				s << "*";
			}
			copy_label(s.str().c_str());

			if (!world || world->GetBaseMeshs()->getNrMeshs() < 1) {
				unmark_all_item->deactivate();
				reset_camera_item->deactivate();
				save_item->deactivate();
				save_as_item->deactivate();
				close_item->deactivate();
				reopen_item->deactivate();
				controlmesh_visible_item->deactivate();
				finemesh_visible_item->deactivate();
				triangulate_item->deactivate();
				save_snowmesh_as_item->deactivate();
				modifier_browser->clear();
				modifier_browser->deactivate();
				modifierChoice->deactivate();
				executeModifiersButton->deactivate();
				mark_all_item->deactivate();
				unmark_all_item->deactivate();
				world_render_boundingbox->deactivate();
				world_render_axes->deactivate();
			} else {
				if (currentMesh) {
					unmark_all_item->activate();
					mark_all_item->activate();
				} else {
					unmark_all_item->deactivate();
					mark_all_item->deactivate();
				}
				reset_camera_item->activate();
				if (modified) {
					save_item->activate();
					reopen_item->activate();
				} else {
					save_item->deactivate();
					reopen_item->deactivate();
				}
				save_as_item->activate();
				close_item->activate();
				controlmesh_visible_item->activate();
				finemesh_visible_item->activate();
				triangulate_item->activate();
				save_snowmesh_as_item->activate();
				modifier_browser->activate();
				modifierChoice->activate();
				executeModifiersButton->activate();
				world_render_boundingbox->activate();
				world_render_boundingbox->value(world->renderBoundingBox());
				world_render_axes->activate();
				world_render_axes->value(world->GetRenderAxes());

				//if (basemesh_drawable->get_visible()) {
				//	controlmesh_visible_item->label("Hide &control mesh");
				//} else {
				//	controlmesh_visible_item->label("Show &control mesh");
				//}
				//if (modifiedmesh_drawable->get_visible()) {
				//	finemesh_visible_item->label("Hide &modified mesh");
				//} else {
				//	finemesh_visible_item->label("Show &modified mesh");
				//}
			}

			if (m_modifier_pending) {
				executeModifiersButton->label("Execute");
			} else {
				executeModifiersButton->label("Re-execute");
			}

			tabs->redraw();
		}

		/// callback for triangulate
		void window::flip_notmals_cb()
		{
			if (this->currentMesh != NULL)
			{
				std::cout << "[Mesh] flipping vertex and face normals" << std::endl;

				vertex* v = currentMesh->mesh()->vertex_source;

				while (v != NULL)
				{
					v->normal = -v->normal;
					v = v->next_vertex;
				}

				face* f = currentMesh->mesh()->face_source;

				while (f != NULL)
				{
					f->normal = -f->normal;
					f = f->next_face;
				}

				std::cout << "\t*Recalculating face slope angles" << std::endl;

				f = currentMesh->mesh()->face_source;

				while (f != NULL)
				{
					f->slope_angle = f->compute_slope();
					f = f->next_face;
				}
			}
		}
	}
}
