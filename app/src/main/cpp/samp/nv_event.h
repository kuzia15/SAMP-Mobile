//----------------------------------------------------------------------------------
// File:            libs\jni\nv_event\nv_event.h
// Samples Version: Android NVIDIA samples 2 
// Email:           tegradev@nvidia.com
// Forum:           http://developer.nvidia.com/tegra/forums/tegra-forums/android-development
//
// Copyright 2009-2010 NVIDIA® Corporation 
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
//----------------------------------------------------------------------------------
#ifndef _NV_EVENT_H
#define _NV_EVENT_H
#include <sys/types.h>

#include <mutex>

/** @file nv_event.h
Contains a framework for event loop-based applications.  This library replaces
most or all of the normally-required JNI code for Android NDK applications,
presenting the application developer with two callbacks into which they can
write their application.  The framework runs in a natively-created thread,
allowing the application to implement a classic "event loop and render" structure
without having to return to Java code to avoid ANR warnings.  The library 
includes support for input and system event passing as well as JNI initialization
and exposes basic EGL functionality to native as well.  Put together, the library
can form the basis of a simple interactive 3D application.  All applications that
are based on this library must also be subclassed on the Java side from the
provided NvEventQueueActivity.  Additional external documentation on the use of
this library, the related Java code and the tool provided to create a template
application based on the library are provided with the SDK.
@see NvEventQueueActivity
*/
#include <jni.h>
#ifdef ANDROID

#include <jni.h>
typedef jobject NVEventPlatformAppHandle;

#else // unknown platform

typedef void* NVEventPlatformAppHandle;

#endif


enum
{
/** Timeout argument to NVEventGetNextEvent() that indicates the function should 
	block until there is an event pending or the app exits 
	@see NVEventGetNextEvent()
	*/
	NV_EVENT_WAIT_FOREVER = -1
};

/** Event type values
*/
typedef enum NVEventType
{
	/** Key up/down events */
	NV_EVENT_KEY = 1,
	/** Translated character events */
	NV_EVENT_CHAR,
	/** Single-touch pointer events */
	NV_EVENT_TOUCH,
	/** Multi-touch events */
	NV_EVENT_MULTITOUCH,
	/** Accelerometer events */
	NV_EVENT_ACCEL,

	// The events below, up to resume are special, because they relate to the app's status
	// they must be preserved in the framework's internal code. Please update
	// isSacred if this is changed.
	/** Window size changed events */
	NV_EVENT_WINDOW_SIZE,
	/** Quit request events */
	NV_EVENT_QUIT,
	/** the app should release all graphics resources and block waiting for
	 * RESTART or QUIT after receiving this event */
	NV_EVENT_PAUSE,
	/** the app should block waiting for RESUME OR QUIT after doing all non-graphics
	 * initialization and initalize graphics resources after receiving RESTART */
	NV_EVENT_RESUME,

	/* a dummy enum value used to compute num_events */
	NV_EVENT_NUM_EVENT_DUMMY_DONTUSEME,
	/* total number of events */
	NV_EVENT_NUM_EVENTS = NV_EVENT_NUM_EVENT_DUMMY_DONTUSEME - 1,
	NV_EVENT_FORCE_32BITS = 0x7fffffff
} NVEventType;

/** Touch event actions
*/
typedef enum NVTouchEventType
{
	/** Pointer has just left the screen */
	NV_TOUCHACTION_UP,
	/** Pointer has just gone down onto the screen */
	NV_TOUCHACTION_DOWN,
	/** Pointer is moving on the screen */
	NV_TOUCHACTION_MOVE,
	NV_TOUCHACTION_FORCE_32BITS = 0x7fffffff
} NVTouchEventType;

/** Multitouch event flags 
*/
typedef enum NVMultiTouchEventType
{
	/** Indicated pointers are leaving the screen */
	NV_MULTITOUCH_UP    =   0x00000001,
	/** Indicated pointers have just touched the screen */
	NV_MULTITOUCH_DOWN  =   0x00000002,
	/** Indicated pointers are moving on the screen */
	NV_MULTITOUCH_MOVE  =   0x00000003,
	/** Indicated pointers have halted the current gesture
		app should cancel any actions implied by the gesture */
	NV_MULTITOUCH_CANCEL =  0x00000004,
	/** Mask to be AND'ed with the flag value 
		to get the active pointer bits */
	NV_MULTITOUCH_POINTER_MASK =  0x0000ff00,
	/** Number of bits to right-shift the masked value 
		to get the active pointer bits */
	NV_MULTITOUCH_POINTER_SHIFT = 0x00000008,
	/** Mask to be AND'ed with the flag value 
		to get the event action */
	NV_MULTITOUCH_ACTION_MASK =   0x000000ff,
	NV_MULTITOUCH_FORCE_32BITS = 0x7fffffff
} NVMultiTouchEventType;

/** Key event types
*/
typedef enum NVKeyEventType
{
	/** Key has just been pressed (no repeats) */
	NV_KEYACTION_UP,
	/** Key has just been release */
	NV_KEYACTION_DOWN,
	NV_KEYACTION_FORCE_32BITS = 0x7fffffff
} NVKeyEventType;

