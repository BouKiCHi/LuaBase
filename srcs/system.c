#include <SDL.h>
#include "system.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

#if defined(__linux__) || defined(__APPLE__)
#define SYS_PATHDIV_CHR '/'
#else
#define SYS_PATHDIV_CHR '\\'
#endif



int g_back;
int g_quit;
int g_calllua;
int g_launch;
int g_exec;

int  g_result;
char g_result_str[SYS_STRING];

int  g_argument;
char g_argument_str[SYS_STRING];


char g_script_dir[SYS_MAXPATH];
char g_exec_dir[SYS_MAXPATH];
char g_luafile[SYS_MAXPATH];
char g_launch_cmd[SYS_MAXPATH];

char *g_exec_path = NULL;
char *g_exec_argv[128];

void system_exec(void)
{
	if (!g_exec || !g_exec_path)
		return;
	
	// the function doesn't return if success.
	execvp( g_exec_path , g_exec_argv );
}


void system_set_dir(const char *exec,const char *script)
{
	char old_cwd[SYS_MAXPATH];
	
	// 結合
	strcpy(g_exec_dir , exec );
	char *p = strrchr(g_exec_dir,SYS_PATHDIV_CHR);
	
	// ファイル名の部分を削除
	if (p)
		p[0] = 0;

	// 正規化
	getcwd( old_cwd , SYS_MAXPATH );
	chdir( g_exec_dir );
	getcwd( g_exec_dir , SYS_MAXPATH );
	chdir( old_cwd );
	
	
	if ( script )
	{
		strcpy( g_script_dir , script );
		char *p = strrchr( g_script_dir , SYS_PATHDIV_CHR);
		
		if (p)
			p[0] = 0;
	
		// 正規化
		getcwd( old_cwd , SYS_MAXPATH );
		chdir ( g_script_dir );
		getcwd( g_script_dir , SYS_MAXPATH);
		chdir( old_cwd );
	}
	else
	{
		getcwd( g_script_dir , SYS_MAXPATH);
	}
	
}

// @function system.GetTick
// @format   tick = system.GetTick()
// @out      tick : time in ms
// @describe Get current ticks in ms
// @version  1.0
// @end

static int system_GetTick(lua_State *L)
{
	lua_pushnumber(L,SDL_GetTicks());
	return 1;	
}


// @function system.Quit
// @format   system.Quit()
// @describe Quit LuaBase
// @version  1.0
// @end

static int system_Quit(lua_State *L)
{
	g_quit = 1;
	return 0;
}

// @function system.Version
// @format   version = system.Version()
// @describe get current version of LuaBase
// @version  1.0
// @in       
// @out      
// @end
static int system_Version(lua_State *L)
{
	lua_pushnumber( L , SYS_VERSION );	
	return 1;	
}

// @function system.BackNotify
// @format   flag = system.BackNotify()
// @describe Notification of back from sub script
// @version  1.0
// @in       
// @out      
// @end
static int system_BackNotify(lua_State *L)
{
	if (g_back)
		lua_pushboolean( L , 1 );
	else
		lua_pushboolean( L , 0 );

	return 1;	
}

// @function system.ResultNotify
// @format   flag = system.ResultNotify()
// @describe Notify if sub script sets any result
// @version  1.0
// @in       
// @out      
// @end
static int system_ResultNotify(lua_State *L)
{
	if (g_result)
		lua_pushboolean( L , 1 );
	else
		lua_pushboolean( L , 0 );
	
	return 1;
}

// @function system.ArgumentNotify
// @format   flag = system.ArgumentNotify()
// @describe Notify if caller sets arguments
// @version  1.0
// @in       
// @out      
// @end
static int system_ArgumentNotify(lua_State *L)
{
	if (g_argument)
		lua_pushboolean( L , 1 );
	else
		lua_pushboolean( L , 0 );
	
	return 1;
}


// @function system.GetResultString
// @format   string = system.GetResultString()
// @describe Get result string
// @version  1.0
// @in       
// @out      
// @end
static int system_GetResultString(lua_State *L)
{
	lua_pushstring( L , g_result_str );	
	return 1;
}


// @function system.SetResultString
// @format   system.SetResultString( string )
// @describe Set result string for caller
// @version  1.0
// @in       
// @out      
// @end
static int system_SetResultString(lua_State *L)
{
	g_result = 1;
	strcpy( g_result_str , lua_tostring( L , 1 ) );
	return 0;
}


