/** Lua bindings for `libxrandr`.
 *
 * Documentation here will extend the official library's documentation, not replace it.
 * Please take a look at the [libxrandr documentation](https://www.x.org/wiki/libraries/libxrandr/) before use,
 * especially the [protocol specification](http://cgit.freedesktop.org/xorg/proto/randrproto/tree/randrproto.txt)
 * and [Xrandr.h](https://cgit.freedesktop.org/xorg/lib/libXrandr/tree/include/X11/extensions/Xrandr.h).
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


/**
 * This table maps the bitfield to individual booleans.
 *
 * @table XRRModeFlags
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

/**
 * Contrary to the other types in this module, this is provided as an actual Lua table, rather than userdata.
 *
 * @table XRRMode
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
 * @field[type=XRRModeFlags] modeFlags
 */

/**
 * @table XRRScreenResources
 * @field[type=number] timestamp
 * @field[type=number] configTimestamp
 * @field[type=table<number>] crtcs
 * @field[type=table<number>] outputs
 * @field[type=table<XRRMode>] modes
 */
typedef struct {
    XRRScreenResources* inner;
} screen_resources_t;


int screen_resources__gc(lua_State*);
int screen_resources__index(lua_State*);

/** Queries the current @{XRRScreenResources}.
 *
 * @function XRRGetScreenResources
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window The XID of the window to query.
 * @treturn XRRScreenResources
 * @usage
 * local display = xlib.xlib.XOpenDisplay()
 * local screen = xlib.DefaultScreen(display)
 * local root = xlib.RootWindow(display, screen)
 * local res = xrandr.XRRGetScreenResources(display, root)
 */
int xrandr_get_screen_resources(lua_State*);


static const struct luaL_Reg screen_resources_mt[] = {
    {"__gc",     screen_resources__gc   },
    { "__index", screen_resources__index},
    { NULL,      NULL                   }
};


/**
 * @table XRROutputInfo
 * @field[type=string] name
 * @field[type=number] mm_width
 * @field[type=number] mm_height
 * @field[type=string] connection
 * @field[type=string] subpixel_order
 * @field[type=table<number>] modes List of IDs. These map to the mode list in @{XRRScreenResources}.
 * @field[type=table<number>] crtcs List of XIDs
 * @field[type=number] crtc
 */
typedef struct {
    XRROutputInfo* inner;
} output_info_t;

int output_info__gc(lua_State*);
int output_info__index(lua_State*);

/** Returns information about the given output.
 *
 * This returns a userdatum that wrapping the output info data. Table index operations
 * are supported to retrieve the fields described in @{XRROutputInfo}.
 *
 * @function XRRGetOutputInfo
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam screen_resources resources
 * @tparam number output The XID of the output to fetch information about.
 * @treturn XRROutputInfo
 * @usage
 * local res = xrandr.XRRGetScreenResources(display, root)
 * for _, output in ipairs(res.outputs) do
 *     local info = xrandr.XRRGetOutputInfo(display, res, output)
 *     printf("%s %s", info.name, info.connection)
 * end
 */
int xrandr_get_output_info(lua_State*);

/** Returns the primary output for the given window.
 *
 * @function XRRGetOutputPrimary
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window The XID of the window to query.
 * @treturn number The XID of the primary output.
 * @usage
 * local res = xrandr.XRRGetScreenResources(display, root)
 * local primary = xrandr.XRRGetOutputPrimary(display, root)
 * for _, output in ipairs(res.outputs) do
 *     if output == primary then
 *       local info = xrandr.XRRGetOutputInfo(display, res, output)
 *       printf("Primary: %s", info.name)
 *     end
 * end
 */
int xrandr_get_output_primary(lua_State*);

/** Sets the given output as primary for the window.
 *
 * @function XRRSetOutputPrimary
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window The XID of the window.
 * @tparam number output The XID of the output.
 */
int xrandr_set_output_primary(lua_State*);


static const struct luaL_Reg output_info_mt[] = {
    {"__gc",     output_info__gc   },
    { "__index", output_info__index},
    { NULL,      NULL              }
};


/**
 * @table XRRCrtcInfo
 * @field[type=string] name
 * @field[type=number] mm_width
 * @field[type=number] mm_height
 * @field[type=string] connection
 * @field[type=string] subpixel_order
 * @field[type=table<number>] modes
 */
typedef struct {
    XRRCrtcInfo* inner;
} crtc_info_t;

int crtc_info__gc(lua_State*);
int crtc_info__index(lua_State*);

/** Returns information about the given CRTC.
 *
 * This returns a userdatum that wrapping the CRTC info data. Table index operations
 * are supported to retrieve the fields described in @{XRRCrtcInfo}.
 *
 * @function XRRGetCrtcInfo
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam screen_resources resources
 * @tparam number crtc The XID of the CRTC to fetch information about.
 * @treturn XRRCrtcInfo
 * @usage
 * local res = xrandr.XRRGetScreenResources(display, root)
 * local info = xrandr.XRRGetCrtcInfo(display, res, res.crtcs[1])
 * local mode = find_by_id(res.modes, info.mode)
 * require("pl.pretty").dump(mode)
 */
int xrandr_get_crtc_info(lua_State*);

/** Sets CRTC configuration.
 *
 * @function XRRSetCrtcConfig
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam screen_resources resources
 * @tparam number crtc The XID of the CRTC to change.
 * @tparam number timestamp
 * @tparam number x
 * @tparam number y
 * @tparam number|nil mode The id of the @{XRRMode} to set for this CRTC. If `0` or `nil` is passed,
 *   the CRTC will be disabled.
 * @tparam number rotation
 * @tparam table<number> outputs The list of output XIDs to assign to this CRTC.
 * @treturn number An X11 `Status`.
 * @usage
 * local res = xrandr.XRRGetScreenResources(display, root)
 * local info = xrandr.XRRGetCrtcInfo(display, res, res.crtcs[1])
 * local mode = find_by_id(res.modes, info.mode)
 * local status = xrandr.XRRSetCrtcConfig(
 *     display,
 *     res,
 *     res.crtcs[1],
 *     info.timestamp,
 *     info.x,
 *     info.y,
 *     res.modes[2].id,
 *     info.rotation,
 *     info.outputs
 * )
 */
int xrandr_set_crtc_config(lua_State*);


static const struct luaL_Reg crtc_info_mt[] = {
    {"__gc",     crtc_info__gc   },
    { "__index", crtc_info__index},
    { NULL,      NULL            }
};


/** Queries the maximum supported extension version from the server.
 *
 * The return values of the major and minor extension version are only defined when
 * the status value is `1`. In all other cases, using the version numbers is considered
 * undefined behaviour.
 *
 * @function XRRQueryVersion
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @treturn number The request status.
 * @treturn[opt] number The major extension version.
 * @treturn[opt] number The minor extension version.
 */
int xrandr_query_version(lua_State*);


static const struct luaL_Reg xrandr_lib[] = {
    {"XRRQueryVersion",        xrandr_query_version       },
    { "XRRGetScreenResources", xrandr_get_screen_resources},
    { "XRRGetOutputInfo",      xrandr_get_output_info     },
    { "XRRGetOutputPrimary",   xrandr_get_output_primary  },
    { "XRRGetCrtcInfo",        xrandr_get_crtc_info       },
    { "XRRSetCrtcConfig",      xrandr_set_crtc_config     },
    { "XRRSetOutputPrimary",   xrandr_set_output_primary  },
    { NULL,                    NULL                       }
};

#endif // xrandr_h_INCLUDED

