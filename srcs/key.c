#include <SDL.h>
#include "key.h"


int g_key_current = 0;

#define KEY_MAT_DOWN( x ) case DG_KEY_##x:g_key_current |= DG_MAT_##x;break
#define KEY_MAT_UP( x ) case DG_KEY_##x:g_key_current &= ~(DG_MAT_##x);break


#ifdef KEY_STEP

int g_key_past = 0;
int g_key_time = 0;
int g_key_mode = 0;
int g_key_fire = 0;

#define KEY_STEP1_MS 500
#define KEY_STEP2_MS 50

void key_update_key( int current )
{
	if (g_key_current != g_key_past)
	{
		g_key_mode = 0;
		g_key_time = current;
		g_key_past = g_key_current;	
	}
	else
	{
		if (!g_key_current)
			return;
		
		if ( !g_key_mode && (current - g_key_time) >= KEY_STEP1_MS )
		{
			g_key_fire = 1;
			g_key_mode = 1;
			g_key_time = current;
		}
		
		if ( g_key_mode && (current - g_key_time) >= KEY_STEP2_MS )
		{
			g_key_fire = 1;
			g_key_time = current;
		}
	}
}

int key_get_current(void)
{
	if (g_key_fire)
	{
		g_key_fire = 0;
		return g_key_current;
	}
	return g_key_current & (g_key_current ^ g_key_past);
}

#endif

int key_get_data(void)
{
	return g_key_current;
}

int key_mat_down(int sw)
{
	
	switch(sw)
	{
			KEY_MAT_DOWN(UP);
			KEY_MAT_DOWN(DOWN);
			KEY_MAT_DOWN(LEFT);
			KEY_MAT_DOWN(RIGHT);
			
			KEY_MAT_DOWN(START);
			KEY_MAT_DOWN(SELECT);
			KEY_MAT_DOWN(L);
			KEY_MAT_DOWN(R);
			
			KEY_MAT_DOWN(A);
			KEY_MAT_DOWN(B);
			KEY_MAT_DOWN(X);
			KEY_MAT_DOWN(Y);
	}
}

int key_mat_up(int sw)
{
	switch(sw)
	{
			KEY_MAT_UP(UP);
			KEY_MAT_UP(DOWN);
			KEY_MAT_UP(LEFT);
			KEY_MAT_UP(RIGHT);
			
			KEY_MAT_UP(START);
			KEY_MAT_UP(SELECT);
			KEY_MAT_UP(L);
			KEY_MAT_UP(R);
			
			KEY_MAT_UP(A);
			KEY_MAT_UP(B);
			KEY_MAT_UP(X);
			KEY_MAT_UP(Y);
	}
}

/***********************
  Functions
************************/

// @function key.Read
// @format   key = key.Read()
// @describe Read key data
// @version  1.0
// @in       
// @out      
// @end
static int key_Read(lua_State *L)
{
	lua_pushnumber(L,g_key_current);
	return 1;	
}


// @function key.Up
// @format   flag = key.Up()
// @describe check up key
// @version  1.0
// @in       
// @out      
// @end
static int key_Up(lua_State *L)
{
	if (g_key_current & DG_MAT_UP)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);

	return 1;
}


// @function key.Down
// @format   flag = key.Down()
// @describe check down key
// @version  1.0
// @in       
// @out      
// @end
static int key_Down(lua_State *L)
{
	if (g_key_current & DG_MAT_DOWN)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	
	return 1;
}

// @function key.Left
// @format   flag = key.Left()
// @describe check left key
// @version  1.0
// @in       
// @out      
// @end
static int key_Left(lua_State *L)
{
	if (g_key_current & DG_MAT_LEFT)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	
	return 1;
}

// @function key.Right
// @format   flag = key.Right()
// @describe check right key
// @version  1.0
// @in       
// @out      
// @end
static int key_Right(lua_State *L)
{
	if (g_key_current & DG_MAT_RIGHT)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}


// @function key.A
// @format   flag = key.A()
// @describe Check A key
// @version  1.0
// @in       
// @out      
// @end
static int key_A(lua_State *L)
{
	if (g_key_current & DG_MAT_A)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.B
// @format   flag = key.B()
// @describe Check B key
// @version  1.0
// @in       
// @out      
// @end
static int key_B(lua_State *L)
{
	if (g_key_current & DG_MAT_B)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.X
// @format   flag = key.X()
// @describe Check X key
// @version  1.0
// @in       
// @out      
// @end
static int key_X(lua_State *L)
{
	if (g_key_current & DG_MAT_X)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}


// @function key.Y
// @format   flag = key.Y()
// @describe Check Y key
// @version  1.0
// @in       
// @out      
// @end
static int key_Y(lua_State *L)
{
	if (g_key_current & DG_MAT_Y)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.L
// @format   flag = key.L()
// @describe Check L key
// @version  1.0
// @in       
// @out      
// @end
static int key_L(lua_State *L)
{
	if (g_key_current & DG_MAT_L)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.R
// @format   flag = key.R()
// @describe Check R key
// @version  1.0
// @in       
// @out      
// @end
static int key_R(lua_State *L)
{
	if (g_key_current & DG_MAT_R)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.Start
// @format   flag = key.Start()
// @describe Check Start key
// @version  1.0
// @in       
// @out      
// @end
static int key_Start(lua_State *L)
{
	if (g_key_current & DG_MAT_START)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}

// @function key.Select
// @format   key.Select()
// @describe Check Select key
// @version  1.0
// @in       
// @out      
// @end
static int key_Select(lua_State *L)
{
	if (g_key_current & DG_MAT_SELECT)
		lua_pushboolean(L,1);
	else
		lua_pushboolean(L,0);
	return 1;
}


/****************
 Register 
****************/

static const struct luaL_reg key_lib[] =
{
	{ "Read"   , key_Read },
	{ "Up"     , key_Up },
	{ "Down"   , key_Down },
	{ "Left"   , key_Left },
	{ "Right"  , key_Right },
	{ "A"      , key_A },
	{ "B"      , key_B },
	{ "X"      , key_X },
	{ "Y"      , key_Y },
	{ "L"      , key_L },
	{ "R"      , key_R },
	{ "Start"  , key_Start },
	{ "Select" , key_Select },
	
	{ NULL , NULL }
};


void key_register(lua_State *L)
{
	luaL_openlib(L,"key",key_lib,0);
}
