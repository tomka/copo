#pragma once

#include <types.h>
#include <Shading/Texture.h>

namespace SnowSim {
	namespace Rendering {
		class RenderingInfo
		{
			private:
				point3f m_eye_position;
				Shading::Texture m_noise_tex;

			public:
				RenderingInfo(point3f eye_pos, const Shading::Texture& tex);

				point3f GetEyePosition() const;

				const Shading::Texture&  GetNoiseTexture() const;
		};
	}
}
