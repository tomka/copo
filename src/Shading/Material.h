#pragma once
#include <types.h>

namespace SnowSim {
	namespace Shading {

		struct Material
		{
			point3f	ambient_color;
			point3f	diffuse_color;
			point3f	specular_color;
			float	alpha;
			float	shininess;
			bool	hasSpecularHighlights;

			/*
			Ka r g b 
				defines the ambient color of the material to be (r,g,b). The default is (0.2,0.2,0.2); 
			Kd r g b 
				defines the diffuse color of the material to be (r,g,b). The default is (0.8,0.8,0.8); 
			Ks r g b 
				defines the specular color of the material to be (r,g,b). This color shows up in highlights. The default is (1.0,1.0,1.0); 
			d alpha 
				defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all) Some formats use Tr instead of d; 
			Tr alpha 
				defines the transparency of the material to be alpha. The default is 1.0 (not transparent at all). Some formats use d instead of Tr; 
			Ns s 
				defines the shininess of the material to be s. The default is 0.0; 
			illum n 
				denotes the illumination model used by the material. illum = 1 indicates a flat material with no specular highlights, so the value of Ks is not used. illum = 2 denotes the presence of specular highlights, and so a specification for Ks is required. 
			map_Ka filename 
				names a file containing a texture map, which should just be an ASCII dump of RGB values;
			*/

			Material()
				:	ambient_color(0.2f, 0.2f, 0.2f),
					diffuse_color(0.8f, 0.8f, 0.8f),
					specular_color(1.0f, 1.0f, 1.0f),
					alpha(1.0f),
					shininess(0.0f),
					hasSpecularHighlights(false)
			{
			}
		};
	}
}