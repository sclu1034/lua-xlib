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


typedef struct {
    Display* inner;
} display_t;

int display__gc(lua_State*);


int xlib_default_screen(lua_State*);
int xlib_display_name(lua_State*);
int xlib_display_height(lua_State*);
int xlib_display_width(lua_State*);
int xlib_open_display(lua_State*);
int xlib_root_window(lua_State*);
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

