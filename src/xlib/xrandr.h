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

#include "lua_util.h"

#include <X11/extensions/Xrandr.h>
#include <lauxlib.h>
#include <lua.h>

#define LUA_XRANDR                  "xlib.xrandr"
#define LUA_XRANDR_SCREEN_RESOURCES "xlib.xrandr.screen_resources"
#define LUA_XRANDR_OUTPUT_INFO      "xlib.xrandr.output_info"
#define LUA_XRANDR_CRTC_INFO        "xlib.xrandr.crtc_info"
#define LUA_XRANDR_SCREEN_CONFIG    "xlib.xrandr.screen_configuration"

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


/** An enum of well-known output property names.
 *
 * @table RR_OUTPUT
 * @field[type=string] BACKLIGHT `Backlight`
 * @field[type=string] RANDR_EDID `EDID`
 * @field[type=string] SIGNAL_FORMAT `SignalFormat`
 * @field[type=string] SIGNAL_PROPERTIES `SignalProperties`
 * @field[type=string] CONNECTOR_TYPE `ConnectorType`
 * @field[type=string] CONNECTOR_NUMBER `ConnectorNumber`
 * @field[type=string] COMPATIBILITY_LIST `CompatibilityList`
 * @field[type=string] CLONE_LIST `CloneList`
 * @field[type=string] BORDER `Border`
 * @field[type=string] BORDER_DIMENSIONS `BorderDimensions`
 * @field[type=string] GUID `GUID`
 * @field[type=string] RANDR_TILE `TILE`
 * @field[type=string] NON_DESKTOP `non-desktop`
 */


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

/** Returns the base event codes.
 *
 * @function XRRQueryExtension
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @treturn boolean
 * @treturn[opt] number The event base code
 * @treturn[opt] number The error base code
 */
int xrandr_query_extension(lua_State*);


/**
 * This only functions as a marker. Values need to be retrieved through the `XRRConfig*` functions, such as
 * @{XRRConfigCurrentRate}.
 *
 * @table XRRScreenConfiguration
 */
typedef struct {
    XRRScreenConfiguration* inner;
} screen_configuration_t;

int screen_configuration__gc(lua_State*);

static const struct luaL_Reg screen_config_mt[] = {
    {"__gc", screen_configuration__gc},
    { NULL,  NULL                    }
};

/** Queries the screen configuration.
 *
 * @function XRRGetScreenInfo
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window
 * @treturn XRRScreenConfiguration
 * @usage
 * local display = xlib.xlib.XOpenDisplay()
 * local screen = xlib.DefaultScreen(display)
 * local root = xlib.RootWindow(display, screen)
 * local info = xrandr.XRRGetScreenInfo(display, root)
 */
int xrandr_get_screen_info(lua_State*);

/** Updates the screen configuration.
 *
 * @function XRRSetScreenConfig
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam XRRScreenConfiguration config
 * @tparam Drawable draw
 * @tparam integer size_index
 * @tparam number rotation
 * @tparam number timestamp
 * @treturn number
 */
int xrandr_set_screen_config(lua_State*);

/** Updates the screen configuration.
 *
 * @function XRRSetScreenConfigAndRate
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam XRRScreenConfiguration config
 * @tparam Drawable draw
 * @tparam integer size_index
 * @tparam number rotation
 * @tparam integer rate
 * @tparam number timestamp
 * @treturn number
 */
int xrandr_set_screen_config_rate(lua_State*);


/** Gets the rotations from a screen config.
 *
 * @function XRRConfigRotations
 * @tparam XRRScreenConfiguration config
 * @treturn number `rotations`
 * @treturn number `current_rotation`
 */
int xrandr_config_rotations(lua_State*);

/** Gets the timestamps from a screen config.
 *
 * @function XRRConfigTimes
 * @tparam XRRScreenConfiguration config
 * @treturn number `timestamp`
 * @treturn number `config_timestamp`
 */
int xrandr_config_times(lua_State*);

/** Gets the list of sizes from a screen config.
 *
 * @function XRRConfigSizes
 * @tparam XRRScreenConfiguration config
 * @treturn table
 */
int xrandr_config_sizes(lua_State*);

/** Gets the list or refresh rates from a screen config.
 *
 * @function XRRConfigRates
 * @tparam XRRScreenConfiguration config
 * @tparam number size_index Index of the current screen size from @{XRRConfigCurrentConfiguration}.
 * @treturn table
 */
