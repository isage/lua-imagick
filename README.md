Lua G.I. Magick
===============

Pure-C lua bindings to Imagemagick and GraphicsMagick

Table of Contents
=================

* [Requirements](#requirements)
* [Installation](#installation)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)


Requirements
============

* GraphicksMagick and/or ImageMagick
* Lua (5.1/5.2) or LuaJit
* Cmake 2.8.12 or later
* Working C compiler

Installation
============

As easy as  
```
mkdir build
cd build
cmake ..
make
make install
```

You can also use `make unittest` after make to run tests.  
By default module compiles with support for luajit and grahicksmagick  
For ImageMagick and other Lua interpreters see cmake options.

Authors
=======

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

Copyright and License
=====================

This module is licensed under the WTFPL license.  
(See LICENSE)