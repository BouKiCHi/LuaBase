/*******************************************=*
 LuaBase
 
 Copyright (c) 2011 BouKiCHi
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
*********************************************/

#include <stdio.h>
#include <time.h>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <math.h>

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "system.h"
#include "key.h"
#include "graph.h"

#include "mini_gui.h"


SDL_Surface *gSurfaceMain = NULL;
SDL_Surface *gUISurface = NULL;
SDL_Color g_white = { 255,255,255,0 };
SDL_Color g_black = { 0,0,0,0 };

int g_screen_count = 0;
int g_init_flags;

#define SDL_INIT (1)
#define TTF_INIT (1<<1)
#define IMG_INIT (1<<2)
#define MIX_INIT (1<<3)

int app_init(void)
{
	int i;

	gSurfaceMain = NULL;
	
	g_init_flags = 0;
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		return -1;
	
	g_init_flags |= SDL_INIT;
	
	if (TTF_Init())
	{
		printf("Fatal : SDL_TTF is not initialized!¥n");
		return -1;
	}
	
	g_init_flags |= TTF_INIT;

#ifndef __linux__
	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
	{
		printf("Fatal : SDL_IMG  is not initialized!¥n");
		return -1;
	}
#endif
	
	g_init_flags |= IMG_INIT;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("Fatal error : SDL_mixer is not initialized!¥n");

		return -1;
	}
	
	g_init_flags |= MIX_INIT;
	
	gSurfaceMain = SDL_SetVideoMode(320,240,16,0);
	
	if (!gSurfaceMain)
		return -1;
	
	gui_init();
	
#ifdef __A320__
	SDL_ShowCursor(SDL_DISABLE);
#endif

	return 0;
}

void app_free(void)
{
	system_free();
	gui_free();
	
	if (gUISurface)
	{
		SDL_FreeSurface ( gUISurface );
		gUISurface = NULL;
	}
	if (g_init_flags & MIX_INIT) Mix_CloseAudio();

#ifndef __linux__
	if (g_init_flags & IMG_INIT) IMG_Quit();
#endif
	if (g_init_flags & TTF_INIT) TTF_Quit();
	if (g_init_flags & SDL_INIT) SDL_Quit();
}

int g_close = 0;

void app_loop( char *file )
{	
	SDL_Event event;
	
	lua_State *L;
	
	if (!file)
		return;
	
	L = lua_open();
	luaL_openlibs(L);

	graph_set_surface(gSurfaceMain);
	graph_register(L);
	system_register(L);
	key_register(L);
	
	
	if(luaL_dofile(L, file))
	{
		printf("Failed to Load! : %s\n", lua_tostring(L, -1) );
		return;
	}

	int errfunc = lua_gettop(L) + 1;
	
	lua_getglobal(L, "do_error");
	if ( lua_isnil ( L , -1 ) )
	{
//		printf("Warning : Failed to set error handling\n");
		errfunc = 0;		
	}
			
	lua_getglobal(L, "init");
	
	if( lua_pcall(L, 0, 0, errfunc) )
	{
		printf("error : %s\n", lua_tostring(L, -1) );
		lua_pop(L,1);
		g_quit = 1;
	}
	
	SDL_Flip(gSurfaceMain);
	SDL_Flip(gSurfaceMain);

	// ループ本体
	while (!g_quit && !g_close)
	{
		while(SDL_PollEvent(&event) && !g_quit)
		{
			
			switch(event.type)
			{
				case SDL_KEYDOWN:
					if (event.key.keysym.sym == SDLK_F1)
					{
						char name[256];
						
						sprintf(name,"scrn%04d.bmp",g_screen_count++);
						SDL_SaveBMP(gSurfaceMain,name);
					}
					key_mat_down(event.key.keysym.sym);
					break;					
				case SDL_KEYUP:
					key_mat_up(event.key.keysym.sym);
					break;
				case SDL_QUIT:
					g_close = 1;
					break;
			}
		}

		lua_getglobal(L, "update");
		
		if( lua_pcall(L, 0, 0, errfunc) )
		{
			printf("error : %s\n", lua_tostring(L, -1) );
			lua_pop(L,1);
			g_quit = 1;
		}
		
		if (g_update)
		{
			g_update = 0;
			SDL_Flip(gSurfaceMain);
		}
		
		// ネストで呼び出し
		if (g_calllua)
		{
			app_loop(g_luafile);
		}
		
	}
	
	
	lua_getglobal(L, "free");
	
	if( lua_pcall(L, 0, 0, errfunc) )
	{
		printf("error : %s\n", lua_tostring(L, -1) );
		lua_pop(L,1);
	}
	
    lua_close(L);

	if (!g_launch)
		g_quit = 0;
	
	g_back = 1;
}


int main(int argc,char *argv[])
{
	if (app_init())
	{
		app_free();
		return -1;
	}
		
	if (argc > 1)
	{
		system_set_dir(argv[0],argv[1]);
		app_loop(argv[1]);
	}
	else 
	{
		system_set_dir(argv[0],NULL);
		app_loop("startup.lua");
	}
	
	app_free();
	
	system_exec();
	
	if (g_launch)
		system(g_launch_cmd);

	return 0;	
}

