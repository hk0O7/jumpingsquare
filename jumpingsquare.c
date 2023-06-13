#include <stdio.h>
#include <SDL.h>

short int
	WIDTH = 800,
	HEIGHT = 600,
	DEPTH = 32,
	FRAMERATE = 60;
;

#define DEBUG 1

int
	square_x = 0,
	square_y = 0,
	square_mx = 0,
	square_my = 0,
	square_px = 0,
	square_py = 0,
	square_ppx = 0,
	square_ppy = 0,
	square_ix = 0,
	square_iy = 0,
	square_pix = 0,
	square_piy = 0,
	size = 100,
	initialspeed = 1,
	acceleration = 3,
	gravity = 3,
	deceleration_ground = 4,
	deceleration_air = 1,
	maxspeed = 30;
/*	frametime = 16,*/
/*	frame = 0;*/
short int
	decelerate = 0,
	forward = 0,
	jump = 0;

SDL_Surface *screen;
SDL_Event event;

void setPixel(int x, int y, Uint8 r, Uint8 g, Uint8 b) {
	Uint32 *pixmem32;
	Uint32 color;
	color = SDL_MapRGB(screen->format, r, g, b);
	pixmem32 = (Uint32*) screen->pixels + (y * WIDTH) + x;
	*pixmem32 = color;
}

void drawSquare(int x, int y, Uint8 tone) {
	for(short int iy = size + y; iy != y; iy--) {
		for(short int ix = x; ix != size + x; ix++) {
			setPixel(ix, HEIGHT - iy, tone, tone, tone);
		}
	}
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_WM_SetCaption("\"Jumping Square\" (by hk)", NULL);
	screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_SWSURFACE);
	short int
		frametime_target = 1000 / FRAMERATE,
		framedelay = frametime_target - 3;
	int ticks_start, ticks_end, frametime_real;
	int deceleration = deceleration_ground;
	
	
	drawsquare:;
	if(ticks_start && ticks_end) {
		frametime_real = ticks_end - ticks_start;
		if(frametime_real > frametime_target) framedelay--;
		else if(frametime_real < frametime_target) framedelay++;
		if(framedelay < 1) framedelay = 0;
	}
	ticks_start = SDL_GetTicks();
	#if DEBUG
		printf("frametime_target: %i; frametime_real: %i; framedelay: %i\n",
			frametime_target, frametime_real, framedelay);
		printf("Square x/y: %i %i; fwd/jmp: %i %i; movement: %i %i; decel: %i %i\n",
			square_x, square_y, forward, jump, square_mx, square_my, decelerate, deceleration);
	#endif
	
	// frame interpolation for motion blur
	square_ix = (square_x + square_px) / 2;
	square_iy = (square_y + square_py) / 2;
	drawSquare(square_px, square_py, 0);
	drawSquare(square_pix, square_piy, 0);
	drawSquare(square_ix, square_iy, 127);
	drawSquare(square_x, square_y, 255);
	
	int updaterect_x = square_x;
	if(updaterect_x > square_px) updaterect_x = square_px;
	if(updaterect_x > square_ppx) updaterect_x = square_ppx;
	
	int updaterect_y = square_y;
	if(updaterect_y < square_py) updaterect_y = square_py;
	if(updaterect_y < square_ppy) updaterect_y = square_ppy;
	
	int updaterect_w = square_x;
	if(updaterect_w < square_px) updaterect_w = square_px;
	if(updaterect_w < square_ppx) updaterect_w = square_ppx;
	updaterect_w = updaterect_w - updaterect_x + size;
	
	int updaterect_h = square_y;
	if(updaterect_h > square_py) updaterect_h = square_py;
	if(updaterect_h > square_ppy) updaterect_h = square_ppy;
	updaterect_h = size + (updaterect_y - updaterect_h);
	
	updaterect_y = HEIGHT - size - updaterect_y;
	
	#if DEBUG
		printf("Render boundaries: %i %i, %i %i\n",
			updaterect_x, updaterect_y, updaterect_w, updaterect_h);
	#endif
	
	SDL_UpdateRect(screen, updaterect_x, updaterect_y, updaterect_w, updaterect_h);
