/************************
 Graphics API
************************/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include "graph.h"
#include "mini_gui.h"

int g_update;

// current surface
static SDL_Surface *c_surf;

// main surface
static SDL_Surface *mainsurf;

void graph_set_surface(SDL_Surface *surface)
{
	mainsurf = surface;
	c_surf = mainsurf;
}

#define USERSURF(x) ((SDL_Surface **)lua_touserdata(L,x) ? *(SDL_Surface **)lua_touserdata(L,x) : NULL )
#define USERSURF_RAW(x) (SDL_Surface **)lua_touserdata(L,x)

/*****************
@section 制御系
*****************/


static int gfx_Lock(lua_State *L)
{
	if (SDL_MUSTLOCK( mainsurf ))
		SDL_LockSurface ( mainsurf );
	return 0;
}

static int gfx_Unlock(lua_State *L)
{
	if (SDL_MUSTLOCK(mainsurf))
		SDL_UnlockSurface(mainsurf);
	return 0;
}

static int gfx_Update(lua_State *L)
{
	g_update = 1;
	return 0;
}

static int gfx_SetSurf(lua_State *L)
{
	c_surf = USERSURF(1);
	return 0;
}

static int gfx_MainSurf(lua_State *L)
{
	SDL_Surface **surf = (SDL_Surface **)lua_newuserdata(L,sizeof(SDL_Surface **));		
	*surf = mainsurf;

	return 1;
}

static int gfx_FreeSurf(lua_State *L);

static int gfx_CreateSurf(lua_State *L)
{
	SDL_Surface *p_surf;
	int w,h;
	
	w  = lua_tonumber(L,1);
	h  = lua_tonumber(L,2);	
	
	p_surf = SDL_CreateRGBSurface( SDL_SWSURFACE , w , h , 
								  mainsurf->format->BitsPerPixel,
								  mainsurf->format->Rmask,
								  mainsurf->format->Gmask,
								  mainsurf->format->Bmask,
								  mainsurf->format->Amask);
								  
	if ( p_surf )
	{
		SDL_Surface **surf = (SDL_Surface **)lua_newuserdata(L,sizeof(SDL_Surface **));		
		*surf = p_surf;
		
		lua_newtable(L);
		lua_pushliteral(L,"__gc");
		lua_pushcfunction(L,gfx_FreeSurf);
		lua_rawset(L,-3);
		lua_setmetatable(L,-2);
		
	}
	else
		lua_pushnil( L );

	return 1;
}

static int gfx_Width(lua_State *L)
{
	lua_pushnumber(L,SCREEN_W);
	return 1;
}

static int gfx_Height(lua_State *L)
{
	lua_pushnumber(L,SCREEN_H);
	return 1;
}

static int gfx_SurfWidth(lua_State *L)
{
	SDL_Surface *p_surf = USERSURF(1);
	
	if (!p_surf)
		lua_pushnumber(L,0);
	else 		
		lua_pushnumber(L,p_surf->w);

	return 1;
}

static int gfx_SurfHeight(lua_State *L)
{
	SDL_Surface *p_surf = USERSURF(1);
	
	if (!p_surf)
		lua_pushnumber(L,0);
	else
	lua_pushnumber(L,p_surf->h);
	return 1;
}

static int gfx_SurfSize(lua_State *L)
{
	SDL_Surface *p_surf = USERSURF(1);
	
	if (!p_surf)
	{
		lua_pushnumber(L,0);
		lua_pushnumber(L,0);
	}
	else
	{
		lua_pushnumber(L,p_surf->w);
		lua_pushnumber(L,p_surf->h);
	}
	return 2;
}


static int gfx_SaveSurf(lua_State *L)
{
	SDL_Surface *p_surf = USERSURF(1);
	
	if (p_surf && lua_isstring( L , 2 ))
	{
		SDL_SaveBMP(p_surf , lua_tostring (L,2 ) );
	}
	return 0;
}



///////////////////////
// プリミティブ
// 色の作成
static int gfx_Color(lua_State *L)
{
	int r,g,b;
	
	r  = lua_tonumber(L,1);
	g  = lua_tonumber(L,2);
	b  = lua_tonumber(L,3);	
	
	SDL_Color *color = (SDL_Color *)lua_newuserdata(L,sizeof(SDL_Color));
	
	color->r = r;
	color->g = g;
	color->b = b;
	
	return 1;
}


