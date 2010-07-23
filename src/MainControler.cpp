#include "MainControler.h"
#include <Exceptions/NotInitializedException.h>
#include <Exceptions/MeshPoolException.h>
#include <fltk/ask.h>
#include <string>

namespace SnowSim {

	MainControler* MainControler::m_instance = NULL;

	/// Constructor
	MainControler::MainControler()
	{
		m_World = new SnowSim::Model::World(&m_MeshPool);
	}

	MainControler* MainControler::GetInstance()
	{
		if ( m_instance == NULL )
		{
			m_instance = new MainControler();
		}

		return m_instance;
	}

	/// Adds a modifier to the stack and executes it
	bool MainControler::AddModifier(Modifier_Constant modifier)
	{
		return AddModifier(modifier, true);
	}

	/// Adds a modifier to the stack and optionally executes it
	bool MainControler::AddModifier(Modifier_Constant modifier, bool execute)
	{
		SnowSim::Modifiers::Modifier * newModifier = NULL;

		if (modifier == Modifiers::MOD_Triangulate) {
			newModifier = new SnowSim::Modifiers::ModifierTriangulate(m_World);
		} else if (modifier == Modifiers::MOD_SubDivide) {
			newModifier = new SnowSim::Modifiers::ModifierSubDivide(m_World);
		} else if (modifier == Modifiers::MOD_CreateEdgeGroups) {
			newModifier = new SnowSim::Modifiers::ModifierCreateEdgeGroups(m_World);
		} else if (modifier == Modifiers::MOD_CreateLaunchSites) {
			newModifier = new SnowSim::Modifiers::ModifierCreateLaunchSites(m_World);
		} else if (modifier == Modifiers::MOD_AccumulateSnow) {
			newModifier = new SnowSim::Modifiers::ModifierAccumulateSnow(m_World);
		} else if (modifier == Modifiers::MOD_StabilizeSnow) {
			newModifier = new SnowSim::Modifiers::ModifierSnowStability(m_World);
		} else if (modifier == Modifiers::MOD_RegularSnow) {
			newModifier = new SnowSim::Modifiers::ModifierRegularStorm(m_World);
		} else if (modifier == Modifiers::MOD_ShadeSnow) {
			newModifier = new SnowSim::Modifiers::ModifierShadeSnowModel(m_World);
		} else if (modifier == Modifiers::MOD_ExtraBorderPoints) {
			newModifier = new SnowSim::Modifiers::ModifierAddAdditionalBorderPoints(m_World);
		} else
			return false;

		return AddModifier(newModifier, execute);
	}


	/// Adds a modifier to the stack and executes it
	bool MainControler::AddModifier(Modifier* modifier)
	{
		return AddModifier(modifier, true);
	}

	/// Adds a modifier to the stack and optionally executes it
	bool MainControler::AddModifier(Modifier* modifier, bool execute)
	{
		//return m_window->addModifier(modifier);

		if ( !(m_World->GetBaseMeshs()->getNrMeshs() > 0) ) {
			throw Exceptions::MeshPoolException("Please load first at least one mesh");
		}

		try
		{
			if (execute) {
				modifier->execute();
			}

			modifiers.push_back(modifier);

			ModifierAdded(modifier);

			std::cout << "[Modifiers] Added Modifier \"" << modifier->getName() << "\" to the stack" << std::endl;

			return true;

		} catch (SnowSim::Modifiers::Exceptions::PreConditionsViolatedException &ex) {
			std::string msg("Precondition violated: ");
			msg.append(ex.what());

			fltk::message(msg.c_str());

		} catch (std::exception &ex) {
			std::string msg("An error occurred: ");
			msg.append(ex.what());

			fltk::message(msg.c_str());
		}

		return true;
	}

	void MainControler::ExecuteModifierStack()
	{
		std::vector<SnowSim::Modifiers::Modifier * >::iterator it;

		MeshPool* pool = m_World->GetBaseMeshs();

		for (unsigned int i=0; i < pool->getNrMeshs(); ++i) {
			pool->getMesh(i)->resetModifiedMesh();
		}

		for (it = modifiers.begin(); it != modifiers.end(); ++it) {
			try {
				SnowSim::Modifiers::Modifier * mod = *it;
				mod->execute();
			} catch (SnowSim::Modifiers::Exceptions::PreConditionsViolatedException &ex) {
				std::string msg("Precondition violated: ");
				msg.append(ex.what());

				fltk::message(msg.c_str());

				break;
			} catch (std::exception &ex) {
				std::string msg("An error occurred on the modifier stack: ");
				msg.append(ex.what());

				fltk::message(msg.c_str());

				break;
			}
		}
	}

	/// Resets the modifier stack
	void MainControler::ResetModifierStack()
	{
		MeshPool* pool = m_World->GetBaseMeshs();

		for (unsigned int i=0; i < pool->getNrMeshs(); ++i) {
			pool->getMesh(i)->resetModifiedMesh();
		}

		int prev_size = modifiers.size();
		modifiers.clear();

		ModifierStackCleared(prev_size);
	}

	MainControler::Modifier* MainControler::GetModifier(int index)
	{
		return modifiers.at(index);
	}

	/// Gets the world
	SnowSim::Model::World* MainControler::GetWorld()
	{
		return m_World;
	}

	MeshPool& MainControler::GetMeshPool()
	{
		return m_MeshPool;
	}

}
