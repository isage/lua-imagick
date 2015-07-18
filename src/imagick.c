#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <wand/magick_wand.h>

#define IMG_METATABLE "image-im metatable"

#ifndef IMAGICK_MODNAME
#define IMAGICK_MODNAME   "imagick"
#endif

#ifndef IMAGICK_VERSION
#define IMAGICK_VERSION   "1.0"
#endif

typedef struct {
  MagickWand* m_wand;
  PixelWand* p_wand;
  DrawingWand* d_wand;
} LuaImage;

static LuaImage* checkimage(lua_State* L)
{
  void* ud = luaL_checkudata(L, 1, IMG_METATABLE);
  luaL_argcheck(L, ud != NULL, 1, "'image' expected");
  return (LuaImage *)ud;
}

static int imagick_open(lua_State* L)
{
  const char* path = luaL_checkstring(L, 1);
  
  LuaImage* a = (LuaImage* )lua_newuserdata(L, sizeof(LuaImage));

  luaL_getmetatable(L, IMG_METATABLE);
  lua_setmetatable(L, -2);

  // init magickwand
  if (IsMagickWandInstantiated() == MagickFalse)
  {
    MagickWandGenesis();
  }

  a->m_wand = NewMagickWand();
  a->p_wand = NewPixelWand();
  a->d_wand = NewDrawingWand();
  if (MagickReadImage(a->m_wand, path) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushnil(L);
    lua_pushstring(L, error);
    return 2;
  }

  return 1;  /* new userdatum is already on the stack */
}

static int imagick_destroy(lua_State* L)
{
  LuaImage* a = checkimage(L);
  DestroyMagickWand(a->m_wand);
  DestroyPixelWand(a->p_wand);
  DestroyDrawingWand(a->d_wand);
  return 0;
}

int imagick_tostring(lua_State* L)
{
  LuaImage* a = checkimage(L);
  int w = MagickGetImageWidth(a->m_wand);
  int h = MagickGetImageHeight(a->m_wand);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushfstring(L, "%s(%dx%d)", format, w, h);
  MagickRelinquishMemory(format);
  return 1;
}

static int imagick_width(lua_State* L)
{
  LuaImage *a = checkimage(L);
  int w = MagickGetImageWidth(a->m_wand);
  lua_pushnumber(L, w);
  return 1;
}

static int imagick_height(lua_State* L)
{
  LuaImage *a = checkimage(L);
  int h = MagickGetImageHeight(a->m_wand);
  lua_pushnumber(L, h);
  return 1;
}

