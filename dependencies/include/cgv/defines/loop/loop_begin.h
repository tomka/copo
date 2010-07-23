#include <cgv/defines/join.h>
#include <cgv/defines/slot.h>
#ifndef CGV_DEFINES_LOOP_STD_BEGIN
/// defines default initial value for loops
#define CGV_DEFINES_LOOP_STD_BEGIN 0
#endif
#ifndef CGV_DEFINES_LOOP_STD_END
/// defines default upper bound for loops
#define CGV_DEFINES_LOOP_STD_END 8
#endif
/// \cond
// count nesting level in slot 0
#ifndef CGV_DEFINES_LOOP_ENTERED
#	define CGV_DEFINES_LOOP_ENTERED
#	define CGV_DEFINES_EXPRESSION 1
#	include CGV_DEFINES_ASSIGN_SLOT(0)
// enter join into local namespace of defines in outer loop
#	ifndef JOIN
#		define JOIN(X,Y) CGV_DEFINES_JOIN(X,Y)
#	endif
#else
#	define CGV_DEFINES_EXPRESSION CGV_DEFINES_SLOT(0)+1
#	include CGV_DEFINES_ASSIGN_SLOT(0)
#endif
#define LOOP_NESTING_LEVEL CGV_DEFINES_SLOT(0)
/// \endcond