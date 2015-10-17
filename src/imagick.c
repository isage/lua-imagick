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

static LuaImage* checkimage(lua_State* L, int index)
{
  void* ud = luaL_checkudata(L, index, IMG_METATABLE);
  luaL_argcheck(L, ud != NULL, index, "'image' expected");
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

static int imagick_open_blob(lua_State* L)
{
  size_t length;
  const char* data = luaL_checklstring(L, 1, &length);

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
  if (MagickReadImageBlob(a->m_wand, data, length) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushnil(L);
    lua_pushstring(L, error);
    return 2;
  }

  return 1;  /* new userdatum is already on the stack */
}

static int imagick_open_pseudo(lua_State* L)
{
  int width = luaL_checkinteger(L, 1);
  int height = luaL_checkinteger(L, 2);
  const char* data = luaL_checkstring(L, 3);

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

  if (MagickSetSize(a->m_wand, width, height) != MagickTrue)
  {
    ExceptionType severity;
    char* error=MagickGetException(a->m_wand, &severity);
    lua_pushnil(L);
    lua_pushstring(L, error);
    return 2;
  }

  if (MagickReadImage(a->m_wand, data) != MagickTrue)
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
  LuaImage* a = checkimage(L, 1);
  DestroyMagickWand(a->m_wand);
  DestroyPixelWand(a->p_wand);
  DestroyDrawingWand(a->d_wand);
  return 0;
}

int imagick_tostring(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int w = MagickGetImageWidth(a->m_wand);
  int h = MagickGetImageHeight(a->m_wand);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushfstring(L, "%s(%dx%d)", format, w, h);
  MagickRelinquishMemory(format);
  return 1;
}

static int imagick_width(lua_State* L)
{
  LuaImage *a = checkimage(L, 1);
  int w = MagickGetImageWidth(a->m_wand);
  lua_pushnumber(L, w);
  return 1;
}

static int imagick_height(lua_State* L)
{
  LuaImage *a = checkimage(L, 1);
  int h = MagickGetImageHeight(a->m_wand);
  lua_pushnumber(L, h);
  return 1;
}

static int imagick_write(lua_State* L)
{
  LuaImage *a = checkimage(L, 1);
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
  LuaImage *a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
  char* format = MagickGetImageFormat(a->m_wand);
  lua_pushstring(L,format);
  MagickRelinquishMemory(format);
  return 1;
}

static int imagick_set_format(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);

  size_t quality = MagickGetImageCompressionQuality(a->m_wand);
  lua_pushinteger(L, quality);
  return 1;
}

static int imagick_set_quality(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
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

  LuaImage* a = checkimage(L, 1);

  unsigned char* data = MagickGetImageBlob(a->m_wand, &length);

  lua_pushlstring(L, (const char*)data, length);
  lua_pushinteger(L, length);

  MagickRelinquishMemory(data);

  return 2;
}

static int imagick_get_gravity(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);

  size_t gravity = MagickGetImageGravity(a->m_wand);
  lua_pushinteger(L, gravity);
  return 1;
}

static int imagick_set_gravity(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
  const char* key = luaL_checkstring(L,2);

  char* value = MagickGetOption(a->m_wand, key);
  lua_pushstring(L, value);
  MagickRelinquishMemory(value);
  return 1;
}

static int imagick_set_option(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);

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
  LuaImage* a = checkimage(L, 1);

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
  LuaImage* a = checkimage(L, 1);

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
  LuaImage* a = checkimage(L, 1);

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
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
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

