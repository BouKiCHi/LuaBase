/***************************
 Primitivev Graphic drawing routines
 **************************/

#include <SDL.h>
#include <math.h>

#include "mini_gui.h"

#define TBL_MAX 1024

#define getpixel_func(bits) \
static inline Uint##bits gui_getpixel##bits(SDL_Surface *data,int x,int y) \
{ \
if (x >= 0 && x < data->w && y >= 0 && y < data->h) \
return ((Uint##bits *)(data->pixels + (data->pitch * y)))[x]; \
return 0; \
} 

getpixel_func(8)
getpixel_func(16)
getpixel_func(32)

#define putpixel_func(bits) \
static inline void gui_putpixel##bits(SDL_Surface *data,int x,int y,Uint##bits color) \
{\
 if (x >= 0 && x < data->w && y >= 0 && y < data->h)  \
((Uint##bits *)(data->pixels + (data->pitch * y)))[x] = color; \
} 

putpixel_func(8)
putpixel_func(16)
putpixel_func(32)


// you have to lock the surface before use this!!
#define drawline_func(bits) \
void gui_draw_line##bits(SDL_Surface *surface,int x,int y,int px,int py,Uint##bits color) \
{ \
	int i,j; \
	float nx,ny; \
	float dx,dy; \
	\
	nx = x; ny = y; \
	\
	dx = px - x; \
	dy = py - y; \
\
	if (abs(dx) < abs(dy)) \
		j = abs(dy); \
	else \
		j = abs(dx); \
\
	dx /= j; \
	dy /= j; \
\
	for(i = 0; i <= j; i++) \
	{ \
		gui_putpixel##bits(surface,nx,ny,color); \
		nx += dx; \
		ny += dy; \
	} \
}

drawline_func(8)
drawline_func(16)
drawline_func(32)

typedef struct 
{ 
	int w;
	int h;
	unsigned char *data;
	unsigned char *hdirt;
} t_mask;


static inline void gui_put_maskpixel(t_mask *p,int x,int y,int bit)
{
	if (bit && ! p->data[(y * p->w) + x] && p->hdirt[y] < 0xff) 
		p->hdirt[y]++;
	
	if (x >= 0 && x < p->w && y >= 0 && y < p->h)
		p->data[(y * p->w) + x] = bit;
	
}

static inline int gui_get_maskpixel(t_mask *p,int x,int y)
{
	if (x >= 0 && x < p->w && y >= 0 && y < p->h)
		return p->data[(y * p->w) + x];
	
	return 0;
}

void gui_clear_mask ( t_mask *p )
{
	memset( p->data  , 0 , p->w * p->h );
	memset( p->hdirt , 0 , p->h );	
}

t_mask *gui_create_mask ( int w, int h )
{
	t_mask *p = (t_mask *)malloc( sizeof(t_mask) );

	p->data = (unsigned char *)malloc ( w * h );
	p->hdirt = (unsigned char *)malloc ( h );
	
	p->w = w; p->h = h;
	
	gui_clear_mask ( p );
	
	return p;
}

void gui_free_mask ( t_mask *p )
{
	if ( p->data )  free( p->data );
	if ( p->hdirt ) free( p->hdirt );
	
	p->data = NULL;
	p->hdirt = NULL;
	
	free ( p );	
}

void gui_draw_maskline( t_mask *p , int x,int y,int px,int py)
{
	int i,j; 
	float nx,ny;
	float dx,dy;
	
	nx = x; ny = y;
	
	dx = px - x;
	dy = py - y;
	
	j = abs(dy); 
	
	dx /= j;
	dy /= j; 
	for(i = 0; i <= j; i++) 
	{
			gui_put_maskpixel(p,nx,ny,1); 
			nx += dx; 
			ny += dy; 
	} 
}

int gui_scan_next ( t_mask *p , int x, int y )
{
	for ( x = x; x < p->w; x++ )
	{
		if (p->data[(p->w * y) + x])
			return x;
	}
	return -1;
}


int gui_scan_next_blank ( t_mask *p , int x, int y )
{
	for ( x = x; x < p->w; x++ )
	{
		if (!p->data[(p->w * y) + x])
			return x;
	}
	return -1;
}


void gui_fill_mask ( SDL_Surface *surface , t_mask *p , SDL_Color color )
{
	int x,y,flag,hold;
	int i;
	
	for ( y = 0; y < p->h; y++)
	{
		flag = 0;
		hold = 0;
		x = 0;
		
		if (p->hdirt[y])
		{
			while(1)
			{
				i = gui_scan_next ( p , x , y );
				if (i < 0)
					break;
				
				x = gui_scan_next_blank ( p , i , y );
				if (x < 0)
					break;
			
				x = gui_scan_next ( p , x , y );
				
				if (x < 0)
					break;
				
				gui_draw_line( surface , i  , y , x , y , color );
				
				x++;
			}			
		}
	}
}

void gui_fill_mask_ex ( SDL_Surface *surface , t_mask *p , SDL_Color color )
{
	int x,y,flag,hold;
	int start,end;
		
	for ( y = 0; y < p->h; y++)
	{
		flag = 0;
		hold = 0;
		start = -1;
		end = -1;
		
		if (p->hdirt[y])
		{
			for ( x = 0; x < p->w; x++ )
			{
				if (p->data[(p->w * y) + x] && !hold)
				{
					if (start >= 0 && end < 0)
						end = x;
					
					if (start < 0)
						start = x;

					hold = 1;
				} else
					hold = 0;					
			}
			
			if (start >= 0 && end >= 0)
				gui_draw_line( surface , start , y , end , y , color );
		}
	}
}


void gui_draw_triangle_fill (SDL_Surface *surface,int x[3],int y[3], SDL_Color color)
{
	t_mask *p = gui_create_mask ( surface->w , surface->h );
	
	gui_draw_maskline ( p , x[0] , y[0] , x[1] , y[1] );
	gui_draw_maskline ( p , x[1] , y[1] , x[2] , y[2] );
	gui_draw_maskline ( p , x[2] , y[2] , x[0] , y[0] );
	
	gui_fill_mask ( surface , p , color );
	
	gui_free_mask ( p );
}


void gui_draw_poly_fill (SDL_Surface *surface,int *x,int *y,int n,SDL_Color color)
{
	int i;
	if (n < 3)
		return;

	t_mask *p = gui_create_mask ( surface->w , surface->h );
	
	for ( i = 0; i < n-1; i++ )
	{
		gui_draw_maskline ( p , x[i] , y[i] , x[i+1] , y[i+1] );
	}
	gui_draw_maskline ( p , x[n-1] , y[n-1] , x[0] , y[0] );
	gui_fill_mask ( surface , p , color );
	
	gui_free_mask ( p );
}



void gui_draw_triangle_fill_l (SDL_Surface *surface,int x[3],int y[3], SDL_Color color)
{
	gui_draw_line ( surface , x[0] , y[0] , x[1] , y[1] , color );
	gui_draw_line ( surface , x[1] , y[1] , x[2] , y[2] , color );
	gui_draw_line ( surface , x[2] , y[2] , x[0] , y[0] , color );

}


void gui_putpixel( SDL_Surface *surface , int x , int y , SDL_Color color )
{
	Uint32 rgbc = SDL_MapRGB( surface->format , color.r , color.g , color.b );
	
	switch( surface->format->BytesPerPixel )
	{
		case 1:
			gui_putpixel8( surface , x , y , (Uint8)rgbc );
			break;
		case 2:
			gui_putpixel16( surface , x , y , (Uint16)rgbc );
			break;
		case 4:
			gui_putpixel32( surface , x , y , rgbc );
			break;
	}
}



void gui_draw_line( SDL_Surface *surface , int x , int y , int px , int py , SDL_Color color )
{
	Uint32 rgbc = SDL_MapRGB( surface->format , color.r , color.g , color.b );
	
	
	switch(surface->format->BytesPerPixel)
	{
		case 1:
			gui_draw_line8( surface , x , y , px , py , rgbc );
		break;
		case 2:
			gui_draw_line16( surface , x , y , px , py , rgbc );
			break;
		case 4:
			gui_draw_line32( surface , x , y , px , py , rgbc );
			break;
	}
}

// in : x,y,radius
void gui_draw_circle_fill(SDL_Surface *surface,int cx,int cy,int rad,SDL_Color color)
{
	int x,y;
	int hw;
	int i,j,k;
	
	if (!rad)
		return;

		
	Uint32 rgbc = SDL_MapRGB( surface->format , color.r , color.g , color.b );
	
	j = 0 - rad;
	k = rad * rad;
	
	for(i = 0; i < rad * 2; i++,j++)
	{

		hw = sqrt( k - ( j * j ) );

		switch(surface->format->BytesPerPixel)
		{
			case 1:
				gui_draw_line8 ( surface , cx - hw ,  cy + j , cx + hw , cy + j , rgbc );
				break;
			case 2:
				gui_draw_line16 ( surface , cx - hw , cy + j , cx + hw , cy + j , rgbc );
				break;
			case 4:
				gui_draw_line32 ( surface , cx - hw , cy + j , cx + hw , cy + j , rgbc );
				break;
		}
	}
}

#define circle_line(bits) \
 gui_draw_line##bits ( surface , cx - old_hw , cy + j , cx - hw , cy + j , rgbc ); \
 gui_draw_line##bits ( surface , cx + old_hw , cy + j , cx + hw , cy + j , rgbc )



void gui_draw_circle(SDL_Surface *surface,int cx,int cy,int rad,SDL_Color color)
{
	int x,y;
	int hw,old_hw;
	int i,j,k;
	
	if (!rad)
		return;
	
	Uint32 rgbc = SDL_MapRGB( surface->format , color.r , color.g , color.b );
	
	j = 0 - rad;
	k = rad * rad;

	old_hw = 0;
	
	for(i = 0; i <= rad * 2; i++,j++)
	{
		
		hw = sqrt( k - ( j * j ) );
		
		switch(surface->format->BytesPerPixel)
		{
			case 1:
				circle_line(8);
				break;
			case 2:
				circle_line(16);
				break;
			case 4:
				circle_line(32);
				break;
		}
		old_hw = hw;
	}
	
}

void gui_draw_box_grad(SDL_Surface *surface,int x,int y,int w,int h,SDL_Color c1,SDL_Color c2)
{ 
	int i,j; 
	int dx,dy;
	
	float rs = c2.r - c1.r;
	float gs = c2.g - c1.g;
	float bs = c2.b - c1.b;
	
	rs /= h; gs /= h; bs /= h;
	
	float r = c1.r;
	float g = c1.g;
	float b = c1.b;
		
	
	dx = x + w; 
	dy = y + h;
	
	if (dx > surface->w) dx = surface->w;
	if (dy > surface->h) dy = surface->h;
	
	switch(surface->format->BytesPerPixel)
	{
		case 1:
			for ( j = y; j < dy; j++ )
			{
				Uint32 rgbc = SDL_MapRGB( surface->format , r , g , b );
				r += rs; g += gs; b += bs;
				
				gui_draw_line8( surface , x , j , dx , j , rgbc );
			}

			break;
		case 2:
			
			for ( j = y; j < dy; j++ )
			{
				Uint32 rgbc = SDL_MapRGB( surface->format , r , g , b );
				r += rs; g += gs; b += bs;
				
				gui_draw_line16( surface , x , j , dx , j , rgbc );
			}
		break;
		case 3:
			for ( j = y; j < dy; j++ )
			{
				Uint32 rgbc = SDL_MapRGB( surface->format , r , g , b );
				r += rs; g += gs; b += bs;
				
				gui_draw_line32( surface , x , j , dx , j , rgbc );
			}
		break;
	}
}


/* original
void gui_draw_box_fill(SDL_Surface *surface,int x,int y,int w,int h,SDL_Color color)
{ 
	int i,j; 
	int dx,dy; 
	
	Uint32 rgbc = SDL_MapRGB(surface->format,color.r,color.g,color.b);

	dx = x+w; 
	dy = y+h;
	if (dx > surface->w) dx = surface->w;
	if (dy > surface->h) dy = surface->h;
	
	switch(surface->format->BytesPerPixel)
	{
		case 1:
			for ( j = y; j < dy; j++ )
				for ( i = x; i < dx; i++ )
					gui_putpixel8( surface , i , j , rgbc );
			break;
		case 2:
			for ( j = y; j < dy; j++ )
				for ( i = x; i < dx; i++ )
					gui_putpixel16( surface , i , j , rgbc );
			break;
		case 3:
			for ( j = y; j < dy; j++ )
				for ( i = x; i < dx; i++ )
					gui_putpixel32( surface , i , j , rgbc );
			break;
	}
} */

void gui_draw_box_fill(SDL_Surface *surface,int x,int y,int w,int h,SDL_Color color)
{ 
	SDL_Rect rct;

	Uint32 rgbc = SDL_MapRGB(surface->format,color.r,color.g,color.b);

	rct.x = x;
	rct.y = y;
	rct.w = w;
	rct.h = h;
	
	SDL_FillRect ( surface , &rct , rgbc );
}

void gui_init(void)
{
}

void gui_free(void)
{
}


long text_utf8_chr_2bytes( const unsigned char * str )
{
	if (! ( str[0] & 0x80 ) )
	{
		return str[0] & 0x7f;
	}
	else
	if ( ( str[0] & 0xe0 ) == 0xc0 )
	{
		return (((long)str[0] & 0x1f) << 6) | (str[1] & 0x3f);
	}
	return 0; //  it must be longer than 2 bytes or not utf8
}


int text_utf8_next_chr( const unsigned char * str )
{
	int pos = 0;
	if (! ( str [ pos ] & 0x80 ) )
	{
		pos++;
	}
	else 
		if ( ( str [ pos ] & 0xe0 ) == 0xc0 )
		{
			pos+=2;
		}
		else
			if ( ( str [ pos ] & 0xf0 ) == 0xe0 )
			{
				pos+=3;
			}
			else
				if ( ( str [ pos ] & 0xf8 ) == 0xf0 )
				{
					pos+=4;
				}
				else
					if ( ( str [ pos ] & 0xfc ) == 0xf8 )
					{
						pos+=5;
					}
					else
						if ( ( str [ pos ] & 0xfe ) == 0xfc )
						{
							pos+=6;
						}
	
	return pos;
}


int text_utf8_len ( const unsigned char *str )
{
	int pos = 0;
	int cnt = 0;
	
	while( *(str + pos ) )
	{
		pos += text_utf8_next_chr ( str + pos );
		cnt ++;
	}
	
	return pos;
}


int text_utf8_pos_by_len ( const unsigned char *str , int len )
{
	int pos = 0;
	int cnt = 0;
	
	while( *(str + pos ) )
	{
		pos += text_utf8_next_chr ( str + pos );
		cnt ++;
		if ( cnt >= len )
			return pos;
	}
	
	return pos;
}

// wide = zenkaku , half = hankaku
int text_utf8_pos_by_halflen ( unsigned char *str , int len )
{
	int pos = 0;
	int cnt = 0;
	
	
	while( *(str + pos ) )
	{
		int ret = text_utf8_next_chr ( str + pos );
		
		if (ret < 3)
		{
			int code = text_utf8_chr_2bytes( str + pos );
			
			if ( code > 0 && code < 0x100)
				cnt++;
		}
		else 
			cnt += 2;
		
		pos += ret;
		
		if (!ret)
			return pos; // may be failed
		
		if ( cnt >= len )
			return pos;
	}
	
	return pos;
}

