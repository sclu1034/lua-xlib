#include "xrandr.h"

#include "lua_util.h"
#include "xlib.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/extensions/randr.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void modeflags_to_lua(lua_State* L, int flags) {
    lua_createtable(L, 14, 0);

    for (int i = 0; i < 14; ++i) {
        lua_pushboolean(L, flags & (1 << i));
        lua_setfield(L, -2, mode_flags[i]);
    }
}

void mode_to_lua(lua_State* L, XRRModeInfo* info) {
    lua_createtable(L, 0, 0);

    lua_pushinteger(L, info->id);
    lua_setfield(L, -2, "id");

    lua_pushlstring(L, info->name, info->nameLength);
    lua_setfield(L, -2, "name");

    lua_pushinteger(L, info->width);
    lua_setfield(L, -2, "width");

    lua_pushinteger(L, info->height);
    lua_setfield(L, -2, "height");

    lua_pushinteger(L, info->dotClock);
    lua_setfield(L, -2, "dotClock");

    lua_pushinteger(L, info->hSyncStart);
    lua_setfield(L, -2, "hSyncStart");

    lua_pushinteger(L, info->hSyncEnd);
    lua_setfield(L, -2, "hSyncEnd");

    lua_pushinteger(L, info->hTotal);
    lua_setfield(L, -2, "hTotal");

    lua_pushinteger(L, info->hSkew);
    lua_setfield(L, -2, "hSkew");

    lua_pushinteger(L, info->vSyncStart);
    lua_setfield(L, -2, "vSyncStart");

    lua_pushinteger(L, info->vSyncEnd);
    lua_setfield(L, -2, "vSyncEnd");

    lua_pushinteger(L, info->vTotal);
    lua_setfield(L, -2, "vTotal");

    modeflags_to_lua(L, info->modeFlags);
    lua_setfield(L, -2, "modeFlags");
}

int xrandr_get_output_info(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    screen_resources_t* res = luaL_checkudata(L, 2, LUA_XRANDR_SCREEN_RESOURCES);
    lua_Integer output = luaL_checkinteger(L, 3);

    XRROutputInfo* info = XRRGetOutputInfo(display->inner, res->inner, (RROutput) output);
    if (!info) {
        return luaL_error(L, "Failed to get info for output %d", output);
    }

    output_info_t* out = lua_newuserdata(L, sizeof(output_info_t));
    luaL_getmetatable(L, LUA_XRANDR_OUTPUT_INFO);
    lua_setmetatable(L, -2);

    out->inner = info;

    return 1;
}

int xrandr_get_output_primary(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer window = luaL_checkinteger(L, 2);
    lua_pushboolean(L, XRRGetOutputPrimary(display->inner, (Window) window));
    return 1;
}

int xrandr_set_output_primary(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer window = luaL_checkinteger(L, 2);
    lua_Integer output = luaL_checkinteger(L, 3);
    XRRSetOutputPrimary(display->inner, (Window) window, (RROutput) output);
    return 1;
}

int output_info__gc(lua_State* L) {
    output_info_t* out = luaL_checkudata(L, 1, LUA_XRANDR_OUTPUT_INFO);
    XRRFreeOutputInfo(out->inner);
    return 0;
}

