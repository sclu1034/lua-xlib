/** Lua bindings for `libxrandr`.
 *
 * Documentation here will extend the official librarie's documentation, not replace it.
 *
 * Please take a look at the [libxrandr documentation](https://www.x.org/wiki/libraries/libxrandr/) before use,
 * especilly the [protocol specification](http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt)
 * and [Xrandr.h](https://cgit.freedesktop.org/xorg/lib/libXrandr/tree/include/X11/extensions/Xrandr.h).
 *
 * Additionally, `libxrandr` provides man pages locally (packaging depends on the distribution).
 *
 * @module xrandr
 */
#ifndef xrandr_h_INCLUDED
#define xrandr_h_INCLUDED

#include <X11/extensions/Xrandr.h>
#include <lauxlib.h>
#include <lua.h>

#define LUA_XRANDR                  "xlib.xrandr"
#define LUA_XRANDR_SCREEN_RESOURCES "xlib.xrandr.screen_resources"
#define LUA_XRANDR_OUTPUT_INFO      "xlib.xrandr.output_info"
#define LUA_XRANDR_CRTC_INFO        "xlib.xrandr.crtc_info"

// Enums as defined in https://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt

static const char* connection_states[3] = {
    "Connected",
    "Disconnected",
    "UnknownConnection",
};

static const char* subpixel_orders[6] = {
    "SubPixelUnknown",     "SubPixelHorizontalRGB", "SubPixelHorizontalBGR",
    "SubPixelVerticalRGB", "SubPixelVerticalBGR",   "SubPixelNone",
};

static const char* mode_flags[14] = {
    "HSyncPositive", "HSyncNegative",  "VSyncPositive", "VSyncNegative",  "Interlace",
    "DoubleScan",    "CSync",          "CSyncPositive", "CSyncNegative",  "HSkewPresent",
    "BCast",         "PixelMultiplex", "DoubleClock",   "ClockDivideBy2",
};

/** CRTC
 * @table crtc
 * @field[type=number] x
 * @field[type=number] y
 * @field[type=number] width
 * @field[type=number] height
 * @field[type=number] mode
 * @field[type=number] rotation
 * @field[type=number] rotations
 * @field[type=array] outputs
 * @field[type=array] possible
 */

/** Mode flags
 * @table modeflags
 * @field[type=boolean] HSyncPositive
 * @field[type=boolean] HSyncNegative
 * @field[type=boolean] VSyncPositive
 * @field[type=boolean] VSyncNegative
 * @field[type=boolean] Interlace
 * @field[type=boolean] DoubleScan
 * @field[type=boolean] CSync
 * @field[type=boolean] CSyncPositive
 * @field[type=boolean] CSyncNegative
 * @field[type=boolean] HSkewPresent
 * @field[type=boolean] BCast
 * @field[type=boolean] PixelMultiplex
 * @field[type=boolean] DoubleClock
 * @field[type=boolean] ClockDivideBy2
 */

/** Mode Info
 * @table mode

 * @field[type=number] id
 * @field[type=number] name
 * @field[type=number] width
 * @field[type=number] height
 * @field[type=number] dotClock
 * @field[type=number] hSyncStart
 * @field[type=number] hSyncEnd
 * @field[type=number] hTotal
 * @field[type=number] hSkew
 * @field[type=number] vSyncStart
 * @field[type=number] vSyncEnd
 * @field[type=number] vTotal
 * @field[type=modeflags] modeFlags
 */

/** Screen Resources
 * @table XRRScreenResources
 * @field[type=number] timestamp
 * @field[type=number] configTimestamp
 * @field[type=array<number>] crtcs
 * @field[type=array<number>] outputs
 * @field[type=array<mode>] modes
 */
typedef struct {
    XRRScreenResources* inner;
} screen_resources_t;


int screen_resources__gc(lua_State*);
int screen_resources__index(lua_State*);
int xrandr_get_screen_resources(lua_State*);


static const struct luaL_Reg screen_resources_mt[] = {
    {"__gc",     screen_resources__gc   },
    { "__index", screen_resources__index},
    { NULL,      NULL                   }
};


/** Output
 * @table output
 * @field[type=string] name
 * @field[type=number] mm_width
 * @field[type=number] mm_height
 * @field[type=string] connection
 * @field[type=string] subpixel_order
 * @field[type=array<mode>] modes
 * @field[type=array<crtc>] crtcs
 * @field[type=crtc] crtc
 */
typedef struct {
    XRROutputInfo* inner;
} output_info_t;

int output_info__gc(lua_State*);
int output_info__index(lua_State*);
int xrandr_get_output_info(lua_State*);
int xrandr_get_output_primary(lua_State*);
int xrandr_set_output_primary(lua_State*);


static const struct luaL_Reg output_info_mt[] = {
    {"__gc",     output_info__gc   },
    { "__index", output_info__index},
    { NULL,      NULL              }
};


/** CRTC Info
 * @table crtc
 * @field[type=string] name
 * @field[type=number] mm_width
 * @field[type=number] mm_height
 * @field[type=string] connection
 * @field[type=string] subpixel_order
 * @field[type=array<mode>] modes
 */
typedef struct {
    XRRCrtcInfo* inner;
} crtc_info_t;

int crtc_info__gc(lua_State*);
int crtc_info__index(lua_State*);
int xrandr_get_crtc_info(lua_State*);
int xrandr_set_crtc_config(lua_State*);


static const struct luaL_Reg crtc_info_mt[] = {
    {"__gc",     crtc_info__gc   },
    { "__index", crtc_info__index},
    { NULL,      NULL            }
};


static const struct luaL_Reg xrandr_lib[] = {
    {"XRRGetScreenResources", xrandr_get_screen_resources},
    { "XRRGetOutputInfo",     xrandr_get_output_info     },
    { "XRRGetOutputPrimary",  xrandr_get_output_primary  },
    { "XRRGetCrtcInfo",       xrandr_get_crtc_info       },
    { "XRRSetCrtcConfig",     xrandr_set_crtc_config     },
    { "XRRSetOutputPrimary",  xrandr_set_output_primary  },
    { NULL,                   NULL                       }
};

#endif // xrandr_h_INCLUDED

