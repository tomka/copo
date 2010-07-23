#ifndef GLSL_SHADER_H
#define GLSL_SHADER_H

#include "Shader.h"
#include <string>
#include <Utilities/OpenGL.h>

#ifdef _WIN32
#include <Utilities/glext.h>
#else
#include <GL/glext.h>
#endif

#include <GL/glew.h>
/*
 *	todo:
 *		- shrink class(only one createShader function)
 *		- use std::string for sources if possible!
 */

/*
 *	class for loading glsl shaders
 */
namespace SnowSim {
	namespace Shading {

		class GLSL_Shader : public Shader
		{
			private:
				unsigned int program;
				unsigned int vertexShader;
				unsigned int pixelShader;

				bool shaderLoaded;

				// indicates if the object is in the process of beeing destroyed
				// (e. g. the destructor got called)
				bool m_is_destroying;

				// the path of the vertex shader
				std::string m_vertex_program_path;

				// the path of the fragment shader
				std::string m_fragment_program_path;

				// the type of the loaded shader
				Shader_Type m_shader_type;

			public:
				GLSL_Shader();
				~GLSL_Shader();

				bool load_vertex_shader(const char* path);
				bool load_fragment_shader(const char* path);
				bool loadShaders(const char* vertexShaderName, const char* pixelShaderName);
				void unloadShaders();
				void begin();
				void end();

				// Gets the path of the loaded (if any) vertex program
				const std::string& GetVertexProgramPath() const { return m_vertex_program_path; }
				// Gets the path of the loaded (if any) fragment program
				const std::string& GetFragmentProgramPath() const { return m_fragment_program_path; }

				Shader_Type GetShaderType() const { return m_shader_type; }

				// Give uniform parameter: one float
				void GiveUniform1i(const char* name, int value);

				// Give uniform parameter: one float
				void GiveUniform1f(const char* name, float value);

				// Give uniform parameter:  vector of 3 floats
				void GiveUniform3fv(const char* name, const float* values);

				// Give uniform parameter:  vector of 3 floats
				void GiveUniform3f(const char* name, float x, float y, float z);

				// Give uniform parameter:  vector of 4 floats
				void GiveUniform4fv(const char* name, const float* values);

				// Give uniform parameter:  vector of 4 floats
				void GiveUniform4f(const char* name, float x, float y, float z, float w);

				// Returns if the hardware is supporting the use of shaders
				bool IsSupportByHardware();

			private:
				bool createVertexShader(const char* vertexShaderName);
				bool createPixelShader(const char* pixelShaderName);
				void linkProgram();
		};
	}
}
#endif
