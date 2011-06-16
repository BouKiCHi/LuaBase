#ifndef __key_h__
#define __key_h__

/* dingux key table */

#define DG_KEY_A SDLK_LCTRL
#define DG_KEY_B SDLK_LALT
#define DG_KEY_X SDLK_SPACE
#define DG_KEY_Y SDLK_LSHIFT

#define DG_KEY_L SDLK_TAB
#define DG_KEY_R SDLK_BACKSPACE
#define DG_KEY_START  SDLK_RETURN
#define DG_KEY_SELECT SDLK_ESCAPE

#define DG_KEY_UP    SDLK_UP
#define DG_KEY_DOWN  SDLK_DOWN
#define DG_KEY_LEFT  SDLK_LEFT
#define DG_KEY_RIGHT SDLK_RIGHT

/* */

#define DG_MAT_A (1)
#define DG_MAT_B (1<<1)
#define DG_MAT_X (1<<2)
#define DG_MAT_Y (1<<3)
#define DG_MAT_L (1<<4)
#define DG_MAT_R (1<<5)
#define DG_MAT_START (1<<6)
#define DG_MAT_SELECT (1<<7)

#define DG_MAT_UP (1<<8)
#define DG_MAT_DOWN (1<<9)
#define DG_MAT_LEFT (1<<10)
#define DG_MAT_RIGHT (1<<11)


#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

int key_get_data(void);
void key_register(lua_State *L);
int key_mat_down(int sw);
int key_mat_up(int sw);

#endif