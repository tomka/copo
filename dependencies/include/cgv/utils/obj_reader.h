#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <string>

namespace cgv{
	namespace utils{

		/**
		* OBJ-File Format Parser. This class can be used to load geometry from an obj-file into
		* an own data structure. Just derive from this class and implement the process_??? Methods.
		* One example using this parser is the polygonal_mesh_obj_reader in cgv/scene/geometry.
		*
		* Usage:
		*
		* class MyReader :public cgv::utils::obj_reader
		* {
		* public:
		*	MyReader(const std::string& filename):cgv::utils::obj_reader(filename){}
		*	void process_comment(int index,const std::string& comment)
		*	{
		*		std::cout << comment << std::endl;
		*	}
		*	...
		* };
		*
		* MyReader r("test.obj");
		* if(r.open())
		* {
		*	r.read();
		* }
		*/
		class obj_reader
		{


		protected:

			std::string filename;
			std::fstream file;
			bool is_opened;
			unsigned int cc,vc,tc,nc,fc, mtllibc;

			template<class T>
			void findandreplace( T& source, const T& find, const T& replace )
			{
				size_t j;
				for (;(j = source.find( find )) != T::npos;)
				{
					source.replace( j, find.length(), replace );
				}
			}


		public:
			///initialize parser with filename
			obj_reader(const std::string& filename)
			{
				this->filename = filename;
				is_opened=false;
			}
			///destructor
			virtual ~obj_reader()
			{
				if(is_opened)
					file.close();

			}

			///open file, return true if successful
			bool open()
			{
				file.open(filename.c_str());
				if(file)
				{
					is_opened=true;
					cc=0;
					vc=0;
					tc=0;
					nc=0;
					fc=0;
					mtllibc = 0;
					return true;
				}
				std::cerr << std::endl << "[Error] Could not open file: "<<filename<<"!"<<std::endl;
				throw std::invalid_argument(std::string("Could not open file"));

				return false;

			}
			///close file
			void close()
			{
				file.close();
			}
			///read data from file
			void read()
			{
				char line[4000];
				if(is_opened)
				{
					while(true)
					{
						file.getline(&line[0],4000,'\n');
						if(file.eof()||file.fail())
							break;
						parse_line(line);
					}
				}
			}

			void parse_line(std::string line)
			{
				std::string::size_type index;
				//search comment
				index = line.find("#");
				if(index != std::string::npos)
				{
					line.erase(0,index+1);
					cc++;
					process_comment(cc,line);
					return;
				}

				//remove linux line endings
				findandreplace(line, string("\r"), string(""));

				index = line.find("$");
				if(index != std::string::npos)
				{

					return;
				}

				index = line.find("mtllib");
				if(index != std::string::npos)
				{
					line.erase(0,index+6);
					findandreplace(line, string(" "), string(""));
					++mtllibc;
					process_mtllib(index, line);

					return;
				}

				index = line.find("usemtl");
				if(index != std::string::npos)
				{
					line.erase(0,index+6);
					findandreplace(line, string(" "), string(""));
					process_usemtl(fc, line);

					return;
				}


				index = line.find("bmat");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("step");
				if(index != std::string::npos)
				{
					return;
				}

				index = line.find("curv2");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("curv");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("surf");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("parm");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("trim");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("hole");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("scrv");
				if(index != std::string::npos)
				{
					return;
				}

				index = line.find("end");
				if(index != std::string::npos)
				{
					return;
				}

				index = line.find("con");
				if(index != std::string::npos)
				{
					return;
				}

				index = line.find("deg");
				if(index != std::string::npos)
				{
					return;
				}
				index = line.find("sp");
				if(index != std::string::npos)
				{
					return;
				}

				index = line.find("vp");
				if(index != std::string::npos)
				{
					return;
				}

				//search texcoord
				index = line.find("vt");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					tc++;
					float u,v,w = 0;
					std::istringstream ins;
					ins.str(line);
					ins >> u >>v >> w;
					if(ins.fail())
						w=0;

					process_texcoord(tc,u,v,w);
					return;
				}
				//search normal
				index = line.find("vn ");
				if(index != std::string::npos)
				{
					line.erase(0,index+2);
					nc++;
					float x,y,z = 0;
					std::istringstream ins;
					ins.str(line);
					ins >> x >> y >> z;

					process_normal(nc,x,y,z);
					return;
				}
				index = line.find("mg ");
				if(index != std::string::npos)
				{

					return;
				}
				index = line.find("g ");
				if(index != std::string::npos)
				{
					//group
					return;
				}


				index = line.find("o ");
				if(index != std::string::npos)
				{
					//object name
					return;
				}

				index = line.find("s ");
				if(index != std::string::npos)
				{
					//smoothing group
					return;
				}
				index = line.find("p ");
				if(index != std::string::npos)
				{
					//point
					return;
				}
				index = line.find("l ");
				if(index != std::string::npos)
				{
					//line
					return;
				}
				//search vertex
				index = line.find("v ");
				if(index != std::string::npos)
				{
					line.erase(0,index+1);
					vc++;
					float x,y,z = 0;

					std::istringstream ins;
					ins.str(line);
					ins >> x >> y >> z;

					process_vertex(vc,x,y,z);
					return;
				}
				//search face
				index = line.find("f ");
				if(index != std::string::npos)
				{
					line.erase(0,index+1);
					fc++;
					std::vector<int> vindices;
					std::vector<int> tindices;
					std::vector<int> nindices;
					int vi,ti,ni;
					unsigned char ch;

					std::string::size_type index;
					while(true)
					{
						index = line.find(" /");
						if(index != std::string::npos)
							line.replace(index,2,"/");
						else
							break;
					}
					while(true)
					{
						index = line.find("/ ");
						if(index != std::string::npos)
							line.replace(index,2,"/");
						else
							break;
					}
					std::stringstream ins;
					ins.str(line);
					do{

						ins >> vi;
						if(ins.fail())
							break;
						vindices.push_back(vi);
						ch=ins.get();
						if(ch=='/')
						{
							ch=ins.get();
							if(ch == '/')
							{
								ins >> ni;
								nindices.push_back(ni);

							}else
							{
								ins.unget();
								ins >> ti;
								tindices.push_back(ti);
								ch=ins.get();
								if(ch == '/')
								{
									ins >> ni;
									nindices.push_back(ni);
								}

							}
						}

					}
					while(true);

					int *varray=NULL,*tarray=NULL,*narray=NULL;

					if(!vindices.empty())
						varray = &(vindices[0]);


					if(!tindices.empty())
						tarray = &(tindices[0]);


					if(!nindices.empty())
						narray = &(nindices[0]);


					process_face(fc,(int)vindices.size(),varray,tarray,narray);

					return;
				}
			}

			///overide this function to process a comment
			virtual void process_comment(int index,const std::string& comment)
			{

			}
			///overide this function to process a material lib declaration
			///the index is the face index from which on the material should be valid
			virtual void process_mtllib(int index,const std::string& lib)
			{

			}
			///overide this function to process a comment
			virtual void process_usemtl(int index,const std::string& mtl)
			{

			}
			///overide this function to process a vertex
			virtual void process_vertex(int index, float x, float y, float z)
			{

			}
			///overide this function to process a texcoord
			virtual void process_texcoord(int index, float u, float v, float w=0)
			{

			}
			///overide this function to process a normal
			virtual void process_normal(int index, float x, float y, float z)
			{

			}
			///overide this function to process a face
			virtual void process_face(int index, int vcount, int *vertices, int *texcoords=NULL, int *normals=NULL)
			{

			}
		};
	}
}
