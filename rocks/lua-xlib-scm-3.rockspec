-- kak: filetype=lua
package = "lua-xlib"
version = "scm-3"

source = {
    url = "git://github.com/sclu1034/lua-xlib.git"
}

description = {
    summary = "Lua bindings for XLib and XRandR",
    homepage = "https://github.com/sclu1034/lua-xlib",
    license = "GPLv3"
}

dependencies = {
    "lua >= 5.1"
}

build = {
    type = "cmake",
    variables = {
        CMAKE_INSTALL_PREFIX="$(PREFIX)",
        CMAKE_BUILD_TYPE="Release",
        LUA="$(LUA)",
        LUA_LIBDIR="$(LIBDIR)",
        LUA_BINDIR="$(BINDIR)",
        LUA_LUADIR="$(LUADIR)",
        LUA_DOCDIR="$(PREFIX)/doc",
    },
    copy_directories = {},
}