// @function system.GetArgumentString
// @format   argument = system.GetArgumentString()
// @describe Get argument string
// @version  1.0
// @in       
// @out      
// @end
static int system_GetArgumentString(lua_State *L)
{
	lua_pushstring( L , g_argument_str );	
	return 1;
}


// @function system.SetArgumentString
// @format   system.SetArgumentString( argument )
// @describe Set argument string
// @version  1.0
// @in       
// @out      
// @end
static int system_SetArgumentString(lua_State *L)
{
	g_argument = 1;
	strcpy( g_argument_str , lua_tostring( L , 1 ) );
	return 0;
}


// @function system.ClearNotify
// @format   system.ClearNotify()
// @describe Clear Notification flags ( argument , result , back )
// @version  1.0
// @in       
// @out      
// @end
static int system_ClearNotify(lua_State *L)
{
	g_argument = 0;
	g_result = 0;
	g_back = 0;
	return 0;
}

// @function system.Delay
// @format   system.Delay( time )
// @describe Delay
// @version  1.0
// @in       time : time in milisecond
// @out      
// @end
static int system_Delay(lua_State *L)
{
	SDL_Delay(lua_tonumber(L,1));
	return 0;
}



static FILE *lcd_fp = NULL;
static FILE *batt_fp = NULL;
static FILE *lock_fp = NULL;

#ifdef __A320__

// @function system.ReadLCD
// @format   bright = system.ReadLCD()
// @describe Get backlight
// @version  1.0
// @in       
// @out      
// @end
static int system_ReadLCD(lua_State *L)
{
	int lcd_bright;
	
	if (!lcd_fp)
		lcd_fp = fopen("/proc/jz/lcd_backlight","r+");
	
	if (!lcd_fp)
		return 0;
	
	fscanf(lcd_fp, "%d" , &lcd_bright );
	rewind(lcd_fp);
	
	lua_pushnumber( L , lcd_bright );
	return 1;
}

// @function system.WriteLCD
// @format   system.WriteLCD( bright )
// @describe Set backlight
// @version  1.0
// @in       
// @out      
// @end
static int system_WriteLCD(lua_State *L)
{
	if (!lcd_fp)
		lcd_fp = fopen("/proc/jz/lcd_backlight","r+");

	if (!lcd_fp)
		return 0;
	
	int lcd_bright = lua_tonumber( L , 1 );
	
	fprintf(lcd_fp,"%d\n",lcd_bright);
	rewind(lcd_fp);	
	
	return 0;
}

// @function system.ReadBatt
// @format   batt = system.ReadBatt()
// @describe Get battery
// @version  1.0
// @in       
// @out      
// @end
static int system_ReadBatt(lua_State *L)
{
	int val = 0;
	
	if (!batt_fp)
		batt_fp = fopen("/proc/jz/battery","r");
	
	if (!batt_fp)
		return 0;
	
	fscanf(batt_fp, "%d" , &val );
	rewind(batt_fp);

	lua_pushnumber( L , val );
	return 1;
	
}

// @function system.ReadLock
// @format   lock = system.ReadLock()
// @describe Read lock switch 
// @version  1.0
// @in       
// @out      
// @end
static int system_ReadLock(lua_State *L)
{
	int val = 0;
	
	if (!lock_fp)
		lock_fp = fopen("/proc/jz/gpio3_pxpin","r");
	
	if (!lock_fp)
		return 0;
	
	fscanf(lock_fp, "0x%x" , &val );
	rewind(lock_fp);
	
	if (!(val & (1 << 22) ) )
		lua_pushboolean( L , 1 );
	else 
		lua_pushboolean( L , 0 );
	
	return 1;	
}


#else


static int system_ReadLCD( lua_State *L )
{
	lua_pushnumber( L , 100 );
	return 1;	
}


static int system_WriteLCD( lua_State *L )
{
	return 0;	
}

static int system_ReadBatt(lua_State *L)
{
	lua_pushnumber( L , 4000 );
	return 1;	
}

static int system_ReadLock(lua_State *L)
{	
	lua_pushboolean( L , 0 );
	return 1;	
}

#endif


/***************************************
 Directory stuff
 ***************************************/

