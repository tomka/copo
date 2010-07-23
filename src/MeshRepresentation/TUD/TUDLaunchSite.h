/*
 * TUDLaunchSite.h
 *
 *  Created on: Aug 22, 2009
 *      Author: tom
 */

#ifndef TUDLAUNCHSITE_H_
#define TUDLAUNCHSITE_H_

#pragma once

#include <Exceptions/ArgumentException.h>
#include <Exceptions/NotImplementedException.h>
#include "Mesh.h"
#include <Model/ILaunchSite.h>
#include <types.h>
#include <Model/EdgeGroupFace.h>

// forward declarations
namespace SnowSim
{
	namespace Model {
		class EdgeGroup;
	}
}

namespace MeshRepresentations {
	namespace TUDRepresentation {

		template< class T=float>
		class TUDLaunchSite
			: public SnowSim::Model::ILaunchSite<T>
		{
			public:
				typedef SnowSim::Model::EdgeGroupFace					EdgeGroupFace;
				typedef SnowSim::Model::ILaunchSite<T>					BaseLS;
				typedef typename BaseLS::LaunchSiteType					LaunchSiteType;

			private:
				EdgeGroupFace*	m_face;
				bool			m_initial;
				T				m_height; /* total height, needs always to be up to date (base + snow) */
				T				m_baseHeight;
				T				m_snowHeight;
				point2f			m_position;
				LaunchSiteType	m_type;
				T				m_responsibility_area;
				cgv::math::linalg::fix_col_vec<T,3>
								m_normal;

			public:

				TUDLaunchSite(T x, T y)
					: m_face(NULL), m_initial(false), m_height(0), m_baseHeight(0),
						m_snowHeight(0), m_position(x,y), m_type(BaseLS::Normal), m_responsibility_area(0)
				{

				}

				TUDLaunchSite(T x, T y, EdgeGroupFace * face, bool initial = false, LaunchSiteType type = BaseLS::Normal)
					: m_face(face), m_initial(initial), m_height(0), m_position(x,y), m_type(type), m_baseHeight(0),
						m_snowHeight(0), m_position(x,y), m_responsibility_area(0)
				{
				}

				TUDLaunchSite(T x, T y, EdgeGroupFace * face, Real height, bool initial = false, LaunchSiteType type = LaunchSiteType::Normal)
					: m_face(face), m_initial(initial), m_height(height), m_type(type), m_baseHeight(0), m_snowHeight(0),
						m_position(x,y), m_responsibility_area(0)
				{
				}

				virtual ~TUDLaunchSite();

				/* interface implementation */

				// Gets the x component of the launch sites position
				T x()
				{
					return this->m_position.x();
				}

				// Gets the y component of the launch sites position
				T y()
				{
					return this->m_height;
				}

				// Gets the z component of the launch sites position
				T z()
				{
					return this->m_position.y();
				}

				void SetBaseHeight(T height)
				{
					this->m_baseHeight = height;
					this->m_height = m_baseHeight + m_snowHeight;

					if (m_face != NULL)
						m_face->PropagateNewHeight(this);
				}

				T GetBaseHeight()
				{
					return this->m_baseHeight;
				}

				T GetSnowHeight()
				{
					return this->m_snowHeight;
				}

				void AddToSnowHeight(T addition)
				{
					m_height += addition;
					m_snowHeight += addition;

					if (m_face != NULL)
						m_face->PropagateNewHeight(this);
				}

				void ReduceSnowHeight(T removal)
				{
					m_height -= removal;
					m_snowHeight -= removal;

					if (m_face != NULL)
						m_face->PropagateNewHeight(this);
				}

				T GetTotalHeight()
				{
					return this->m_height;
				}

				// Gets the responsibility area of the launch site
				T GetResponsibility()
				{
					return m_responsibility_area;
				}

				// Sets the responsibility area of the launch site
				void SetResponsibility(T responsibility)
				{
					m_responsibility_area = responsibility;
				}

				cgv::scene::geometry::point<T, 3> GetPosition()
				{
					return cgv::scene::geometry::point<T, 3>(x(), m_height, z());
				}

				// Gets the normal of the launch site
				cgv::math::linalg::fix_col_vec<T,3> GetNormal()
				{
					return m_normal;
				}

				// Gets the normal of the launch site
				void SetNormal(cgv::math::linalg::fix_col_vec<T,3>& n)
				{
					m_normal = n;
				}

				EdgeGroupFace* GetFace()
				{
					return m_face;
				}

				void SetFace(EdgeGroupFace* face)
				{
					if (face == NULL)
						throw SnowSim::Exceptions::ArgumentException("face must not be null");

					m_face = face;
				}

				// Gets if the launch site is an initial one or not
				bool IsInitial()
				{
					return m_initial;
				}

				void SetInitial(bool isInitial)
				{
					m_initial = isInitial;
				}

				LaunchSiteType GetType()
				{
					return m_type;
				}

				void SetType(LaunchSiteType type)
				{
					m_type = type;
				}
		};

		template< class T>
		TUDLaunchSite<T>::~TUDLaunchSite() {
			// TODO Auto-generated destructor stub
		}
	}

}

#endif /* TUDLAUNCHSITE_H_ */
