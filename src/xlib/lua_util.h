#ifndef lua_util_h_INCLUDED
#define lua_util_h_INCLUDED

#include <lauxlib.h>
#include <lua.h>


#define LUA_MOD_EXPORT extern

#if LUA_VERSION_NUM <= 501
#define luaL_newlib(L, l) (luaL_register(L, LUA_PULSEAUDIO, l))
#define lua_rawlen(L, i)  (lua_objlen(L, i))

void luaL_setfuncs(lua_State*, const luaL_Reg*, int);
#endif

#endif // lua_util_h_INCLUDED

