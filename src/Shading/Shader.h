#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#ifdef _WIN32
#include <Utilities/glext.h>
#else
#include <GL/glext.h>
#endif

#include <string>
#include <Utilities/OpenGL.h>
/*
 *	todo:
 *		- shrink class(only one createShader function)
 *		- use std::string for sources if possible!
 */

/*
 *	class for loading shaders
 */
namespace SnowSim {
	namespace Shading {

		class Shader {

			public:
				enum Shader_Type { GLSL, CG };

			protected:
				bool shaderLoaded;

				// indicates if the object is in the process of beeing destroyed
				// (e. g. the destructor got called)
				bool m_is_destroying;

				// the path of the vertex shader
				std::string m_vertex_program_path;

				// the path of the fragment shader
				std::string m_fragment_program_path;

				Shader();

			public:
				virtual ~Shader();

				virtual bool load_vertex_shader(const char* path) = 0;
				virtual bool load_fragment_shader(const char* path) = 0;
				virtual bool loadShaders(const char* vertexShaderName, const char* pixelShaderName) = 0;
				virtual void unloadShaders() { };
				virtual void begin() = 0;
				virtual void end() = 0;

				// Gets the path of the loaded (if any) vertex program
				const std::string& GetVertexProgramPath() const { return m_vertex_program_path; }
				// Gets the path of the loaded (if any) fragment program
				const std::string& GetFragmentProgramPath() const { return m_fragment_program_path; }

				// Give uniform parameter: one float
				virtual void GiveUniform1i(const char* name, int value) = 0;

				// Give uniform parameter: one float
				virtual void GiveUniform1f(const char* name, float value) = 0;

				// Give uniform parameter:  vector of 3 floats
				virtual void GiveUniform3fv(const char* name, const float* values) = 0;

				// Give uniform parameter:  vector of 3 floats
				virtual void GiveUniform3f(const char* name, float x, float y, float z) = 0;

				// Give uniform parameter:  vector of 4 floats
				virtual void GiveUniform4fv(const char* name, const float* values) = 0;

				// Give uniform parameter:  vector of 4 floats
				virtual void GiveUniform4f(const char* name, float x, float y, float z, float w) = 0;

				// Returns if the hardware is supporting the use of shaders
				virtual bool IsSupportByHardware() = 0;

			protected:
				char* readShader(const char* fileName);

				virtual bool createVertexShader(const char* vertexShaderName) = 0;
				virtual bool createPixelShader(const char* pixelShaderName) = 0;
				virtual void linkProgram() = 0;
		};
	}
}
#endif
