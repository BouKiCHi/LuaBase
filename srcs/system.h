#ifndef __system_h__
#define __system_h__

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#define SYS_VERSION 1.0
#define SYS_STRING 1024
#define SYS_MAXPATH 1024

extern int g_back;
extern int g_quit;
extern int g_calllua;
extern int g_launch;

extern char g_luafile[SYS_MAXPATH];
extern char g_launch_cmd[SYS_MAXPATH];

void system_set_dir(const char *exec,const char *script);
void system_register(lua_State *L);

#endif




