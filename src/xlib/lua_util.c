#include "lua_util.h"


#if LUA_VERSION_NUM <= 501
// Shamelessly copied from Lua 5.3 source.
// TODO: What's the official way to do this in 5.1?
void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup) {
    luaL_checkstack(L, nup, "too many upvalues");
    for (; l->name != NULL; l++) { /* fill the table with given functions */
        if (l->func == NULL)       /* place holder? */
            lua_pushboolean(L, 0);
        else {
            int i;
            for (i = 0; i < nup; i++) /* copy upvalues to the top */
                lua_pushvalue(L, -nup);
            lua_pushcclosure(L, l->func, nup); /* closure with those upvalues */
        }
        lua_setfield(L, -(nup + 2), l->name);
    }
    lua_pop(L, nup); /* remove upvalues */
}
#endif