/** Key event key codes
*/
typedef enum NVKeyCode
{
	NV_KEYCODE_NULL = 0,
	NV_KEYCODE_BACK,
	NV_KEYCODE_TAB,
	NV_KEYCODE_ENTER,
	NV_KEYCODE_DEL,
	NV_KEYCODE_SPACE,
	NV_KEYCODE_ENDCALL,
	NV_KEYCODE_HOME,

	NV_KEYCODE_STAR,
	NV_KEYCODE_PLUS,
	NV_KEYCODE_MINUS,
	NV_KEYCODE_NUM,

	NV_KEYCODE_DPAD_LEFT,
	NV_KEYCODE_DPAD_UP,
	NV_KEYCODE_DPAD_RIGHT,
	NV_KEYCODE_DPAD_DOWN,

	NV_KEYCODE_0,
	NV_KEYCODE_1,
	NV_KEYCODE_2,
	NV_KEYCODE_3,
	NV_KEYCODE_4,
	NV_KEYCODE_5,
	NV_KEYCODE_6,
	NV_KEYCODE_7,
	NV_KEYCODE_8,
	NV_KEYCODE_9,

	NV_KEYCODE_A,
	NV_KEYCODE_B,
	NV_KEYCODE_C,
	NV_KEYCODE_D,
	NV_KEYCODE_E,
	NV_KEYCODE_F,
	NV_KEYCODE_G,
	NV_KEYCODE_H,
	NV_KEYCODE_I,
	NV_KEYCODE_J,
	NV_KEYCODE_K,
	NV_KEYCODE_L,
	NV_KEYCODE_M,
	NV_KEYCODE_N,
	NV_KEYCODE_O,
	NV_KEYCODE_P,
	NV_KEYCODE_Q,
	NV_KEYCODE_R,
	NV_KEYCODE_S,
	NV_KEYCODE_T,
	NV_KEYCODE_U,
	NV_KEYCODE_V,
	NV_KEYCODE_W,
	NV_KEYCODE_X,
	NV_KEYCODE_Y,
	NV_KEYCODE_Z,

	NV_KEYCODE_ALT_LEFT,
	NV_KEYCODE_ALT_RIGHT,

	NV_KEYCODE_SHIFT_LEFT,
	NV_KEYCODE_SHIFT_RIGHT,

	NV_KEYCODE_APOSTROPHE,
	NV_KEYCODE_SEMICOLON,
	NV_KEYCODE_EQUALS,
	NV_KEYCODE_COMMA,
	NV_KEYCODE_PERIOD,
	NV_KEYCODE_SLASH,
	NV_KEYCODE_GRAVE,
	NV_KEYCODE_BACKSLASH,

	NV_KEYCODE_LEFT_BRACKET,
	NV_KEYCODE_RIGHT_BRACKET,

	NV_KEYCODE_FORCE_32BIT = 0x7fffffff
} NVKeyCode;

/** Single-touch event data
*/
typedef struct NVEventTouch
{
	/** The action code */
    NVTouchEventType   m_action;
	/** The window-relative X position (in pixels) */
    float   m_x;
	/** The window-relative Y position (in pixels) */
    float   m_y;
} NVEventTouch;

/** Multi-touch event data
*/
typedef struct NVEventMultiTouch
{
	/** The action flags */
    NVMultiTouchEventType   m_action;
	/** The window-relative X position of the first pointer (in pixels) 
		only valid if bit 0 of the pointer bits is set */
    float   m_x1;
	/** The window-relative Y position of the first pointer (in pixels) 
		only valid if bit 0 of the pointer bits is set */
    float   m_y1;
	/** The window-relative X position of the second pointer (in pixels) 
		only valid if bit 1 of the pointer bits is set */
    float   m_x2;
	/** The window-relative Y position of the second pointer (in pixels) 
		only valid if bit 1 of the pointer bits is set */
    float   m_y2;

	/** The window-relative X position of the first pointer (in pixels)
		only valid if bit 2 of the pointer bits is set */
	float   m_x3;
	/** The window-relative Y position of the first pointer (in pixels)
		only valid if bit 2 of the pointer bits is set */
	float   m_y3;
	/** The window-relative X position of the second pointer (in pixels)
		only valid if bit 3 of the pointer bits is set */
	float   m_x4;
	/** The window-relative Y position of the second pointer (in pixels)
		only valid if bit 3 of the pointer bits is set */
	float   m_y4;
} NVEventMultiTouch;

/** Key down/up event data
*/
typedef struct NVEventKey
{
	/** The action flags */
	NVKeyEventType m_action;
	/** The code of the key pressed or released */
	NVKeyCode m_code;
} NVEventKey;

/** Translated character event data
*/
typedef struct NVEventChar
{
	/** The UNICODE character represented */
	int32_t m_unichar;
} NVEventChar;

/** Accelerometer event data
*/
typedef struct NVEventAccel
{
	/** Signed X magnitude of the force vector */
    float   m_x;
	/** Signed Y magnitude of the force vector */
    float   m_y;
	/** Signed Z magnitude of the force vector */
    float   m_z;
} NVEventAccel;

/** Windows size change event data
*/
typedef struct NVEventWindowSize
{
	/** New window client area width (in pixels) */
	int32_t m_w;
	/** New window client area height (in pixels) */
	int32_t m_h;
} NVEventWindowSize;

/** All-encompassing event structure 
*/
typedef struct NVEvent 
{
	/** The type of the event, which also indicates which m_data union holds the data */
    NVEventType m_type;
	/** Union containing all possible event type data */
    union NVEventData 
	{
		/** Data for single-touch events */
		NVEventTouch m_touch;
		/** Data for multi-touch events */
		NVEventMultiTouch m_multi;
		/** Data for key up/down events */
        NVEventKey m_key;
		/** Data for charcter events */
		NVEventChar m_char;
		/** Data for accelerometer events */
        NVEventAccel m_accel;
		/** Data for window size events */
		NVEventWindowSize m_size;
    } m_data;
} NVEvent;

bool NVEventGetNextEvent(NVEvent* ev);

#endif
