#pragma once
#
#include <cgv/defines/join.h>
#
# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
#
# define CGV_DEFINES_SLOT_OFFSET_10(x) (x) % 1000000000UL
# define CGV_DEFINES_SLOT_OFFSET_9(x) CGV_DEFINES_SLOT_OFFSET_10(x) % 100000000UL
# define CGV_DEFINES_SLOT_OFFSET_8(x) CGV_DEFINES_SLOT_OFFSET_9(x) % 10000000UL
# define CGV_DEFINES_SLOT_OFFSET_7(x) CGV_DEFINES_SLOT_OFFSET_8(x) % 1000000UL
# define CGV_DEFINES_SLOT_OFFSET_6(x) CGV_DEFINES_SLOT_OFFSET_7(x) % 100000UL
# define CGV_DEFINES_SLOT_OFFSET_5(x) CGV_DEFINES_SLOT_OFFSET_6(x) % 10000UL
# define CGV_DEFINES_SLOT_OFFSET_4(x) CGV_DEFINES_SLOT_OFFSET_5(x) % 1000UL
# define CGV_DEFINES_SLOT_OFFSET_3(x) CGV_DEFINES_SLOT_OFFSET_4(x) % 100UL
# define CGV_DEFINES_SLOT_OFFSET_2(x) CGV_DEFINES_SLOT_OFFSET_3(x) % 10UL
#
# define CGV_DEFINES_SLOT_CC_2(a, b) CGV_DEFINES_SLOT_CC_2_D(a, b)
# define CGV_DEFINES_SLOT_CC_3(a, b, c) CGV_DEFINES_SLOT_CC_3_D(a, b, c)
# define CGV_DEFINES_SLOT_CC_4(a, b, c, d) CGV_DEFINES_SLOT_CC_4_D(a, b, c, d)
# define CGV_DEFINES_SLOT_CC_5(a, b, c, d, e) CGV_DEFINES_SLOT_CC_5_D(a, b, c, d, e)
# define CGV_DEFINES_SLOT_CC_6(a, b, c, d, e, f) CGV_DEFINES_SLOT_CC_6_D(a, b, c, d, e, f)
# define CGV_DEFINES_SLOT_CC_7(a, b, c, d, e, f, g) CGV_DEFINES_SLOT_CC_7_D(a, b, c, d, e, f, g)
# define CGV_DEFINES_SLOT_CC_8(a, b, c, d, e, f, g, h) CGV_DEFINES_SLOT_CC_8_D(a, b, c, d, e, f, g, h)
# define CGV_DEFINES_SLOT_CC_9(a, b, c, d, e, f, g, h, i) CGV_DEFINES_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i)
# define CGV_DEFINES_SLOT_CC_10(a, b, c, d, e, f, g, h, i, j) CGV_DEFINES_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j)
#
# define CGV_DEFINES_SLOT_CC_2_D(a, b) a ## b
# define CGV_DEFINES_SLOT_CC_3_D(a, b, c) a ## b ## c
# define CGV_DEFINES_SLOT_CC_4_D(a, b, c, d) a ## b ## c ## d
# define CGV_DEFINES_SLOT_CC_5_D(a, b, c, d, e) a ## b ## c ## d ## e
# define CGV_DEFINES_SLOT_CC_6_D(a, b, c, d, e, f) a ## b ## c ## d ## e ## f
# define CGV_DEFINES_SLOT_CC_7_D(a, b, c, d, e, f, g) a ## b ## c ## d ## e ## f ## g
# define CGV_DEFINES_SLOT_CC_8_D(a, b, c, d, e, f, g, h) a ## b ## c ## d ## e ## f ## g ## h
# define CGV_DEFINES_SLOT_CC_9_D(a, b, c, d, e, f, g, h, i) a ## b ## c ## d ## e ## f ## g ## h ## i
# define CGV_DEFINES_SLOT_CC_10_D(a, b, c, d, e, f, g, h, i, j) a ## b ## c ## d ## e ## f ## g ## h ## i ## j
#
#define CGV_DEFINES_ASSIGN_SLOT_0 <cgv/defines/slot/slot_0.h>
#define CGV_DEFINES_ASSIGN_SLOT_1 <cgv/defines/slot/slot_1.h>
#define CGV_DEFINES_ASSIGN_SLOT_2 <cgv/defines/slot/slot_2.h>
#define CGV_DEFINES_ASSIGN_SLOT_3 <cgv/defines/slot/slot_3.h>
#define CGV_DEFINES_SLOT(I) CGV_DEFINES_JOIN(CGV_DEFINES_SLOT_,I)()
#define CGV_DEFINES_ASSIGN_SLOT(I) CGV_DEFINES_JOIN(CGV_DEFINES_ASSIGN_SLOT_,I)