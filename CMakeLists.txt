cmake_minimum_required(VERSION 3.6)

project(lua-xlib)

set(CMAKE_SHARED_LIBRARY_PREFIX "")

if(NOT LUA)
    set(LUA lua)
endif()

execute_process(COMMAND sh -c "command -v '${LUA}'" RESULT_VARIABLE LUA_BINARY_FOUND OUTPUT_QUIET)
if(LUA_BINARY_FOUND)
    message(FATAL_ERROR "Lua binary '${LUA}' not found. You may need to specify the full path.")
endif()

execute_process(COMMAND ${LUA} -v OUTPUT_VARIABLE LUA_OUTPUT ERROR_VARIABLE LUA_OUTPUT)
string(REGEX MATCH "Lua (5\.[0-9])" LUA_VERSION "${LUA_OUTPUT}")
if("${CMAKE_MATCH_COUNT}" LESS 1)
    message(FATAL_ERROR "Unsupported Lua version: ${LUA_OUTPUT}")
else()
    set(LUA_VERSION "${CMAKE_MATCH_1}")
endif()

find_package(Lua ${LUA_VERSION} EXACT REQUIRED)

if(NOT LUA_LIBDIR)
    set(LUA_LIBDIR "${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION}")
endif()
if(NOT LUA_LUADIR)
    set(LUA_LUADIR "${CMAKE_INSTALL_DATADIR}/lua/${LUA_VERSION}")
endif()
if(NOT LUA_DOCDIR)
    set(LUA_DOCDIR "${CMAKE_INSTALL_DOCDIR}")
endif()

find_package(X11 REQUIRED)

include_directories(src/xlib "${LUA_INCLUDE_DIR}" "${X11_INCLUDE_DIR}")

set(SRC src/xlib/xlib.c
        src/xlib/xrandr.c
        src/xlib/lua_util.c)

add_library(xlib SHARED ${SRC})
set_property(TARGET xlib PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(xlib ${LUA_LIBRARIES} ${X11_X11_LIB} ${X11_Xrandr_LIB})

install(TARGETS xlib DESTINATION "${LUA_LIBDIR}")

if(CI)
    target_compile_options(xlib PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

if(NOT SKIP_DOCUMENTATION)
    file(GLOB DOC_DEPENDS doc/pages/*.md)
    file(GLOB DOC_STYLE_DEPENDS doc/scss/**/*)

    set(GENERATED_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/")

    foreach(FILE IN LISTS SRC)
        string(CONCAT OUT_FILE "${GENERATED_SRC_DIR}" "${FILE}")
        list(APPEND DOC_DEPENDS "${OUT_FILE}")

        get_filename_component(OUT_DIR "${OUT_FILE}" DIRECTORY)
        file(MAKE_DIRECTORY "${OUT_DIR}")

        add_custom_command(
            OUTPUT "${OUT_FILE}"
            COMMAND ${LUA} "./tools/preprocessor.lua" "${FILE}" "${OUT_FILE}"
            DEPENDS "${FILE}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            VERBATIM
        )

        # For C files, we also need to handle the corresponding header file.
        string(FIND "${FILE}" ".c" IS_C)
        if(IS_C)
            string(REPLACE ".c" ".h" HEADER "${FILE}")
            string(REPLACE ".c" ".h" OUT_HEADER "${OUT_FILE}")
            list(APPEND DOC_DEPENDS "${OUT_HEADER}")

            add_custom_command(
                OUTPUT "${OUT_HEADER}"
                COMMAND ${LUA} "./tools/preprocessor.lua" "${HEADER}" "${OUT_HEADER}"
                DEPENDS "${HEADER}"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                VERBATIM
            )
        endif()
    endforeach()

    add_custom_command(
        OUTPUT doc/index.html
        COMMAND ldoc --config doc/config.ld --dir "${CMAKE_CURRENT_BINARY_DIR}/doc" --project ${PROJECT_NAME} "${GENERATED_SRC_DIR}"
        DEPENDS doc/config.ld doc/ldoc.ltp "${DOC_DEPENDS}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        VERBATIM
    )

    add_custom_command(
        OUTPUT doc/ldoc.css
        COMMAND sass "doc/ldoc.scss" "doc/ldoc.css"
        DEPENDS doc/ldoc.scss "${DOC_STYLE_DEPENDS}"
        MAIN_DEPENDENCY doc/ldoc.scss
        VERBATIM
    )

    if(CI)
        file(GENERATE OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/doc/.nojekyll" CONTENT "")
    endif()

    add_custom_target(docs ALL DEPENDS doc/index.html doc/ldoc.css)

    get_filename_component(LUA_DOCDIR_PARENT "${LUA_DOCDIR}" DIRECTORY)
    install(DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/doc" DESTINATION "${LUA_DOCDIR_PARENT}")
else()
    add_custom_target(docs echo "Documentation generation disabled")
endif()
