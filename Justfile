build_dir := "build"

cmake *ARGS:
    cmake -S . -B {{ build_dir }} {{ ARGS }}

build: cmake
    cd {{ build_dir }} && make

rock version="scm-3":
    luarocks --local make rocks/lua-xlib-{{ version }}.rockspec

run:
    env LUA_CPATH="./{{ build_dir }}/?.so;$${LUA_CPATH}" xvfb-run lua test.lua