// @function system.OpenDir
// @format   dir = system.OpenDir( path )
// @describe Open Directory
// @version  1.0
// @in       
// @out      
// @end
static int system_OpenDir(lua_State *L)
{
	DIR *dir = NULL;
	
	if (!lua_isnil ( L , 1 ))
		dir = opendir( lua_tostring( L , 1 ) );
	
	if (dir)
		lua_pushlightuserdata( L , dir );
	else
		lua_pushnil( L );
	
	return 1;
}

// @function system.CloseDir
// @format   system.CloseDir( dir )
// @describe Close Directory
// @version  1.0
// @in       
// @out      
// @end
static int system_CloseDir(lua_State *L)
{
	if (!lua_isnil( L , 1 ))
		closedir ( (DIR *)lua_touserdata( L , 1 ) );
	
	return 0;
}

// @function system.ReadDir
// @format   entry = system.ReadDir( dir )
// @describe Read directory entry
// @version  1.0
// @in       
// @out      
// @end
static int system_ReadDir(lua_State *L)
{
	struct dirent *p_ent = NULL;
	
	if (!lua_isnil( L , 1 ))
		p_ent = readdir( (DIR *)lua_touserdata( L , 1 ) );
	
	if (p_ent)
		lua_pushlightuserdata( L , p_ent );
	else
		lua_pushnil( L );

	return 1;
}

// @function system.EntName
// @format   name = system.EntName( entry )
// @describe Get name from entry
// @version  1.0
// @in       
// @out      
// @end
static int system_EntName(lua_State *L)
{
	lua_pushstring( L , 
				   ((struct dirent *)lua_touserdata( L , 1 ))->d_name );
	
	return 1;
}

// @function system.IsDir
// @format   flag = system.IsDir( path )
// @describe Check if the path is directory
// @version  1.0
// @in       
// @out      
// @end
static int system_IsDir(lua_State *L)
{
	struct stat s_stat;
	
	stat ( lua_tostring( L , 1 ) , &s_stat );
	lua_pushboolean( L , S_ISDIR ( s_stat.st_mode ) );
	
	return 1;
}

// @function system.IsExist
// @format   flag = system.IsExist( path )
// @describe Check if the path exists
// @version  1.0
// @in       
// @out      
// @end
static int system_IsExist(lua_State *L)
{
	struct stat s_stat;
	
	if (stat ( lua_tostring( L , 1 ) , &s_stat ))
		lua_pushboolean( L , 0 );
	else 
		lua_pushboolean( L , 1 );
	
	return 1;
}



// @function system.Filesize
// @format   size = system.Filesize( path )
// @describe Get filesize
// @version  1.0
// @in       
// @out      
// @end
static int system_Filesize(lua_State *L)
{
	struct stat s_stat;
	
	stat ( lua_tostring( L , 1 ) , &s_stat );
	lua_pushnumber( L , s_stat.st_size );
	
	return 1;
}

// @function system.GetCwd
// @format   path = system.GetCwd()
// @describe Get current working directory
// @version  1.0
// @in       
// @out      
// @end
static int system_GetCwd(lua_State *L)
{
	char path[SYS_MAXPATH];
	getcwd( path , SYS_MAXPATH );
	
	lua_pushstring( L , path );

	return 1;
}

// @function system.ChDir
// @format   system.ChDir( path )
// @describe Set current working directry
// @version  1.0
// @in       
// @out      
// @end
static int system_ChDir(lua_State *L)
{
	chdir( lua_tostring ( L , 1 ) );
	return 0;
}

// @function system.MkDir
// @format   system.MkDir( directory )
// @describe Make directry
// @version  1.0
// @in       
// @out      
// @end
static int system_MkDir(lua_State *L)
{
	mkdir( lua_tostring ( L , 1 ) , S_IRWXU | S_IRWXG | S_IRWXO);
	return 0;
}

// @function system.GetExecDir
// @format   path = system.GetExecDir()
// @describe Get executable directory
// @version  1.0
// @in       
// @out      
// @end
static int system_GetExecDir(lua_State *L)
{	
	lua_pushstring( L , g_exec_dir );
	return 1;
}

// @function system.GetScriptDir
// @format   path = system.GetScriptDir()
// @describe Get script directory
// @version  1.0
// @in       
// @out      
// @end
static int system_GetScriptDir(lua_State *L)
{	
	lua_pushstring( L , g_script_dir );
	return 1;
}

