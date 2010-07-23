/*! \file loop_k.h
	\brief Include this header to perform a preprocessor loop in the variable \c K.

	See loop_i.h for a detailed explanation.
*/
#include <cgv/defines/loop/loop_begin.h>
/// \cond
// map local definitions to global name space
#ifdef LOOP_K_BEGIN
#	define CGV_DEFINES_LOOP_K_BEGIN LOOP_K_BEGIN
#else
#	define CGV_DEFINES_LOOP_K_BEGIN CGV_DEFINES_LOOP_STD_BEGIN
#endif
#
#ifdef LOOP_K_END
#	define CGV_DEFINES_LOOP_K_END LOOP_K_END
#else
#	define CGV_DEFINES_LOOP_K_END CGV_DEFINES_LOOP_STD_END
#endif
#ifdef LOOP_K_BODY
#	define CGV_DEFINES_LOOP_K_BODY LOOP_K_BODY
#endif
#ifdef LOOP_K_FILE
#	define CGV_DEFINES_LOOP_K_FILE LOOP_K_FILE
#endif
#ifdef LOOP_K_SEP
#	define CGV_DEFINES_LOOP_K_SEP LOOP_K_SEP
#endif
// loop initialization
#define CGV_DEFINES_EXPRESSION CGV_DEFINES_LOOP_K_BEGIN
#include CGV_DEFINES_ASSIGN_SLOT(3)
#include <cgv/defines/loop/loop_k_recurse.h>
#
#undef CGV_DEFINES_LOOP_K_END
#undef CGV_DEFINES_LOOP_K_BEGIN
#undef CGV_DEFINES_LOOP_K_BODY
#undef CGV_DEFINES_LOOP_K_FILE
#undef CGV_DEFINES_LOOP_K_SEP
#undef LOOP_K_BEGIN
#undef LOOP_K_END
#undef LOOP_K_BODY
#undef LOOP_K_FILE
#undef LOOP_K_SEP
/// \endcond
#include <cgv/defines/loop/loop_end.h>