static int imagick_write(lua_State* L)
{
  LuaImage *a = checkimage(L);
  const char* path = luaL_checkstring(L, 2);

  if (MagickWriteImage(a->m_wand, path) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;

  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_write_all(lua_State* L)
{
  LuaImage *a = checkimage(L);
  const char* path = luaL_checkstring(L, 2);
  const int adjoin = lua_toboolean(L, 3);

  if (MagickWriteImages(a->m_wand, path, adjoin) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_format(lua_State* L)
{
  LuaImage* a = checkimage(L);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushstring(L,format);
  MagickRelinquishMemory(format);
  return 1;
}

static int imagick_set_format(lua_State* L)
{
  LuaImage* a = checkimage(L);
  const char* format = luaL_checkstring(L, 2);
  if (MagickSetImageFormat(a->m_wand, format) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_quality(lua_State* L)
{
  LuaImage* a = checkimage(L);

  size_t quality = MagickGetImageCompressionQuality(a->m_wand);
  lua_pushinteger(L, quality);
  return 1;
}

static int imagick_set_quality(lua_State* L)
{
  LuaImage* a = checkimage(L);
  const size_t quality = luaL_checkinteger(L, 2);

  if (MagickSetImageCompressionQuality(a->m_wand, quality) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_blob(lua_State* L)
{
  size_t length;

  LuaImage* a = checkimage(L);

  unsigned char* data = MagickGetImageBlob(a->m_wand, &length);

  lua_pushlstring(L, (const char*)data, length);
  lua_pushinteger(L, length);

  MagickRelinquishMemory(data);

  return 2;
}

static int imagick_get_gravity(lua_State* L)
{
  LuaImage* a = checkimage(L);

  size_t gravity = MagickGetImageGravity(a->m_wand);
  lua_pushinteger(L, gravity);
  return 1;
}

static int imagick_set_gravity(lua_State* L)
{
  LuaImage* a = checkimage(L);
  const size_t gravity = luaL_checkinteger(L, 2);

  if (gravity > StaticGravity)
  {
    lua_pushboolean(L, 0);
    lua_pushstring(L, "Wrong gravity type");
    return 2;
  }

  if (MagickSetImageGravity(a->m_wand, gravity) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_option(lua_State* L)
{
  LuaImage* a = checkimage(L);
  const char* key = luaL_checkstring(L,2);

  char* value = MagickGetOption(a->m_wand, key);
  lua_pushstring(L, value);
  MagickRelinquishMemory(value);
  return 1;
}

static int imagick_set_option(lua_State* L)
{
  LuaImage* a = checkimage(L);

  const char* key = luaL_checkstring(L, 2);
  const char* value = luaL_checkstring(L, 3);

  if (MagickSetOption(a->m_wand, key, value) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_coalesce(lua_State* L)
{
  LuaImage* a = checkimage(L);

  MagickWand* tempwand;
  if ((tempwand=MagickCoalesceImages(a->m_wand)) != NULL)
  {
    DestroyMagickWand(a->m_wand);
    a->m_wand = tempwand;
    lua_pushboolean(L, 1);
    return 1;
  }
  else
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
}

static int imagick_optimize(lua_State* L)
{
  LuaImage* a = checkimage(L);

  MagickWand* tempwand;
  if ((tempwand=MagickOptimizeImageLayers(a->m_wand)) != NULL)
  {
    DestroyMagickWand(a->m_wand);
    a->m_wand = tempwand;
    lua_pushboolean(L, 1);
    return 1;
  }
  else
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
}

static int imagick_strip(lua_State* L)
{
  LuaImage* a = checkimage(L);

  if (MagickStripImage(a->m_wand) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_swirl(lua_State* L)
{
  LuaImage* a = checkimage(L);
  double degrees = luaL_checknumber(L,2);
  if (MagickSwirlImage(a->m_wand, degrees) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_oilpaint(lua_State* L)
{
  LuaImage* a = checkimage(L);
  double radius = luaL_checknumber(L,2);
  if (MagickOilPaintImage(a->m_wand, radius) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_blur(lua_State* L)
{
  LuaImage* a = checkimage(L);
  double sigma = luaL_checknumber(L,2);
  double radius = luaL_checknumber(L,3);

  if (MagickBlurImage(a->m_wand, sigma, radius) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_sharpen(lua_State* L)
{
  LuaImage* a = checkimage(L);
  double sigma = luaL_checknumber(L,2);
  double radius = luaL_checknumber(L,3);

  if (MagickSharpenImage(a->m_wand, sigma, radius) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }
  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_colorspace(lua_State* L)
{
  LuaImage* a = checkimage(L);
  lua_pushnumber(L, MagickGetImageColorspace(a->m_wand));
  return 1;
}

static int imagick_has_alphachannel(lua_State* L)
{
  LuaImage* a = checkimage(L);
  lua_pushboolean(L, MagickGetImageAlphaChannel(a->m_wand));
  return 1;
}

static int imagick_get_icc_profile(lua_State* L)
{
  LuaImage* a = checkimage(L);
  size_t length;
  unsigned char* data = MagickGetImageProfile(a->m_wand, "ICC", &length);

  lua_pushlstring(L, (const char*)data, length);
  lua_pushinteger(L, length);

  MagickRelinquishMemory(data);

  return 2;
}

static int imagick_set_icc_profile(lua_State* L)
{
  LuaImage* a = checkimage(L);
  size_t length;
  const char* data = luaL_checklstring(L, 2, &length);

  if (MagickProfileImage(a->m_wand, "ICC", data, length) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_compose(lua_State* L)
{
  LuaImage* a = checkimage(L);
  int compose = luaL_checkinteger(L, 2);

  if (MagickSetImageCompose(a->m_wand, compose) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_bg_color(lua_State* L)
{
  LuaImage* a = checkimage(L);
  const char* color = luaL_checkstring(L, 2);

  PixelSetColor(a->p_wand, color);
  char* format = MagickGetImageFormat(a->m_wand);

  if (!strcmp(format, "gif"))
  {
    MagickResetIterator(a->m_wand);
    while (MagickNextImage(a->m_wand) == 1)
    {
      if (MagickSetImageBackgroundColor(a->m_wand, a->p_wand) != MagickTrue)
      {
        ExceptionType severity;
        char* error=MagickGetException(a->m_wand, &severity);
        lua_pushboolean(L, 0);
        lua_pushstring(L, error);
        return 2;
      }
    }
    MagickResetIterator(a->m_wand);
  }
  else
  {
    if (MagickSetImageBackgroundColor(a->m_wand, a->p_wand) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_bg_color(lua_State* L)
{
  LuaImage* a = checkimage(L);

  MagickGetImageBackgroundColor(a->m_wand, a->p_wand);
  char* color = PixelGetColorAsNormalizedString(a->p_wand);
  lua_pushstring(L, color);
  MagickRelinquishMemory(color);
  return 1;
}


static const struct luaL_Reg imagicklib_f[] = {
  {"open", imagick_open},
  {NULL, NULL}
};

static const struct luaL_Reg imagicklib_meta[] = {
  {"__tostring", imagick_tostring},
  {"__gc",       imagick_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg imagicklib_m[] = {
  {"width",             imagick_width},
  {"height",            imagick_height},
  {"write",             imagick_write},
  {"write_all",         imagick_write_all},
  {"get_format",        imagick_get_format},
  {"set_format",        imagick_set_format},
  {"get_quality",       imagick_get_quality},
  {"set_quality",       imagick_set_quality},
  {"blob",              imagick_blob},
  {"get_gravity",       imagick_get_gravity},
  {"set_gravity",       imagick_set_gravity},
  {"get_option",        imagick_get_option},
  {"set_option",        imagick_set_option},
  {"coalesce",          imagick_coalesce},
  {"optimize",          imagick_optimize},
  {"strip",             imagick_strip},
  {"swirl",             imagick_swirl},
  {"oilpaint",          imagick_oilpaint},
  {"blur",              imagick_blur},
  {"sharpen",           imagick_sharpen},
  {"get_colorspace",    imagick_get_colorspace},
  {"has_alphachannel",  imagick_has_alphachannel},
  {"get_icc_profile",   imagick_get_icc_profile},
  {"set_icc_profile",   imagick_set_icc_profile},
  {"set_compose",       imagick_set_compose},
  {"set_bg_color",      imagick_set_bg_color},
  {"get_bg_color",      imagick_get_bg_color},

  /*
    TODO:

    annotate = function(self, color, size, x, y, angle, text)

    smart_resize = function(self, sizestr)
    resize = function(self, w, h, f, blur)
    adaptive_resize = function(self, w, h)
    scale = function(self, w, h)
    crop = function(self, w, h, x, y)
    extent = function(self, w, h)

    composite = function(self, blob, x, y, opstr)

    get_pixel = function(self, x, y)

*/
  {NULL, NULL}
};


void maketable(lua_State* L, const char* t, char** ar, size_t size)
{
  lua_newtable(L);
  int i;
  for(i = 0;i<size;i++)
  {
    lua_pushnumber(L, i);
    lua_setfield(L, -2, ar[i]);
  }
  lua_setfield(L, -2, t);
}


int luaopen_imagick(lua_State* L)
{
  // image "class"
  luaL_newmetatable(L, IMG_METATABLE);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, imagicklib_meta , 0);
#else
  luaL_register(L, NULL, imagicklib_meta);
#endif

  lua_newtable(L);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, imagicklib_m , 0);
#else
  luaL_register(L, NULL, imagicklib_m);
#endif

  lua_setfield(L, -2, "__index");

  // gimagick "class"
  lua_newtable(L);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, imagicklib_f , 0);
#else
  luaL_register(L, NULL, imagicklib_f);
#endif

  lua_pushliteral(L, IMAGICK_MODNAME);
  lua_setfield(L, -2, "_NAME");
  lua_pushliteral(L, IMAGICK_VERSION);
  lua_setfield(L, -2, "_VERSION");

  // enums

  // gravity
  char* gravity[] = {
    "ForgetGravity",
    "NorthWestGravity",
    "NorthGravity",
    "NorthEastGravity",
    "WestGravity",
    "CenterGravity",
    "EastGravity",
    "SouthWestGravity",
    "SouthGravity",
    "SouthEastGravity",
    "StaticGravity"
  };
  maketable(L, "gravity", gravity, 11);

  // colorspace
  char* colorspace[] = {
    "UndefinedColorspace",
    "RGBColorspace",            /* Linear RGB colorspace */
    "GRAYColorspace",           /* greyscale (linear) image (faked 1 channel) */
    "TransparentColorspace",
    "OHTAColorspace",
    "LabColorspace",
    "XYZColorspace",
    "YCbCrColorspace",
    "YCCColorspace",
    "YIQColorspace",
    "YPbPrColorspace",
    "YUVColorspace",
    "CMYKColorspace",           /* negated linear RGB with black separated */
    "sRGBColorspace",           /* Default: non-linear sRGB colorspace */
    "HSBColorspace",
    "HSLColorspace",
    "HWBColorspace",
    "Rec601LumaColorspace",
    "Rec601YCbCrColorspace",
    "Rec709LumaColorspace",
    "Rec709YCbCrColorspace",
    "LogColorspace",
    "CMYColorspace",            /* negated linear RGB colorspace */
    "LuvColorspace",
    "HCLColorspace",
    "LCHColorspace",            /* alias for LCHuv */
    "LMSColorspace",
    "LCHabColorspace",          /* Cylindrical (Polar) Lab */
    "LCHuvColorspace",          /* Cylindrical (Polar) Luv */
    "scRGBColorspace",
    "HSIColorspace",
    "HSVColorspace",            /* alias for HSB */
    "HCLpColorspace",
    "YDbDrColorspace"
  };
  maketable(L, "colorspace", colorspace, 34);

  // filters
  char* filters[] = {
    "UndefinedFilter",
    "PointFilter",
    "BoxFilter",
    "TriangleFilter",
    "HermiteFilter",
    "HanningFilter",
    "HammingFilter",
    "BlackmanFilter",
    "GaussianFilter",
    "QuadraticFilter",
    "CubicFilter",
    "CatromFilter",
    "MitchellFilter",
    "JincFilter",
    "SincFilter",
    "SincFastFilter",
    "KaiserFilter",
    "WelshFilter",
    "ParzenFilter",
    "BohmanFilter",
    "BartlettFilter",
    "LagrangeFilter",
    "LanczosFilter",
    "LanczosSharpFilter",
    "Lanczos2Filter",
    "Lanczos2SharpFilter",
    "RobidouxFilter",
    "RobidouxSharpFilter",
    "CosineFilter",
    "SplineFilter",
    "LanczosRadiusFilter",
    "SentinelFilter"  /* a count of all the filters, not a real filter */
  };
  maketable(L, "filters", filters, 32);

  // composite operation
  char* composite_op[] = {
    "UndefinedCompositeOp",
    "NoCompositeOp",
    "ModulusAddCompositeOp",
    "AtopCompositeOp",
    "BlendCompositeOp",
    "BumpmapCompositeOp",
    "ChangeMaskCompositeOp",
    "ClearCompositeOp",
    "ColorBurnCompositeOp",
    "ColorDodgeCompositeOp",
    "ColorizeCompositeOp",
    "CopyBlackCompositeOp",
    "CopyBlueCompositeOp",
    "CopyCompositeOp",
    "CopyCyanCompositeOp",
    "CopyGreenCompositeOp",
    "CopyMagentaCompositeOp",
    "CopyOpacityCompositeOp",
    "CopyRedCompositeOp",
    "CopyYellowCompositeOp",
    "DarkenCompositeOp",
    "DstAtopCompositeOp",
    "DstCompositeOp",
    "DstInCompositeOp",
    "DstOutCompositeOp",
    "DstOverCompositeOp",
    "DifferenceCompositeOp",
    "DisplaceCompositeOp",
    "DissolveCompositeOp",
    "ExclusionCompositeOp",
    "HardLightCompositeOp",
    "HueCompositeOp",
    "InCompositeOp",
    "LightenCompositeOp",
    "LinearLightCompositeOp",
    "LuminizeCompositeOp",
    "MinusDstCompositeOp",
    "ModulateCompositeOp",
    "MultiplyCompositeOp",
    "OutCompositeOp",
    "OverCompositeOp",
    "OverlayCompositeOp",
    "PlusCompositeOp",
    "ReplaceCompositeOp",
    "SaturateCompositeOp",
    "ScreenCompositeOp",
    "SoftLightCompositeOp",
    "SrcAtopCompositeOp",
    "SrcCompositeOp",
    "SrcInCompositeOp",
    "SrcOutCompositeOp",
    "SrcOverCompositeOp",
    "ModulusSubtractCompositeOp",
    "ThresholdCompositeOp",
    "XorCompositeOp",
    "DivideDstCompositeOp",
    "DistortCompositeOp",
    "BlurCompositeOp",
    "PegtopLightCompositeOp",
    "VividLightCompositeOp",
    "PinLightCompositeOp",
    "LinearDodgeCompositeOp",
    "LinearBurnCompositeOp",
    "MathematicsCompositeOp",
    "DivideSrcCompositeOp",
    "MinusSrcCompositeOp",
    "DarkenIntensityCompositeOp",
    "LightenIntensityCompositeOp"
  };
  maketable(L, "composite_op", composite_op, 68);
  return 1;
}
