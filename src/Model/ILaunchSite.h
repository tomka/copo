#pragma once

#include "types.h"

namespace SnowSim {
	namespace Model {

		class EdgeGroupFace;

		/// Defines an interface for a launch site
		/// suitable for the snow simulation.
		template<class T = float>
		class ILaunchSite {
			private:
				bool m_isHighlighted;
				float m_exposure;

			public:

				ILaunchSite()
					: 	m_isHighlighted(false),
						m_exposure(0.0)
				{
				}

				enum LaunchSiteType
				{
					Normal,
					OnDrop
				};

				// Gets the x component of the launch sites position
				virtual T x() = 0;

				// Gets the y component of the launch sites position
				virtual T y() = 0;

				// Gets the z component of the launch sites position
				virtual T z() = 0;

				// Sets the height component of the launch site.
				virtual void SetBaseHeight(T height) = 0;

				// Sets the height component of the launch site.
				virtual T GetBaseHeight() = 0;

				// Gets the snow height component of the launch site.
				// This is only the height of the snow. If no snow is there it is 0.
				virtual T GetSnowHeight() = 0;

				// Adds the given value to the height of the launch site
				virtual void AddToSnowHeight(T addition) = 0;

				// Reduces the height of the launch site by the given value
				virtual void ReduceSnowHeight(T removal) = 0;

				// Gets the total height component of the launch site.
				// this is the base height plus snow height
				virtual T GetTotalHeight() = 0;

				// Gets the responsibility area of the launch site
				virtual T GetResponsibility() = 0;

				// Sets the responsibility area of the launch site
				virtual void SetResponsibility(T responsibility) = 0;

				// Gets the position of the launch site
				virtual point<T, 3> GetPosition() = 0;

				// Gets the normal of the launch site
				virtual fix_col_vec<T,3> GetNormal() = 0;

				// Gets the normalized normal of the launch site
				virtual void SetNormal(fix_col_vec<T,3>& n) = 0;

				// Gets the face the launch site belongs to
				virtual EdgeGroupFace* GetFace() = 0;

				// Sets the corresponding face of the launch site
				virtual void SetFace(EdgeGroupFace* face) = 0;

				// Gets if the launch site is an initial one or not
				virtual bool IsInitial() = 0;

				// Sets if the launch site is an initial one or not
				virtual void SetInitial(bool isInitial) = 0;

				// Gets the type of the launch site
				virtual LaunchSiteType GetType() = 0;

				// Sets the type of the launch site
				virtual void SetType(LaunchSiteType type) = 0;

				/// Is the launch site highlighted?
				bool IsHighlighted()
				{
					return m_isHighlighted;
				}

				/// Set the launch site highlighted or not
				void SetHighlighted(bool highlighted)
				{
					m_isHighlighted = highlighted;
				}

				// Sets the occlusion factor of the launch site
				void SetExposure(float exposure) {
					m_exposure = exposure;
				}

				// Gets the occlusion factor of the launch site
				float GetExposure() {
					return m_exposure;
				}
		};
	};
};
