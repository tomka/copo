#include <cgv/opengl/glsl_shader.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <fstream>
#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace cgv{
	namespace opengl{

glsl_shader::glsl_shader()
{
	ready = false;
	ok = false;
}

glsl_shader::~glsl_shader()
{
}


	
void glsl_shader::attach_source_from_file(SHADERTYPE type, const std::string &filename)
{
	std::ifstream f;
	std::string source;

	char ch;
	
	f.open(filename.c_str());
	if(f)
	{
		while (true)
		{
			ch = f.get();
			if(f.eof())
				break;
			source +=ch;  
		}
		f.close();
		attach_source(type,source);
	}else
	{
		std::cout << "file not found: "<< filename<<std::endl;
	}
}

void glsl_shader::attach_source(SHADERTYPE type,const std::string &source)
{
	const char * s =source.c_str();
	if(type == glsl_shader::VERTEX_SHADER)
		s_id = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB); 
	else
		s_id =  glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);	
	glShaderSourceARB(s_id, 1, &s,NULL);
	ready = false;

}

bool glsl_shader::compile()
{
	glCompileShaderARB(s_id);
	int result;
	glGetObjectParameterivARB(s_id, GL_OBJECT_COMPILE_STATUS_ARB, &result); 
	ok = result == 1;
	if(ok)
		ready = true;
	else
		ready = false;
	return ok;

}

const std::string glsl_shader::get_last_error()
{
	if (!ok)
	{
		int infologLength = 0;
	    int charsWritten  = 0;
	    char *infoLog;
		glGetObjectParameterivARB(s_id, GL_OBJECT_INFO_LOG_LENGTH_ARB,
						 &infologLength);
		if (infologLength > 0)
	    {
			infoLog = (char *)malloc(infologLength);
			glGetInfoLogARB(s_id, infologLength, &charsWritten, infoLog);
			std::string err = infoLog;
			free(infoLog);
			return err;
	    }

	}
	return "no errors\n";
}

unsigned int glsl_shader::id()
{
	return s_id;
}
	}
}
