/* config.h.  Generated from configh.in by configure.  */
/* "$Id: configh.in 6480 2008-10-22 06:49:47Z spitzak $"
 *
 * Configuration file for compiling the Fast Light Tool Kit (FLTK).
 *
 * Copyright 1998-2005 by Bill Spitzak and others.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA.
 *
 * Please report all bugs and problems to "fltk-bugs@fltk.org".
 */

/* Where to find files. Currently not used, and if they are they will
   be overridden by environment variables with the same name. */
#define FLTK_DATADIR "/usr/local/share/fltk"
#define FLTK_DOCDIR "/usr/local/share/doc/fltk"

/* Byte order of your machine: 1 = big-endian, 0 = little-endian. */
#define WORDS_BIGENDIAN		0

/* Types used by fl_draw_image.  One of U32 or U64 must be defined.
   U16 is optional but FLTK will work better with it! */
#define U16 unsigned short
#define U32 unsigned
#define U64 unsigned long

/* Set this to 1 to compile for X11 on Windows or OS/X. */
/* #undef USE_X11 */
#if !defined(USE_X11) && !defined(_WIN32) && !defined(__APPLE__)
# define USE_X11 1
#endif

/* Set this to 1 to compile using Quartz drawing instead of X11
   on Apple OS X */
#if !defined(USE_X11) && defined(__APPLE__)
# define USE_QUARTZ 1
#endif

/* Setting this to zero will delete code to handle X and WIN32
   colormaps, this will save a good deal of code (especially for
   fl_draw_image), but FLTK will only work on TrueColor visuals. */
#define USE_COLORMAP		1

/* If USE_X11 is on, this also uses XLockDisplay() to implement
   fltk::lock()/unlock(). This is NOT recommended, see the file
   src/x11/lock.cxx for details.
*/
#define USE_X11_MULTITHREADING	0

/* Use the new Xft library to draw fonts, which is how you get
   anti-aliasing. (ignored if !USE_X11) */
#define USE_XFT 1

/* Use the Cairo library to draw everything. Ignored if !USE_X11
   Currently only partly implemented, though line drawing looks
   nice. For more info on Cairo see http://www.cairographics.org */
#define USE_CAIRO		0

/* Use the fltk::clipout() method to make redrawing blink less, by
   drawing the widgets first, and then the background of the
   window. This is unnecessary in double-buffered windows, and
   tests have shown it to be slower, and it is more complicated.
   Also Cairo and OpenGL do not support clipout() so it does not
   work in them. */
#define USE_CLIPOUT 0

/* Use the MIT_SHM extension to share memory between the program
   and X server for images, if possible. This is disabled by default
   as it will permanently leak system memory if your program crashes
   or otherwise exits without destroying the Image objects.
*/
#define USE_XSHM 0

/* Do we have the X double-buffer extension header file
   <X11/extensions/Xdbe.h>? Turning this on will make the list_visuals
   program produce info about it. (ignored if !USE_X11) */
#define HAVE_XDBE 1

/* Actually try to use the double-buffer extension to make
   double-buffered windows? If this is false then Pixmaps will
   be used for the double buffers. Pixmaps will also be used if
   at runtime fltk detects that double-buffering does not work.
   Some older X servers may claim that double buffering works
   when it does not and you will have to turn this off.
   (ignored if !USE_X11)*/
#define USE_XDBE		HAVE_XDBE

/* Did we detect the X overlay extension on the X server? This
   is currently not used anywhere. */
#define HAVE_OVERLAY		0

/* Use the X overlay extension for MenuWindow and Tooltips. Pretty
   much depreciated, this will add a substantial amount of code
   to manage more than one visual, and has only worked on Irix.
   (ignored if !USE_X11) */
#define USE_OVERLAY		0

/* Use the Xinerama extension? This allows an X server to describe
   multiple monitors. If not used FLTK will assumme the entire X
   display area is one monitor, unless it is very wide, in which
   case FLTK will guess that it is two monitors next to each other.
   (ignored if !USE_X11) */
#define USE_XINERAMA 1

/* Use the Windows NT5.0/Win98 multi-monitor calls? If ths is false it
   will guess 1 or 2 monitors, similar to the X version.
   (ignored if !_WIN32) */
#define USE_MULTIMONITOR	1

/* Use the Windows NT4.0/Win95 "stock brushes" calls. These calls
   modernize the Win32 GDI into a "set current color" type of
   interface and make drawing substantially faster. Without them
   fltk must create/destroy "pens" and "brushes" whenever it wants
   to change the color it is drawing with.
   (ignored if !_WIN32) */
#define USE_STOCK_BRUSH		1

/* Use "X Input Method" for i18n text input. Most familiar as the
   "dead key prefix" code. But Japanese users use this to run far more
   complex programs for selecting glyphs. You can turn it off on the
   (rare) systems that produce an "encoding" that fltk does not
   understand, you will still be able to type many Latin characters
   with fltk's right-Ctrl+letters input method. Turning this off
   should also disable such input methods on other systems, nyi. */
