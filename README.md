# Lua IMagick [![Build Status](https://travis-ci.org/isage/lua-imagick.svg?branch=master)](https://travis-ci.org/isage/lua-imagick)

Pure-C lua bindings to ImageMagick

## Why?

Because existing FFI-based bindings are hackish and buggy, duh.

## Table of Contents

* [FAQ](#faq)
* [Requirements](#requirements)
* [Installation](#installation)
* [Usage](#usage)
* [Examples](#examples)
* [Authors](#authors)
* [Copyright and License](#copyright-and-license)

## FAQ

1. Q) Will this work with openresty/nginx-lua?  
   A) Yes. But remember, that IM operations **are** blocking.
2. Q) Is this production ready?  
   A) Yes, we're using it for couple of months now without problems.
3. Q) Is this feature-complete?  
   A) Hell no. There's lot of uncovered IM api. I've implemented only needed for me api for now.
4. Q) How do i properly resize animated gif?  
   A) Firstly, coalesce() it, then resize as usual, then optimize() it back. You really should cache coalesce()'ed image if you resizing it frequently.

## Requirements

* ImageMagick developer headers (>=6.8.8.3)
* Lua (5.1/5.2) or LuaJit
* Cmake 2.8.12 or later
* Working C compiler

You can get fresh IM and cmake for ubuntu 12.04/14.04 [here](https://launchpad.net/~isage-dna/+archive/ubuntu/imagick/)

## Installation

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

## Usage

### Synopsis

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

### Enumerations

#### imagick.gravity

Gravity values  
See [here](http://www.imagemagick.org/api/MagickCore/geometry_8h.html#afd1e527b17eba5305ea949fa7c717069)  
Example  
```lua
local magick = require "imagick"
print(magick.gravity["WestGravity"])
```

#### imagick.interlace

Interlace scheme values  
See [here](https://www.imagemagick.org/api/MagickCore/image_8h.html#af89b808293a7faf805254d1b01e69dc2)  
Example  
```lua
local magick = require "imagick"
print(magick.interlace["JPEGInterlace"])
```

#### imagick.colorspace

Colorspace values  
See [here](http://www.imagemagick.org/api/MagickCore/colorspace_8h.html#a5d516b430fa42c1f83b557f08128f3c2)  

Example  
```lua
local magick = require "imagick"
print(magick.colorspace["YUVColorspace"])
```

#### imagick.filters

Scale filters  
See [here](http://www.imagemagick.org/api/MagickCore/resample_8h.html#a12be80da7313b1cc5a7e1061c0c108ea)  

Example  
```lua
local magick = require "imagick"
print(magick.filters["LanczosSharpFilter"])
```

#### imagick.composite_op

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

#### imagick.font_style

Font styles  

* UndefinedStyle
* NormalStyle
* ItalicStyle
* ObliqueStyle
* AnyStyle

#### imagick.text_align

Font align  

* UndefinedAlign
* LeftAlign
* CenterAlign
* RightAlign

#### imagick.channel

Color channels  
See [here](http://www.imagemagick.org/Magick++/Enumerations.html#ChannelType)

#### imagick.distort_method

methods for MagickDistortImage  
See [here](https://www.imagemagick.org/api/MagickCore/distort_8h.html#a3f53506aaaafd01ef4d52174edfce258)

Color channels  
See [here](http://www.imagemagick.org/Magick++/Enumerations.html#ChannelType)

### imagick functions

#### `<image>image, <string>error = imagick.open(<string> filepath)`

Opens image from given filepath or image definition  

Example  
```lua
local img = magick.open("input.jpg")  -- open jpg file
or
local img = magick.open("xc:rgb(255,0,0)") -- open empty red image
```

******

#### `<image>image, <string>error = imagick.open_blob(<string> data)`

Open image from data blob

******

#### `<image>image, <string>error = imagick.open_pseudo(<int> width, <int> height, <string> definition)`

Create image from pseudo-image definition. See [here](http://www.imagemagick.org/script/formats.php#pseudo)

******

###imagick image functions

#### `<int>width =  img:width()`

Get image width in pixels

******

#### `<int>height =  img:height()`

Get image height in pixels

******

#### `<image>image =  img:clone()`

Clone image with all current settings/values

******

#### `<bool>status, <string>error = img:write(<string> filename)`

Write image to file  
_This outputs only first frame_

******

#### `<bool>status, <string>error = img:write_all(<string> filename,  <bool> join)`

Write all image frames to file  
If **join** is **false** this will create sequentaly numbered file for each image frame
If **join** is **true** this will create one file with all frames (this demends on image format, works with gif, for example)

******

#### `<string>data, <int>lenght = img:blob()`

Return raw image data as string

******

#### `<string>format = img:get_format()`

Get image format ("JPEG"/"GIF"/"PNG"/etc.)

******

#### `<bool>result, <string>error = img:set_format(<string> format)`

Set image format ("JPEG"/"GIF"/"PNG"/etc.)

******

#### `<int>quality = img:get_quality()`

Get image compression quality (0-100)

******

#### `<bool>status, <string>error = img:set_quality(<int> quality)`

Set image compression quality (0-100)

******

#### `<int>gravity = img:get_gravity()`

Get current image gravity

******

#### `<bool>status, <string>error = img:set_gravity(<int> gravity)`

Set image gravity (See imagick.gravity enum)

******

#### `<int>scheme = img:get_interlace()`

Get current image interlace scheme

******

#### `<bool>status, <string>error = img:set_interlace(<int> scheme)`

Set image interlace sheme (See imagick.interlace enum)  
e.g. for Progressive JPEG set it to JPEGInterlace

******

#### `<string>value = img:get_option(<string> name)`

Get imagemagick option for image

******

#### `<bool>status, <string>error = img:set_option(<string> name, <string>value)`

Set imagemagick option for image

******

#### `<string>value = img:get_artifact(<string> name)`

Get imagemagick artifact for image. See [here](http://www.imagemagick.org/script/command-line-options.php#define)

******

#### `<bool>status, <string>error = img:set_artifact(<string> name, <string>value)`

Set imagemagick artifact for image

******

#### `<string>color = img:get_bg_color()`

Get image background color  
Returns comma-separated color values.

******

#### `<bool>status, <string>error = img:set_bg_color(<string> color)`

Set image background color (html hex notation or comma-separated)

******

#### `<int>colorspace = img:get_colorspace()`

Get image colorspace (See imagick.colorspace enum)

******

#### `<bool>status, <string>error = img:set_colorspace(<colorspace> colorspace)`

Set image colorspace (See imagick.colorspace enum)

******

#### `<bool>alpha = img:has_alphachannel()`

Returns true if image has alpha-channel

******

#### `<bool>icc = img:has_icc_profile()`

Returns true if image has embedded icc profile

******

#### `<string>data = img:get_icc_profile()`

Returns image icc profile as blob

******

#### `<bool>status, <string>error = img:set_icc_profile(<string> blob)`

Set (and convert image to) image icc profile from blob

******

#### `<bool>status, <string>error = img:set_compose(<int> compose)`

Set image composite operator (See imagick.composite_op)

******

#### `<bool>status, <string>error = img:set_font(<string> path)`

Set font to use in annotate, full path to font file

******

#### `<bool>status, <string>error = img:set_font_family(<string> family)`

Set font to use in annotate, font family string

******

#### `<bool>status, <string>error = img:set_font_size(<int> size)`
  
Set font size to use in annotate

******

#### `<bool>status, <string>error = img:set_font_style(<int> style)`
  
Set font style to use in annotate (See imagick.font_style enum)

******

#### `<bool>status, <string>error = img:set_font_weight(<int> weight)`

Set font weight to use in annotate

******

#### `<bool>status, <string>error = img:set_font_align(<int> align)`

Set font align to use in annotate (See imagick.font_align enum)

******

#### `<bool>status, <string>error = img:annotate(<string> color, <string> text, <int> x, <int> y, <int> angle)`

Annotate image 

******

#### `<bool>status, <string>error = img:set_mask(<image> mask)`

Set image mask for compositing operations
You can set it to `nil` to reset

******

#### `<bool>status, <string>error = img:coalesce()`

Coalesce (rebuild) all image frames

******

#### `<bool>status, <string>error = img:optimize()`

Optimise all image frames

******

#### `<bool>status, <string>error = img:strip()`

Strip exif data and profiles from image

******

#### `<bool>status, <string>error = img:swirl(<int>degrees)`

Apply swirl filter

******

#### `<bool>status, <string>error = img:oilpaint(<int> radius)`

Apply oilpaint filter

******

#### `<bool>status, <string>error = img:rotate(<string>color, <double> angle)`

Rotate image on angle filling empty space with color

******

#### `<bool>status, <string>error = img:modulate(<double> brightness, <double> saturation, <double> hue)`

Modify brightness, saturation, and hue of an image

******

#### `<bool>status, <string>error = img:blur(<double> sigma, <double> radius)`

Blur image

******

#### `<bool>status, <string>error = img:sharpen(<double> sigma, <double> radius)`

Sharpen image

******

#### `<bool>status, <string>error = img:adaptive_blur(<double> sigma, <double> radius)`

Blur image adaptively

******

#### `<bool>status, <string>error = img:adaptive_sharpen(<double> sigma, <double> radius)`

Sharpen image adaptively

******

#### `<bool>status, <string>error = img:blur_channel(<channel> channel, <double> sigma, <double> radius)`

Blur image channel

******

#### `<bool>status, <string>error = img:sharpen_channel(<channel> channel, <double> sigma, <double> radius)`

Sharpen image channel

******

#### `<bool>status, <string>error = img:adaptive_blur_channel(<channel> channel, <double> sigma, <double> radius)`

Blur image channel adaptively

******

#### `<bool>status, <string>error = img:adaptive_sharpen_channel(<channel> channel, <double> sigma, <double> radius)`

Sharpen image channel adaptively

******

#### `<bool>status, <string>error = img:gamma(<double> gamma)`

Modify image gamma

******

#### `<bool>status, <string>error = img:gamma_channel(<double> gamma, <channel> channel)`

Modify image channel gamma

******

#### `<bool>status, <string>error = img:auto_gamma()`

Auto-adjust image gamma

******

#### `<bool>status, <string>error = img:auto_gamma_channel(<channel> channel)`

Auto-adjust image channel gamma

******

#### `<bool>status, <string>error = img:level(<double> black, <double> white, <double> gamma)`

Adjust image levels. Black/white points is 0-100%

******

#### `<bool>status, <string>error = img:level_channel(<double> black, <double> white, <double> gamma, <channel> channel)`

Adjust image levels for channel. Black/white points is 0-100%

******

#### `<bool>status, <string>error = img:contrast(<bool> sharpen)`

Enhances the intensity differences between the lighter and darker elements of the image.  
Set sharpen to 'true' to increase the image contrast, otherwise the contrast is reduced

******

#### `<bool>status, <string>error = img:border(<strinng> color, <int> width, <int> height)`

Surrounds the image with a border of the color

******

#### `<bool>status, <string>error = img:colorize(<string> color, <double> opacity)`

Blends the fill color with each pixel in the image

******

#### `<bool>status, <string>error = img:resize(<int>width, <int> height)`

Resize image using current scale filter

******

#### `<bool>status, <string>error = img:adaptive_resize(<int>width, <int> height)`

Adaptively resize image with data dependent triangulation using current image filter

******

#### `<bool>status, <string>error = img:resample(<int>width, <int> height, <int> filter, <double> blur)`

Resample image. See [http://www.imagemagick.org/api/magick-image.php#MagickResampleImage](http://www.imagemagick.org/api/magick-image.php#MagickResampleImage)

******

#### `<bool>status, <string>error = img:scale(<int> width, <int> height)`

Fast scale image 

******

#### `<bool>status, <string>error = img:crop(<int> width, <int> height)`

Crop image

******

#### `<bool>status, <string>error = img:thumbnail(<int> width, <int> height)`

Resize image and remove all profiles.

******

#### `<bool>status, <string>error = img:composite(<image> src, <int> x, <int> y, <int> compositeop)`

Apply one image on top of another at x/y with composite operator compositeop (See imagick.composite_op)

******

#### `<bool>status, <string>error = img:composite_channel(<image> src, <channel> channel, <int> x, <int> y, <int> compositeop)`

Apply one image channel on top of another at x/y with composite operator compositeop (See imagick.composite_op)

******

#### `<bool>status, <string>error = img:extent(<int> width, <int> height)`

Extent image

******

#### `<bool>status, <string>error = img:smart_resize(<string> size)`

Smartly resize image.  
Format is one of:
* WxH (Keep aspect-ratio, use higher dimension)
* WxH^ (Keep aspect-ratio, use lower dimension (crop))
* WxH! (Ignore aspect-ratio)

It uses Mitchell filter for upscaling/downscaling all formats and Lanczos for downscaling JPEG.  

You should use img:extent after it to really crop or add borders (with `img:get_bg_color()`) to image.

******

#### `<bool>status, <string>error = img:set_fill_color(<string> color)`

Set DrawingWand fill color

******

#### `<bool>status, <string>error = img:set_stroke_color(<string> color)`

Set DrawingWand stroke color

******

#### `img:query_metrics(<string> text)`

Query font metrics. See http://www.imagemagick.org/api/magick-wand.php#MagickQueryFontMetrics
Returns 13 <double> variables or nil,<string>error

******

#### `<bool>status, <string>error = img:distort(<distort_method> method, <array> params, <bool> bestfit)`

Distorts image. See https://www.imagemagick.org/api/magick-image.php#MagickDistortImage  
Example (http://www.imagemagick.org/Usage/distorts/#arc):  
```lua
local magick = require "imagick"
local img = magick.open("rose.jpg")

img:distort(magick.distort_method["ArcDistortion"], { 60, 90 }, false)
img:write("out.jpg")

```


******


## Examples

### Captcha
```lua
local magick = require "imagick"

local code ="hello"

local i = 0
math.randomseed(os.time())
local r=100+math.random(35)
local g=100+math.random(35)
local b=100+math.random(35)


local img = assert(magick.open_pseudo(150,50, "xc:rgb("..r..","..g..","..b..")"))

for c in code:gmatch(".") do
  local r=150+math.random(105)
  local g=150+math.random(105)
  local b=150+math.random(105)
  img:set_font_size(25+math.random(10))
  img:annotate("rgb("..r..","..g..","..b..")", c, (i*20)+10, math.random(50-50)+30, math.random(55)-20)
  i=i+1
end

img:swirl(10)
img:oilpaint(1)
img:write("captcha.png")

```

### Simple filters

#### Gotham
```lua
local ima = require("imagick")

local img, err = ima.open("input.jpg")
img:modulate(120, 10 ,100)
img:colorize("#222b6d", 0.2)
img:gamma(0.5)
img:contrast(true)
img:contrast(true)
img:border('black', 20, 20)

img:write("out.jpg")

```

#### Lomo
```lua
local ima = require("imagick")

local img, err = ima.open("input.jpg")
img:level_channel(33, 66, 1.0, ima.channel["RedChannel"])
img:level_channel(33, 66, 1.0, ima.channel["GreenChannel"])

crop_x = math.floor(img:width() * 1.5)
crop_y = math.floor(img:height() * 1.5)

local gradient = ima.open_pseudo(crop_x, crop_y, "radial-gradient:none-black")

gradient:set_gravity(ima.gravity['CenterGravity'])
gradient:crop(img:width(), img:height())
img:composite(gradient, 0, 0, ima.composite_op['MultiplyCompositeOp'])

img:border('black', 20, 20)

img:write("out.jpg")

```


## Authors

Epifanov Ivan <isage.dna@gmail.com>

[Back to TOC](#table-of-contents)

## Copyright and License

This module is licensed under the WTFPL license.  
(See LICENSE)