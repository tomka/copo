#include <cgv/opengl/glsl_program.h>
#ifdef WIN32
#include <windows.h>
#endif

#include <iostream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

namespace cgv{
	namespace opengl{

glsl_program::glsl_program()
{
	ready=false;
	ok=false;
	p_id = glCreateProgramObjectARB(); 


}

glsl_program::~glsl_program()
{
}

void glsl_program::attach_shader(glsl_shader *shader)
{
	glAttachObjectARB(p_id,shader->id()); 
}
bool glsl_program::attach_shader_from_file(glsl_shader::SHADERTYPE type,const std::string& filename)
{
	glsl_shader * s = new glsl_shader();
	s->attach_source_from_file(type,filename);
	if(s->compile())
	{
		glAttachObjectARB(p_id,s->id()); 
		return true;
	}else
		return false;
}

bool glsl_program::link()
{
	glLinkProgramARB(p_id); 
	int result;
	glGetObjectParameterivARB(p_id, GL_OBJECT_LINK_STATUS_ARB, &result); 
	ok = result == 1;
	if(ok)
		ready = true;
	else
		ready = false;
	return ok;

}

void glsl_program::begin()
{
	glUseProgramObjectARB(p_id); 
}

void glsl_program::end()
{
	glUseProgramObjectARB(0); 
}

void glsl_program::uniform1f(char* varname, float v0)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform1fARB(loc, v0);
}

void glsl_program::uniform2f(char* varname, float v0, float v1)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform2fARB(loc, v0,v1);
}

void glsl_program::uniform3f(char* varname, float v0, float v1, float v2)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform3fARB(loc, v0,v1,v2);
}

void glsl_program::uniform4f(char* varname, float v0, float v1, float v2,float v3)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform4fARB(loc, v0,v1,v2,v3);
}

void glsl_program::uniform1i(char* varname, int v0)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform1iARB(loc, v0);
}

void glsl_program::uniform2i(char* varname, int v0, int v1)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform2iARB(loc, v0,v1);
}

void glsl_program::uniform3i(char* varname, int v0, int v1, int v2)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform3iARB(loc, v0,v1,v2);
}

void glsl_program::uniform4i(char* varname, int v0, int v1, int v2,int v3)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable  
    glUniform4iARB(loc, v0,v1,v2,v3);
}


void glsl_program::uniform_matrix2fv(char* varname, int count, bool transpose, float *value)
{
   
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    glUniformMatrix2fvARB(loc, count, transpose, value);
}


void glsl_program::uniform_matrix3fv(char* varname, int count, bool transpose, float *value)
{

    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return ;  // can't find variable
    
    glUniformMatrix3fvARB(loc, count, transpose, value);

}


void glsl_program::uniform_matrix4fv(char* varname, int count, bool transpose, float *value)
{
  
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return ;  // can't find variable
    
    glUniformMatrix4fvARB(loc, count, transpose, value);

}


void glsl_program::uniform1fv(char* varname, int count, float *value)
{  
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    glUniform1fvARB(loc, count, value);
}

void glsl_program::uniform2fv(char* varname, int count, float *value)
{   
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    glUniform2fvARB(loc, count, value);
}


void glsl_program::uniform3fv(char* varname, GLsizei count, float *value)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    glUniform3fvARB(loc, count, value);

}


void glsl_program::uniform4fv(char* varname, int count, float *value)
{

    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    glUniform4fvARB(loc, count, value);

}



void glsl_program::uniform1iv(char* varname, int count, int *value)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    
    glUniform1ivARB(loc, count, value);

}


void glsl_program::uniform2iv(char* varname, int count, int *value)
{
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    
    glUniform2ivARB(loc, count, value);

}


void glsl_program::uniform3iv(char* varname, int count, int *value)
{
    
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    
    glUniform3ivARB(loc, count, value);

}


void glsl_program::uniform4iv(char* varname, int count, int *value)
{
    
    GLint loc = glGetUniformLocationARB(p_id,varname);
    if (loc==-1) return;  // can't find variable
    
    glUniform4ivARB(loc, count, value);

}

void glsl_program::vertex_attrib1f(unsigned int index, float v0)
{
   glVertexAttrib1fARB(index, v0);
}

void glsl_program::vertex_attrib2f(unsigned int index, float v0, float v1)
{
   glVertexAttrib2fARB(index, v0, v1);
}

void glsl_program::vertex_attrib3f(unsigned int index, float v0, float v1, float v2)
{
    glVertexAttrib3fARB(index, v0, v1, v2);
}

void glsl_program::vertex_attrib4f(unsigned int index, float v0, float v1, float v2, float v3)
{ 
   glVertexAttrib4fARB(index, v0, v1, v2, v3);
}
void glsl_program::vertex_attrib1d(unsigned int index, double v0)
{
   glVertexAttrib1dARB(index, v0);
}

void glsl_program::vertex_attrib2d(unsigned int index, double v0, double v1)
{
   glVertexAttrib2dARB(index, v0, v1);
}

void glsl_program::vertex_attrib3d(unsigned int index, double v0, double v1, double v2)
{
    glVertexAttrib3dARB(index, v0, v1, v2);
}

void glsl_program::vertex_attrib4d(unsigned int index, double v0, double v1, double v2, double v3)
{ 
   glVertexAttrib4dARB(index, v0, v1, v2, v3);
}

unsigned int glsl_program::vertex_attrib_index(char *name)
{ 
	return glGetAttribLocationARB(p_id,name);    
}




const std::string glsl_program::get_last_error()
{
	if (!ok)
	{
		int infologLength = 0;
	    int charsWritten  = 0;
	    char *infoLog;
		glGetObjectParameterivARB(p_id, GL_OBJECT_INFO_LOG_LENGTH_ARB,
						 &infologLength);
		if (infologLength > 0)
	    {
			infoLog = (char *)malloc(infologLength);
			glGetInfoLogARB(p_id, infologLength, &charsWritten, infoLog);
			std::string err = infoLog;
			free(infoLog);
			return err;
	    }

	}
	return "no errors\n";
}

	}
}

