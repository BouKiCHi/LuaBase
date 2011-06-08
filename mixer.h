#ifndef __mixer_h__
#define __mixer_h__

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <SDL.h>

void mixer_register(lua_State *L);

#endif