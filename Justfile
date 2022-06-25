build_dir := "build"

cmake *ARGS:
    cmake -S . -B {{ build_dir }} {{ ARGS }}

build: cmake
    cd {{ build_dir }} && make

rock version="scm-3":
    luarocks --local make rocks/lua-xlib-{{ version }}.rockspec

run file="test.lua":
    env LUA_CPATH_5_4="./{{ build_dir }}/?.so;${LUA_CPATH_5_4}" xvfb-run lua5.4 {{ file }}
