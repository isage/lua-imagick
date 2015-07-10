#include <stdio.h>
#include <string.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <wand/magick_wand.h>

#if defined(USE_IMAGEMAGICK)
#define IMG_METATABLE "image-im metatable"
#else
#define IMG_METATABLE "image-gm metatable"
#endif

#ifndef GIMAGICK_MODNAME
#define GIMAGICK_MODNAME   "gimagick"
#endif

#ifndef GIMAGICK_VERSION
#define GIMAGICK_VERSION   "1.0"
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

static int gimagick_open(lua_State* L)
{
  const char* path = luaL_checkstring(L, 1);
  
  LuaImage* a = (LuaImage* )lua_newuserdata(L, sizeof(LuaImage));

  luaL_getmetatable(L, IMG_METATABLE);
  lua_setmetatable(L, -2);

#if defined(USE_IMAGEMAGICK)
  // init magickwand
  if (IsMagickWandInstantiated() == MagickFalse)
  {
    MagickWandGenesis();
  }
#endif

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

int gimagick_api(lua_State* L)
{
#if defined(USE_IMAGEMAGICK)
  lua_pushfstring(L, "im");
#else
  lua_pushfstring(L, "gm");
#endif
  return 1;
}


static int gimagick_destroy(lua_State* L)
{
  LuaImage* a = checkimage(L);
  DestroyMagickWand(a->m_wand);
  DestroyPixelWand(a->p_wand);
  DestroyDrawingWand(a->d_wand);
  return 0;
}

int gimagick_tostring(lua_State* L)
{
  LuaImage* a = checkimage(L);
  int w = MagickGetImageWidth(a->m_wand);
  int h = MagickGetImageHeight(a->m_wand);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushfstring(L, "%s(%dx%d)", format, w, h);
  MagickRelinquishMemory(format);
  return 1;
}


static int gimagick_getwidth(lua_State* L)
{
  LuaImage *a = checkimage(L);
  int w = MagickGetImageWidth(a->m_wand);
  lua_pushnumber(L, w);
  return 1;
}

static int gimagick_getheight(lua_State* L)
{
  LuaImage *a = checkimage(L);
  int h = MagickGetImageHeight(a->m_wand);
  lua_pushnumber(L, h);
  return 1;
}

static int gimagick_write(lua_State* L)
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

static int gimagick_write_all(lua_State* L)
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

static int gimagick_get_format(lua_State* L)
{
  LuaImage* a = checkimage(L);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushstring(L,format);
  MagickRelinquishMemory(format);
  return 1;
}

static int gimagick_set_format(lua_State* L)
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

static const struct luaL_Reg gimagicklib_f[] = {
  {"open", gimagick_open},
  {"api", gimagick_api},
  {NULL, NULL}
};

static const struct luaL_Reg gimagicklib_meta[] = {
  {"__tostring", gimagick_tostring},
  {"__gc",       gimagick_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg gimagicklib_m[] = {
  {"width",       gimagick_getwidth},
  {"height",      gimagick_getheight},
  {"write",       gimagick_write},
  {"write_all",   gimagick_write_all},
  {"get_format",  gimagick_get_format},
  {"set_format",  gimagick_set_format},
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


int luaopen_gimagick(lua_State* L)
{
  // image "class"
  luaL_newmetatable(L, IMG_METATABLE);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, gimagicklib_meta , 0);
#else
  luaL_register(L, NULL, gimagicklib_meta);
#endif

  lua_newtable(L);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, gimagicklib_m , 0);
#else
  luaL_register(L, NULL, gimagicklib_m);
#endif

  lua_setfield(L, -2, "__index");

  // gimagick "class"
  lua_newtable(L);
#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L, gimagicklib_f , 0);
#else
  luaL_register(L, NULL, gimagicklib_f);
#endif

  lua_pushliteral(L, GIMAGICK_MODNAME);
  lua_setfield(L, -2, "_NAME");
  lua_pushliteral(L, GIMAGICK_VERSION);
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
#if defined(USE_IMAGEMAGICK)

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

#else

  char* colorspace[] = {
    "UndefinedColorspace",
    "RGBColorspace",         /* Plain old RGB colorspace */
    "GRAYColorspace",        /* Plain old full-range grayscale */
    "TransparentColorspace", /* RGB but preserve matte channel during quantize */
    "OHTAColorspace",
    "XYZColorspace",         /* CIE XYZ */
    "YCCColorspace",         /* Kodak PhotoCD PhotoYCC */
    "YIQColorspace",
    "YPbPrColorspace",
    "YUVColorspace",
    "CMYKColorspace",        /* Cyan, magenta, yellow, black, alpha */
    "sRGBColorspace",        /* Kodak PhotoCD sRGB */
    "HSLColorspace",         /* Hue, saturation, luminosity */
    "HWBColorspace",         /* Hue, whiteness, blackness */
    "LABColorspace",         /* LAB colorspace not supported yet other than via lcms */
    "CineonLogRGBColorspace",/* RGB data with Cineon Log scaling, 2.048 density range */
    "Rec601LumaColorspace",  /* Luma (Y) according to ITU-R 601 */
    "Rec601YCbCrColorspace", /* YCbCr according to ITU-R 601 */
    "Rec709LumaColorspace",  /* Luma (Y) according to ITU-R 709 */
    "Rec709YCbCrColorspace"
  };
  maketable(L, "colorspace", colorspace, 20);

#endif

  // filters
#if defined(USE_IMAGEMAGICK)

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

#else

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
    "LanczosFilter",
    "BesselFilter",
    "SincFilter"
  };
  maketable(L, "filters", filters, 16);

#endif

  // composite operation
#if defined(USE_IMAGEMAGICK)

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

#else

  char* composite_op[] = {
    "UndefinedCompositeOp",
    "OverCompositeOp",
    "InCompositeOp",
    "OutCompositeOp",
    "AtopCompositeOp",
    "XorCompositeOp",
    "PlusCompositeOp",
    "MinusCompositeOp",
    "AddCompositeOp",
    "SubtractCompositeOp",
    "DifferenceCompositeOp",
    "MultiplyCompositeOp",
    "BumpmapCompositeOp",
    "CopyCompositeOp",
    "CopyRedCompositeOp",
    "CopyGreenCompositeOp",
    "CopyBlueCompositeOp",
    "CopyOpacityCompositeOp",
    "ClearCompositeOp",
    "DissolveCompositeOp",
    "DisplaceCompositeOp",
    "ModulateCompositeOp",
    "ThresholdCompositeOp",
    "NoCompositeOp",
    "DarkenCompositeOp",
    "LightenCompositeOp",
    "HueCompositeOp",
    "SaturateCompositeOp",
    "ColorizeCompositeOp",
    "LuminizeCompositeOp",
    "ScreenCompositeOp",
    "OverlayCompositeOp",
    "CopyCyanCompositeOp",
    "CopyMagentaCompositeOp",
    "CopyYellowCompositeOp",
    "CopyBlackCompositeOp",
    "DivideCompositeOp",
    "HardLightCompositeOp",
    "ExclusionCompositeOp",
    "ColorDodgeCompositeOp",
    "ColorBurnCompositeOp",
    "SoftLightCompositeOp",
    "LinearBurnCompositeOp",
    "LinearDodgeCompositeOp",
    "LinearLightCompositeOp",
    "VividLightCompositeOp",
    "PinLightCompositeOp",
    "HardMixCompositeOp"
  };
  maketable(L, "composite_op", composite_op, 48);

#endif
  return 1;
}

int luaopen_gimagick_im(lua_State* L)
{
    return luaopen_gimagick(L);
}

int luaopen_gimagick_gm(lua_State* L)
{
    return luaopen_gimagick(L);
}
