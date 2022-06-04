#include "xrandr.h"

#include "lua_util.h"
#include "xlib.h"

#include <X11/Xlib.h>
#include <X11/extensions/randr.h>
#include <assert.h>
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
    crtc_info_t* res = luaL_checkudata(L, 1, LUA_XRANDR_CRTC_INFO);
    XRRFreeCrtcInfo(res->inner);
    return 0;
}

int crtc_info__index(lua_State* L) {
    crtc_info_t* res = luaL_checkudata(L, 1, LUA_XRANDR_CRTC_INFO);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "timestamp") == 0) {
        lua_pushinteger(L, res->inner->timestamp);
    } else if (strcmp(key, "x") == 0) {
        lua_pushinteger(L, res->inner->x);
    } else if (strcmp(key, "y") == 0) {
        lua_pushinteger(L, res->inner->y);
    } else if (strcmp(key, "height") == 0) {
        lua_pushinteger(L, res->inner->height);
    } else if (strcmp(key, "width") == 0) {
        lua_pushinteger(L, res->inner->width);
    } else if (strcmp(key, "mode") == 0) {
        lua_pushinteger(L, res->inner->mode);
    } else if (strcmp(key, "rotation") == 0) {
        lua_pushinteger(L, res->inner->rotation);
    } else if (strcmp(key, "rotations") == 0) {
        lua_pushinteger(L, res->inner->rotations);
    } else if (strcmp(key, "outputs") == 0) {
        lua_createtable(L, 0, res->inner->noutput);
        for (int i = 0; i < res->inner->noutput; ++i) {
            lua_pushinteger(L, res->inner->outputs[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else if (strcmp(key, "possible") == 0) {
        lua_createtable(L, 0, res->inner->npossible);
        for (int i = 0; i < res->inner->npossible; ++i) {
            lua_pushinteger(L, res->inner->possible[i]);
            lua_rawseti(L, -2, i + 1);
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}


LUA_MOD_EXPORT int luaopen_xlib_xrandr(lua_State* L) {
    luaL_newmetatable(L, LUA_XRANDR_SCREEN_RESOURCES);
    luaL_setfuncs(L, screen_resources_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR_OUTPUT_INFO);
    luaL_setfuncs(L, output_info_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR_CRTC_INFO);
    luaL_setfuncs(L, crtc_info_mt, 0);

    luaL_newmetatable(L, LUA_XRANDR);

#if LUA_VERSION_NUM <= 501
    luaL_register(L, LUA_XRANDR, xrandr_lib);
#else
    luaL_newlib(L, xrandr_lib);
#endif
    return 1;
}
