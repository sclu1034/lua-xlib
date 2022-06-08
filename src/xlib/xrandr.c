#include "xrandr.h"

#include "lua_util.h"
#include "xlib.h"

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
    return 1;
}
