# A script for setting up environment for travis-ci testing.

set -e

if [ "$LUA" == "LuaJIT 2.0" ]; then
  wget -O - http://luajit.org/download/LuaJIT-2.0.4.tar.gz | tar xz
  cd LuaJIT-2.0.4
  make && sudo make install;
else
  if [ "$LUA" == "Lua 5.1" ]; then
    wget -O - http://www.lua.org/ftp/lua-5.1.5.tar.gz | tar xz
    cd lua-5.1.5;
  elif [ "$LUA" == "Lua 5.2" ]; then
    wget -O - http://www.lua.org/ftp/lua-5.2.4.tar.gz | tar xz
    cd lua-5.2.4;
  fi
  sudo make linux install;
fi

cd ..
