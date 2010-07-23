/*
 *  PolygonOffset.cpp
 *  Legion
 *
 *  Created by Shamyl Zakariya on 6/5/05.
 *  Copyright 2005 Shamyl Zakariya. All rights reserved.
 *
 */

#include "PolygonOffset.h"
#include <GL/glew.h>

namespace Utilities {
	/*
			typedef std::pair< float, float > Offset;
			typedef std::vector< Offset > OffsetVec;

			static OffsetVec _offsetStack;
			static float _currentFactor, _currentUnits;
	*/

	PolygonOffset::OffsetVec PolygonOffset::_offsetStack;
	float PolygonOffset::_currentFactor = 0;
	float PolygonOffset::_currentUnits = 0;


	void PolygonOffset::push( float factor, float units )
	{
		_currentFactor += factor;
		_currentUnits += units;

		_offsetStack.push_back( Offset( factor, units ));

		glPolygonOffset( _currentFactor, _currentUnits );
	}

	void PolygonOffset::pop( void )
	{
		Offset offset( _offsetStack.back() );
		_currentFactor -= offset.first;
		_currentUnits -= offset.second;
		_offsetStack.pop_back();

		glPolygonOffset( _currentFactor, _currentUnits );
	}

	void PolygonOffset::reset( void )
	{
		_offsetStack.clear();
		_currentFactor = _currentUnits = 0;
		glPolygonOffset( _currentFactor, _currentUnits );
	}
}
