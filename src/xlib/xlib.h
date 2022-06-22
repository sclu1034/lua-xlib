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
    Bool closed;
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

/** Opens a connection to the given display.
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

/** Closes a connection.
 *
 * @function XCloseDisplay
 * @tparam Display display
 */
int xlib_close_display(lua_State*);

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

/** Locks the given display connection for use with the current thread only.
 *
 * Other threads attempting to use the same connection will block until it is unlocked with @{XUnlockDisplay}.
 * Calls to this function may be nested.
 *
 * @function XLockDisplay
 * @tparam Display display
 */
int xlib_lock_display(lua_State*);

/** Releases a lock from the given display connection.
 *
 * @function XUnlockDisplay
 * @tparam Display display
 */
int xlib_unlock_display(lua_State*);

/** Returns an atom identifier for the given name.
 *
 * @function XInternAtom
 * @tparam Display display
 * @tparam string name
 * @tparam booolean only_if_exists
 * @treturn number
 */
int xlib_intern_atom(lua_State*);

/** Returns the atom identifier for a list of names.
 *
 * @function XInternAtoms
 * @tparam Display display
 * @tparam table names A list of strings.
 * @tparam boolean only_if_exists
 * @treturn number An Xlib `Status`.
 * @treturn table
 */
int xlib_intern_atoms(lua_State*);

/** Returns the name associated with the given atom.
 *
 * @function XGetAtomName
 * @tparam Display display
 * @tparam number atom An X11 `Atom`.
 * @treturn string
 */
int xlib_get_atom_name(lua_State*);

/** Returns the names associated with the list of atoms.
 *
 * @function XGetAtomNames
 * @tparam Display display
 * @tparam table atoms A list of numbers.
 * @treturn table
 */
int xlib_get_atom_names(lua_State*);


static const struct luaL_Reg display_mt[] = {
    {"__gc", display__gc},
    { NULL,  NULL       }
};

static const struct luaL_Reg xlib_lib[] = {
    {"DefaultScreen",   xlib_default_screen},
    { "DisplayHeight",  xlib_display_height},
    { "DisplayWidth",   xlib_display_width },
    { "RootWindow",     xlib_root_window   },
    { "ScreenCount",    xlib_screen_count  },
    { "XDisplayName",   xlib_display_name  },
    { "XOpenDisplay",   xlib_open_display  },
    { "XLockDisplay",   xlib_lock_display  },
    { "XCloseDisplay",  xlib_close_display },
    { "XUnlockDisplay", xlib_unlock_display},
    { "XInternAtom",    xlib_intern_atom   },
    { "XInternAtoms",   xlib_intern_atoms  },
    { "XGetAtomName",   xlib_get_atom_name },
    { "XGetAtomNames",  xlib_get_atom_names},
    { NULL,             NULL               }
};

#endif // xlib_h_INCLUDED

