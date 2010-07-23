/*
 *  PolygonOffset.h
 *  Legion
 *
 *  Created by Shamyl Zakariya on 6/5/05.
 *  Copyright 2005 Shamyl Zakariya. All rights reserved.
 *  From: http://www.idevgames.com/forum/showthread.php?t=9284
 *
 */

#ifndef __POLYGON_OFFSET_H__
#define __POLYGON_OFFSET_H__

#include <map>
#include <vector>

/**
	@class PolygonOffset
	This is a wrapper for the OpenGL mechanism glPolygonOffset.
	PolygonOffset maintains a stack of polygon offsets, such that
	deeply nested uses of glPolygonOffset will work cleanly. Imagine,
	for example, the situation where you have a solid-wireframed object
	being drawn in a multi-pass lighting situation. You'll need to offset
	the fill of the solid so the wireframe can be drawn, but since the
	object has to be drawn at least twice ( for lighting ) you need differing
	offsets for each pass to prevent z-fighting. In a normal usage of
	glPolygonOffset one would have to keep track of the current pass
	and set offsets accordingly.

	But, with the stack-nature of PolygonOffset, one can simply push
	an offset, which will be added to the current offset. When done drawing,
	one pops the offset ( important, don't forget to pop ).

	For reasons of cleanliness, one should never disable GL_POLYGON_OFFSET_FILL
	and one should call reset() at the beginning of every drawing loop to
	zero the offset.
*/


namespace Utilities {

	class PolygonOffset
	{
		public:

			/**
				Push an offset -- this will be added to the
				current offset. Always call ::pop() when
				done drawing with this offset.
			*/
			static void push( float factor, float units );

			/**
				Pop the most recently added offset.
			*/
			static void pop( void );

			/**
				Resets the offsets to zero and empties the stack.
			*/
			static void reset( void );

			/**
				@return the current offset factor
			*/
			static float currentFactor( void ) { return _currentFactor; }

			/**
				@return the current offset units
			*/
			static float currentUnits( void ) { return _currentUnits; }

		protected:

			PolygonOffset( void ){}

			typedef std::pair< float, float > Offset;
			typedef std::vector< Offset > OffsetVec;

			static OffsetVec _offsetStack;
			static float _currentFactor, _currentUnits;

	};
}

#endif