static int imagick_adaptive_blur(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  double sigma = luaL_checknumber(L,2);
  double radius = luaL_checknumber(L,3);

  if (MagickAdaptiveBlurImage(a->m_wand, sigma, radius) != MagickTrue)
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

static int imagick_adaptive_sharpen(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  double sigma = luaL_checknumber(L,2);
  double radius = luaL_checknumber(L,3);

  if (MagickAdaptiveSharpenImage(a->m_wand, sigma, radius) != MagickTrue)
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

static int imagick_blur_channel(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int chan = luaL_checkinteger(L,2);
  double sigma = luaL_checknumber(L,3);
  double radius = luaL_checknumber(L,4);

  if (MagickBlurImageChannel(a->m_wand, chan, sigma, radius) != MagickTrue)
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

static int imagick_sharpen_channel(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int chan = luaL_checkinteger(L,2);
  double sigma = luaL_checknumber(L,3);
  double radius = luaL_checknumber(L,4);

  if (MagickSharpenImageChannel(a->m_wand, chan, sigma, radius) != MagickTrue)
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

static int imagick_adaptive_blur_channel(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int chan = luaL_checkinteger(L,2);
  double sigma = luaL_checknumber(L,3);
  double radius = luaL_checknumber(L,4);

  if (MagickAdaptiveBlurImageChannel(a->m_wand, chan, sigma, radius) != MagickTrue)
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

static int imagick_adaptive_sharpen_channel(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int chan = luaL_checkinteger(L,2);
  double sigma = luaL_checknumber(L,3);
  double radius = luaL_checknumber(L,4);

  if (MagickAdaptiveSharpenImageChannel(a->m_wand, chan, sigma, radius) != MagickTrue)
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
  LuaImage* a = checkimage(L, 1);
  lua_pushnumber(L, MagickGetImageColorspace(a->m_wand));
  return 1;
}

static int imagick_has_alphachannel(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  lua_pushboolean(L, MagickGetImageAlphaChannel(a->m_wand));
  return 1;
}

static int imagick_get_icc_profile(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t length;
  unsigned char* data = MagickGetImageProfile(a->m_wand, "ICC", &length);

  lua_pushlstring(L, (const char*)data, length);
  lua_pushinteger(L, length);

  MagickRelinquishMemory(data);

  return 2;
}

static int imagick_has_icc_profile(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t length;
  char** data = MagickGetImageProfiles(a->m_wand, "icc", &length);

  lua_pushboolean(L, (length>0));
  MagickRelinquishMemory(data);

  return 1;
}

static int imagick_set_icc_profile(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
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
  LuaImage* a = checkimage(L, 1);
  const char* color = luaL_checkstring(L, 2);

  PixelSetColor(a->p_wand, color);

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

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_get_bg_color(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);

  MagickGetImageBackgroundColor(a->m_wand, a->p_wand);
  char* color = PixelGetColorAsNormalizedString(a->p_wand);
  lua_pushstring(L, color);
  MagickRelinquishMemory(color);
  return 1;
}

static int imagick_set_font(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  const char* font = luaL_checkstring(L, 2);

  if (DrawSetFont(a->d_wand, font) != MagickTrue)
  {
    ExceptionType severity;
    char* error=DrawGetException(a->d_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_font_family(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  const char* font = luaL_checkstring(L, 2);

  if (DrawSetFontFamily(a->d_wand, font) != MagickTrue)
  {
    ExceptionType severity;
    char* error=DrawGetException(a->d_wand, &severity);
    lua_pushboolean(L, 0);
    lua_pushstring(L, error);
    return 2;
  }

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_font_size(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  double size = luaL_checknumber(L, 2);

  DrawSetFontSize(a->d_wand, size);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_font_style(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int style = luaL_checkinteger(L, 2);

  DrawSetFontStyle(a->d_wand, style);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_font_weight(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int weight = luaL_checkinteger(L, 2);

  DrawSetFontWeight(a->d_wand, weight);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_set_font_align(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  int align = luaL_checkinteger(L, 2);

  DrawSetTextAlignment(a->d_wand, align);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_annotate(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  const char* color = luaL_checkstring(L, 2);
  const char* text = luaL_checkstring(L, 3);
  double x = luaL_checknumber(L, 4);
  double y = luaL_checknumber(L, 5);
  double angle = luaL_optnumber(L, 6, 0);

  PixelSetColor(a->p_wand, color);
  DrawSetFillColor(a->d_wand, a->p_wand);

  if (MagickAnnotateImage(a->m_wand, a->d_wand, x, y, angle, text) != MagickTrue)
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

static int imagick_resize(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  FilterTypes filter = luaL_optinteger(L, 4, LanczosFilter);
  double blur = luaL_optnumber(L, 5, 1.0);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickResizeImage(a->m_wand, w, h, filter, blur) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_adaptive_resize(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickAdaptiveResizeImage(a->m_wand, w, h) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_resample(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  FilterTypes filter = luaL_optinteger(L, 4, LanczosFilter);
  double blur = luaL_optnumber(L, 5, 1.0);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickResampleImage(a->m_wand, w, h, filter, blur) != MagickTrue)
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

static int imagick_scale(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickScaleImage(a->m_wand, w, h) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_crop(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  size_t x = luaL_optinteger(L, 4, 0);
  size_t y = luaL_optnumber(L, 5, 0);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickCropImage(a->m_wand, w, h, x, y) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_thumbnail(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickThumbnailImage(a->m_wand, w, h) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_composite(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  LuaImage* b = checkimage(L, 2);
  size_t x = luaL_checkinteger(L, 3);
  size_t y = luaL_checkinteger(L, 4);
  CompositeOperator op = luaL_optinteger(L, 5, OverCompositeOp);

  if (MagickCompositeImage(a->m_wand, b->m_wand, op, x, y) != MagickTrue)
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

static int imagick_extent(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  size_t w = luaL_checkinteger(L, 2);
  size_t h = luaL_checkinteger(L, 3);

  // This is needed because IM ignores geometry settings, duh
  RectangleInfo geometry;
  SetGeometry(GetImageFromMagickWand(a->m_wand), &geometry);
  geometry.width = w;
  geometry.height = h;
  GravityAdjustGeometry(MagickGetImageWidth(a->m_wand), MagickGetImageHeight(a->m_wand) , MagickGetImageGravity(a->m_wand), &geometry);

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickExtentImage(a->m_wand, geometry.width, geometry.height, geometry.x, geometry.y) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

void _parse_size_str(const char* size_str, size_t sw, size_t sh, size_t* w, size_t* h)
{
  short unsigned int expand = 0;
  short unsigned int ignore = 0;

  char lastchar = size_str[strlen(size_str)-1];

  int nw = 0;
  int nh = 0;
  int scanned = 0;

  if (lastchar < '0' || lastchar > '9') //not int
  {
    switch (lastchar)
    {
      case '!':
        ignore = 1;
        break;
      case '^':
        expand = 1;
        break;
      default: //error
        *w=*h=0;
        return;
        break;
    }
    scanned = sscanf(size_str, "%dx%d%*c", &nw, &nh);
  }
  else
  {
    scanned = sscanf(size_str, "%dx%d", &nw, &nh);
  }

  if (nw == 0 || nh == 0 || scanned != 2) //error
  {
    *w=*h=0;
    return;
  }

  if (ignore)
  {
    *w = nw;
    *h = nh;
    return;
  }

  double x_ratio = (double)nw / (double)sw;
  double y_ratio = (double)nh / (double)sh;

  if (expand) // "fill" given area
  {
    if (x_ratio > y_ratio)
    {
      nh = sh * x_ratio;
    }
    else
    {
      nw = sw * y_ratio;
    }
  }
  else
  {
    if (x_ratio < y_ratio)
    {
      nh = sh * x_ratio;
    }
    else
    {
      nw = sw * y_ratio;
    }
  }

  *w = nw;
  *h = nh;

  return;
}

static int imagick_smart_resize(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  const char* size_str = luaL_checkstring(L, 2);

  FilterTypes filter = LanczosFilter; // downscale for jpg
  double blur = 1.0;

  size_t w;
  size_t h;

  size_t sw = MagickGetImageWidth(a->m_wand);
  size_t sh = MagickGetImageHeight(a->m_wand);

  char* format = MagickGetImageFormat(a->m_wand);

  _parse_size_str(size_str, sw, sh, &w, &h);

  if (!!strcmp(format, "JPEG") || (w > sw) || (h > sh)) // if upscaling jpg or resizing any other format
  {
    filter = MitchellFilter;
  }

  MagickResetIterator(a->m_wand);
  while (MagickNextImage(a->m_wand) == 1)
  {
    if (MagickResizeImage(a->m_wand, w, h, filter, blur) != MagickTrue)
    {
      ExceptionType severity;
      char* error=MagickGetException(a->m_wand, &severity);
      lua_pushboolean(L, 0);
      lua_pushstring(L, error);
      return 2;
    }
  }
  MagickResetIterator(a->m_wand);

  lua_pushboolean(L, 1);
  return 1;
}

static int imagick_rotate(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  const char* color = luaL_checkstring(L, 2);
  double angle = luaL_optnumber(L, 3, 0);

  PixelSetColor(a->p_wand, color);

  if (MagickRotateImage(a->m_wand, a->p_wand, angle) != MagickTrue)
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

static int imagick_modulate(lua_State* L)
{
  LuaImage* a = checkimage(L, 1);
  double brightness = luaL_optnumber(L, 2, 0);
  double saturation = luaL_optnumber(L, 3, 0);
  double hue = luaL_optnumber(L, 4, 0); // huehuehuehue

  if (MagickModulateImage(a->m_wand, brightness, saturation, hue) != MagickTrue)
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


static const struct luaL_Reg imagicklib_f[] = {
  {"open", imagick_open},
  {"open_blob", imagick_open_blob},
  {"open_pseudo", imagick_open_pseudo},
  {NULL, NULL}
};

static const struct luaL_Reg imagicklib_meta[] = {
  {"__tostring", imagick_tostring},
  {"__gc",       imagick_destroy},
  {NULL, NULL}
};

static const struct luaL_Reg imagicklib_m[] = {
  {"width",                           imagick_width},
  {"height",                          imagick_height},
  {"write",                           imagick_write},
  {"write_all",                       imagick_write_all},
  {"get_format",                      imagick_get_format},
  {"set_format",                      imagick_set_format},
  {"get_quality",                     imagick_get_quality},
  {"set_quality",                     imagick_set_quality},
  {"blob",                            imagick_blob},
  {"get_gravity",                     imagick_get_gravity},
  {"set_gravity",                     imagick_set_gravity},
  {"get_option",                      imagick_get_option},
  {"set_option",                      imagick_set_option},
  {"coalesce",                        imagick_coalesce},
  {"optimize",                        imagick_optimize},
  {"strip",                           imagick_strip},
  {"swirl",                           imagick_swirl},
  {"oilpaint",                        imagick_oilpaint},
  {"blur",                            imagick_blur},
  {"sharpen",                         imagick_sharpen},
  {"adaptive_blur",                   imagick_adaptive_blur},
  {"adaptive_sharpen",                imagick_adaptive_sharpen},
  {"blur_channel",                    imagick_blur_channel},
  {"sharpen_channel",                 imagick_sharpen_channel},
  {"adaptive_blur_channel",           imagick_adaptive_blur_channel},
  {"adaptive_sharpen_channel",        imagick_adaptive_sharpen_channel},
  {"get_colorspace",                  imagick_get_colorspace},
  {"has_alphachannel",                imagick_has_alphachannel},
  {"get_icc_profile",                 imagick_get_icc_profile},
  {"has_icc_profile",                 imagick_has_icc_profile},
  {"set_icc_profile",                 imagick_set_icc_profile},
  {"set_compose",                     imagick_set_compose},
  {"set_bg_color",                    imagick_set_bg_color},
  {"get_bg_color",                    imagick_get_bg_color},
  {"set_font",                        imagick_set_font},
  {"set_font_family",                 imagick_set_font_family},
  {"set_font_size",                   imagick_set_font_size},
  {"set_font_style",                  imagick_set_font_style},
  {"set_font_weight",                 imagick_set_font_weight},
  {"set_font_align",                  imagick_set_font_align},
  {"annotate",                        imagick_annotate},
  {"resize",                          imagick_resize},
  {"adaptive_resize",                 imagick_adaptive_resize},
  {"resample",                        imagick_resample},
  {"scale",                           imagick_scale},
  {"crop",                            imagick_crop},
  {"thumbnail",                       imagick_thumbnail},
  {"composite",                       imagick_composite},
  {"extent",                          imagick_extent},
  {"smart_resize",                    imagick_smart_resize},
  {"rotate",                          imagick_rotate},
  {"modulate",                        imagick_modulate},
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

  // font style
  char* font_style[] = {
    "UndefinedStyle",
    "NormalStyle",
    "ItalicStyle",
    "ObliqueStyle",
    "AnyStyle"
  };
  maketable(L, "font_style", font_style, 5);

  // text align
  char* text_align[] = {
    "UndefinedAlign",
    "LeftAlign",
    "CenterAlign",
    "RightAlign"
  };
  maketable(L, "text_align", text_align, 4);

  // color channels
  lua_newtable(L);
  lua_pushnumber(L, 0);
  lua_setfield(L, -2, "UndefinedChannel");
  lua_pushnumber(L, 0x0001);
  lua_setfield(L, -2, "RedChannel");
  lua_pushnumber(L, 0x0001);
  lua_setfield(L, -2, "GrayChannel");
  lua_pushnumber(L, 0x0001);
  lua_setfield(L, -2, "CyanChannel");
  lua_pushnumber(L, 0x0002);
  lua_setfield(L, -2, "GreenChannel");
  lua_pushnumber(L, 0x0002);
  lua_setfield(L, -2, "MagentaChannel");
  lua_pushnumber(L, 0x0004);
  lua_setfield(L, -2, "BlueChannel");
  lua_pushnumber(L, 0x0004);
  lua_setfield(L, -2, "YellowChannel");
  lua_pushnumber(L, 0x0008);
  lua_setfield(L, -2, "AlphaChannel");
  lua_pushnumber(L, 0x0008);
  lua_setfield(L, -2, "OpacityChannel");
  lua_pushnumber(L, 0x0008);
  lua_setfield(L, -2, "MatteChannel");
  lua_pushnumber(L, 0x0020);
  lua_setfield(L, -2, "BlackChannel");
  lua_pushnumber(L, 0x0020);
  lua_setfield(L, -2, "IndexChannel");
  lua_pushnumber(L, 0x002F);
  lua_setfield(L, -2, "CompositeChannels");
  lua_pushnumber(L, 0x7ffffff);
  lua_setfield(L, -2, "AllChannels");
  lua_setfield(L, -2, "channel");

  return 1;
}
