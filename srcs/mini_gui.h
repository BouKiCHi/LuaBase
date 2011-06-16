/****************
 GUI PART 
 ***************/

#ifndef __mini_gui_h__
#define __mini_gui_h__

#include <SDL.h>


// gui init
void gui_init(void);

// gui free
void gui_free(void);


// draw pixel
void gui_putpixel(SDL_Surface *surface,int x,int y,SDL_Color color);

// draw line
void gui_draw_line(SDL_Surface *surface,int x,int y,int px,int py,SDL_Color color);

// draw box
void gui_draw_box_fill(SDL_Surface *surface,int x,int y,int w,int h,SDL_Color color);

// draw tri fill
void gui_draw_triangle_fill (SDL_Surface *surface,int x[3],int y[3], SDL_Color color);

// draw poly fill
void gui_draw_poly_fill (SDL_Surface *surface,int *x,int *y,int n,SDL_Color color);


// draw circle
void gui_draw_circle_fill(SDL_Surface *surface,int cx,int cy,int rad,SDL_Color color);

// utf8 len
int text_utf8_len ( const unsigned char *str );

// utf8 pos by len
int text_utf8_pos_by_len ( const unsigned char *str , int len );


#define SCREEN_W (320)
#define SCREEN_H (240)
#define SCREEN_CAPTION_H (16)
#define SCREEN_BAR_W (5)

#endif

