#pragma once

typedef unsigned int		SNOW_KEY;

namespace SnowSim {
	namespace Utilities {
		namespace Keys {
			// From: FLTK

			/*! Values returned by event_key(), passed to event_key_state() and
			  get_key_state(), and used for the low 16 bits of add_shortcut().

			  The actual values returned are based on X11 keysym values, though
			  fltk always returns "unshifted" values much like Windows does. A
			  given key always returns the same value no matter what shift keys
			  are held down. Use event_text() to see the results of any shift
			  keys.

			  The lowercase letters 'a' through 'z' and the ascii symbols
			  '`', '-', '=', '[', ']', '\\', ',', '.', '/', ';', '\'' and space
			  are used to identify the keys in the main keyboard.

			  On X systems unrecognized keys are returned unchanged as their
			  X keysym value. If they have no keysym it uses the scan code
			  or'd with 0x8000, this is what all those blue buttons on a
			  Microsoft keyboard will do. I don't know how to get those
			  buttons on Windows.
			*/
			enum {
			  LeftButton	= 1,		/*!< PUSH/RELEASE set event_key to this */
			  MiddleButton	= 2,		/*!< PUSH/RELEASE set event_key to this */
			  RightButton	= 3,		/*!< PUSH/RELEASE set event_key to this */
			  SpaceKey	= 32,		/*!< Same as ' ' or 32 */
			  // 'a'-'z', and all punctuation go here in numerical order
			  BackSpaceKey	= 0xff08,	/*!< Backspace */
			  TabKey	= 0xff09,	/*!< Tab */
			  ClearKey	= 0xff0b,	/*!< On some systems with NumLock off '5' produces this */
			  ReturnKey	= 0xff0d,	/*!< Main Enter key, Windows and X documentation calls this "Return" */
			  PauseKey	= 0xff13,	/*!< Pause + Break button */
			  ScrollLockKey	= 0xff14,	/*!< Scroll Lock button */
			  EscapeKey	= 0xff1b,	/*!< Esc */
			  HomeKey	= 0xff50,	/*!< Home */
			  LeftKey	= 0xff51,	/*!< Left */
			  UpKey		= 0xff52,	/*!< Up arrow */
			  RightKey	= 0xff53,	/*!< Right arrow */
			  DownKey	= 0xff54,	/*!< Down arrow */
			  PageUpKey	= 0xff55,	/*!< Page Up */
			  PageDownKey	= 0xff56,	/*!< Page Down */
			  EndKey	= 0xff57,	/*!< End */
			  PrintKey	= 0xff61,	/*!< Print Scrn key + SysRq key */
			  InsertKey	= 0xff63,	/*!< Insert */
			  MenuKey	= 0xff67,	/*!< Key in lower-right with picture of popup menu */
			  HelpKey	= 0xff68,	/*!< Help key on Macintosh keyboards */
			  NumLockKey	= 0xff7f,	/*!< NumLock key */
			  Keypad	= 0xff80,	/*!< Add ASCII to get keypad keys */
			  KeypadEnter	= Keypad+'\r',	/*!< Keypad+'\\r' */
			  MultiplyKey	= Keypad+'*',	/*!< Keypad+'*' */
			  AddKey	= Keypad+'+',	/*!< Keypad+'+' */
			  SubtractKey	= Keypad+'-',	/*!< Keypad+'-' */
			  DecimalKey	= Keypad+'.',	/*!< Keypad+'.' */
			  DivideKey	= Keypad+'/',	/*!< Keypad+'/' */
			  Keypad0	= Keypad+'0',	/*!< Keypad+'0' */
			  Keypad1	= Keypad+'1',	/*!< Keypad+'1' */
			  Keypad2	= Keypad+'2',	/*!< Keypad+'2' */
			  Keypad3	= Keypad+'3',	/*!< Keypad+'3' */
			  Keypad4	= Keypad+'4',	/*!< Keypad+'4' */
			  Keypad5	= Keypad+'5',	/*!< Keypad+'5' */
			  Keypad6	= Keypad+'6',	/*!< Keypad+'6' */
			  Keypad7	= Keypad+'7',	/*!< Keypad+'7' */
			  Keypad8	= Keypad+'8',	/*!< Keypad+'8' */
			  Keypad9	= Keypad+'9',	/*!< Keypad+'9' */
			  KeypadLast	= 0xffbd,	/*!< Keypad+'=', largest legal keypad key */
			  F0Key		= 0xffbd,	/*!< Add a number to get function key */
			  F1Key		= F0Key+1,	/*!< F0Key+1 */
			  F2Key		= F0Key+2,	/*!< F0Key+2 */
			  F3Key		= F0Key+3,	/*!< F0Key+3 */
			  F4Key		= F0Key+4,	/*!< F0Key+4 */
			  F5Key		= F0Key+5,	/*!< F0Key+5 */
			  F6Key		= F0Key+6,	/*!< F0Key+6 */
			  F7Key		= F0Key+7,	/*!< F0Key+7 */
			  F8Key		= F0Key+8,	/*!< F0Key+8 */
			  F9Key		= F0Key+9,	/*!< F0Key+9 */
			  F10Key	= F0Key+10,	/*!< F0Key+10 */
			  F11Key	= F0Key+11,	/*!< F0Key+11 */
			  F12Key	= F0Key+12,	/*!< F0Key+12 */
			  // use F0Key+n to get function key n
			  LastFunctionKey = F0Key+35,	/*!< F0Key+35, largest legal function key */
			  LeftShiftKey	= 0xffe1,	/*!< Left-hand Shift */
			  RightShiftKey	= 0xffe2,	/*!< Right-hand Shift */
			  LeftCtrlKey	= 0xffe3,	/*!< Left-hand Ctrl */
			  RightCtrlKey	= 0xffe4,	/*!< Right-hand Ctrl */
			  CapsLockKey	= 0xffe5,	/*!< Caps Lock */
			  LeftMetaKey	= 0xffe7,	/*!< The left "Windows" or "Apple" key */
			  RightMetaKey	= 0xffe8,	/*!< The right "Windows" or "Apple" key */
			  LeftAltKey	= 0xffe9,	/*!< Left-hand Alt (option on Mac) */
			  RightAltKey	= 0xffea,	/*!< Right-hand Alt (option on Mac) */
			  DeleteKey	= 0xffff,	/*!< Delete */
			#if  defined(__APPLE__)
			  LeftAccKey	= LeftCtrlKey,
			  RightAccKey	= RightCtrlKey,
			  LeftCmdKey	= LeftMetaKey,
			  RightCmdKey	= RightMetaKey
			#else
			  LeftAccKey	= LeftAltKey,
			  RightAccKey	= RightAltKey,
			  LeftCmdKey	= LeftCtrlKey,
			  RightCmdKey	= RightCtrlKey
			#endif
			};
		}
	}
}
