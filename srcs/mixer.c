/************************
 Mixer API
 ************************/

#include <SDL.h>
#include <SDL_mixer.h>

#include "mixer.h"

/************************
 Music Functions
 ************************/

static int mixer_LoadMusic(lua_State *L)
{
	Mix_Music *mus = Mix_LoadMUS ( lua_tostring( L , 1 ) );
	
	if (mus)
		lua_pushlightuserdata( L , mus );
	else
		lua_pushnil ( L );
	
	return 1;
}

static int mixer_FreeMusic(lua_State *L)
{
	Mix_FreeMusic ( lua_touserdata ( L , 1 ) );	
	return 0;
}

static int mixer_VolumeMusic(lua_State *L)
{
	Mix_VolumeMusic ( lua_tonumber( L , 1 ) );
	return 0;
}

static int mixer_PauseMusic(lua_State *L)
{
	Mix_PauseMusic();
	return 0;
}


static int mixer_HaltMusic(lua_State *L)
{
	Mix_PauseMusic();
	return 0;
}


static int mixer_RewindMusic(lua_State *L)
{
	Mix_RewindMusic();
	return 0;
}

static int mixer_ResumeMusic(lua_State *L)
{
	Mix_RewindMusic();
	return 0;
}

static int mixer_SetMusicPos(lua_State *L)
{
	Mix_SetMusicPosition( lua_tonumber ( L , 1 ) );
	return 0;
}


static int mixer_PlayingMusic(lua_State *L)
{
	int num = Mix_PlayingMusic ();
	
	lua_pushnumber ( L , num );
	return 1;
}


static int mixer_PlayMusic(lua_State *L)
{
	Mix_PlayMusic( (Mix_Music *)lua_touserdata ( L , 1 ) , lua_tonumber ( L , 2 ) );
	return 0;
}


static int mixer_FadeInMusic(lua_State *L)
{
	Mix_FadeInMusic( (Mix_Music *)lua_touserdata ( L , 1 ) 
					, lua_tonumber ( L , 2 )
					, lua_tonumber ( L , 3 ) );
	return 0;
}

static int mixer_FadeInMusicPos(lua_State *L)
{
	Mix_FadeInMusicPos( (Mix_Music *)lua_touserdata ( L , 1 ) 
					, lua_tonumber ( L , 2 )
					, lua_tonumber ( L , 3 )
					, lua_tonumber ( L , 4 ) );
	return 0;
}

static int mixer_FadeOutMusic( lua_State *L )
{
	Mix_FadeOutMusic( lua_tonumber ( L , 1 ) );
	return 0;
}


/************************
 Sound Functions
 ************************/

static int mixer_LoadWav(lua_State *L)
{
	Mix_Chunk *sample;
	sample = Mix_LoadWAV( lua_tostring ( L , 1 ) );
	if (sample)
		lua_pushlightuserdata( L , sample );
	else
		lua_pushnil( L );
	
	return 1;
}

static int mixer_VolumeChunk(lua_State *L)
{
	lua_pushnumber ( L ,
				Mix_VolumeChunk( (Mix_Chunk *)lua_touserdata ( L , 1 ) ,
								lua_tonumber ( L , 2 ) ) );	
	return 1;
}

static int mixer_FreeChunk(lua_State *L)
{
	Mix_FreeChunk( (Mix_Chunk *)lua_touserdata ( L , 1 ) );
	
	return 0;
}


static int mixer_AllocateChannels(lua_State *L)
{
	int num = Mix_AllocateChannels ( lua_tonumber ( L , 1 ) );
	
	lua_pushnumber ( L , num );	
	return 1;
}

static int mixer_Volume(lua_State *L)
{
	int num = Mix_Volume ( lua_tonumber ( L , 1 ) , lua_tonumber ( L , 2 ) );

	lua_pushnumber ( L , num );
	
	return 1;
}

static int mixer_PlayChannel(lua_State *L)
{
	int num = Mix_PlayChannel ( lua_tonumber ( L , 1 ) , 
							   (Mix_Chunk *)lua_touserdata ( L , 2 ),
							   lua_tonumber ( L , 3 ) );
	
	lua_pushnumber ( L , num );
	
	return 1;
}


static int mixer_PlayChannelTimed(lua_State *L)
{
	int num = Mix_PlayChannelTimed ( 
								lua_tonumber ( L , 1 ) , 
							    (Mix_Chunk *)lua_touserdata ( L , 2 ),
							    lua_tonumber ( L , 3 ),
								lua_tonumber ( L , 4 ));

	lua_pushnumber ( L , num );
	
	return 1;
}

static int mixer_FadeInChannel(lua_State *L)
{
	int num = Mix_FadeInChannel ( lua_tonumber ( L , 1 ) , 
							   (Mix_Chunk *)lua_touserdata ( L , 2 ),
							     lua_tonumber ( L , 3 ),
								 lua_tonumber ( L , 4 ));
	
	lua_pushnumber ( L , num );
	
	return 1;
}

static int mixer_FadeInChannelTimed(lua_State *L)
{
	int num = Mix_FadeInChannelTimed ( 
									lua_tonumber ( L , 1 ) , 
									(Mix_Chunk *)lua_touserdata ( L , 2 ),
									lua_tonumber ( L , 3 ),
									lua_tonumber ( L , 4 ),
									lua_tonumber ( L , 5 ));
	
	lua_pushnumber ( L , num );
	
	return 1;
}

static int mixer_Pause(lua_State *L)
{
	Mix_Pause( lua_tonumber ( L , 1 ) );

	return 0;
}

static int mixer_Resume(lua_State *L)
{
	Mix_Resume( lua_tonumber ( L , 1 ) );
	
	return 0;
}

static int mixer_HaltChannel(lua_State *L)
{
	Mix_HaltChannel( lua_tonumber ( L , 1 ) );
	
	return 0;
}

/************************
 登録関数
 ************************/

static const struct luaL_reg mixer_lib[] =
{
	{ "LoadMusic" , mixer_LoadMusic },
	{ "FreeMusic" , mixer_FreeMusic },
	{ "VolumeMusic" , mixer_VolumeMusic },
	{ "PauseMusic" , mixer_PauseMusic },
	{ "HaltMusic" , mixer_HaltMusic },
	{ "RewindMusic" , mixer_RewindMusic },
	{ "ResumeMusic" , mixer_ResumeMusic },
	{ "SetMusicPos" , mixer_SetMusicPos },
	{ "PlayingMusic" , mixer_PlayingMusic },
	{ "PlayMusic" , mixer_PlayMusic },
	{ "FadeInMusic" , mixer_FadeInMusic },
	{ "FadeInMusicPos" , mixer_FadeInMusicPos },
	{ "FadeOutMusic" , mixer_FadeOutMusic },
	{ "LoadWav" , mixer_LoadWav },
	{ "VolumeChunk" , mixer_VolumeChunk },
	{ "FreeChunk" , mixer_FreeChunk },
	{ "AllocateChannels" , mixer_AllocateChannels },
	{ "Volume" , mixer_Volume },
	{ "PlayChannel" , mixer_PlayChannel },
	{ "PlayChannelTimed" , mixer_PlayChannelTimed },
	{ "FadeInChannel" , mixer_FadeInChannel },
	{ "FadeInChannelTimed" , mixer_FadeInChannelTimed },
	{ "Pause" , mixer_Pause },
	{ "Resume" , mixer_Resume },
	{ "HaltChannel" , mixer_HaltChannel },
	{ NULL , NULL }
};


void mixer_register(lua_State *L)
{
	luaL_openlib(L,"mixer",mixer_lib,0);
}