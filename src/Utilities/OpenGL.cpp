#include "OpenGL.h"

#include <iostream>
#include <cstring>

namespace SnowSim {
	namespace Utilities {
		namespace OpenGL {

			double Shaders::t0 = 0.0;
			int Shaders::frames = 0;
			char Shaders::titlestring[200];

			GLuint Shaders::permTextureID;
			GLuint Shaders::gradTextureID;
			GLuint Shaders::sphereList;
			GLboolean Shaders::updateTime = GL_TRUE;
			GLboolean Shaders::animateObject = GL_TRUE;

			GLhandleARB Shaders::programObj;
			GLhandleARB Shaders::vertexShader;
			GLhandleARB Shaders::fragmentShader;
			GLint Shaders::location_permTexture = -1;
			GLint Shaders::location_gradTexture = -1;
			GLint Shaders::location_time = -1;

			const char * Shaders::vertexShaderStrings[1];
			const char * Shaders::fragmentShaderStrings[1];
			GLint Shaders::vertexCompiled;
			GLint Shaders::fragmentCompiled;
			GLint Shaders::shadersLinked;
			char Shaders::str[4096]; // For error messages from the GLSL compiler and linker

			int Shaders::perm[256] = {151,160,137,91,90,15,
				  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180};

			/* These are Ken Perlin's proposed gradients for 3D noise. I kept them for
			   better consistency with the reference implementation, but there is really
			   no need to pad this to 16 gradients for this particular implementation.
			   If only the "proper" first 12 gradients are used, they can be extracted
			   from the grad4[][] array: grad3[i][j] == grad4[i*2][j], 0<=i<=11, j=0,1,2
			*/
			int Shaders::grad3[16][3] = {{0,1,1},{0,1,-1},{0,-1,1},{0,-1,-1},
								   {1,0,1},{1,0,-1},{-1,0,1},{-1,0,-1},
								   {1,1,0},{1,-1,0},{-1,1,0},{-1,-1,0}, // 12 cube edges
								   {1,0,-1},{-1,0,-1},{0,-1,1},{0,1,1}}; // 4 more to make 16

			int Shaders::grad4[32][4] = {{0,1,1,1}, {0,1,1,-1}, {0,1,-1,1}, {0,1,-1,-1}, // 32 tesseract edges
								   {0,-1,1,1}, {0,-1,1,-1}, {0,-1,-1,1}, {0,-1,-1,-1},
								   {1,0,1,1}, {1,0,1,-1}, {1,0,-1,1}, {1,0,-1,-1},
								   {-1,0,1,1}, {-1,0,1,-1}, {-1,0,-1,1}, {-1,0,-1,-1},
								   {1,1,0,1}, {1,1,0,-1}, {1,-1,0,1}, {1,-1,0,-1},
								   {-1,1,0,1}, {-1,1,0,-1}, {-1,-1,0,1}, {-1,-1,0,-1},
								   {1,1,1,0}, {1,1,-1,0}, {1,-1,1,0}, {1,-1,-1,0},
								   {-1,1,1,0}, {-1,1,-1,0}, {-1,-1,1,0}, {-1,-1,-1,0}};


			unsigned char* Shaders::readShaderFile(const char *filename) {
				FILE *file = fopen(filename, "r");
				if(file == NULL)
				{
					printError("ERROR", "Cannot open shader file!");
					  return 0;
				}

				// //(NOT POSIX!)
				// int bytesinfile = filelength(fileno(file));

				fseek(file, 0, SEEK_END); // seek to end of file
				int bytesinfile = ftell(file); // get current file pointer
				fseek(file, 0, SEEK_SET); // seek back to beginning of file
				// proceed with allocating memory and reading the file


				unsigned char *buffer = (unsigned char*)malloc(bytesinfile+1);
				int bytesread = fread( buffer, 1, bytesinfile, file);
				buffer[bytesread] = 0; // Terminate the string with 0
				fclose(file);

				return buffer;
			}

			 void Shaders::createShaders() {
				  // Create the vertex shader.
				vertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);

				unsigned char *vertexShaderAssembly = readShaderFile("GLSLnoisetest4.vert");
				vertexShaderStrings[0] = (char*)vertexShaderAssembly;
				glShaderSourceARB( vertexShader, 1, vertexShaderStrings, NULL );
				glCompileShaderARB( vertexShader);
				free((void *)vertexShaderAssembly);

				glGetObjectParameterivARB( vertexShader, GL_OBJECT_COMPILE_STATUS_ARB,
										   &vertexCompiled );
				if(vertexCompiled  == GL_FALSE)
				{
					glGetInfoLogARB(vertexShader, sizeof(str), NULL, str);
					  printError("Vertex shader compile error", str);
				}

				// Create the fragment shader.
				fragmentShader = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

				unsigned char *fragmentShaderAssembly = readShaderFile( "GLSLnoisetest4.frag" );
				fragmentShaderStrings[0] = (char*)fragmentShaderAssembly;
				glShaderSourceARB( fragmentShader, 1, fragmentShaderStrings, NULL );
				glCompileShaderARB( fragmentShader );
				free((void *)fragmentShaderAssembly);