int xrandr_config_rates(lua_State*);

/** Gets the current size index and rotation from a screen config.
 *
 * @function XRRConfigCurrentConfiguration
 * @tparam XRRScreenConfiguration config
 * @treturn number Index into the return value of @{XRRConfigSizes}
 * @treturn number Current rotation
 */
int xrandr_config_current_configuration(lua_State*);

/** Gets the current refresh rate from a screen config.
 *
 * @function XRRConfigCurrentRate
 * @tparam XRRScreenConfiguration config
 * @treturn number
 */
int xrandr_config_current_rate(lua_State*);

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


/*
 * Internal representation of the property data.
 */
typedef struct {
    int type;
    void* data;
} output_property_data_t;

#define LUA_TINTEGER


/** Returns the list of properties on the given output.
 *
 * This returns a list of `Atom`s (mapped to simple integer numbers).
 * Metadata of the property can be queried with using @{XRRQueryOutputProperty},
 * values with @{XRRGetOutputProperty}.
 *
 * @function XRRListOutputProperties
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @treturn number
 */
int xrandr_list_output_properties(lua_State*);

/**
 * If `pending == true`, changes to the property via @{XRRChangeOutputProperty} will be queued
 * until the next call to @{XRRSetCrtcConfig}.
 *
 * If `range == true`, `values` will contain exactly two entries, that denote the minimum and maximum
 * values for the property. Otherwise `values` is a list of valid values.
 *
 * If `immutable == true`, the property cannot be changed by the client.
 *
 * @table XRRPropertyInfo
 * @field[type=bool] pending
 * @field[type=bool] range
 * @field[type=bool] immutable
 * @field[type=table] values
 */

/** Returns metadata about an output property.
 *
 * @function XRRQueryOutputProperty
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @tparam number property An X11 `Atom`.
 * @treturn XRRPropertyInfo
 */
int xrandr_query_output_property(lua_State*);

/** Sets the metadata for an output property.
 *
 * See @{XRRPropertyInfo} for the semantics of the various metadata fields.
 *
 * @function XRRConfigureOutputProperty
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @tparam number property An X11 `Atom`.
 * @tparam boolean pending
 * @tparam boolean range
 * @tparam table values
 */
int xrandr_configure_output_property(lua_State*);

/** Changes the value of an output property.
 *
 * The value has to match with the metadata from @{XRRQueryOutputProperty}.
 *
 * For now, only @{string} values are supported, and they are treated as raw, unsigned byte buffers.
 *
 * If "append" or "prepend" modes are chosen, the types of the existing and new values must match.
 * For undefined properties, all three modes work the same.
 *
 * The maximum allowed size of the property data depends on the server implementation,
 * and may change dynamically at runtime or between resets.
 * The lifetime of a property is tied to the output and server, not the client that set it.
 *
 * @function XRRChangeOutputProperty
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @tparam number property An X11 `Atom`.
 * @tparam number type An X11 `Atom`.
 * @tparam number|nil mode If `1`, prepend data. If `2`, append data. Otherwise replace data.
 * @tparam string data
 */
int xrandr_change_output_property(lua_State*);

/** Returns the value of an output property.
 *
 * As with @{XRRChangeOutputProperty}, only properties of type @{string}
 * (which maps to Xorg's `XA_STRING` from `X11/Xatom.h`) are currently supported. The `req_type` property
 * is just a placeholder because of that.
 *
 * If there is no such property, the function will return nothing.
 *
 * For properties of unknown length, first call this function with `offset == 0`, `length == 0`,
 * and the second return value will report the full length of the value. Then call the function
 * a second time with the desired offset and length.
 *
 * @function XRRGetOutputProperty
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @tparam number property An X11 `Atom`.
 * @tparam number offset The offset at which to start reading the return value.
 * @tparam number length The amount of bytes (regardless of the element size) to read into the return value.
 * @tparam boolean delete If `true`, delete the property after reading.
 * @tparam boolean pending If `true` and there is a pending change for the property, return that change
 *  instead of the current value.
 * @tparam[opt] number req_type An X11 `Atom`. Currently ignored and hardcoded to XLib's `XA_STRING`.
 * @treturn string The data.
 * @treturn number The number of elements in the value, as passed when setting the property
 *  (see @{XRRChangeOutputProperty}).
 * @treturn number The element size of the byte array as passed when setting the property. One of `8`, `16`, `32`.
 * @treturn number The number of bytes (regardless of the element size) left in the value.
 * @usage
 * -- Check the length
 * local _, length = xrandr.XRRGetOutputProperty(display, output, atom, 0, 0)
 * local value, nitems, format = xrandr.XRRGetOutputProperty(display, output, atom, 0, length)
 * print(value) -- may be arbitrary, non-printable bytes. If so, use `nitems` and `format` to interpret it
 */
