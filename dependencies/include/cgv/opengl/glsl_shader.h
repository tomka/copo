#pragma once
#include <string>

namespace cgv{
	namespace opengl{
/**
* A class to handle glsl vertex and fragment shaders.
*/
class glsl_shader
{
private:
	bool ready,ok;
	unsigned int s_id;

public:
	enum SHADERTYPE {VERTEX_SHADER, FRAGMENT_SHADER};

	///create shader
	glsl_shader();
	///destroy shader
	~glsl_shader();
	///attach source from file
	void attach_source_from_file(SHADERTYPE type, const std::string &filename);
	///attach source from string
	void attach_source(SHADERTYPE type,const std::string &source);
	///compile attached source; returns true if successful
	bool compile();
	///return last compilation error
	const std::string get_last_error();
	///return internal shader id
	unsigned int id();
	
};
	}
}
