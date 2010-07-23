#include "RenderingInfo.h"

namespace SnowSim {
	namespace Rendering {

		RenderingInfo::RenderingInfo(point3f eye_pos, const Shading::Texture& tex)
			: 	m_eye_position(eye_pos),
				m_noise_tex(tex)
		{
		}

		point3f RenderingInfo::GetEyePosition() const
		{
			return m_eye_position;
		}

		const Shading::Texture&
		RenderingInfo::GetNoiseTexture() const
		{
			return m_noise_tex;
		}
	}
}
