#Lua IMagick [![Build Status](https://travis-ci.org/isage/lua-imagick.svg?branch=master)](https://travis-ci.org/isage/lua-imagick)

Pure-C lua bindings to ImageMagick

##Why?

Because existing FFI-based bindings are hackish and buggy, duh.

##Table of Contents

* [FAQ](#faq)
* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)

##FAQ

1. Q) Will this work with openresty/nginx-lua?  
   A) Yes. But remember, that IM operations **are** blocking.
2. Q) Is this production ready?  
   A) Yes, we're using it for couple of months now without problems.
3. Q) Is this feature-complete?  
   A) Hell no. There's lot of uncovered IM api. I've implemented only needed for me api for now.
4. Q) How do i properly resize animated gif?  
   A) Firstly, coalesce() it, then resize as usual, then optimize() it back. You really should cache coalesce()'ed image if you resizing it frequently.

##Requirements

* ImageMagick developer headers (>=6.8.8.3)
* Lua (5.1/5.2) or LuaJit
* Cmake 2.8.12 or later
* Working C compiler

You can get fresh IM and cmake for ubuntu 12.04/14.04 [here](https://launchpad.net/~isage-dna/+archive/ubuntu/imagick/)

##Installation

As easy as  
```bash
mkdir build
cd build
cmake ..
make
make install
```

You can also use `make unittest` after make to run tests.  
By default module compiles with support for luajit  
For other Lua interpreters see cmake options.

##Usage

###Synopsis

```lua
local magick = require "imagick"

local img = magick.open("filename.jpg")
img:set_gravity(magick.gravity["NorthGravity"])
img:smart_resize("100x100^")
img:extent(100, 100)
img:set_quality(90)
img:strip()
img:write("out.jpg")
```

###Enumerations

####imagick.gravity

Gravity values  
See [here](http://www.imagemagick.org/api/MagickCore/geometry_8h.html#afd1e527b17eba5305ea949fa7c717069)  
Example  
```lua
local magick = require "imagick"
print(magick.gravity["WestGravity"])
```

####imagick.colorspace

Colorspace values  
See [here](http://www.imagemagick.org/api/MagickCore/colorspace_8h.html#a5d516b430fa42c1f83b557f08128f3c2)  

Example  
```lua
local magick = require "imagick"
print(magick.colorspace["YUVColorspace"])
```

####imagick.filters

Scale filters  
See [here](http://www.imagemagick.org/api/MagickCore/resample_8h.html#a12be80da7313b1cc5a7e1061c0c108ea)  

Example  
```lua
local magick = require "imagick"
print(magick.filters["LanczosSharpFilter"])
```

####imagick.composite_op

Composite operations  
See [here](http://www.imagemagick.org/api/MagickCore/composite_8h.html#a55ded0ef54def8597243db2375b987fb)  

Example  
```lua
local magick = require "imagick"
local img = magick.open("input.jpg")
local logo = magick.open("logo.jpg")
img:set_compose( magick.composite_op["CopyCompositeOp"] )
img:composite(logo, 0, 0)
img:write("out.jpg")
```

####imagick.font_style

Font styles  

* UndefinedStyle
* NormalStyle
* ItalicStyle
* ObliqueStyle
* AnyStyle

####imagick.text_align

Font align  

* UndefinedAlign
* LeftAlign
* CenterAlign
* RightAlign

####imagick.channel

Color channels  
See [here](http://www.imagemagick.org/Magick++/Enumerations.html#ChannelType)

###imagick functions

####`<image>image, <string>error = imagick.open(<string> filepath)`

Opens image from given filepath or image definition  

Example  
```lua
local img = magick.open("input.jpg")  -- open jpg file
or
local img = magick.open("xc:rgb(255,0,0)") -- open empty red image
```

******

####`<image>image, <string>error = imagick.open_blob(<string> data)`

Open image from data blob

******

###imagick image functions

####`<int>width =  img:width()`

Get image width in pixels

******

####`<int>height =  img:height()`

Get image height in pixels

******

####`<bool>status, <string>error = img:write(<string> filename)`

Write image to file  
_This outputs only first frame_

******

####`<bool>status, <string>error = img:write_all(<string> filename,  <bool> join)`

Write all image frames to file  
If **join** is **false** this will create sequentaly numbered file for each image frame
If **join** is **true** this will create one file with all frames (this demends on image format, works with gif, for example)

******

####`<string>data, <int>lenght = img:blob()`

Return raw image data as string

******

####`<string>format = img:get_format()`

Get image format ("JPEG"/"GIF"/"PNG"/etc.)

******

####`<bool>result, <string>error = img:set_format(<string> format)`

Set image format ("JPEG"/"GIF"/"PNG"/etc.)

******

####`<int>quality = img:get_quality()`

Get image compression quality (0-100)

******

####`<bool>status, <string>error = img:set_quality(<int> quality)`

Set image compression quality (0-100)

******

####`<int>gravity = img:get_gravity()`

Get current image gravity

******

####`<bool>status, <string>error = img:set_gravity(<int> gravity)`

Set image gravity (See imagick.gravity enum)

******

####`<string>value = img:get_option(<string> name)`

Get imagemagick option for image

******

####`<bool>status, <string>error = img:set_option(<string> name, <string>value)`

Set imagemagick option for image

******

####`<string>color = img:get_bg_color()`

Get image background color  
Returns comma-separated color values.

******

####`<bool>status, <string>error = img:set_bg_color(<string> color)`

Set image background color (html hex notation or comma-separated)

******

####`<int>colorspace = img:get_colorspace()`

Get image colorspace (See imagick.colorspace enum)

******

####`<bool>alpha = img:has_alphachannel()`

Returns true if image has alpha-channel

******

####`<bool>icc = img:has_icc_profile()`

Returns true if image has embedded icc profile

******

####`<string>data = img:get_icc_profile()`

Returns image icc profile as blob

******

####`<bool>status, <string>error = img:set_icc_profile(<string> blob)`

Set (and convert image to) image icc profile from blob

******

####`<bool>status, <string>error = img:set_compose(<int> compose)`

Set image composite operator (See imagick.composite_op)

******

####`<bool>status, <string>error = img:set_font(<string> path)`

Set font to use in annotate, full path to font file

******

####`<bool>status, <string>error = img:set_font_family(<string> family)`

Set font to use in annotate, font family string

******

####`<bool>status, <string>error = img:set_font_size(<int> size)`
  
Set font size to use in annotate

******

####`<bool>status, <string>error = img:set_font_style(<int> style)`
  
Set font style to use in annotate (See imagick.font_style enum)

******

####`<bool>status, <string>error = img:set_font_weight(<int> weight)`

Set font weight to use in annotate

******

####`<bool>status, <string>error = img:set_font_align(<int> align)`

Set font align to use in annotate (See imagick.font_align enum)

******

####`<bool>status, <string>error = img:annotate(<string> color, <string> text, <int> x, <int> y, <int> angle)`

Annotate image 

******

####`<bool>status, <string>error = img:coalesce()`

Coalesce (rebuild) all image frames

******

####`<bool>status, <string>error = img:optimize()`

Optimise all image frames

******

####`<bool>status, <string>error = img:strip()`

Strip exif data and profiles from image

******

####`<bool>status, <string>error = img:swirl(<int>degrees)`

Apply swirl filter

******

####`<bool>status, <string>error = img:oilpaint(<int> radius)`
  
Apply oilpaint filter

******

####`<bool>status, <string>error = img:blur(<int> sigma, <int> radius)`

Blur image

******

####`<bool>status, <string>error = img:sharpen(<int> sigma, <int> radius)`

Sharpen image

******

####`<bool>status, <string>error = img:adaptive_blur(<int> sigma, <int> radius)`

Blur image adaptively

******

####`<bool>status, <string>error = img:adaptive_sharpen(<int> sigma, <int> radius)`

Sharpen image adaptively

******

####`<bool>status, <string>error = img:blur_channel(<channel> channel, <int> sigma, <int> radius)`

Blur image channel

******

####`<bool>status, <string>error = img:sharpen_channel(<channel> channel, <int> sigma, <int> radius)`

Sharpen image channel

******

####`<bool>status, <string>error = img:adaptive_blur_channel(<channel> channel, <int> sigma, <int> radius)`

Blur image channel adaptively

******

####`<bool>status, <string>error = img:adaptive_sharpen_channel(<channel> channel, <int> sigma, <int> radius)`

Sharpen image channel adaptively

******

####`<bool>status, <string>error = img:resize(<int>width, <int> height)`

Resize image using current scale filter

******

####`<bool>status, <string>error = img:adaptive_resize(<int>width, <int> height)`

Adaptively resize image with data dependent triangulation using current image filter

******

####`<bool>status, <string>error = img:resample(<int>width, <int> height, <int> filter, <double> blur)`

Resample image. See [http://www.imagemagick.org/api/magick-image.php#MagickResampleImage](http://www.imagemagick.org/api/magick-image.php#MagickResampleImage)

******

####`<bool>status, <string>error = img:scale(<int> width, <int> height)`

Fast scale image 

******

####`<bool>status, <string>error = img:crop(<int> width, <int> height)`

Crop image

******

####`<bool>status, <string>error = img:thumbnail(<int> width, <int> height)`

Resize image and remove all profiles.

******

####`<bool>status, <string>error = img:composite(<image> src, <int> x, <int> y, <int> compositeop)`

Apply one image on top of another at x/y with composite operator compositeop (See imagick.composite_op)

******

####`<bool>status, <string>error = img:extent(<int> width, <int> height)`

Extent image

******

####`<bool>status, <string>error = img:smart_resize(<string> size)`

Smartly resize image.  
Format is one of:
* WxH (Keep aspect-ratio, use higher dimension)
* WxH^ (Keep aspect-ratio, use lower dimension (crop))
* WxH! (Ignore aspect-ratio)

It uses Mitchell filter for upscaling/downscaling all formats and Lanczos for downscaling JPEG.  

You should use img:extent after it to really crop or add borders (with `img:get_bg_color()`) to image.

******

##Authors

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

##Copyright and License

This module is licensed under the WTFPL license.  
(See LICENSE)