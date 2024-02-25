LUA     = 5.1
PREFIX  = /usr
LIBDIR  = $(PREFIX)/lib/lua/$(LUA)

LUA_CFLAGS  = $(shell pkg-config --cflags lua$(LUA)) 
CFLAGS  = -fPIC $(LUA_CFLAGS) -I/usr/include/ $(shell pkg-config --cflags MagickWand)
LIBS    = $(shell pkg-config --libs lua$(LUA) --libs MagickWand)

imagick.so: src/imagick.o
	$(CC) -shared $(CFLAGS) -o $@ src/imagick.o $(LIBS) -lm

install:
	mkdir -p $(DESTDIR)$(LIBDIR)
	cp imagick.so $(DESTDIR)$(LIBDIR)

clean:
	rm -rf src/*.o *.so

.PHONY: imagick.so
