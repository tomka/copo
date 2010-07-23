/*! \file loop_j.h
	\brief Include this header to perform a preprocessor loop in the variable \c J.

	See loop_i.h for a detailed explanation.
*/
#include <cgv/defines/loop/loop_begin.h>
/// \cond
// map local definitions to global name space
#ifdef LOOP_J_BEGIN
#	define CGV_DEFINES_LOOP_J_BEGIN LOOP_J_BEGIN
#else
#	define CGV_DEFINES_LOOP_J_BEGIN CGV_DEFINES_LOOP_STD_BEGIN
#endif
#
#ifdef LOOP_J_END
#	define CGV_DEFINES_LOOP_J_END LOOP_J_END
#else
#	define CGV_DEFINES_LOOP_J_END CGV_DEFINES_LOOP_STD_END
#endif
#ifdef LOOP_J_BODY
#	define CGV_DEFINES_LOOP_J_BODY LOOP_J_BODY
#endif
#ifdef LOOP_J_FILE
#	define CGV_DEFINES_LOOP_J_FILE LOOP_J_FILE
#endif
#ifdef LOOP_J_SEP
#	define CGV_DEFINES_LOOP_J_SEP LOOP_J_SEP
#endif
// loop initialization
#define CGV_DEFINES_EXPRESSION CGV_DEFINES_LOOP_J_BEGIN
#include CGV_DEFINES_ASSIGN_SLOT(2)
#include <cgv/defines/loop/loop_j_recurse.h>
#
#undef CGV_DEFINES_LOOP_J_END
#undef CGV_DEFINES_LOOP_J_BEGIN
#undef CGV_DEFINES_LOOP_J_BODY
#undef CGV_DEFINES_LOOP_J_FILE
#undef CGV_DEFINES_LOOP_J_SEP
#undef LOOP_J_BEGIN
#undef LOOP_J_END
#undef LOOP_J_BODY
#undef LOOP_J_FILE
#undef LOOP_J_SEP
/// \endcond
#include <cgv/defines/loop/loop_end.h>