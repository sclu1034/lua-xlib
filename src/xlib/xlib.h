/**
 * @module xlib
 */
#ifndef xlib_h_INCLUDED
#define xlib_h_INCLUDED

#include "lua_util.h"

#include <X11/Xlib.h>
#include <lua.h>


#define LUA_XLIB         "xlib"
#define LUA_XLIB_DISPLAY "xlib.display"


/**
 * @table Display
 */
typedef struct {
    Display* inner;
} display_t;

int display__gc(lua_State*);


/** Returns the default screen for the given display.
 *
 * @function DefaultScreen
 * @tparam Display display
 * @treturn number
 */
int xlib_default_screen(lua_State*);

/** Returns the display name that @{XOpenDisplay} would attempt to use.
 *
 * If the argument is `nil`, XLib will check the `DISPLAY` environment variable.
 *
 * The main use case of this function is to report the name that @{XOpenDisplay} attempted
 * when the initial connection attempt failed.
 *
 * @function XDisplayName
 * @tparam[opt] string name
 * @treturn string
 */
int xlib_display_name(lua_State*);

/** Returns the height of the given screen in pixels.
 *
 * @function DisplayHeight
 * @tparam Display display
 * @tparam number screen
 * @treturn number
 */
int xlib_display_height(lua_State*);

/** Returns the width of the given screen in pixels.
 *
 * @function DisplayHeight
 * @tparam Display display
 * @tparam number screen
 * @treturn number
 */
int xlib_display_width(lua_State*);

/** Open a connection to the given display.
 *
 * The connection is closed automatically when the returned handler is garbage collected
 * (i.e. when it goes out of scope).
 *
 * @function XOpenDisplay
 * @tparam[opt] string display_name The connection string. When `nil`, XLib will fall back to the value of
 *  the `DISPLAY` environment variable.
 * @treturn Display
 */
int xlib_open_display(lua_State*);

/** Returns the root window for the given display.
 *
 * @function RootWindow
 * @tparam Display display
 * @treturn number
 */
int xlib_root_window(lua_State*);

/** Returns the number of screens for the given display.
 *
 * @function ScreenCount
 * @tparam Display display
 * @treturn number
 */
int xlib_screen_count(lua_State*);


static const struct luaL_Reg display_mt[] = {
    {"__gc", display__gc},
    { NULL,  NULL       }
};

static const struct luaL_Reg xlib_lib[] = {
    {"DefaultScreen",  xlib_default_screen},
    { "DisplayHeight", xlib_display_height},
    { "DisplayWidth",  xlib_display_width },
    { "RootWindow",    xlib_root_window   },
    { "ScreenCount",   xlib_screen_count  },
    { "XDisplayName",  xlib_display_name  },
    { "XOpenDisplay",  xlib_open_display  },
    { NULL,            NULL               }
};

#endif // xlib_h_INCLUDED

