package = "imagick"
version = "0.0.1-1"
source = {
   url = "git://github.com/mediba-system/lua-imagick.git",
   tag = "master",
}
description = {
   summary = "Lua IMagick",
   detailed = "Pure-C lua bindings to ImageMagick",
   homepage = "https://github.com/isage/lua-imagick",
   license = "MIT"
}
dependencies = {}
external_dependencies = {
   -- g++
   -- libluajit-5.1-dev
   -- libmagickwand-dev
}
build = {
   type = 'cmake',
   variables = {
      INSTALL_CMOD      = "$(LIBDIR)",
      CMAKE_BUILD_TYPE  = "$(CMAKE_BUILD_TYPE)",
      ["CFLAGS:STRING"] = "$(CFLAGS)",
   },
}
