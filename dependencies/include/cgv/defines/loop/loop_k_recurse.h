// check for termination
#if CGV_DEFINES_SLOT(3) < CGV_DEFINES_LOOP_K_END
// define loop variable from slot
#	ifdef K
#		error CGV_DEFINES_LOOP_K_ERROR:  loop variable K already defined
#	endif
#	define K CGV_DEFINES_SLOT(3)
// execute loop body
#	ifdef CGV_DEFINES_LOOP_K_BODY
#		if defined(CGV_DEFINES_LOOP_K_SEP) && (CGV_DEFINES_SLOT(3)+1 < CGV_DEFINES_LOOP_K_END)
JOIN(CGV_DEFINES_LOOP_K_BODY,CGV_DEFINES_LOOP_K_SEP)
#		else
CGV_DEFINES_LOOP_K_BODY
#		endif
#	else
#		ifdef CGV_DEFINES_LOOP_K_FILE
#			include CGV_DEFINES_LOOP_K_FILE
#			if defined(CGV_DEFINES_LOOP_K_SEP) && (CGV_DEFINES_SLOT(3)+1 < CGV_DEFINES_LOOP_K_END)
CGV_DEFINES_LOOP_K_SEP
#			endif
#		endif
#	endif
// increment loop variable
#	define  CGV_DEFINES_EXPRESSION CGV_DEFINES_SLOT(3)+1
#	include CGV_DEFINES_ASSIGN_SLOT(3)
#	undef K
// iterate by recursve include
#	include <cgv/defines/loop/loop_k_recurse.h>
#endif