static int gfx_Pixel(lua_State *L)
{
	int px,py;
	
	px = lua_tonumber(L,1);
	py = lua_tonumber(L,2);
	
	SDL_Color *color = (SDL_Color *)lua_touserdata(L,3);
	
	if (!color) 
	{
		printf("%s : color is null\n",__FUNCTION__);
		return 0;
	}
	
	gui_putpixel(c_surf, px , py  , *color );
	
	return 0;
}

static int gfx_Line(lua_State *L)
{
	int cx,cy,px,py;

	cx = lua_tonumber(L,1);
	cy = lua_tonumber(L,2);
	px = lua_tonumber(L,3);
	py = lua_tonumber(L,4);
	
	SDL_Color *color = (SDL_Color *)lua_touserdata(L,5);
	
	if (!color) 
	{
		printf("%s : color is null\n",__FUNCTION__);
		return 0;
	}
	
	gui_draw_line(c_surf, cx , cy , px , py , *color );

	return 0;
}


static int gfx_Poly(lua_State *L)
{
	int i,len;
	
	int x[128];
	int y[128];
	
	if (!lua_istable(L,1))
		return 0;
	
	len = lua_objlen(L,1);
	if (len >= 127)
		return 0;
		
	for ( i = 1; i <= len; i++ )
	{
		int px,py;
		lua_pushnumber( L , i );
		lua_gettable ( L , 1 );
		px = lua_tonumber( L , -1 );
		lua_pop ( L , 1 );
		
		lua_pushnumber( L , i );
		lua_gettable ( L , 2 );
		py = lua_tonumber( L , -1 );
		lua_pop ( L , 1 );
				
		x[i-1] = px;
		y[i-1] = py;
	}

	SDL_Color *color = (SDL_Color *)lua_touserdata(L,3);
	
	gui_draw_poly_fill(c_surf, x , y , len , *color );
	
	return 0;
}

static int gfx_Box(lua_State *L)
{
	int x,y,dx,dy;
	
	x  = lua_tonumber(L,1);
	y  = lua_tonumber(L,2);
	dx = lua_tonumber(L,3);
	dy = lua_tonumber(L,4);
	
	SDL_Color *color = (SDL_Color *)lua_touserdata(L,5);
	
	if (!color) 
	{
		printf("%s : color is null\n",__FUNCTION__);
		return 0;
	}

	gui_draw_line(c_surf, x,y ,dx,y,*color);	
	gui_draw_line(c_surf, x,dy,dx,dy,*color);	
	gui_draw_line(c_surf, x,y ,x ,dy,*color);	
	gui_draw_line(c_surf,dx,y ,dx,dy,*color);	
	
	return 0;
}

static int gfx_BoxFill(lua_State *L)
{
	int x,y,dx,dy;
	
	x  = lua_tonumber(L,1);
	y  = lua_tonumber(L,2);
	dx = lua_tonumber(L,3);
	dy = lua_tonumber(L,4);
	
	SDL_Color *color = (SDL_Color *)lua_touserdata(L,5);
	
	if (!color) 
		printf("%s : color is null\n",__FUNCTION__);
	else
		gui_draw_box_fill(c_surf,x,y,dx,dy,*color);	
	
	return 0;
}

static int gfx_BoxGrad(lua_State *L)
{
	int x,y,dx,dy;
	
	x  = lua_tonumber(L,1);
	y  = lua_tonumber(L,2);
	dx = lua_tonumber(L,3);
	dy = lua_tonumber(L,4);
	
	SDL_Color *c1 = (SDL_Color *)lua_touserdata(L,5);
	SDL_Color *c2 = (SDL_Color *)lua_touserdata(L,6);
	
	if (!c1 || !c2) 
		printf("%s : color is null\n",__FUNCTION__);
	else
		gui_draw_box_grad(c_surf,x,y,dx,dy,*c1,*c2);	
	
	return 0;
}


static int gfx_Circle(lua_State *L)
{
	int x,y,rad;
	
	x   = lua_tonumber(L,1);
	y   = lua_tonumber(L,2);
	rad = lua_tonumber(L,3);
	
	SDL_Color *c1 = (SDL_Color *)lua_touserdata(L,4);
	
	if (!c1) 
		printf("%s : color is null\n",__FUNCTION__);
	else
		gui_draw_circle(c_surf,x,y,rad,*c1);	
	
	return 0;
}

