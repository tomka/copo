#include "Texture.h"
#include <stdexcept>

namespace SnowSim {
	namespace Shading {

		Texture
		GetRGBTexture()
		{
			int dim = 128;
			int bytes_per_pixel = 3;

			Texture texture;
			texture.internal_format = GL_RGB8;
			texture.width = dim;
			texture.height = dim;
			texture.depth = dim;
			texture.level = 0;
			texture.border = 0;
			texture.format = GL_RGB;
			texture.type = GL_UNSIGNED_BYTE;

			// ask for enough memory for the texels and make sure we got it before proceeding
			unsigned char * texels = (unsigned char *) malloc(dim * dim * dim * bytes_per_pixel);
			if (texels == NULL)
				throw std::runtime_error(std::string("Snow model: not enough memory for creating texture."));

				// Did not compile on linux:
				//throw std::exception("Snow model: not enough memory for creating texture.");
			int s, t, r;

			// each of the following loops defines one layer of our 3d texture, there are 3 unsigned bytes (red, green, blue) for each texel so each iteration sets 3 bytes
			// the memory pointed to by texels is technically a single dimension (C++ won't allow more than one dimension to be of variable length), the
			// work around is to use a mapping function like the one above that maps the 3 coordinates onto one dimension
			// layer 0 occupies the first (width * height * bytes per texel) bytes, followed by layer 1, etc...
			// define layer 0 as red
			for (s = 0; s < texture.width; ++s) {
				for (t = 0; t < texture.height; ++t) {
					for (r = 0; r < texture.depth; ++r) {
						int pos = bytes_per_pixel * (s + t*texture.width + r*texture.width*texture.height );
						texels[pos] = (unsigned char)s;
						texels[pos+1] = (unsigned char)t;
						texels[pos+2] = (unsigned char)r;
					}
				}
			}

			texture.pixels = texels;

			return texture;
		}

		Texture GetRGBATexture()
		{
			int dim = 128;
			int bytes_per_pixel = 4;

			Texture texture;
			texture.internal_format = GL_RGBA;
			texture.width = dim;
			texture.height = dim;
			texture.depth = dim;
			texture.level = 0;
			texture.border = 0;
			texture.format = GL_RGBA;
			texture.type = GL_UNSIGNED_BYTE;

			// ask for enough memory for the texels and make sure we got it before proceeding
			unsigned char * texels = (unsigned char *) malloc(dim * dim * dim * bytes_per_pixel);
			if (texels == NULL)
				throw std::runtime_error(std::string("Snow model: not enough memory for creating texture."));
			int s, t, r;

			// each of the following loops defines one layer of our 3d texture, there are 3 unsigned bytes (red, green, blue) for each texel so each iteration sets 3 bytes
			// the memory pointed to by texels is technically a single dimension (C++ won't allow more than one dimension to be of variable length), the
			// work around is to use a mapping function like the one above that maps the 3 coordinates onto one dimension
			// layer 0 occupies the first (width * height * bytes per texel) bytes, followed by layer 1, etc...
			// define layer 0 as red
			for (s = 0; s < texture.width; ++s) {
				for (t = 0; t < texture.height; ++t) {
					for (r = 0; r < texture.depth; ++r) {
						int pos = bytes_per_pixel * (s + t*texture.width + r*texture.width*texture.height );
						texels[pos] = (unsigned char)s;
						texels[pos+1] = (unsigned char)t;
						texels[pos+2] = (unsigned char)r;
						texels[pos+3] = (unsigned char)100;
					}
				}
			}

			texture.pixels = texels;

			return texture;
		}

		Texture GetAlphaTexture()
		{
			SnowSim::Shading::Texture texture;

			int dim = 128;
			int bytes_per_pixel = 1;
			texture.internal_format = GL_ALPHA8;
			texture.width = dim;
			texture.height = dim;
			texture.depth = dim;
			texture.level = 0;
			texture.border = 0;
			texture.format = GL_ALPHA;
			texture.type = GL_UNSIGNED_BYTE;

			// ask for enough memory for the texels and make sure we got it before proceeding
			unsigned char * texels = (unsigned char *) malloc(dim * dim * dim * bytes_per_pixel);
			if (texels == NULL)
				throw std::runtime_error(std::string("Snow model: not enough memory for creating texture."));
			int s, t, r;

			for (s = 0; s < texture.width; ++s) {
				for (t = 0; t < texture.height; ++t) {
					for (r = 0; r < texture.depth; ++r) {
						int pos = bytes_per_pixel * (s + t*texture.width + r*texture.width*texture.height );
							texels[pos] = (unsigned char)150;
					}
				}
			}

			texture.pixels = texels;

			return texture;
		}
	}
}
