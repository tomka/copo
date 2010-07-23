// check for termination
#if CGV_DEFINES_SLOT(1) < CGV_DEFINES_LOOP_I_END
// define loop variable from slot
#	ifdef I
#		error CGV_DEFINES_LOOP_I_ERROR:  loop variable I already defined
#	endif
#	define I CGV_DEFINES_SLOT(1)
// execute loop body
#	ifdef CGV_DEFINES_LOOP_I_BODY
#		if defined(CGV_DEFINES_LOOP_I_SEP) && (CGV_DEFINES_SLOT(1)+1 < CGV_DEFINES_LOOP_I_END)
JOIN(CGV_DEFINES_LOOP_I_BODY,CGV_DEFINES_LOOP_I_SEP)
#		else
CGV_DEFINES_LOOP_I_BODY
#		endif
#	else
#		ifdef CGV_DEFINES_LOOP_I_FILE
#			include CGV_DEFINES_LOOP_I_FILE
#			if defined(CGV_DEFINES_LOOP_I_SEP) && (CGV_DEFINES_SLOT(1)+1 < CGV_DEFINES_LOOP_I_END)
CGV_DEFINES_LOOP_I_SEP
#			endif
#		endif
#	endif
// increment loop variable
#	define  CGV_DEFINES_EXPRESSION CGV_DEFINES_SLOT(1)+1
#	include CGV_DEFINES_ASSIGN_SLOT(1)
#	undef I
// iterate by recursve include
#	include <cgv/defines/loop/loop_i_recurse.h>
#endif