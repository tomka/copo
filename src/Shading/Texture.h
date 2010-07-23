#pragma once

#include <Utilities/OpenGL.h>
#include <types.h>

namespace SnowSim {
	namespace Shading {

		/* Represents a texture to be used with OpenGL */
		struct Texture
		{
			bool bound; /* indicates whether the texture is already bound */
			unsigned int texname;
			GLint	level;
			GLenum	internal_format;
			GLsizei	width;
			GLsizei	height;
			GLsizei	depth; /* used for 3D textures */
			GLint	border;
			GLenum	format;
			GLenum	type;
			GLvoid*	pixels;

			Texture()
				:	bound(false),
					texname(0),
					level(0),
					pixels(NULL)
			{
			}
		};

		Texture GetRGBTexture();
		Texture GetRGBATexture();
		Texture GetAlphaTexture();
	}
}