#define USE_XIM			1

/* Do we have a working iconv program?
   (only used with X11 with USE_XIM when the XFree86 Xutf8LookupString
   function is not available). If this is not available than XIM will
   probably not work. */
#define HAVE_ICONV		0

/* Set this to 0 if your system does not have OpenGL. This will
   disable all the code in libfltk2_gl and disable the demo programs
   that use OpenGL. */
#define HAVE_GL 1

/* Do you have the OpenGL Utility Library header file?
   (many broken Mesa RPMs do not...). This is only used by demo
   programs. */
#define HAVE_GL_GLU_H 1

/* Does OpenGL have the ability to draw into the overlay? Currently
   this uses the same code as X for finding the overlay visual,
   however it is possible that future versions of GLX will use
   other methods. */
#define HAVE_GL_OVERLAY		HAVE_OVERLAY

/* Use the OpenGL overlay for Gl_Window::draw_overlay(). Tested only
   on Irix. Overlay is usually colormapped, limiting what OpenGL
   you can draw, and requiring fltk to compile in a lot of colormap
   and visual management code. */
#define USE_GL_OVERLAY	 	0

/* Use the Open GL Extension Wrangler library. This does a far better
   job than fltk/gl.h at fixing OpenGL on windows and allows you
   to call any OpenGL extension and have the program run without
   missing symbols (though you need to check if the extension
   exists before you call it). See http://glew.sourceforge.net */
#define USE_GLEW                0

/* Standard autoconf stuff to figure out readdir() header files.
   (ignored for _WIN32) */
#define HAVE_DIRENT_H 1
/* #undef HAVE_SYS_NDIR_H */
/* #undef HAVE_SYS_DIR_H */
/* #undef HAVE_NDIR_H */
#define HAVE_SCANDIR 1

/* If not set fltk will define it's own versions of these string
   functions. By including the fltk/string.h header file you will
   be able to call these from user programs on any platform. */
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_VSNPRINTF 1
#define HAVE_SNPRINTF 1
#define HAVE_STRCASECMP 1
#define HAVE_STRDUP 1
/* #undef HAVE_STRLCAT */
/* #undef HAVE_STRLCPY */
#define HAVE_STRNCASECMP 1

/* Whether or not select() call has its own header file. */
#define HAVE_SYS_SELECT_H 1

/* Whether or not we have the <sys/stdtypes.h> header file. */
/* #undef HAVE_SYS_STDTYPES_H */

/* Use the poll() call provided on Linux and IRIX instead of select() */
#define USE_POLL		0

/* Do we have various image libraries? */
#define HAVE_LIBPNG 1
#define HAVE_LIBZ 1
#define HAVE_LIBJPEG 1

/* Which header file do we include for libpng? */
/* #undef HAVE_LOCAL_PNG_H */
#define HAVE_PNG_H 1
#define HAVE_LIBPNG_PNG_H 1

/* Which header file do we include for libjpeg? */
/* #undef HAVE_LOCAL_JPEG_H */

/* Do we have POSIX thread library? fltk::lock() will not work unless
   this exists or WIN32 exists. */
#define HAVE_PTHREAD 1
#define HAVE_PTHREAD_H 1

/* Do we have exceptions in the compiler? Only used by test programs. */
#define HAVE_EXCEPTIONS 1

/**** MRS: THESE TWO ARE GOING AWAY! ****/
/* Enables fl_load_plugin() on Unix */
#define HAVE_DLOPEN		0

/**** MRS: These should be run-time checks!!!! ****/
/* The BoXX machines (and possibly other Linux machines) have bugs in
   the X/OpenGL driver for FireGL for handling hardware overlays. Turn
   this on to work around these, but it will cause the overlay to
   blink unnecessarily. Ignored if !USE_GL_OVERLAY. */
#define BOXX_OVERLAY_BUGS	0

/* The SGI 320 NT machines have a bug where the cursor interferes with
   the hardware overlay. Turn this on to work around this. I have been
   told this slows down display quite a bit on some NT machines.
   Ignored if !USE_GL_OVERLAY. */
#define SGI320_BUG		0

/**** MRS: This may go away! ****/
/* Enables Windows GUI emulation: clicking on most widgets (such as
   buttons) moves the focus to them. This makes some useful GUI designs
   impossible. */
#define CLICK_MOVES_FOCUS	0

/* If false or not defined, then if NumLock is off keypad events are
   translated to fltk::LEFT_KEY, etc.  If on, keypad keys are always
   reported as fltk::KEYPAD0, fltk::KEYPAD1, ..., ignoring the setting
   of the NumLock key. This is incredibly useful if you want your
   program to actually use the keypad, and is recommended. */
#define IGNORE_NUMLOCK		1

/* Progressive image draw (as for jpeg and png images)
	sets to 1 for enabling this feature (default)
	sets to 0 for disabling this 
		and favorising fetch() image code  reuse
*/
#define USE_PROGRESSIVE_DRAW 1

/* End of "$Id: configh.in 6480 2008-10-22 06:49:47Z spitzak $". */