static int gfx_CircleFill(lua_State *L)
{
	int x,y,rad;
	
	x   = lua_tonumber(L,1);
	y   = lua_tonumber(L,2);
	rad = lua_tonumber(L,3);
	
	SDL_Color *c1 = (SDL_Color *)lua_touserdata(L,4);
	
	if (!c1) 
		printf("%s : color is null\n",__FUNCTION__);
	else
		gui_draw_circle_fill(c_surf,x,y,rad,*c1);	
	
	return 0;
}

static int gfx_Clear(lua_State *L)
{
	SDL_FillRect(c_surf,NULL,0);
	return 0;
}

// 画像処理
static int gfx_Blit(lua_State *L)
{
	int x,y;

	SDL_Surface *p_surf = USERSURF(1);

	x  = lua_tonumber(L,2);
	y  = lua_tonumber(L,3);
	
	
	SDL_Rect rect;
	
	rect.x = x;	rect.y = y;
	rect.w = 0;	rect.h = 0;

	SDL_BlitSurface(p_surf,NULL,c_surf,&rect);
	return 0;
}


static int gfx_CopySurf(lua_State *L)
{
	SDL_Surface *src_surf  = USERSURF(1);
	SDL_Surface *dest_surf = USERSURF(2);
	
	int sx,sy,sw,sh,dx,dy;
	
	sx  = lua_tonumber(L,3);
	sy  = lua_tonumber(L,4);
	sw  = lua_tonumber(L,5);
	sh  = lua_tonumber(L,6);
	
	dx  = lua_tonumber(L,7);
	dy  = lua_tonumber(L,8);
	
	SDL_Rect src,dest;
	
	src.x = sx; src.y = sy;
	src.w = sw; src.h = sh;
	
	dest.x = dx; dest.y = dy;
	
	SDL_BlitSurface(src_surf,&src,dest_surf,&dest);
	
	return 0;
}

static int gfx_CopySurfAll(lua_State *L)
{
	SDL_Surface *src  = USERSURF(1);
	SDL_Surface *dest = USERSURF(2);
	
	SDL_BlitSurface(src,NULL,dest,NULL);	
	
	return 0;
}



static int gfx_BlitRect(lua_State *L)
{
	int sx,sy,sw,sh,dx,dy;

	SDL_Surface *p_surf = USERSURF(1);

	sx  = lua_tonumber(L,2);
	sy  = lua_tonumber(L,3);
	sw  = lua_tonumber(L,4);
	sh  = lua_tonumber(L,5);

	dx  = lua_tonumber(L,6);
	dy  = lua_tonumber(L,7);

	SDL_Rect src,dest;
	
	src.x = sx; src.y = sy;
	src.w = sw; src.h = sh;
	
	dest.x = dx; dest.y = dy;
	
	SDL_BlitSurface(p_surf,&src,c_surf,&dest);
	return 0;
}

static int gfx_ImgLoad(lua_State *L)
{
	SDL_Surface *image = IMG_Load(lua_tostring(L,1));
								 
	if (!image)
	{
		printf("imgload:%s\n",IMG_GetError());
		lua_pushnil( L );
	}
	else 
	{
		SDL_Surface **surf = (SDL_Surface **)lua_newuserdata(L,sizeof(SDL_Surface **));	
		*surf = image;

		lua_newtable(L);
		lua_pushliteral(L,"__gc");
		lua_pushcfunction(L,gfx_FreeSurf);
		lua_rawset(L,-3);
		lua_setmetatable(L,-2);
	}
	
	return 1;
}

static int gfx_FreeSurf(lua_State *L)
{
	SDL_Surface **surf = USERSURF_RAW(1);
	
	if (surf && *surf)
	{
		// printf("FreeSurf\n");
		SDL_FreeSurface(*surf);	
		*surf = NULL;
	}
	return 0;
}


/******************************************
 テキスト関連描画

******************************************/

static int gfx_OpenFont(lua_State *L)
{
	const char *fontfile = lua_tostring(L,1);
	int fh = lua_tonumber(L,2);
	
	TTF_Font *ttf = TTF_OpenFont ( fontfile , fh );

	if (!ttf)
	{
		printf("openfont : the font couldn't be open\n");
		lua_pushnil( L );
	}
	else
		lua_pushlightuserdata( L , ttf );

	return 1;
}

