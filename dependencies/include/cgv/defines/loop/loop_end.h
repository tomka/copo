/// \cond
// decrease nesting level in slot 0
#define CGV_DEFINES_EXPRESSION CGV_DEFINES_SLOT(0)-1
#include CGV_DEFINES_ASSIGN_SLOT(0)
#undef LOOP_NESTING_LEVEL
// if end of outer loop, undef local join and entered define
#if CGV_DEFINES_SLOT(0) == 0
#	undef CGV_DEFINES_LOOP_ENTERED
#	undef JOIN
#else
#	define LOOP_NESTING_LEVEL CGV_DEFINES_SLOT(0)
#endif
/// \endcond