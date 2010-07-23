#pragma once

#include <cgv/opengl/glsl_shader.h>

namespace cgv{
	namespace opengl{

/**
* Class to handle glsl shader programs.
*/
class glsl_program
{
private:
	bool ready,ok;
	unsigned int p_id;

public:
	///create shader program
	glsl_program();
	///destroy shader program
	~glsl_program();

	///attach a compiled shader to the program
	void attach_shader(glsl_shader *shader);
	///read, compile and attach a shader from a shader source file
	///return false if something goes wrong
	bool attach_shader_from_file(glsl_shader::SHADERTYPE type,const std::string& filename);
	///link shaders to an executable program
	bool link();
	///execute shader program
	void begin();
	///stop shader program and return to fixed functionality
	void end();

	///set uniform float named varname to given constant value
	void uniform1f(char* varname, float v0);  
	///set uniform floats named varname to given constant value
	void uniform2f(char* varname, float v0, float v1);
	///set uniform floats named varname to given constant value
	void uniform3f(char* varname, float v0, float v1, float v2); 
	///set uniform floats named varname to given constant value
	void uniform4f(char* varname, float v0, float v1, float v2, float v3); 
	///set uniform int named varname to given constant value, its not only for int but also to assign texture units to samplers
	void uniform1i(char* varname, int v0);
	///set uniform ints named varname to given constant value
	void uniform2i(char* varname, int v0, int v1);
	///set uniform ints named varname to given constant value
	void uniform3i(char* varname, int v0, int v1, int v2);
	///set uniform ints named varname to given constant value
	void uniform4i(char* varname, int v0, int v1, int v2, int v3);

	///set uniform 2x2 matrix named varname to given constant value
	void uniform_matrix2fv(char* varname, int count, bool transpose, float *value);
	///set uniform 3x3 matrix named varname to given constant value
	void uniform_matrix3fv(char* varname, int count, bool transpose, float *value);
	///set uniform 4x4 matrix named varname to given constant value
	void uniform_matrix4fv(char* varname, int count, bool transpose, float *value);

	///set uniform float array named varname to given value
	void uniform1fv(char* varname, int count, float *value);
	///set uniform float array named varname to given value
	void uniform2fv(char* varname, int count, float *value);
	///set uniform float array named varname to given value
	void uniform3fv(char* varname, int count, float *value);
	///set uniform float array named varname to given value
	void uniform4fv(char* varname, int count, float *value);

	///set uniform int array named varname to given value
	void uniform1iv(char* varname, int count, int *value);
	///set uniform int array named varname to given value
	void uniform2iv(char* varname, int count, int *value);
	///set uniform int array named varname to given value
	void uniform3iv(char* varname, int count, int *value);
	///set uniform int array named varname to given value
	void uniform4iv(char* varname, int count, int *value);

	///return index of vertex attribute by name
	unsigned int vertex_attrib_index(char *name);
	///set float vertex attribute by its index to constant value
	void vertex_attrib1f(unsigned int index, float v0);
	///set float vertex attribute by its index to constant value
	void vertex_attrib2f(unsigned int index, float v0, float v1);
	///set float vertex attribute by its index to constant value
	void vertex_attrib3f(unsigned int index, float v0, float v1, float v2);
	///set float vertex attribute by its index to constant value
	void vertex_attrib4f(unsigned int index, float v0, float v1, float v2, float v3);
	///set double vertex attribute by its index to constant value
	void vertex_attrib1d(unsigned int index, double v0);
	///set double vertex attribute by its index to constant value
	void vertex_attrib2d(unsigned int index, double v0, double v1);
	///set double vertex attribute by its index to constant value
	void vertex_attrib3d(unsigned int index, double v0, double v1, double v2);
	///set double vertex attribute by its index to constant value
	void vertex_attrib4d(unsigned int index, double v0, double v1, double v2, double v3);
	///return last error as string 
	const std::string get_last_error();
};
	}
}
