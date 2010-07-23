// check for termination
#if CGV_DEFINES_SLOT(2) < CGV_DEFINES_LOOP_J_END
// define loop variable from slot
#	ifdef J
#		error CGV_DEFINES_LOOP_J_ERROR:  loop variable J already defined
#	endif
#	define J CGV_DEFINES_SLOT(2)
// execute loop body
#	ifdef CGV_DEFINES_LOOP_J_BODY
#		if defined(CGV_DEFINES_LOOP_J_SEP) && (CGV_DEFINES_SLOT(2)+1 < CGV_DEFINES_LOOP_J_END)
JOIN(CGV_DEFINES_LOOP_J_BODY,CGV_DEFINES_LOOP_J_SEP)
#		else
CGV_DEFINES_LOOP_J_BODY
#		endif
#	else
#		ifdef CGV_DEFINES_LOOP_J_FILE
#			include CGV_DEFINES_LOOP_J_FILE
#			if defined(CGV_DEFINES_LOOP_J_SEP) && (CGV_DEFINES_SLOT(2)+1 < CGV_DEFINES_LOOP_J_END)
CGV_DEFINES_LOOP_J_SEP
#			endif
#		endif
#	endif
// increment loop variable
#	define  CGV_DEFINES_EXPRESSION CGV_DEFINES_SLOT(2)+1
#	include CGV_DEFINES_ASSIGN_SLOT(2)
#	undef J
// iterate by recursve include
#	include <cgv/defines/loop/loop_j_recurse.h>
#endif