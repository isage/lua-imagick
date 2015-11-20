cmake_minimum_required(VERSION 2.8.12)
include(CheckCXXCompilerFlag)

project(imagick)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

option(USE_LUA "Use Lua (also called 'C' Lua) version 5.1 includes" OFF)
option(USE_LUA52 "Use Lua (also called 'C' Lua) version 5.2 includes" OFF)
option(USE_LUAJIT "Use LuaJIT includes instead of 'C' Lua ones (default)" ON)

if(USE_LUA)
    find_package(Lua51 REQUIRED)
    set(USE_LUA52 OFF)
    set(USE_LUAJIT OFF)
    set(LUA_COMMAND "lua")
elseif(USE_LUA52)
    find_package(Lua52 REQUIRED)
    set(USE_LUA OFF)
    set(USE_LUAJIT OFF)
    set(LUA_COMMAND "lua")
elseif(USE_LUAJIT)
    find_package(LuaJIT REQUIRED)
    set(USE_LUA52 OFF)
    set(USE_LUA OFF)
    set(LUA_COMMAND "luajit")
endif()

get_filename_component(_lua_lib_dir ${LUA_LIBRARY} PATH)


set (imagick_VERSION_MAJOR 1)
set (imagick_VERSION_MINOR 0)

file(GLOB sources "src/[a-zA-Z]*.c")
file(GLOB private_headers "src/[a-zA-Z]*.h")


include_directories(${imagick_SOURCE_DIR}/src)
include_directories(${LUA_INCLUDE_DIR})

add_definitions("-Wall")


find_package(ImageMagick COMPONENTS MagickWand MagickCore REQUIRED)


if(APPLE)
    set(CMAKE_SHARED_MODULE_CREATE_C_FLAGS
        "${CMAKE_SHARED_MODULE_CREATE_C_FLAGS} -undefined dynamic_lookup")
endif()

if(WIN32)
    # Win32 modules need to be linked to the Lua library.
    target_link_libraries(imagick ${LUA_LIBRARY})
    set(_lua_module_dir "${_lua_lib_dir}")
else()
  if (USE_LUA52)
    set(_lua_module_dir "${_lua_lib_dir}/lua/5.2")
  else ()
    set(_lua_module_dir "${_lua_lib_dir}/lua/5.1")
  endif ()
endif()

add_library(imagick MODULE ${sources})
set_target_properties(imagick PROPERTIES PREFIX "")
if (NOT WIN32)
  set_target_properties(imagick PROPERTIES SUFFIX ".so")
endif ()
target_compile_options(imagick PRIVATE -fopenmp -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 -DIMAGICK_VERSION=\"${imagick_VERSION_MAJOR}.${imagick_VERSION_MINOR}\")
target_include_directories(imagick PRIVATE ${ImageMagick_INCLUDE_DIRS})
target_link_libraries(imagick ${ImageMagick_LIBRARIES})


install(TARGETS imagick DESTINATION "${_lua_module_dir}/")

add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/
                   COMMAND rm -rf ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/luaunit.lua ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/input.* ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_BINARY_DIR}/imagick.so ${CMAKE_BINARY_DIR}/tests/)

add_custom_target(unittest
                    COMMAND cp ${CMAKE_SOURCE_DIR}/test/test.lua ${CMAKE_BINARY_DIR}/tests/
                    COMMAND ${LUA_COMMAND} test.lua -v
                    DEPENDS ${CMAKE_BINARY_DIR}/tests/
                    DEPENDS imagick
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)
