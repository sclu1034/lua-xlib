#include "xlib.h"

#include "lua_util.h"

#include <stdlib.h>


int display__gc(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    // All we care about is that the connection has been closed somehow.
    // So rather than failing, like `xlib_close_display`, we just silently ignore closed connections.
    if (!display->closed) {
        XCloseDisplay(display->inner);
    }
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
    d->closed = False;

    return 1;
}

int xlib_close_display(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    if (display->closed) {
        return luaL_error(L, "this display connection has already been closed");
    }
    XCloseDisplay(display->inner);
    return 0;
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

int xlib_lock_display(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    XLockDisplay(display->inner);
    return 0;
}

int xlib_unlock_display(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    XUnlockDisplay(display->inner);
    return 0;
}

int xlib_intern_atom(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    const char* name = luaL_checkstring(L, 2);
    Bool only_if_exists = lua_toboolean(L, 3);

    lua_pushinteger(L, XInternAtom(display->inner, name, only_if_exists));
    return 1;
}

int xlib_intern_atoms(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Bool only_if_exists = lua_toboolean(L, 3);
    luaL_checktype(L, 2, LUA_TTABLE);

    int count = lua_rawlen(L, 2);
    char** names = calloc(count, sizeof(char*));

    for (int i = 0; i < count; ++i) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, 2);
        names[i] = (char*) lua_tostring(L, -1);
    }

    Atom* atoms = calloc(count, sizeof(Atom));
    Status status = XInternAtoms(display->inner, names, count, only_if_exists, atoms);
    lua_pushinteger(L, status);

    lua_createtable(L, 0, count);

    for (int i = 0; i < count; ++i) {
        lua_pushinteger(L, atoms[i]);
        lua_rawseti(L, -2, i + 1);
    }

    free(names);
    free(atoms);

    return 2;
}

int xlib_get_atom_name(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    Atom atom = (Atom) luaL_checkinteger(L, 2);

    char* name = XGetAtomName(display->inner, atom);
    lua_pushstring(L, name);
    XFree(name);
    return 1;
}

int xlib_get_atom_names(lua_State* L) {
    display_t* display = luaL_checkudata(L, 1, LUA_XLIB_DISPLAY);
    luaL_checktype(L, 2, LUA_TTABLE);

    int count = lua_rawlen(L, 2);
    Atom* atoms = calloc(count, sizeof(Atom));

    for (int i = 0; i < count; ++i) {
        lua_pushinteger(L, i + 1);
        lua_gettable(L, 2);

        int type = lua_type(L, -1);
#if LUA_VERSION_NUM >= 503
        if (type != LUA_TNUMBER || !lua_isinteger(L, -1)) {
#else
        if (type != LUA_TNUMBER || !lua_isnumber(L, -1) || lua_tonumber(L, -1) < 0) {
#endif
            free(atoms);
            return luaL_error(L, "at index %i: expected integer, got %s", i + 1, lua_typename(L, type));
        }

        atoms[i] = lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    char** names = calloc(count, sizeof(char*));
    Status status = XGetAtomNames(display->inner, atoms, count, names);
    lua_pushinteger(L, status);

    lua_createtable(L, 0, count);

    for (int i = 0; i < count; ++i) {
        lua_pushstring(L, names[i]);
        lua_rawseti(L, -2, i + 1);
    }

    free(atoms);
    free(names);

    return 2;
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
