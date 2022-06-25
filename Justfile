build_dir := "build"

cmake *ARGS:
    cmake -S . -B {{ build_dir }} -DLUA=lua5.3 {{ ARGS }}

build: cmake
    cd {{ build_dir }} && make

rock version="scm-3":
    luarocks --local --lua-version 5.3 make rocks/lua-xlib-{{ version }}.rockspec

test *ARGS: build
    xvfb-run busted --lua=lua5.3 --config-file=.busted.lua {{ ARGS }}

run file="test.lua": build
    env LUA_CPATH_5_3="./{{ build_dir }}/?.so;${LUA_CPATH_5_3}" xvfb-run lua5.3 {{ file }}

clean:
    if [ -d {{ build_dir }} ]; then rm -r {{ build_dir }}; fi
    if [ -d build.luarocks ]; then rm -r build.luarocks; fi
