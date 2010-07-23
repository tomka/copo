#include "GLSL_Shader.h"
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <Exceptions/NotSupportedException.h>

namespace SnowSim {
	namespace Shading {

		GLSL_Shader::GLSL_Shader()
			:	program(0),
				vertexShader(0),
				pixelShader(0),
				shaderLoaded(false),
				m_is_destroying(false)
		{}

		GLSL_Shader::~GLSL_Shader()
		{
			m_is_destroying = true;
			unloadShaders();
		}

		// Returns if the hardware is supporting the use of shaders
		bool GLSL_Shader::IsSupportByHardware()
		{
			bool is_supported = true;

			is_supported &= glCreateProgram != NULL;
			is_supported &= glLinkProgram != NULL;
			is_supported &= glDeleteShader != NULL;
			is_supported &= glDeleteProgram != NULL;
			is_supported &= glShaderSource != NULL;
			is_supported &= glCompileShader != NULL;
			is_supported &= glGetShaderiv != NULL;
			is_supported &= glGetShaderInfoLog != NULL;
			is_supported &= glAttachShader != NULL;

			return is_supported;
		}

		bool GLSL_Shader::load_vertex_shader(const char* path)
		{
			if (glCreateProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glCreateProgram is not support by your graphics card!");
			if (glLinkProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glLinkProgram is not support by your graphics card!");

			// delete previously loaded shaders
			unloadShaders();

			// create program
			program = glCreateProgram();

			// create vertex shader
			if(!createVertexShader(path))
				return false;

			glLinkProgram(program);
			shaderLoaded = true;

			return shaderLoaded;

		}

		// Give uniform parameter: one float
		void GLSL_Shader::GiveUniform1i(const char* name, int value)
		{
			int location = glGetUniformLocation(program, name);
			glUniform1i(location, value);
		}

		// Give uniform parameter: one float
		void GLSL_Shader::GiveUniform1f(const char* name, float value)
		{
			int location = glGetUniformLocation(program, name);
			glUniform1f(location, value);
		}

		// Give uniform parameter:  vector of 3 floats
		void GLSL_Shader::GiveUniform3fv(const char* name, const float* values)
		{
			int location = glGetUniformLocation(program, name);
			glUniform3fv(location, 1, values);
		}

		// Give uniform parameter:  vector of 4 floats
		void GLSL_Shader::GiveUniform3f(const char* name, float x, float y, float z)
		{
			int location = glGetUniformLocation(program, name);
			glUniform3f(location, x, y, z);
		}

		// Give uniform parameter:  vector of 4 floats
		void GLSL_Shader::GiveUniform4fv(const char* name, const float* values)
		{
			int location = glGetUniformLocation(program, name);
			glUniform4fv(location, 1, values);
		}

		// Give uniform parameter:  vector of 4 floats
		void GLSL_Shader::GiveUniform4f(const char* name, float x, float y, float z, float w)
		{
			int location = glGetUniformLocation(program, name);
			glUniform4f(location, x, y, z, w);
		}


		bool GLSL_Shader::load_fragment_shader(const char* path)
		{
			if (glCreateProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glCreateProgram is not support by your graphics card!");
			if (glLinkProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glLinkProgram is not support by your graphics card!");

			// delete previously loaded shaders
			unloadShaders();

			// create program
			program = glCreateProgram();

			// create pixel shader
			if(!createPixelShader(path))
				return false;

			glLinkProgram(program);
			shaderLoaded = true;

			return shaderLoaded;
		}

		bool GLSL_Shader::loadShaders(const char* vertexShaderName, const char* pixelShaderName)
		{
			if (glCreateProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glCreateProgram is not support by your graphics card!");
			if (glLinkProgram == NULL)
				throw SnowSim::Exceptions::NotSupportedException("glLinkProgram is not support by your graphics card!");

			// delete previously loaded shaders
			unloadShaders();

			// create program
			program = glCreateProgram();

			// create vertex shader
			if(!createVertexShader(vertexShaderName))
				return false;

			// create pixel shader
			if(!createPixelShader(pixelShaderName))
				return false;

			glLinkProgram(program);
			shaderLoaded = true;

			return shaderLoaded;
		}

		void GLSL_Shader::unloadShaders()
		{
			if(shaderLoaded) {

				if ( m_is_destroying )
				{
					if (glDeleteShader == NULL || glDeleteProgram)
						return;
				}
				else
				{
					if (glDeleteShader == NULL)
						throw SnowSim::Exceptions::NotSupportedException("glDeleteShader is not support by your graphics card!");
					if (glDeleteProgram == NULL)
						throw SnowSim::Exceptions::NotSupportedException("glDeleteProgram is not support by your graphics card!");
				}

				glDeleteShader(vertexShader);
				glDeleteShader(pixelShader);
				glDeleteProgram(program);
				shaderLoaded = false;
			}
		}

		void GLSL_Shader::linkProgram()
		{
			glLinkProgram(program);
		}

		void GLSL_Shader::begin()
		{
			glUseProgram(program);
		}

		void GLSL_Shader::end()
		{
			glUseProgram(0);
		}

		bool GLSL_Shader::createVertexShader(const char* vertexShaderName)
		{
			// create vertex shader
			vertexShader = glCreateShader(GL_VERTEX_SHADER);

			const char* vertexShaderSource = this->readShader(vertexShaderName);
			// could not open file
			if(vertexShaderSource == 0) {
				std::cout << "\nKonnte Datei \"" << vertexShaderName << "\" nicht oeffnen!\n";
				return false;
			}

			glShaderSource(vertexShader, 1, &vertexShaderSource, 0);

			glCompileShader(vertexShader);

			// debug
			int info;
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &info);
			if(info == 0) {
				std::cout << "\nVertex Shader \"" << vertexShaderName << "\" wurde nicht kompiliert:\n";
				std::cout << vertexShaderSource;

				int infoLen = 0;
				glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
				if(infoLen > 0) {
					char* infoLog = new char[infoLen];
					int cw = 0;
					glGetShaderInfoLog(vertexShader, infoLen, &cw, infoLog);
					std::cout << infoLog;
					delete[] infoLog;
				}
				delete[] vertexShaderSource;
				return false;
			}

			delete[] vertexShaderSource;
			glAttachShader(program, vertexShader);

			m_vertex_program_path.clear();
			m_vertex_program_path.append(vertexShaderName);

			return true;
		}

		bool GLSL_Shader::createPixelShader(const char* pixelShaderName)
		{
			// create pixel shader
			pixelShader = glCreateShader(GL_FRAGMENT_SHADER);

			const char* pixelShaderSource = this->readShader(pixelShaderName);
			// could not open file
			if(pixelShaderSource == 0) {
				std::cout << "\nKonnte Datei \"" << pixelShaderName << "\" nicht oeffnen!\n";
				return false;
			}

			glShaderSource(pixelShader, 1, &pixelShaderSource, 0);

			glCompileShader(pixelShader);

			// debug
			int info;
			glGetShaderiv(pixelShader, GL_COMPILE_STATUS, &info);
			if(info == 0) {
				std::cout << "\nFragment Shader \"" << pixelShaderName << "\" wurde nicht kompiliert:\n";
				std::cout << pixelShaderSource;

				int infoLen = 0;
				glGetShaderiv(pixelShader, GL_INFO_LOG_LENGTH, &infoLen);
				if(infoLen > 0) {
					char* infoLog = new char[infoLen];
					int cw = 0;
					glGetShaderInfoLog(pixelShader, infoLen, &cw, infoLog);
					std::cout << infoLog;
					delete[] infoLog;
				}
				delete[] pixelShaderSource;
				return false;
			}

			delete[] pixelShaderSource;
			glAttachShader(program, pixelShader);

			m_fragment_program_path.clear();
			m_fragment_program_path.append(pixelShaderName);

			return true;
		}
	}
}