int xrandr_get_output_property(lua_State*);

/** Deletes the property from the given output.
 *
 * @function XRRDeleteOutputProperty
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number output The XID of the output.
 * @tparam number property An X11 `Atom`.
 */
int xrandr_delete_output_property(lua_State*);


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

/** Configures which types of events the X server should enable.
 *
 * @function XRRSelectInput
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window
 * @tparam table mask
 * @tparam boolean mask.screen Configure screen change notifications.
 * @tparam boolean mask.crtc Configure crtc change notifications.
 * @tparam boolean mask.output Configure output change notifications.
 * @tparam boolean mask.output_property Configure output property change notifications.
 * @tparam boolean mask.provider Configure provider change notifications.
 * @tparam boolean mask.provider_property Configure provider property change notifications.
 * @tparam boolean mask.resource Configure resource change notifications.
 * @usage
 * -- Enable notifications for CRTCs and outputs
 * xrandr.XRRSelectInput(display, root, { crtc = true, output = true })
 */
int xrandr_select_input(lua_State* L);

/** Gets the range of possible screen sizes.
 *
 * This returns the minimum and maximum boundaries within which screen sizes may be set.
 *
 * @function XRRGetScreenSizeRange
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window
 * @treturn number status Despite being designated a X11 `Status` in `Xrandr.h`, this returns `1` on success.
 * @treturn number min_width
 * @treturn number min_height
 * @treturn number max_width
 * @treturn number max_height
 */
int xrandr_get_screen_size_range(lua_State*);


/** Sets the screen size.
 *
 * The `width` and `height` must be within the ranges returned by @{XRRGetScreenSizeRange}.
 *
 * @function XRRSetScreenSize
 * @tparam display display A display connection opened with @{xlib.XOpenDisplay}.
 * @tparam number window
 * @tparam number width
 * @tparam number height
 * @tparam number mm_width Physical size of the screen, in millimeters.
 * @tparam number mm_height Physical size of the screen, in millimeters.
 */
int xrandr_set_screen_size(lua_State*);


static const struct luaL_Reg xrandr_lib[] = {
    {"XRRQueryVersion",                xrandr_query_version               },
    { "XRRQueryExtension",             xrandr_query_extension             },
    { "XRRGetScreenResources",         xrandr_get_screen_resources        },
    { "XRRGetOutputInfo",              xrandr_get_output_info             },
    { "XRRGetOutputPrimary",           xrandr_get_output_primary          },
    { "XRRGetCrtcInfo",                xrandr_get_crtc_info               },
    { "XRRSetCrtcConfig",              xrandr_set_crtc_config             },
    { "XRRSetOutputPrimary",           xrandr_set_output_primary          },
    { "XRRGetScreenInfo",              xrandr_get_screen_info             },
    { "XRRSetScreenConfig",            xrandr_set_screen_config           },
    { "XRRSetScreenConfigAndRate",     xrandr_set_screen_config_rate      },
    { "XRRConfigRotations",            xrandr_config_rotations            },
    { "XRRConfigTimes",                xrandr_config_times                },
    { "XRRConfigSizes",                xrandr_config_sizes                },
    { "XRRConfigRates",                xrandr_config_rates                },
    { "XRRConfigCurrentConfiguration", xrandr_config_current_configuration},
    { "XRRConfigCurrentRate",          xrandr_config_current_rate         },
    { "XRRGetScreenSizeRange",         xrandr_get_screen_size_range       },
    { "XRRSetScreenSize",              xrandr_set_screen_size             },
    { "XRRListOutputProperties",       xrandr_list_output_properties      },
    { "XRRQueryOutputProperty",        xrandr_query_output_property       },
    { "XRRConfigureOutputProperty",    xrandr_configure_output_property   },
    { "XRRChangeOutputProperty",       xrandr_change_output_property      },
    { "XRRDeleteOutputProperty",       xrandr_delete_output_property      },
    { "XRRGetOutputProperty",          xrandr_get_output_property         },
    { NULL,                            NULL                               }
};

#endif // xrandr_h_INCLUDED