static int gfx_CloseFont(lua_State *L)
{
	TTF_CloseFont ( (TTF_Font *) lua_touserdata(L,1));
	
	return 0;
}

static int gfx_Text(lua_State *L)
{
	
	TTF_Font *ttffont = lua_touserdata(L,1);
	const char *msg   = lua_tostring(L,2);
	SDL_Color *color  = lua_touserdata(L,3);
	
	if (!ttffont || !msg || !color)
	{
		// printf("gfx_Text : null\n");
		return 0;
	}
	
	SDL_Surface *ttfsurf = TTF_RenderUTF8_Blended ( ttffont , msg , *color );
	
	if (ttfsurf)
	{
		SDL_Surface **surf = (SDL_Surface **)lua_newuserdata(L,sizeof(SDL_Surface **));		
		*surf = ttfsurf;
		
		lua_newtable(L);
		lua_pushliteral(L,"__gc");
		lua_pushcfunction(L,gfx_FreeSurf);
		lua_rawset(L,-3);
		lua_setmetatable(L,-2);
	}
	else
		lua_pushnil(L);
	
	return 1;
}

static int gfx_UTF8Len(lua_State *L)
{
	const unsigned char *msg = lua_tostring(L,1);
	if (!msg)
		return 0;
	
	lua_pushnumber(L,text_utf8_len( msg ) );
	
	return 1;
}

static int gfx_UTF8Next(lua_State *L)
{
	const unsigned char *msg = lua_tostring(L,1);
	if (!msg)
		return 0;
	
	lua_pushnumber(L,text_utf8_next_chr( msg ) );
	
	return 1;
}


static int gfx_TextSize(lua_State *L)
{
	
	TTF_Font *ttffont = lua_touserdata(L,1);
	const char *msg   = lua_tostring(L,2);
	
	if (!ttffont || !msg)
	{
		return 0;
	}
	
	int w,h;
	
	TTF_SizeUTF8 ( ttffont , msg , &w,&h);
	
	lua_pushnumber(L,w);
	lua_pushnumber(L,h);
	
	return 2;
}



static const struct luaL_reg graph_lib[] =
{

	{ "Lock"        , gfx_Lock },
	{ "Unlock"      , gfx_Unlock },
	{ "Update"      , gfx_Update },
	{ "SetSurf"     , gfx_SetSurf },
	{ "MainSurf"    , gfx_MainSurf },
	{ "FreeSurf"    , gfx_FreeSurf },
	{ "CreateSurf"  , gfx_CreateSurf },
	{ "Width"       , gfx_Width },
	{ "Height"      , gfx_Height },
	{ "SurfWidth"   , gfx_SurfWidth },
	{ "SurfHeight"  , gfx_SurfHeight },
	{ "SurfSize"    , gfx_SurfSize },
	{ "SaveSurf"    , gfx_SaveSurf },
	{ "Color"       , gfx_Color },
	{ "Pixel"       , gfx_Pixel },
	{ "Line"        , gfx_Line },
	{ "Poly"        , gfx_Poly },
	{ "Box"         , gfx_Box },
	{ "BoxFill"     , gfx_BoxFill },
	{ "BoxGrad"     , gfx_BoxGrad },
	{ "Circle"      , gfx_Circle },
	{ "CircleFill"  , gfx_CircleFill },
	{ "Clear"       , gfx_Clear },
	{ "Blit"        , gfx_Blit },
	{ "CopySurf"    , gfx_CopySurf },
	{ "CopySurfAll" , gfx_CopySurfAll },
	{ "BlitRect"    , gfx_BlitRect },
	{ "ImgLoad"     , gfx_ImgLoad },
	{ "OpenFont"    , gfx_OpenFont },
	{ "CloseFont"   , gfx_CloseFont },
	{ "Text"        , gfx_Text },
	{ "UTF8Len"     , gfx_UTF8Len },
	{ "UTF8Next"    , gfx_UTF8Next },
	{ "TextSize"    , gfx_TextSize },	
	
	{ NULL , NULL }
};


void graph_register(lua_State *L)
{
	g_update = 0;
	luaL_openlib(L,"graph",graph_lib,0);
}


