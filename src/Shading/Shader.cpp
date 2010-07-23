#include "Shader.h"
#include <iostream>
#include <fstream>
#include <GL/glew.h>
#include <Exceptions/NotSupportedException.h>

namespace SnowSim {
	namespace Shading {

		Shader::Shader()
			:	shaderLoaded(false),
				m_is_destroying(false)
		{}

		Shader::~Shader()
		{
			m_is_destroying = true;
			unloadShaders();
		}

		char* Shader::readShader(const char* fileName)
		{

			std::ifstream in;
			in.open(fileName, std::ios_base::binary);

			if(!in.good())
				return 0;

			// count the chars
			int count = 0;
			in.seekg(0, std::ios::end);
			count = in.tellg();

			// rewind the stream pointer
			in.seekg(0, std::ios::beg);

			char* shaderSource = new char[count + 1];

			in.read(shaderSource, count);
			shaderSource[count] = '\0';

			in.close();

			return shaderSource;
		}
	}
}
