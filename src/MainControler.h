#pragma once

#include <Modifiers/Modifiers.h>
#include <Model/World.h>
#include <MeshPool.h>
#include <boost/signal.hpp>

namespace SnowSim {
	class MainControler
	{
	private:

		static MainControler* m_instance;

		/// The mesh pool, a container for all the meshes in the world
		MeshPool m_MeshPool;

		/// The world, responsible for all snow
		SnowSim::Model::World * m_World;

		/// The modifier stack
		std::vector<SnowSim::Modifiers::Modifier * > modifiers;

		/// Constructor
		MainControler();

	public:

		typedef SnowSim::Modifiers::Modifier	Modifier;
		typedef SnowSim::Modifiers::Modifiers	Modifier_Constant;

		boost::signal<void (Modifier*)> ModifierAdded;

		boost::signal<void (int)> ModifierStackCleared;

		/// Gets a singleton instance.
		static MainControler* GetInstance();

		/// Adds a modifier to the stack and executes it
		bool AddModifier(Modifier* modifier);

		/// Adds a modifier to the stack and optionally executes it
		bool AddModifier(Modifier* modifier, bool execute);

		/// Adds a modifier to the stack and executes it
		bool AddModifier(Modifier_Constant modifier);

		/// Adds a modifier to the stack and optionally executes it
		bool AddModifier(Modifier_Constant modifier, bool execute);

		/// Executes the modifier stack.
		void ExecuteModifierStack();

		/// Resets the modifier stack
		void ResetModifierStack();

		/// Gets the modifier in the stack with the specified index
		Modifier* GetModifier(int index);

		/// Gets the world
		SnowSim::Model::World* GetWorld();

		/// Gets the mesh pool
		MeshPool& GetMeshPool();
	};
}