/*	SDL_Flip(screen);*/
	SDL_Delay(framedelay);
	
	square_ppx = square_px, square_ppy = square_py;
	square_px = square_x, square_py = square_y;
	square_pix = square_ix, square_piy = square_iy;
/*	frame++;*/
	
	// Control
	while(SDL_PollEvent(&event)) {
		if(event.type == SDL_KEYDOWN) {
			switch(event.key.keysym.sym) {
				case 275: //right
					forward = 1;
					break;
				case 276: //left
					forward = -1;
					break;
/*				case 274: //down */
/*					break;*/
				case 273: //up
				case SDLK_SPACE:
					jump = 1;
					break;
				case 'q':
				case SDLK_ESCAPE:
					goto end;
					break;
			}
		} else if(event.type == SDL_KEYUP) {
			switch(event.key.keysym.sym) {
				case 275: //right
					if(forward == 1) forward = 0;
					break;
				case 276: //left
					if(forward == -1) forward = 0;
					break;
				case 273: //up
				case SDLK_SPACE:
					jump = 0;
					break;
			}
		} else if(event.type == SDL_QUIT) goto end;
	}
	
	// Motion engine
	if(square_mx == 0) { //stopped
		switch(forward) {
			case 0:
				decelerate = 0;
				break;
			case 1:
				square_mx += initialspeed;
				break;
			case -1:
				square_mx -= initialspeed;
				break;
		}
	} else { //moving
		if(forward == 0) decelerate = 1;
		if(square_mx > 0) { //moving forward
			if(decelerate) {
				if(forward == 1) decelerate = 0;
				else {
					if(square_mx < deceleration) square_mx = 0;
					else if(forward == -1) square_mx -= deceleration * 2.2;
					else square_mx -= deceleration;
				}
			} else {
				if(forward == 1) {
					if(square_mx <= maxspeed) square_mx += acceleration;
				} else if(forward == -1) decelerate = 1;
			}
		} else { //moving backwards
			if(decelerate) {
				if(forward == -1) decelerate = 0;
				else {
					if(square_mx > (deceleration * -1)) square_mx = 0;
					else if(forward == 1) square_mx += deceleration * 2.2;
					else square_mx += deceleration;
				}
			} else {
				if(forward == -1) {
					if(square_mx >= (maxspeed * -1)) square_mx -= acceleration;
				} else if(forward == 1) decelerate = 1;
			}
		}
	}
	// jumping/falling
	if(square_y == 0 && jump) {
		deceleration = deceleration_air;
		square_my = acceleration * 15;
	} else {
		square_my -= gravity;
	}
	
	square_x += square_mx, square_y += square_my;
	
	// Boundaries
	if(square_x < 0) { 
		square_x = 0;
		if(square_mx < acceleration * -3) square_mx = square_mx / 1.1 * -1;
		else square_mx = 0;
	} else if(square_x > WIDTH - size) {
		square_x = WIDTH - size;
		if(square_mx > acceleration * 3) square_mx = square_mx / 1.1 * -1;
		else square_mx = 0;
	}
	if(
		(square_my < 0 && square_my > acceleration * -1) ||
		(square_my > 0 && square_my < acceleration)
	) square_my = 0;
	if(square_y < 0) {
		square_y = 0;
		deceleration = deceleration_ground;
		if(square_my < 0 && square_my < acceleration * -1) {
			// bounce
			square_my = (square_my / 2) * -1;
			deceleration = deceleration_air;
		} else square_my = 0;
	} else if(square_y > HEIGHT - size) square_my = 0, square_y = HEIGHT - size;
	
	ticks_end = SDL_GetTicks();
	goto drawsquare;
	
	end:
	SDL_Quit();
	return 0;
}