				glGetObjectParameterivARB( fragmentShader, GL_OBJECT_COMPILE_STATUS_ARB,
										   &fragmentCompiled );
				if(fragmentCompiled == GL_FALSE)
				{
					  glGetInfoLogARB( fragmentShader, sizeof(str), NULL, str );
					  printError("Fragment shader compile error", str);
				  }

				// Create a program object and attach the two compiled shaders.
				programObj = glCreateProgramObjectARB();
				glAttachObjectARB( programObj, vertexShader );
				glAttachObjectARB( programObj, fragmentShader );

				// Link the program object and print out the info log.
				glLinkProgramARB( programObj );
				glGetObjectParameterivARB( programObj, GL_OBJECT_LINK_STATUS_ARB, &shadersLinked );

				if( Shaders::shadersLinked == GL_FALSE )
				{
					glGetInfoLogARB( programObj, sizeof(str), NULL, str );
					printError("Program object linking error", str);
				}
				// Locate the uniform shader variables so we can set them later:
			  // a texture ID ("permTexture") and a float ("time").
				location_permTexture = glGetUniformLocationARB( programObj, "permTexture" );
				if(location_permTexture == -1)
				printError("Binding error","Failed to locate uniform variable 'permTexture'.");
				location_gradTexture = glGetUniformLocationARB( programObj, "gradTexture" );
				if(location_gradTexture == -1)
				printError("Binding error","Failed to locate uniform variable 'gradTexture'.");
				location_time = glGetUniformLocationARB( programObj, "time" );
				if(location_time == -1)
				printError("Binding error", "Failed to locate uniform variable 'time'.");
			}


			//PFNGLACTIVETEXTUREPROC glActiveTexture;    //Declare your function pointer in a .cpp file

			bool extInit() {

				GLenum err=glewInit();

				 if(err!=GLEW_OK)
				 {
					std::cerr << "Could not initialize GLEW" << std::endl;
					std::cerr << "Reported Error: " << glewGetErrorString (err) << std::endl;
					return false;
				 }

				 std::cout << "[Status] Using GLEW " << glewGetString(GLEW_VERSION) << std::endl;

				 std::string missing_exts;

				if ( !glewIsSupported("GL_ARB_shading_language_100") )
					missing_exts.append("GL_ARB_shading_language_100 ");

				if ( !glewIsSupported("GL_ARB_shader_objects") )
					missing_exts.append("GL_ARB_shader_objects ");

				if ( !glewIsSupported("GL_ARB_vertex_shader") )
					missing_exts.append("GL_ARB_vertex_shader ");

				if ( !glewIsSupported("GL_ARB_fragment_shader") )
					missing_exts.append("GL_ARB_fragment_shader ");

				if ( !glewIsSupported("GL_ARB_fragment_shader") )
					missing_exts.append("GL_ARB_fragment_shader ");

				if ( glActiveTexture == NULL )
					missing_exts.append("glActiveTexture ");

				if ( glCreateProgramObjectARB == NULL )
					missing_exts.append("glCreateProgramObjectARB ");

				if ( glDeleteObjectARB == NULL )
					missing_exts.append("glDeleteObjectARB ");

				if ( glUseProgramObjectARB == NULL )
					missing_exts.append("glUseProgramObjectARB ");

				if ( glCreateShaderObjectARB == NULL )
					missing_exts.append("glCreateShaderObjectARB ");

				if ( glShaderSourceARB == NULL )
					missing_exts.append("glShaderSourceARB ");

				if ( glCompileShaderARB == NULL )
					missing_exts.append("glCompileShaderARB ");

				if ( glGetObjectParameterivARB == NULL )
					missing_exts.append("glGetObjectParameterivARB ");

				if ( glAttachObjectARB == NULL )
					missing_exts.append("glAttachObjectARB ");

				if ( glGetInfoLogARB == NULL )
					missing_exts.append("glGetInfoLogARB ");

				if ( glLinkProgramARB == NULL )
					missing_exts.append("glLinkProgramARB ");

				if ( glGetUniformLocationARB == NULL )
					missing_exts.append("glGetUniformLocationARB ");

				if ( glUniform4fARB == NULL )
					missing_exts.append("glUniform4fARB ");

				if ( glUniform1fARB == NULL )
					missing_exts.append("glUniform1fARB ");

				if ( glUniform1iARB == NULL )
					missing_exts.append("glUniform1iARB ");

				if ( glTexImage3D == NULL )
					missing_exts.append("glTexImage3D");

				if ( glCreateProgram == NULL )
					missing_exts.append("glCreateProgram");

				if (missing_exts.length() > 0 )
				{
					std::string msg("[Error] Could not load the following needed extensions:\n");
					msg.append(missing_exts);

					printError("Error", msg.c_str());

					return false;
				}

				return true;
			}
		}
	}
}