// @function system.CallLua
// @format   system.CallLua( file )
// @describe Call Lua file
// @version  1.0
// @in       
// @out      
// @end
static int system_CallLua(lua_State *L)
{	
	strcpy( g_luafile , lua_tostring ( L , 1 ) );
	g_calllua = 1;	
	return 0;
}


// @function system.Launch
// @format   system.Launch( exec )
// @describe Launch executable
// @version  1.0
// @in       
// @out      
// @end
static int system_Launch(lua_State *L)
{	
	strcpy( g_launch_cmd , lua_tostring ( L , 1 ) );
	g_quit = 1;
	g_launch = 1;
	
	return 0;
}


// @function system.Execute
// @format   system.Execute( exec , ... )
// @describe Execute command
// @version  1.0
// @in       
// @out      
// @end
static int system_Execute(lua_State *L)
{	
	int i,top;
	
	top = lua_gettop(L);
	
	if (top < 1)
	{
		printf("Error : a few less than requested argument to execute\n");
		
		g_quit = 1;
		lua_pushboolean( L , 1 );
		return 1;
	}
	else
	{
		g_exec_path = strdup ( lua_tostring ( L , 1 ) );
		
		for ( i = 0; i < top; i++ ) 
		{
			g_exec_argv[ i ] = strdup ( lua_tostring( L , 1 + i ) );			
		}
		
		g_exec_argv[ i ] = NULL;
	
	}

	g_quit = 1;
	g_exec = 1;

	lua_pushboolean( L , 0 );
	return 1;
}


// @function system.PathDiv
// @format   div = system.PathDiv()
// @describe Get path divider of current environment
// @version  1.0
// @in       
// @out      
// @end
static int system_PathDiv(lua_State *L)
{
	char div[2];
	
	div[0] = SYS_PATHDIV_CHR;
	div[1] = 0;
	
	lua_pushstring( L , div );
	return 1;
}


/***************************************
 Register
 ***************************************/


static const struct luaL_reg system_lib[] =
{
	{ "GetTick"           , system_GetTick },
	{ "Quit"              , system_Quit },
	{ "Version"           , system_Version },
	{ "BackNotify"        , system_BackNotify },
	{ "ResultNotify"      , system_ResultNotify },
	{ "ArgumentNotify"    , system_ArgumentNotify },
	{ "GetResultString"   , system_GetResultString },
	{ "SetResultString"   , system_SetResultString },
	{ "GetArgumentString" , system_GetArgumentString },
	{ "SetArgumentString" , system_SetArgumentString },
	{ "ClearNotify"       , system_ClearNotify },
	{ "Delay"             , system_Delay },
	{ "ReadLCD"           , system_ReadLCD },
	{ "WriteLCD"          , system_WriteLCD },
	{ "ReadBatt"          , system_ReadBatt },
	{ "ReadLock"          , system_ReadLock },
	{ "OpenDir"           , system_OpenDir },
	{ "CloseDir"          , system_CloseDir },
	{ "ReadDir"           , system_ReadDir },
	{ "EntName"           , system_EntName },
	{ "IsDir"             , system_IsDir },
	{ "IsExist"           , system_IsExist },
	{ "Filesize"          , system_Filesize },
	{ "GetCwd"            , system_GetCwd },
	{ "ChDir"             , system_ChDir },
	{ "MkDir"             , system_MkDir },
	{ "GetExecDir"        , system_GetExecDir },
	{ "GetScriptDir"      , system_GetScriptDir },
	{ "CallLua"           , system_CallLua },
	{ "Launch"            , system_Launch },
	{ "Execute"           , system_Execute },
	{ "PathDiv"           , system_PathDiv },
	
	
	{ NULL , NULL }
};

void system_register(lua_State *L)
{
	g_exec_path = NULL;
	g_exec_argv[0] = NULL;

	g_exec    = 0;
	g_launch  = 0;
	g_calllua = 0;
	g_quit    = 0;
	luaL_openlib(L,"system",system_lib,0);
}

void system_free()
{
	if (lcd_fp)  fclose(lcd_fp);
	if (batt_fp) fclose(batt_fp);
	if (lock_fp) fclose(lock_fp);
	
	lcd_fp = batt_fp = lock_fp = NULL;
}
