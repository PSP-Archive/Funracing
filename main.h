/*
Copyright (C) 2007 Slempkes Clément

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#ifndef MAIN_H
#define MAIN_H

#define DEFAULT 222
#define SCREENW     480
#define SCREENH     272
#define FPS             50
#define GAMEMAXSPEED    3
#define SPEEDUP         0.2
#define MAX_PATH 100
#define COM 8
#define JOYSENS 40

typedef struct colorRGB color;
struct colorRGB
{
    Uint8 r;
    Uint8 g;
    Uint8 b;
};


int pause2(SDL_Surface *screen);
int init();
int init_joy();
int quit();
int game_main(SDL_Surface *screen);
void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel);
Uint32 readColor32(SDL_Surface* surface, int x, int y);
color readColor(SDL_Surface * surface, int x, int y);
int cont(int Nbr);
int initGameplay(int cup,int race,char *pic,char *map,char *race1, char *race2, char *race3, SDL_Rect *StartPos);
int gamePlay(SDL_Surface *screen, int nbrCOM, color *playerColor1, char *raceMapTxt, char *racePicTxt, int startX, int startY, int tour);
#endif
