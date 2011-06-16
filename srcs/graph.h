#ifndef __graph_h__
#define __graph_h__

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include <SDL.h>

extern int g_update;

void graph_set_surface(SDL_Surface *surface);
void graph_register(lua_State *L);

#endif