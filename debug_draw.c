#include <SDL.h>
#include <SDL_image.h>
#include <math.h>


#include "mini_gui.h"

SDL_Surface *gSurfaceMain = NULL;

int app_init(void)
{
	int i;
	
	gSurfaceMain = NULL;
		
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
		return -1;
	
	
	gSurfaceMain = SDL_SetVideoMode(320,240,16,0);
	
	if (!gSurfaceMain)
		return -1;
	
	gui_init();

	return 0;
}

void app_draw_tri(SDL_Surface *p , int angle )
{
	int i;
	int cx,cy;
	int x[3];
	int y[3];
	
	SDL_Color color = { 0xff,0x00,0xff,0x00 };

	cx = SCREEN_W / 2;
	cy = SCREEN_H / 2;
	
	for ( i = 0; i < 3; i++ )
	{
		x[i] = cx + cos( (3.141592 / 256) * angle) * 60;
		y[i] = cy + sin( (3.141592 / 256) * angle) * 60;
		angle += ( 512 / 3);
	}

	gui_draw_triangle_fill ( p , x , y , color );
	
}


void app_draw_poly(SDL_Surface *p , int angle )
{
	int i;
	int cx,cy;
	int x[10];
	int y[10];
	
	int n = 8;
	
	SDL_Color color = { 0x00,0x00,0xff,0x00 };
	
	cx = SCREEN_W / 2;
	cy = SCREEN_H / 2;
	
	for ( i = 0; i < n; i++ )
	{
		x[i] = cx + cos( (3.141592 / 256) * angle) * 60;
		y[i] = cy + sin( (3.141592 / 256) * angle) * 60;
		angle += ( 512 / n );
	}
	
	gui_draw_poly_fill ( p , x , y , n , color );
}

void app_draw(SDL_Surface *surface)
{
	static int angle = 0;
	SDL_Color gray = { 0x80,0x80,0x80,0x00 };
	SDL_Color black = { 0x00,0x00,0x00,0x00 };
	SDL_Color white = { 0xff,0xff,0xff,0x00 };
	
	int px,py;
	int cx,cy;
	cx = SCREEN_W / 2;
	cy = SCREEN_H / 2;
	
	px = cx + cos( (3.141592 / 256) * angle) * 60;
	py = cy + sin( (3.141592 / 256) * angle) * 60;
	
	if (SDL_MUSTLOCK(surface))
		SDL_LockSurface(surface);
	
	
	gui_draw_box_fill(surface,0,0,SCREEN_W,SCREEN_H, black );
	
	gui_draw_circle(surface, cx , cy , angle / 2 , gray );

	app_draw_tri ( surface , angle );

	app_draw_poly ( surface , angle );
	
	gui_draw_line(surface, cx , cy , px , py , white );
	
	
	
	if (SDL_MUSTLOCK(surface))
		SDL_UnlockSurface(surface);
	
	angle = (angle + 1) % 512;
}


void app_loop( char *file )
{	
	int g_quit = 0;

	SDL_Event event;

	SDL_Flip(gSurfaceMain);
	SDL_Flip(gSurfaceMain);
	
	while(!g_quit)
	{
		while(SDL_PollEvent(&event) && !g_quit)
		{
			
			switch(event.type)
			{
				case SDL_QUIT:
					g_quit = 1;
					break;
			}
		}
		
		app_draw(gSurfaceMain);
		SDL_Delay(10);
		SDL_Flip(gSurfaceMain);
	}	
	
}
	
void app_free(void)
{
	gui_free();
	SDL_Quit();
}

int main(int argc,char *argv[])
{
	
	if (app_init())
	{
		app_free();
		return -1;
	}
	app_loop( NULL );
	
	app_free();
	
	return 0;
}
