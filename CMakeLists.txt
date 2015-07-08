cmake_minimum_required(VERSION 2.8.12)
include(CheckCXXCompilerFlag)

project(gimagick)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(InstallSymlink)

option(USE_GRAPHICSMAGICK "Use GraphicsMagick" ON)
option(USE_IMAGEMAGICK "Use ImageMagick (unsupported)" ON)

option(USE_LUA "Use Lua (also called 'C' Lua) version 5.1 includes" OFF)
option(USE_LUA52 "Use Lua (also called 'C' Lua) version 5.2 includes" OFF)
option(USE_LUAJIT "Use LuaJIT includes instead of 'C' Lua ones (default)" ON)

if(USE_LUAJIT)
    find_package(LuaJIT REQUIRED)
    set(USE_LUA52 OFF)
    set(USE_LUA OFF)
    set(LUA_COMMAND "luajit")
elseif(USE_LUA52)
    find_package(Lua52 REQUIRED)
    set(USE_LUA OFF)
    set(USE_LUAJIT OFF)
    set(LUA_COMMAND "lua")
elseif(USE_LUA)
    find_package(Lua51 REQUIRED)
    set(USE_LUA52 OFF)
    set(USE_LUAJIT OFF)
    set(LUA_COMMAND "lua")
endif()

get_filename_component(_lua_lib_dir ${LUA_LIBRARY} PATH)

if (USE_LUA52)
  set(_lua_module_dir "${_lua_lib_dir}/lua/5.2")
else ()
  set(_lua_module_dir "${_lua_lib_dir}/lua/5.1")
endif ()

set (gimagick_VERSION_MAJOR 1)
set (gimagick_VERSION_MINOR 0)

file(GLOB sources "src/[a-zA-Z]*.c")
file(GLOB private_headers "src/[a-zA-Z]*.h")


include_directories(${gimagick_SOURCE_DIR}/src)
include_directories(${LUA_INCLUDE_DIR})

add_definitions("-Wall")

IF(USE_IMAGEMAGICK)

  find_package(ImageMagick COMPONENTS MagickWand REQUIRED)

  add_library(im SHARED ${sources})
  set_target_properties(im PROPERTIES PREFIX "")
  target_compile_options(im PRIVATE -fopenmp -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16 -DUSE_IMAGEMAGICK -DGIMAGICK_VERSION=\"${gimagick_VERSION_MAJOR}.${gimagick_VERSION_MINOR}-imagick\")
  target_include_directories(im PRIVATE ${ImageMagick_INCLUDE_DIRS})
  target_link_libraries(im ${ImageMagick_LIBRARIES})

  install(TARGETS im DESTINATION "${_lua_module_dir}/gimagick")

ENDIF(USE_IMAGEMAGICK)


IF(USE_GRAPHICSMAGICK)

  find_package(GraphicsMagick COMPONENTS GraphicsMagickWand REQUIRED)

  add_library(gm SHARED ${sources})
  set_target_properties(gm PROPERTIES PREFIX "")
  target_compile_options(gm PRIVATE -DUSE_GRAPHICSMAGICK -DGIMAGICK_VERSION=\"${gimagick_VERSION_MAJOR}.${gimagick_VERSION_MINOR}-gmagick\")
  target_include_directories(gm PRIVATE ${GraphicsMagick_INCLUDE_DIRS})
  target_link_libraries(gm ${GraphicsMagick_LIBRARIES})

  install(TARGETS gm DESTINATION "${_lua_module_dir}/gimagick")

ENDIF(USE_GRAPHICSMAGICK)

IF(USE_GRAPHICSMAGICK)
    InstallSymlink("${_lua_module_dir}/gimagick/gm.so" "${_lua_module_dir}/gimagick.so")
ELSE ()
    InstallSymlink("${_lua_module_dir}/gimagick/im.so" "${_lua_module_dir}/gimagick.so")
ENDIF ()

IF(USE_IMAGEMAGICK)
  IF(USE_GRAPHICSMAGICK)

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/
                   COMMAND rm -rf ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/luaunit.lua ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/input.jpg ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/gimagick/
                   COMMAND cp ${CMAKE_BINARY_DIR}/gm.so ${CMAKE_BINARY_DIR}/tests/gimagick/
                   COMMAND cp ${CMAKE_BINARY_DIR}/im.so ${CMAKE_BINARY_DIR}/tests/gimagick/)

    add_custom_target(unittest
                    COMMAND cp ${CMAKE_SOURCE_DIR}/test/test.lua ${CMAKE_BINARY_DIR}/tests/
                    COMMAND ${LUA_COMMAND} test.lua TestIM TestGM -v
                    DEPENDS ${CMAKE_BINARY_DIR}/tests/
                    DEPENDS gm
                    DEPENDS im
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)

  ELSE ()

    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/
                   COMMAND rm -rf ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/luaunit.lua ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/input.jpg ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/gimagick/
                   COMMAND cp ${CMAKE_BINARY_DIR}/im.so ${CMAKE_BINARY_DIR}/tests/gimagick/)

    add_custom_target(unittest
                    COMMAND cp ${CMAKE_SOURCE_DIR}/test/test.lua ${CMAKE_BINARY_DIR}/tests/
                    COMMAND ${LUA_COMMAND} test.lua TestIM -v
                    DEPENDS ${CMAKE_BINARY_DIR}/tests/
                    DEPENDS im
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)
  ENDIF ()
ELSE ()
  add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/tests/
                   COMMAND rm -rf ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/luaunit.lua ${CMAKE_BINARY_DIR}/tests/
                   COMMAND cp ${CMAKE_SOURCE_DIR}/test/input.jpg ${CMAKE_BINARY_DIR}/tests/
                   COMMAND mkdir ${CMAKE_BINARY_DIR}/tests/gimagick/
                   COMMAND cp ${CMAKE_BINARY_DIR}/gm.so ${CMAKE_BINARY_DIR}/tests/gimagick/)

  add_custom_target(unittest
                    COMMAND cp ${CMAKE_SOURCE_DIR}/test/test.lua ${CMAKE_BINARY_DIR}/tests/
                    COMMAND ${LUA_COMMAND} test.lua TestGM -v
                    DEPENDS ${CMAKE_BINARY_DIR}/tests/
                    DEPENDS gm
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/tests/)


ENDIF ()
