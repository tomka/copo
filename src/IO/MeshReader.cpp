#include "MeshReader.h"
#include <types.h>
#include <Utilities/Path.h>
//#include <dvutil/file.h>
#include <Utilities/Timer.h>

namespace SnowSim {
	namespace IO {

		/// Gets the material map of the current file
		/// The faces have been assigned the appr. link to it
		std::map<int, SnowSim::Shading::Material> MeshReader::GetMaterialMap()
		{
			return m_materialMap;
		}

		/// Overridden load method - do some extra stuff.
		polygonal_mesh* MeshReader::load(bool force_new_normals)
		{
			Utilities::Timer t;
			t.start();

			polygonal_mesh* mesh = BaseClass::load(force_new_normals);

			std::cout << "\tassigning face materials...";
			assign_materials();
			std::cout << "ready" << std::endl;

			t.stop();

			std::cout << "\t* Statistics:" << std::endl
				<< "\t\tLoading time: " << t.diffTimeInMSec() << "msec" << std::endl
				<< "\t\tFaces: " << mesh->number_faces << std::endl
				<< "\t\tEdges: " << mesh->number_edges << std::endl
				<< "\t\tHalfedges: " << mesh->number_halfedges << std::endl
				<< "\t\tVertices: " << mesh->number_vertices << std::endl;

			return mesh;
		}

		void MeshReader::assign_materials()
		{
			if ( m_materialMap.size() == 0 )
			{
				// add default material for all faces
				m_materialMap.insert( make_pair(0, SnowSim::Shading::Material()) );
			}

			std::map<int, SnowSim::Shading::Material>::iterator m_it
				= m_materialMap.begin();

			face* current_face = mesh->face_source;
			while ( m_it != m_materialMap.end() )
			{
				std::pair<int, SnowSim::Shading::Material> data = *m_it;
				++m_it; // get data of the next material

				int end;
				if ( m_it == m_materialMap.end() )
					end = mesh->number_faces;
				else
					end = (*m_it).first; // get the beginning of the next material

				for (int face = data.first; face < end; ++face)
				{
					current_face->material = data.first;

					current_face = current_face->next_face;
				}
			}

			assert(current_face == NULL);
		}

		/// Parses the material library file
		void MeshReader::parse_matlib(std::string matlib)
		{
			std::fstream mtlfile;
			std::string filepath = m_matlibPath;
			filepath.append("/");
			filepath.append(matlib);

			mtlfile.open(filepath.c_str());

			if( !mtlfile )
			{
				//std::cerr << std::endl << "Could not open material file: " << Dv::Util::Directory::pwd() << filepath.c_str() <<"!"<<std::endl;
				std::cerr << std::endl << "\t[Error] Could not open material file: \"" << filepath.c_str() <<"\"!"<<std::endl;
				return;
			}

			bool l_ignoreData = false;
			SnowSim::Shading::Material* l_pCurrentMaterial;

			char linebuffer[4000];
			while(true)
			{
				mtlfile.getline(&linebuffer[0],4000,'\n');
				if( mtlfile.eof() || mtlfile.fail() )
					break;

				if (l_ignoreData) {
					continue;
				}

				std::string::size_type index;
				std::string line(linebuffer);

				//search comment
				index = line.find("#");
				if(index != std::string::npos)
				{
					// ignore comments
					continue;
				}

				//search newmtl
				index = line.find("newmtl");
				if(index != std::string::npos)
				{
					line.erase(0,index+6);
					findandreplace(line, string(" "), string(""));
					findandreplace(line, string("\r"), string(""));

					if ( m_materialRepository.find(line) != m_materialRepository.end() )
					{
						std::cout << "[Mesh-Reader] Material \"" << line << "\" already exists. Ignoring it" << std::endl;
						l_ignoreData = true;
					} else {
						l_ignoreData = false;
						// init with default material
						l_pCurrentMaterial = new SnowSim::Shading::Material();

						m_materialRepository.insert( make_pair(line, l_pCurrentMaterial) );
					}

					continue;
				}
				//search Ns (shininess)
				index = line.find("Ns ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float ns = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> ns;

					l_pCurrentMaterial->shininess = ns;

					continue;
				}
				//search Ka (ambient color)
				index = line.find("Ka ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float r = 0.0f, g = 0.0f, b = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> r >> g >> b;

					l_pCurrentMaterial->ambient_color = point3f(r, g, b);

					continue;
				}
				//search Kd (diffuse color)
				index = line.find("Kd ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float r = 0.0f, g = 0.0f, b = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> r >> g >> b;

					l_pCurrentMaterial->diffuse_color = point3f(r, g, b);

					continue;
				}
				//search Ks (specular color)
				index = line.find("Ks ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float r = 0.0f, g = 0.0f, b = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> r >> g >> b;

					l_pCurrentMaterial->specular_color = point3f(r, g, b);

					continue;
				}
				//search d (alpha)
				index = line.find("d ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float a = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> a;

					l_pCurrentMaterial->alpha = a;

					continue;
				}
				//search Tr (alternative alpha)
				index = line.find("d ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					float a = 0.0f;
					std::istringstream ins;
					ins.str(line);
					ins >> a;

					l_pCurrentMaterial->alpha = a;

					continue;
				}
				//search illum (illumination model)
				index = line.find("d ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					int a = 0;
					std::istringstream ins;
					ins.str(line);
					ins >> a;

					l_pCurrentMaterial->hasSpecularHighlights = (a == 2);

					continue;
				}
			}
		}

		void MeshReader::process_mtllib(int index, const std::string& lib)
		{
			//std::cout << std::endl << "[OBJ-Reader] Using material library: " << lib << std::endl;
			m_matlib = lib;
			parse_matlib(m_matlib);
		}

		void MeshReader::process_usemtl(int index, const std::string& mtl)
		{
			SnowSim::Shading::Material* material;

			std::map<std::string, SnowSim::Shading::Material* >::iterator m_it;

			m_it = m_materialRepository.find(mtl);
			if ( m_it == m_materialRepository.end() )
			{
				std::cerr << "\t[Error] A material called \"" << mtl << "\" was not found. Using Default material." << std::endl;
				material = new SnowSim::Shading::Material();
			}
 			else
			{
				material = (*m_it).second;
			}

			m_materialMap.insert( make_pair(index, *material) );
		}
	}
}
