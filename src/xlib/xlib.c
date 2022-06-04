/**
 * @module xlib
 */
#include "xlib.h"

#include "lua_util.h"


int display__gc(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    XCloseDisplay(display->inner);
    return 0;
}

int xlib_open_display(lua_State* L) {
    const char* display_name = lua_tostring(L, 1);
    Display* display = XOpenDisplay(display_name);
    if (display == NULL) {
        return luaL_error(L, "failed to open display %s", display_name);
    }

    display_t* d = lua_newuserdata(L, sizeof(display_t));
    luaL_getmetatable(L, LUA_XLIB_DISPLAY);
    lua_setmetatable(L, -2);

    d->inner = display;

    return 1;
}

int xlib_display_name(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    lua_pushstring(L, XDisplayName(name));
    return 1;
}

int xlib_root_window(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer screen = luaL_checkinteger(L, 2);
    lua_pushinteger(L, RootWindow(display->inner, screen));
    return 1;
}

int xlib_default_screen(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_pushinteger(L, DefaultScreen(display->inner));
    return 1;
}

int xlib_screen_count(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_pushinteger(L, ScreenCount(display->inner));
    return 1;
}

int xlib_display_height(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer screen = luaL_checkinteger(L, 2);
    lua_pushinteger(L, DisplayHeight(display->inner, screen));
    return 1;
}

int xlib_display_width(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    lua_Integer screen = luaL_checkinteger(L, 2);
    lua_pushinteger(L, DisplayWidth(display->inner, screen));
    return 1;
}


LUA_MOD_EXPORT int luaopen_xlib(lua_State* L) {
    luaL_newmetatable(L, LUA_XLIB_DISPLAY);
    luaL_setfuncs(L, display_mt, 0);

    luaL_newmetatable(L, LUA_XLIB);

#if LUA_VERSION_NUM <= 501
    luaL_register(L, LUA_XLIB, xlib_lib);
#else
    luaL_newlib(L, xlib_lib);
#endif
    return 1;
}