int output_info__index(lua_State* L) {
    output_info_t* out = luaL_checkudata(L, 1, LUA_XRANDR_OUTPUT_INFO);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "timestamp") == 0) {
        lua_pushinteger(L, out->inner->timestamp);
    } else if (strcmp(key, "name") == 0) {
        lua_pushlstring(L, out->inner->name, out->inner->nameLen);
    } else if (strcmp(key, "crtc") == 0) {
        lua_pushinteger(L, out->inner->crtc);
    } else if (strcmp(key, "npreferred") == 0) {
        lua_pushinteger(L, out->inner->npreferred);
    } else if (strcmp(key, "mm_width") == 0) {
        lua_pushinteger(L, out->inner->mm_width);
    } else if (strcmp(key, "mm_height") == 0) {
        lua_pushinteger(L, out->inner->mm_height);
    } else if (strcmp(key, "connection") == 0) {
        lua_pushstring(L, connection_states[out->inner->connection]);
    } else if (strcmp(key, "subpixel_order") == 0) {
        lua_pushstring(L, subpixel_orders[out->inner->subpixel_order]);
    } else if (strcmp(key, "crtcs") == 0) {
        lua_createtable(L, 0, out->inner->ncrtc);
        for (int i = 0; i < out->inner->ncrtc; ++i) {
            lua_pushinteger(L, out->inner->crtcs[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "clones") == 0) {
        lua_createtable(L, 0, out->inner->nclone);
        for (int i = 0; i < out->inner->nclone; ++i) {
            lua_pushinteger(L, out->inner->clones[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "modes") == 0) {
        lua_createtable(L, 0, out->inner->nmode);
        for (int i = 0; i < out->inner->nmode; ++i) {
            lua_pushinteger(L, out->inner->modes[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}


int xrandr_get_screen_resources(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer root = luaL_optinteger(L, 2, -1);

    screen_resources_t* res = lua_newuserdata(L, sizeof(screen_resources_t));
    luaL_getmetatable(L, LUA_XRANDR_SCREEN_RESOURCES);
    lua_setmetatable(L, -2);

    res->inner = XRRGetScreenResources(display->inner, root);

    return 1;
}

int screen_resources__gc(lua_State* L) {
    screen_resources_t* res = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_RESOURCES);
    XRRFreeScreenResources(res->inner);
    return 0;
}

int screen_resources__index(lua_State* L) {
    screen_resources_t* res = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_RESOURCES);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "timestamp") == 0) {
        lua_pushinteger(L, res->inner->timestamp);
    } else if (strcmp(key, "configTimestamp") == 0) {
        lua_pushinteger(L, res->inner->configTimestamp);
    } else if (strcmp(key, "outputs") == 0) {
        lua_createtable(L, 0, res->inner->noutput);
        for (int i = 0; i < res->inner->noutput; ++i) {
            lua_pushinteger(L, res->inner->outputs[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "crtcs") == 0) {
        lua_createtable(L, 0, res->inner->ncrtc);
        for (int i = 0; i < res->inner->ncrtc; ++i) {
            lua_pushinteger(L, res->inner->crtcs[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "modes") == 0) {
        lua_createtable(L, 0, res->inner->nmode);
        for (int i = 0; i < res->inner->nmode; ++i) {
            mode_to_lua(L, &res->inner->modes[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

int xrandr_get_crtc_info(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    screen_resources_t* res = luaL_checkudata(L, 2, LUA_XRANDR_SCREEN_RESOURCES);
    lua_Integer crtc = luaL_checkinteger(L, 3);

    XRRCrtcInfo* info = XRRGetCrtcInfo(display->inner, res->inner, (RRCrtc) crtc);
    if (!info) {
        return luaL_error(L, "Failed to get info for crtc %d", crtc);
    }

    crtc_info_t* out = lua_newuserdata(L, sizeof(crtc_info_t));
    luaL_getmetatable(L, LUA_XRANDR_CRTC_INFO);
    lua_setmetatable(L, -2);

    out->inner = info;

    return 1;
}

int crtc_info__gc(lua_State* L) {
    crtc_info_t* crtc = luaL_checkudata(L, 1, LUA_XRANDR_CRTC_INFO);
    XRRFreeCrtcInfo(crtc->inner);
    return 0;
}

int crtc_info__index(lua_State* L) {
    crtc_info_t* crtc = luaL_checkudata(L, 1, LUA_XRANDR_CRTC_INFO);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "timestamp") == 0) {
        lua_pushinteger(L, crtc->inner->timestamp);
    } else if (strcmp(key, "x") == 0) {
        lua_pushinteger(L, crtc->inner->x);
    } else if (strcmp(key, "y") == 0) {
        lua_pushinteger(L, crtc->inner->y);
    } else if (strcmp(key, "height") == 0) {
        lua_pushinteger(L, crtc->inner->height);
    } else if (strcmp(key, "width") == 0) {
        lua_pushinteger(L, crtc->inner->width);
    } else if (strcmp(key, "mode") == 0) {
        lua_pushinteger(L, crtc->inner->mode);
    } else if (strcmp(key, "rotation") == 0) {
        lua_pushinteger(L, crtc->inner->rotation);
    } else if (strcmp(key, "rotations") == 0) {
        lua_pushinteger(L, crtc->inner->rotations);
    } else if (strcmp(key, "outputs") == 0) {
        lua_createtable(L, 0, crtc->inner->noutput);
        for (int i = 0; i < crtc->inner->noutput; ++i) {
            lua_pushinteger(L, crtc->inner->outputs[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "possible") == 0) {
        lua_createtable(L, 0, crtc->inner->npossible);
        for (int i = 0; i < crtc->inner->npossible; ++i) {
            lua_pushinteger(L, crtc->inner->possible[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

int xrandr_set_crtc_config(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    screen_resources_t* res = luaL_checkudata(L, 2, LUA_XRANDR_SCREEN_RESOURCES);
    lua_Integer crtc = luaL_checkinteger(L, 3);
    lua_Integer timestamp = luaL_checkinteger(L, 4);
    lua_Integer x = luaL_checkinteger(L, 5);
    lua_Integer y = luaL_checkinteger(L, 6);
    lua_Integer mode = luaL_optinteger(L, 7, None);
    lua_Integer rotation = luaL_checkinteger(L, 8);
    luaL_checktype(L, 9, LUA_TTABLE);
    int noutputs = lua_rawlen(L, 9);
    RROutput* outputs = (RROutput*) calloc(noutputs, sizeof(RROutput));
    if (!outputs) {
        return luaL_error(L, "failed to allocate outputs array");
    }

    for (int i = 0; i < noutputs; ++i) {
        lua_rawgeti(L, 9, i + 1);
        outputs[i] = luaL_checkinteger(L, -1);
    }

    Status status = XRRSetCrtcConfig(display->inner,
                                     res->inner,
                                     (RRCrtc) crtc,
                                     (Time) timestamp,
                                     (int) x,
                                     (int) y,
                                     (RRMode) mode,
                                     (Rotation) rotation,
                                     outputs,
                                     noutputs);

    free(outputs);
    lua_pushinteger(L, status);

    return 1;
}

int xrandr_query_version(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    int major = 0;
    int minor = 0;

    Status status = XRRQueryVersion(display->inner, &major, &minor);
    lua_pushinteger(L, status);
    lua_pushinteger(L, major);
    lua_pushinteger(L, minor);
    return 3;
}

int xrandr_query_extension(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    int event_base = 0;
    int error_base = 0;

    Bool status = XRRQueryExtension(display->inner, &event_base, &error_base);
    lua_pushboolean(L, status);
    lua_pushinteger(L, event_base);
    lua_pushinteger(L, error_base);
    return 3;
}

int screen_configuration__gc(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    XRRFreeScreenConfigInfo(config->inner);
    return 0;
}

int xrandr_get_screen_info(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Window window = luaL_checkinteger(L, 2);

    screen_configuration_t* config = lua_newuserdata(L, sizeof(screen_configuration_t));
    luaL_getmetatable(L, LUA_XRANDR_SCREEN_CONFIG);
    lua_setmetatable(L, -2);

    XRRScreenConfiguration* inner = XRRGetScreenInfo(display->inner, window);
    if (!inner) {
        return luaL_error(L, "failed to get screen configuration");
    }

    config->inner = inner;

    return 1;
}

int xrandr_config_rotations(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    Rotation current_rotation;
    Rotation rotation = XRRConfigRotations(config->inner, &current_rotation);
    lua_pushinteger(L, rotation);
    lua_pushinteger(L, current_rotation);
    return 2;
}

int xrandr_config_times(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    Time config_timestamp;
    Time timestamp = XRRConfigTimes(config->inner, &config_timestamp);
    lua_pushinteger(L, timestamp);
    lua_pushinteger(L, config_timestamp);
    return 2;
}

int xrandr_config_sizes(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    int nsizes;
    XRRScreenSize* sizes = XRRConfigSizes(config->inner, &nsizes);

    lua_createtable(L, 0, nsizes);
    for (int i = 0; i < nsizes; ++i) {
        XRRScreenSize size = sizes[i];
        lua_createtable(L, 4, 0);

        lua_pushinteger(L, size.width);
        lua_setfield(L, -2, "width");

        lua_pushinteger(L, size.mwidth);
        lua_setfield(L, -2, "mwidth");

        lua_pushinteger(L, size.height);
        lua_setfield(L, -2, "height");

        lua_pushinteger(L, size.mheight);
        lua_setfield(L, -2, "mheight");

        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int xrandr_config_rates(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    lua_Integer size_index = luaL_checkinteger(L, 2);
    int nrates;
    short* rates = XRRConfigRates(config->inner, (SizeID) size_index, &nrates);

    lua_createtable(L, 0, nrates);
    for (int i = 0; i < nrates; ++i) {
        lua_pushinteger(L, rates[i]);
        lua_rawseti(L, -2, i + 1);
    }
    return 1;
}

int xrandr_config_current_configuration(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    Rotation rotation;
    SizeID size_index = XRRConfigCurrentConfiguration(config->inner, &rotation);
    lua_pushinteger(L, size_index);
    lua_pushinteger(L, rotation);
    return 2;
}

int xrandr_config_current_rate(lua_State* L) {
    screen_configuration_t* config = luaL_checkudata(L, 1, LUA_XRANDR_SCREEN_CONFIG);
    lua_pushinteger(L, XRRConfigCurrentRate(config->inner));
    return 1;
}

int xrandr_set_screen_config(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    screen_configuration_t* config = luaL_checkudata(L, 2, LUA_XRANDR_SCREEN_CONFIG);
    Drawable d = (Drawable) luaL_checkinteger(L, 3);
    int size_index = (int) luaL_checkinteger(L, 4);
    Rotation rotation = (Rotation) luaL_checkinteger(L, 5);
    Time timestamp = (Time) luaL_checkinteger(L, 6);

    Status status = XRRSetScreenConfig(display->inner, config->inner, d, size_index, rotation, timestamp);

    lua_pushinteger(L, status);
    return 1;
}

int xrandr_set_screen_config_rate(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    screen_configuration_t* config = luaL_checkudata(L, 2, LUA_XRANDR_SCREEN_CONFIG);
    Drawable d = (Drawable) luaL_checkinteger(L, 3);
    int size_index = (int) luaL_checkinteger(L, 4);
    Rotation rotation = (Rotation) luaL_checkinteger(L, 5);
    short rate = (short) luaL_checkinteger(L, 6);
    Time timestamp = (Time) luaL_checkinteger(L, 7);

    Status status = XRRSetScreenConfigAndRate(display->inner, config->inner, d, size_index, rotation, rate, timestamp);

    lua_pushinteger(L, status);
    return 1;
}

int get_mask_for_field(lua_State* L, int idx, const char* field, int mask) {
    lua_getfield(L, idx, field);
    Bool value = lua_toboolean(L, -1);
    lua_pop(L, 1);

    if (value) {
        return mask;
    } else {
        return 0;
    }
}

int xrandr_select_input(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Window window = (Window) luaL_checkinteger(L, 2);
    luaL_checktype(L, 3, LUA_TTABLE);

    int mask = 0;
    mask += get_mask_for_field(L, 3, "screen", RRScreenChangeNotifyMask);
    mask += get_mask_for_field(L, 3, "crtc", RRCrtcChangeNotifyMask);
    mask += get_mask_for_field(L, 3, "output", RROutputChangeNotifyMask);
    mask += get_mask_for_field(L, 3, "output_property", RROutputPropertyNotifyMask);
    mask += get_mask_for_field(L, 3, "provider", RRProviderChangeNotifyMask);
    mask += get_mask_for_field(L, 3, "provider_property", RROutputPropertyNotifyMask);
    mask += get_mask_for_field(L, 3, "resource", RRResourceChangeNotifyMask);

    XRRSelectInput(display->inner, window, mask);

    return 0;
}

int xrandr_get_screen_size_range(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Window window = (Window) luaL_checkinteger(L, 2);
    int min_width;
    int min_height;
    int max_width;
    int max_height;

    Status status = XRRGetScreenSizeRange(display->inner, window, &min_width, &min_height, &max_width, &max_height);

    lua_pushinteger(L, status);
    lua_pushinteger(L, min_width);
    lua_pushinteger(L, min_height);
    lua_pushinteger(L, max_width);
    lua_pushinteger(L, max_height);

    return 5;
}

int xrandr_set_screen_size(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Window window = (Window) luaL_checkinteger(L, 2);
    int width = (int) luaL_checkinteger(L, 3);
    int height = (int) luaL_checkinteger(L, 4);
    int mm_width = (int) luaL_checkinteger(L, 5);
    int mm_height = (int) luaL_checkinteger(L, 6);

    XRRSetScreenSize(display->inner, window, width, height, mm_width, mm_height);
    return 0;
}


int xrandr_list_output_properties(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    int nprop = 0;
    Atom* properties = XRRListOutputProperties(display->inner, output, &nprop);

    lua_createtable(L, 0, nprop);
    for (int i = 0; i < nprop; ++i) {
        lua_pushinteger(L, properties[i]);
        lua_rawseti(L, -2, i + 1);
    }

    return 1;
}

int xrandr_query_output_property(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    Atom property = (Atom) luaL_checkinteger(L, 3);

    XRRPropertyInfo* info = XRRQueryOutputProperty(display->inner, output, property);

    if (!info) {
        return luaL_error(L, "Failed to query output property %d", property);
    }

    lua_createtable(L, 4, 0);

    lua_pushboolean(L, info->pending);
    lua_setfield(L, -2, "pending");

    lua_pushboolean(L, info->range);
    lua_setfield(L, -2, "range");

    lua_pushboolean(L, info->immutable);
    lua_setfield(L, -2, "immutable");

    lua_createtable(L, 0, info->num_values);
    for (int i = 0; i < info->num_values; ++i) {
        lua_pushinteger(L, info->values[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "values");

    XFree(info);
    return 1;
}

int xrandr_configure_output_property(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    Atom property = (Atom) luaL_checkinteger(L, 3);
    Bool pending = lua_toboolean(L, 4);
    Bool range = lua_toboolean(L, 5);
    luaL_checktype(L, 6, LUA_TTABLE);

    int num_values = lua_rawlen(L, 6);
    long* values = calloc(num_values, sizeof(long));

    for (int i = 0; i < num_values; ++i) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, 6);
        values[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    XRRConfigureOutputProperty(display->inner, output, property, pending, range, num_values, values);

    free(values);
    return 0;
}

int xrandr_change_output_property(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    Atom property = (Atom) luaL_checkinteger(L, 3);
    int mode = (int) luaL_checkinteger(L, 4);
    // For now we only support strings, so we hardcode the type.
    // We still keep the parameter as a placeholder, so we don't need an API change later on,
    // and still map properly to the signature in libxrandr.
    Atom type = XA_STRING; // (Atom) luaL_checkinteger(L, 5);

    int format = 8;
    size_t nelements;
    const unsigned char* data = (const unsigned char*) luaL_checklstring(L, 6, &nelements);

    XRRChangeOutputProperty(display->inner, output, property, type, format, mode, data, (int) nelements);
    return 0;
}

int xrandr_get_output_property(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    Atom property = (Atom) luaL_checkinteger(L, 3);
    long offset = (long) luaL_checkinteger(L, 4);
    long length = (long) luaL_checkinteger(L, 5);
    Bool delete = (Bool) lua_toboolean(L, 6);
    Bool pending = (Bool) lua_toboolean(L, 7);
    // As with changing the output, we hardcode the type for now and keep the parameter only as placeholder.
    // As its the last parameter, that's trivial anyways.
    Atom req_type = XA_STRING; // (Atom) luaL_checkinteger(L, 8);

    Atom actual_type;
    int actual_format;
    unsigned long nitems;
    unsigned long bytes_after;
    unsigned char* prop;

    XRRGetOutputProperty(display->inner,
                         output,
                         property,
                         offset,
                         length,
                         delete,
                         pending,
                         req_type,
                         &actual_type,
                         &actual_format,
                         &nitems,
                         &bytes_after,
                         &prop);

    // `type == None` is returned when the property doesn't exist.
    if (actual_type == None) {
        return 0;
    }

    // If there is a returned type, but no data, the requested type did not match the actual type.
    if (!prop) {
        return luaL_error(L, "Only properties of type `string` are currently supported. Got `(Atom) %d`.", actual_type);
    }

    // if (actual_format != 8) {
    //     return luaL_error(L, "Only byte format `8` is currently supported. Got `%d`.", actual_format);
    // }

    lua_pushlstring(L, (char*) prop, length);
    lua_pushinteger(L, nitems);
    lua_pushinteger(L, actual_type);
    lua_pushinteger(L, actual_format);
    lua_pushinteger(L, bytes_after);

    return 2;
}

int xrandr_delete_output_property(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    RROutput output = (RROutput) luaL_checkinteger(L, 2);
    Atom property = (Atom) luaL_checkinteger(L, 3);

    XRRDeleteOutputProperty(display->inner, output, property);
    return 0;
}


LUA_MOD_EXPORT int luaopen_xlib_xrandr(lua_State* L) {
    luaL_newmetatable(L, LUA_XRANDR_SCREEN_RESOURCES);
    luaL_setfuncs(L, screen_resources_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR_OUTPUT_INFO);
    luaL_setfuncs(L, output_info_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR_CRTC_INFO);
    luaL_setfuncs(L, crtc_info_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR_SCREEN_CONFIG);
    luaL_setfuncs(L, screen_config_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR);

#if LUA_VERSION_NUM <= 501
    luaL_register(L, LUA_XRANDR, xrandr_lib);
#else
    luaL_newlib(L, xrandr_lib);
#endif

    lua_createtable(L, 13, 0);
    luaU_setstringfield(L, -1, "BACKLIGHT", "Backlight");
    luaU_setstringfield(L, -1, "RANDR_EDID", "EDID");
    luaU_setstringfield(L, -1, "SIGNAL_FORMAT", "SignalFormat");
    luaU_setstringfield(L, -1, "SIGNAL_PROPERTIES", "SignalProperties");
    luaU_setstringfield(L, -1, "CONNECTOR_TYPE", "ConnectorType");
    luaU_setstringfield(L, -1, "CONNECTOR_NUMBER", "ConnectorNumber");
    luaU_setstringfield(L, -1, "COMPATIBILITY_LIST", "CompatibilityList");
    luaU_setstringfield(L, -1, "CLONE_LIST", "CloneList");
    luaU_setstringfield(L, -1, "BORDER", "Border");
    luaU_setstringfield(L, -1, "BORDER_DIMENSIONS", "BorderDimensions");
    luaU_setstringfield(L, -1, "GUID", "GUID");
    luaU_setstringfield(L, -1, "RANDR_TILE", "TILE");
    luaU_setstringfield(L, -1, "NON_DESKTOP", "non-desktop");
    lua_setfield(L, -2, "RR_OUTPUT");

    return 1;
}
