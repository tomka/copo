#pragma once

#include <polygonal_mesh.h>
#include "polygonal_mesh_obj_reader.h"
#include <map>
#include <Shading/Material.h>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

#include <Utilities/Path.h>

namespace SnowSim {
	namespace IO {

		class MeshReader
			: public SnowSim::polygonal_mesh_obj_reader
		{
		private:
			typedef SnowSim::polygonal_mesh_obj_reader BaseClass;


			/// the current material lib to use
			std::string m_matlib;

			/// the path in which the material lib resists
			std::string m_matlibPath;

			/// A map setting material names in relation to actual material classes
			std::map< std::string, SnowSim::Shading::Material* > m_materialRepository;

			/// A map that relates face indices to materials (and the access indices)
			std::map<int, SnowSim::Shading::Material > m_materialMap;

		public:
			MeshReader( const std::string &filename )
				: SnowSim::polygonal_mesh_obj_reader( filename )
			{
				std::string mtllib = filename;
				findandreplace(mtllib, std::string(".obj"), std::string(".mtl"));


				m_matlib = filename;

				// set path
				m_matlibPath = SnowSim::Utilities::Path::GetPathWithoutFileName(filename);
			}

			/// Gets the material map of the current file
			/// The faces have been asigned the appr. link to it
			std::map<int, SnowSim::Shading::Material> GetMaterialMap();

			/// Overridden load method - do some extra stuff.
			polygonal_mesh* load(bool force_new_normals = false);

			/// Parses the material library file
			void parse_matlib(std::string matlib);

			void assign_materials();

			void process_mtllib(int index, const std::string& lib);

			void process_usemtl(int index, const std::string& mtl);
		};
	}
}
