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
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <psppower.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>#include <SDL/SDL_ttf.h> #include "main.h"
#include <math.h>
#include <SDL/SDL_rotozoom.h>
#include <time.h>
#include <png.h>
#include <pspdisplay.h>
#include "screenshot.h"


/**
DEBUG :
FILE* debug = NULL;
debug = fopen("./debug", "w+");
fprintf(debug,"");
fclose(debug);
**/


typedef struct player player;
struct player
{
    double px, py;
    double speed;
    double angleR;
    double angleM;
    int detectRoad;
    int caractere;
    int borne;
    int tour;
    int count;
    int stat;
};


#define YES 1
#define NO 0
int classement(SDL_Surface *screen,int Playerstat);

void fondu(SDL_Surface *ecran);
void fondu(SDL_Surface *ecran)//use when you quit the game
{
     SDL_Surface *fondu = NULL;
     SDL_Rect pos;
     int opacity = 0;   
     pos.x = pos.y = 0;   
     fondu = SDL_CreateRGBSurface(SDL_HWSURFACE, 480, 272, 2, 0, 0, 0, 0); // Allocation de la surface
     while (opacity < 250){
           opacity += 10;
           SDL_FillRect(fondu, NULL, SDL_MapRGB(ecran->format,0, 0, 0)); 
           SDL_SetAlpha(fondu,SDL_SRCALPHA,opacity);
           SDL_BlitSurface(fondu, NULL, ecran, &pos);
           SDL_Flip(ecran);
           }
        sceKernelExitGame();
 }

int classement(SDL_Surface *screen,int Playerstat)
{
int continuer = 1;
SDL_Surface *backgroundyes = NULL, *backgroundno = NULL, *playertxt = NULL;
SDL_Rect pos0, pos;
TTF_Font *police = NULL;
SDL_Color fontColor = {0,0,0};
char tmp[MAX_PATH];
int statut = YES;
SceCtrlData pad;
int rep = 0;

police = TTF_OpenFont("./data/font.ttf",15);

backgroundyes = IMG_Load("./data/backgroundyes.png");
backgroundno = IMG_Load("./data/backgroundno.png");

if (Playerstat <= 3)
sprintf(tmp,"Bravo ! vous faites partie des trois meilleurs !");
else
sprintf(tmp,"Zut ! vous n'etes pas qualifier , il faut faire parti des 3 premiers !");
playertxt = TTF_RenderText_Blended(police, tmp, fontColor);


pos0.x = pos0.y = 0;
pos.x = 10;
pos.y = (SCREENH/2) - (playertxt->h / 2);

while(continuer)
	{
	sceCtrlPeekBufferPositive(&pad, 1); 
    if(pad.Buttons != 0)
	{
		if(pad.Buttons & PSP_CTRL_LEFT)
		statut = NO;
		if(pad.Buttons & PSP_CTRL_RIGHT)
		statut = YES;
		if(pad.Buttons & PSP_CTRL_CROSS)
			{
				continuer = 0;
				if(statut == NO)
				rep = 0;
				if(statut == YES && Playerstat > 3)
				rep = 1;
				if(statut == YES && Playerstat <= 3)
				rep = 2;				
			}
		
	}
		if(statut == YES)
		SDL_BlitSurface(backgroundyes,NULL,screen,&pos0);
		if(statut == NO)
		SDL_BlitSurface(backgroundno,NULL,screen,&pos0);
		SDL_BlitSurface(playertxt,NULL,screen,&pos);	
		SDL_Flip(screen);	
	}

SDL_FreeSurface(backgroundyes);
SDL_FreeSurface(backgroundno);
SDL_FreeSurface(playertxt);
TTF_CloseFont(police);
return rep;
}



int pause2(SDL_Surface *screen)
{
SceCtrlData pad;
int continuer = 1;
SDL_Surface *pause = NULL;
SDL_Rect pausePos;
pause = IMG_Load("./data/pause.png");
u32 oldpad = 0;

pausePos.x = (SCREENW/2) - (pause->w/2);
pausePos.y = (SCREENH/2) - (pause->h/2);  

while (continuer)
	{	
sceCtrlPeekBufferPositive(&pad, 1);// init des touches de la psp 
if (oldpad != pad.Buttons) {
		
		switch(pad.Buttons)
			{
				case PSP_CTRL_START :
				continuer = 0;
				break;
			}
				}
	oldpad = pad.Buttons;
	SDL_BlitSurface(pause,NULL,screen,&pausePos);
	SDL_Flip(screen);
	}
SDL_FreeSurface(pause);

return 0;
}


int 
psp_sys_frequence(int mhz)
{
	if (mhz != DEFAULT)
		scePowerSetClockFrequency(mhz, mhz, (mhz/2) );    
return 0;
}

int 
gamePlay(SDL_Surface *screen, int nbrCOM, color *playerColor1, char *raceMapTxt, char *racePicTxt, int startX, int startY, int tour)
{

    psp_sys_frequence(333);
    int close = 0, prev = 0, current = 0, count = 0, count2 = 0, i = 0, j = 0;
    int start = 0;
    int rep = 0;
    int winCount = 1;
    SceCtrlData pad;
    int quick = 0,checkpoint = 0;
    int fini =0;

    color colorV = {0,0,0};
    color colorL = {0,0,0};
    color colorR = {0,0,0};		
	
    Mix_Chunk *tut = NULL;
    Mix_Chunk *tit = NULL;
    Mix_Chunk *blabla = NULL;
    Mix_Chunk *laps = NULL;
    Mix_Chunk *check = NULL;

    Mix_Music* music = NULL;
    Mix_Chunk* skid = NULL;

    SDL_Surface *raceMap = NULL;
    SDL_Surface *racePic = NULL;
    SDL_Surface *racePicFG = NULL;
    SDL_Surface *trace = NULL;
    SDL_Surface *colision = NULL;

    color colorColision[8] = {{0}};

    SDL_Surface *tmp = NULL, *refCars = NULL, *colorCars = NULL, *rotatedCars = NULL, *feu = NULL, *txt = NULL,*checkpointxt = NULL;
    SDL_Rect rect,clip;
    clip.w = 14;
    clip.h = 36;
    clip.x = clip.y = 0;
    TTF_Font *police = NULL;
    SDL_Color colorWhite = {255, 255, 255}, colorBlack = {0, 0, 0};
    char nbrTxt[10];

    feu = IMG_Load("data/feu.bmp");

    player gamePlayer;
    player gameCOM[nbrCOM];
    color COMColor1[nbrCOM];
    color COMColor2[nbrCOM];
    color COMColor3[nbrCOM];

    raceMap = IMG_Load(raceMapTxt);
    trace = IMG_Load("data/trace.png");
    colision = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREENW, SCREENH, 32, 0, 0, 0, 0);
    SDL_FreeSurface(tmp);
    tmp = IMG_Load(racePicTxt);
    racePic = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);
    tmp = IMG_Load("data/tracks/NULL.png");
    racePicFG = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);
    tmp = IMG_Load("data/car.png");
    refCars = SDL_DisplayFormatAlpha(tmp);
    colorCars = SDL_DisplayFormatAlpha(tmp);
    


    for (count = 0; count < nbrCOM ; count++)
    {
        if (count2 == 4)
        {
            startY += refCars->h + 5;
            count2 = 0;
        }
        count2++;

        gameCOM[count].px = startX + ((refCars->w + 2) * count2);
        gameCOM[count].py = startY;

        gameCOM[count].speed = gameCOM[count].tour = gameCOM[count].count = gameCOM[count].borne = gameCOM[count].stat = 0;

        gameCOM[count].angleR = ((rand() % (45 - (-45) + 1)) + (-45));
        gameCOM[count].angleM = gameCOM[count].angleR;

        gameCOM[count].detectRoad = 5;

        gameCOM[count].caractere = ((rand() % (5 - 1 + 1)) + 1);

        COMColor1[count].r = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor1[count].g = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor1[count].b = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor2[count].r = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor2[count].g = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor2[count].b = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor3[count].r = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor3[count].g = ((rand() % (3 - 0 + 1)) + 0) * 85;
        COMColor3[count].b = ((rand() % (3 - 0 + 1)) + 0) * 85;
    }        if (count2 == 4)
        {
            startY += refCars->h + 5;
            count2 = 0;
        }
        count2++;

        gamePlayer.px = startX + ((refCars->w + 2) * count2);
        gamePlayer.py = startY;

        gamePlayer.speed = gamePlayer.tour = gamePlayer.count = gamePlayer.borne = gamePlayer.stat = 0;

        gamePlayer.angleR = ((rand() % (45 - (-45) + 1)) + (-45));
        gamePlayer.angleM = gamePlayer.angleR;

        SDL_FreeSurface(tmp);

        tut = Mix_LoadWAV( "data/sounds/tut.wav");
        blabla = Mix_LoadWAV( "data/sounds/blabla.wav");

        Mix_PlayChannel( -1, blabla, 0 ); 

	while(!close)
	{
	sceCtrlPeekBufferPositive(&pad, 1);// init des touches de la psp 
        current = SDL_GetTicks();	
	if(pad.Buttons & PSP_CTRL_SELECT)fondu(screen);
        if (current - prev > (1000/FPS))
        {
if(start > 210)
{

/** IA ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------
------------------------------
-------------------
-----------
----
--
-**/
            for (count = 0; count < nbrCOM ; count++)
            {
                /*Up/Right*/colorColision[0] = readColor(colision, (gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Up/Left*/colorColision[1] = readColor(colision, (gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Down/Right*/colorColision[2] = readColor(colision, (gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Down/Left*/colorColision[3] = readColor(colision, (gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ));

                /*Right*/colorColision[4] = readColor(colision, gameCOM[count].px + cos((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) + 1), gameCOM[count].py + sin((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) + 1));
                /*Left*/colorColision[5] = readColor(colision, gameCOM[count].px + cos((gameCOM[count].angleR - 180) * M_PI / 180) * ((refCars->w / 2) + 1), gameCOM[count].py + sin((gameCOM[count].angleR - 180) * M_PI / 180) * ((refCars->w / 2) + 1));
                /*Up*/colorColision[6] = readColor(colision, gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1), gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1));
                /*Down*/colorColision[7] = readColor(colision, gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1), gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1));

                if(colorColision[0].g != 254)
                {
                    gameCOM[count].angleM -= 2;
                    gameCOM[count].angleR -= 2;
                }
                if(colorColision[1].g != 254)
                {
                    gameCOM[count].angleM += 2;
                    gameCOM[count].angleR += 2;
                }
                if(colorColision[2].g != 254)
                {
                    gameCOM[count].angleM += 2;
                    gameCOM[count].angleR += 2;
                }
                if(colorColision[3].g != 254)
                {
                    gameCOM[count].angleM -= 2;
                    gameCOM[count].angleR -= 2;
                }
                if(colorColision[4].g != 254)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM - 180) * M_PI / 180) * GAMEMAXSPEED;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM - 180) * M_PI / 180) * GAMEMAXSPEED;
                }
                if(colorColision[5].g != 254)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM) * M_PI / 180) * GAMEMAXSPEED;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM) * M_PI / 180) * GAMEMAXSPEED;
                }
                if(colorColision[6].g != 254)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM + 90) * M_PI / 180) * GAMEMAXSPEED;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM + 90) * M_PI / 180) * GAMEMAXSPEED;
                }
                if(colorColision[7].g != 254)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * GAMEMAXSPEED;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * GAMEMAXSPEED;
                }

                /*Up/Right*/colorColision[0] = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Up/Left*/colorColision[1] = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Down/Right*/colorColision[2] = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gameCOM[count].angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Down/Left*/colorColision[3] = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gameCOM[count].angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ));

                /*Right*/colorColision[4] = readColor(raceMap, gameCOM[count].px + cos((gameCOM[count].angleR) * M_PI / 180) * (refCars->w / 2), gameCOM[count].py + sin((gameCOM[count].angleR) * M_PI / 180) * (refCars->w / 2));
                /*Left*/colorColision[5] = readColor(raceMap, gameCOM[count].px + cos((gameCOM[count].angleR - 180) * M_PI / 180) * (refCars->w / 2), gameCOM[count].py + sin((gameCOM[count].angleR - 180) * M_PI / 180) * (refCars->w / 2));
                /*Up*/colorColision[6] = readColor(raceMap, gameCOM[count].px + cos((gameCOM[count].angleR - 90) * M_PI / 180) * (refCars->h / 2), gameCOM[count].py + sin((gameCOM[count].angleR - 90) * M_PI / 180) * (refCars->h / 2));
                /*Down*/colorColision[7] = readColor(raceMap, gameCOM[count].px + cos((gameCOM[count].angleR + 90) * M_PI / 180) * (refCars->h / 2), gameCOM[count].py + sin((gameCOM[count].angleR + 90) * M_PI / 180) * (refCars->h / 2));

                if(gameCOM[count].speed > colorColision[6].g / (255/GAMEMAXSPEED))
                {
                    gameCOM[count].speed = colorColision[6].g / (255/GAMEMAXSPEED);
                }
                else if(gameCOM[count].speed < -colorColision[7].g / (255/GAMEMAXSPEED))
                {
                    gameCOM[count].speed = -colorColision[7].g / (255/GAMEMAXSPEED);
                }
                if(colorColision[0].g == 0)
                {
                    gameCOM[count].angleM -= 2;
                    gameCOM[count].angleR -= 2;
                }
                if(colorColision[1].g == 0)
                {
                    gameCOM[count].angleM += 2;
                    gameCOM[count].angleR += 2;
                }
                if(colorColision[2].g == 0)
                {
                    gameCOM[count].angleM += 2;
                    gameCOM[count].angleR += 2;
                }
                if(colorColision[3].g == 0)
                {
                    gameCOM[count].angleM -= 2;
                    gameCOM[count].angleR -= 2;
                }
                if(colorColision[4].g == 0)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM - 180) * M_PI / 180) * 1;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM - 180) * M_PI / 180) * 1;
                }
                if(colorColision[5].g == 0)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM) * M_PI / 180) * 1;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM) * M_PI / 180) * 1;
                }
                if(colorColision[6].g == 0)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM + 90) * M_PI / 180) * 1;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM + 90) * M_PI / 180) * 1;
                }
                if(colorColision[7].g == 0)
                {
                    gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * 1;
                    gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * 1;
                }

                if(gameCOM[count].stat == 0)
                {
                    colorV = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 10) + (refCars->h / 2))),
                                                     (gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 10) + (refCars->h / 2))));

                    colorL = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 12) + (refCars->h / 2))) + (cos((gameCOM[count].angleM) * M_PI / 180) * (gameCOM[count].detectRoad + (refCars->w / 2))),
                                                     (gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 12) + (refCars->h / 2))) + (sin((gameCOM[count].angleM) * M_PI / 180) * (gameCOM[count].detectRoad + (refCars->w / 2))));

                    colorR = readColor(raceMap, (gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 10) + (refCars->h / 2))) + (cos((gameCOM[count].angleM) * M_PI / 180) * -(gameCOM[count].detectRoad + (refCars->w / 2))),
                                                     (gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * ((gameCOM[count].speed * 10) + (refCars->h / 2))) + (sin((gameCOM[count].angleM) * M_PI / 180) * -(gameCOM[count].detectRoad + (refCars->w / 2))));

                    switch (gameCOM[count].caractere)
                    {
                        case 1:
                            if(colorL.g != 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].speed -= SPEEDUP;
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].detectRoad < 10)
                                    gameCOM[count].detectRoad += 2;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g == 255)
                            {
                                gameCOM[count].detectRoad -= 4;
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;
                                if(gameCOM[count].speed < SPEEDUP)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].detectRoad--;
                            }
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.2)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            else if(colorL.g != 255 && colorV.g != 255 && colorR.g == 255)
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            break;
///////////////////////////////////////////////////////////////////////////////////////////////////////////
                        case 2:
                            if(colorL.g != 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].speed -= SPEEDUP;
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.2)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].detectRoad < ((rand() % (25 - 16 + 1)) + 16))
                                    gameCOM[count].detectRoad += 4;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g == 255)
                            {
                                gameCOM[count].detectRoad -= 4;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;
                                if(gameCOM[count].speed < SPEEDUP)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].detectRoad--;
                            }
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.2)
                                    gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            else if(colorL.g != 255 && colorV.g != 255 && colorR.g == 255)
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            break;
////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        case 3:
                            if(colorL.g != 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].speed -= SPEEDUP;
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].detectRoad < 15)
                                    gameCOM[count].detectRoad += 1;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g == 255)
                            {
                                gameCOM[count].detectRoad -= 5;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].speed < SPEEDUP)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].detectRoad--;
                            }
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.3)
                                    gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            else if(colorL.g != 255 && colorV.g != 255 && colorR.g == 255)
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            break;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        case 4:
                            if(colorL.g != 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].speed -= SPEEDUP;
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].detectRoad < 25)
                                    gameCOM[count].detectRoad += 1;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g == 255)
                            {
                                gameCOM[count].detectRoad -= 5;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].speed < SPEEDUP)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].detectRoad--;
                            }
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            else if(colorL.g != 255 && colorV.g != 255 && colorR.g == 255)
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            break;
////////////////////////////////////////////////////////////////////////////////////////////////////////
                        case 5:
                            if(colorL.g != 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].speed -= SPEEDUP;
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].detectRoad < 18)
                                    gameCOM[count].detectRoad += 1;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g == 255)
                            {
                                gameCOM[count].detectRoad -= 5;
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                gameCOM[count].speed += SPEEDUP;

                                if(gameCOM[count].speed < SPEEDUP)
                                    gameCOM[count].speed += SPEEDUP;
                                gameCOM[count].detectRoad--;
                            }
                            else if(colorL.g == 255 && colorV.g == 255 && colorR.g != 255)
                            {
                                gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR += GAMEMAXSPEED;
                            }
                            else if(colorL.g != 255 && colorV.g == 255 && colorR.g == 255)
                            {
                                if(gameCOM[count].speed < GAMEMAXSPEED - 0.1)
                                    gameCOM[count].speed += SPEEDUP;

                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            }
                            else if(colorL.g == 255 && colorV.g != 255 && colorR.g != 255)
                                gameCOM[count].angleR += GAMEMAXSPEED;
                            else if(colorL.g != 255 && colorV.g != 255 && colorR.g == 255)
                                gameCOM[count].angleR -= GAMEMAXSPEED;
                            break;
                    }

                }

                if(gameCOM[count].angleM < gameCOM[count].angleR)
                {
                    gameCOM[count].angleM += ((GAMEMAXSPEED*2)-0.7) - (gameCOM[count].speed);
                    if(gameCOM[count].angleM > gameCOM[count].angleR){gameCOM[count].angleM = gameCOM[count].angleR;}
                }
                else if(gameCOM[count].angleM > gameCOM[count].angleR)
                {
                    gameCOM[count].angleM -= ((GAMEMAXSPEED*2)-0.7) - (gameCOM[count].speed);
                    if(gameCOM[count].angleM < gameCOM[count].angleR){gameCOM[count].angleM = gameCOM[count].angleR;}
                }

                if(gameCOM[count].speed > 0)
                {
                    gameCOM[count].speed -= 0.05 + (fabs(gameCOM[count].angleM - gameCOM[count].angleR) / 3);
                    if(gameCOM[count].speed < 0){gameCOM[count].speed = 0;}
                }
                else if(gameCOM[count].speed < 0)
                {
                    gameCOM[count].speed += 0.05 + (fabs(gameCOM[count].angleM - gameCOM[count].angleR) / 3);
                    if(gameCOM[count].speed > 0){gameCOM[count].speed = 0;}
                }


                if(readColor(raceMap, gameCOM[count].px, gameCOM[count].py).r == 255)
                {
                    if(gameCOM[count].borne == 1)
                    {
                        gameCOM[count].tour++;
                        gameCOM[count].borne = 0;
                    }
                }
                if(readColor(raceMap, gameCOM[count].px, gameCOM[count].py).b == 255)
                {
                    if(gameCOM[count].borne == 0)
                    {
                        gameCOM[count].borne = 1;
                    }
                }
                if(gameCOM[count].stat == 0)
                {
                    if(gameCOM[count].tour == tour)
                    {
                        gameCOM[count].stat = winCount;
                        winCount++;
                    }
                }
            }

/** FIN IA ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------
------------------------------
-------------------
-----------
----
--
-**/


/** PLAYER ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------
------------------------------
-------------------
-----------
----
--
-**/

                /*Up/Right*/colorColision[0] = readColor(colision, (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Up/Left*/colorColision[1] = readColor(colision, (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Down/Right*/colorColision[2] = readColor(colision, (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) ));
                /*Down/Left*/colorColision[3] = readColor(colision, (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (cos((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ), (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1) ) + (sin((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 2) ));

                /*Right*/colorColision[4] = readColor(colision, gamePlayer.px + cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) + 1), gamePlayer.py + sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) + 1));
                /*Left*/colorColision[5] = readColor(colision, gamePlayer.px + cos((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) + 1), gamePlayer.py + sin((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) + 1));
                /*Up*/colorColision[6] = readColor(colision, gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1), gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) + 1));
                /*Down*/colorColision[7] = readColor(colision, gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1), gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) + 1));



                if(colorColision[0].g != 254)
                {
                    gamePlayer.angleM -= 2;
                    gamePlayer.angleR -= 2;
                }
                if(colorColision[1].g != 254)
                {
                    gamePlayer.angleM += 2;
                    gamePlayer.angleR += 2;
                }
                if(colorColision[2].g != 254)
                {
                    gamePlayer.angleM += 2;
                    gamePlayer.angleR += 2;
                }
                if(colorColision[3].g != 254)
                {
                    gamePlayer.angleM -= 2;
                    gamePlayer.angleR -= 2;
                }
                if(colorColision[4].g != 254)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM - 180) * M_PI / 180) * GAMEMAXSPEED;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM - 180) * M_PI / 180) * GAMEMAXSPEED;
                }
                if(colorColision[5].g != 254)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM) * M_PI / 180) * GAMEMAXSPEED;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM) * M_PI / 180) * GAMEMAXSPEED;
                }
                if(colorColision[6].g != 254)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM + 90) * M_PI / 180) * 2;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM + 90) * M_PI / 180) * 2;
                    gamePlayer.speed -= 0.2;
                }
                if(colorColision[7].g != 254)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM - 90) * M_PI / 180) * 2;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM - 90) * M_PI / 180) * 2;
                    gamePlayer.speed += 0.2;
                }

                /*Up/Right*/colorColision[0] = readColor(raceMap, (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Up/Left*/colorColision[1] = readColor(raceMap, (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Down/Right*/colorColision[2] = readColor(raceMap, (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 1) ));
                /*Down/Left*/colorColision[3] = readColor(raceMap, (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (cos((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ), (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 1) ) + (sin((gamePlayer.angleR-180) * M_PI / 180) * ((refCars->w / 2) - 1) ));

                /*Right*/colorColision[4] = readColor(raceMap, gamePlayer.px + cos((gamePlayer.angleR) * M_PI / 180) * (refCars->w / 2), gamePlayer.py + sin((gamePlayer.angleR) * M_PI / 180) * (refCars->w / 2));
                /*Left*/colorColision[5] = readColor(raceMap, gamePlayer.px + cos((gamePlayer.angleR - 180) * M_PI / 180) * (refCars->w / 2), gamePlayer.py + sin((gamePlayer.angleR - 180) * M_PI / 180) * (refCars->w / 2));
                /*Up*/colorColision[6] = readColor(raceMap, gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * (refCars->h / 2), gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * (refCars->h / 2));
                /*Down*/colorColision[7] = readColor(raceMap, gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * (refCars->h / 2), gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * (refCars->h / 2));


                if(gamePlayer.speed > colorColision[6].g / (255/GAMEMAXSPEED))
                {
                    gamePlayer.speed = colorColision[6].g / (255/GAMEMAXSPEED);
                }
                else if(gamePlayer.speed < -colorColision[7].g / (255/GAMEMAXSPEED))
                {
                    gamePlayer.speed = -colorColision[7].g / (255/GAMEMAXSPEED);
                }
                if(colorColision[0].g == 0)
                {
                    gamePlayer.angleM -= 2;
                    gamePlayer.angleR -= 2;
                }
                if(colorColision[1].g == 0)
                {
                    gamePlayer.angleM += 2;
                    gamePlayer.angleR += 2;
                }
                if(colorColision[2].g == 0)
                {
                    gamePlayer.angleM += 2;
                    gamePlayer.angleR += 2;
                }
                if(colorColision[3].g == 0)
                {
                    gamePlayer.angleM -= 2;
                    gamePlayer.angleR -= 2;
                }
                if(colorColision[4].g == 0)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM - 180) * M_PI / 180) * 1;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM - 180) * M_PI / 180) * 1;
                }
                if(colorColision[5].g == 0)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM) * M_PI / 180) * 1;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM) * M_PI / 180) * 1;
                }
                if(colorColision[6].g == 0)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM + 90) * M_PI / 180) * 1;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM + 90) * M_PI / 180) * 1;
                }
                if(colorColision[7].g == 0)
                {
                    gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM - 90) * M_PI / 180) * 1;
                    gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM - 90) * M_PI / 180) * 1;
                }

//pad
                if(gamePlayer.stat == 0 && pad.Buttons != 0)
                {
		   if (pad.Buttons & PSP_CTRL_RTRIGGER)
			nomduscreenshot("screenshot",rand() % 999,".png"); 
                   if (pad.Buttons & PSP_CTRL_CROSS)
                             gamePlayer.speed += SPEEDUP;
		   if (pad.Buttons & PSP_CTRL_SQUARE)
                             gamePlayer.speed -= SPEEDUP;
                   if (pad.Buttons & PSP_CTRL_LEFT)
					{
                                if(gamePlayer.speed > SPEEDUP || gamePlayer.speed < -SPEEDUP)
                                gamePlayer.angleR -= GAMEMAXSPEED;
					}
                    if (pad.Buttons & PSP_CTRL_RIGHT)
					{
                                if(gamePlayer.speed > SPEEDUP || gamePlayer.speed < -SPEEDUP)
                                 gamePlayer.angleR += GAMEMAXSPEED;
					}
		  if(pad.Buttons & PSP_CTRL_START)
					{
						sceKernelDelayThread(500000);	
						pause2(screen);	
						sceKernelDelayThread(1000000);		
					}
                }

//joystick
		if(pad.Lx > (255 - JOYSENS))
					{
                                if(gamePlayer.speed > SPEEDUP || gamePlayer.speed < -SPEEDUP)
                                 gamePlayer.angleR += GAMEMAXSPEED;
					}
		if(pad.Lx < JOYSENS)
					{
                                if(gamePlayer.speed > SPEEDUP || gamePlayer.speed < -SPEEDUP)
                                gamePlayer.angleR -= GAMEMAXSPEED;
					}
                

                if(gamePlayer.angleM < gamePlayer.angleR)
                {
                    gamePlayer.angleM += ((GAMEMAXSPEED*2)-0.7) - (gamePlayer.speed);
                    if(gamePlayer.angleM > gamePlayer.angleR)gamePlayer.angleM = gamePlayer.angleR;
                }
                else if(gamePlayer.angleM > gamePlayer.angleR)
                {
                    gamePlayer.angleM -= ((GAMEMAXSPEED*2)-0.7) - (gamePlayer.speed);
                    if(gamePlayer.angleM < gamePlayer.angleR)gamePlayer.angleM = gamePlayer.angleR;
                }


                if(gamePlayer.speed > 0)
                {
                    gamePlayer.speed -= 0.05 + (fabs(gamePlayer.angleM - gamePlayer.angleR) / 230);
                    if(gamePlayer.speed < 0)gamePlayer.speed = 0;
                }
                else if(gamePlayer.speed < 0)
                {
                    gamePlayer.speed += 0.05 + (fabs(gamePlayer.angleM - gamePlayer.angleR) / 230);
                    if(gamePlayer.speed > 0)gamePlayer.speed = 0;
                }


                if(readColor(raceMap, gamePlayer.px, gamePlayer.py).r == 255)
                {
                    if(gamePlayer.borne == 1)
                    {
                        gamePlayer.tour++;
                        gamePlayer.borne = 0;
                        gamePlayer.count = 100;
			Mix_PlayChannel(-1, laps,0);
                    }
                }
                if(readColor(raceMap, gamePlayer.px, gamePlayer.py).b == 255)
                {
                    if(gamePlayer.borne == 0)
                    {
                        gamePlayer.borne = 1;
			Mix_PlayChannel(-1, check,0);
			checkpoint = 1;
                    }
                }
                if(gamePlayer.stat == 0)
                {
                    if(gamePlayer.tour == tour)
                    {
                        gamePlayer.stat = winCount;
                        winCount++;
                    }
                }
            }
 if(start < 211)
	{
            if(start == 90){Mix_PlayChannel(-1, tut,0);}
            if(start == 130){Mix_PlayChannel(-1, tut,0);}
            if(start == 170){Mix_PlayChannel(-1, tut,0);}
            if(start == 210){Mix_PlayChannel(-1, tit,0); Mix_PlayMusic(music, -1);}
	}

 if(winCount >= 2)
	{	
	fini = 1;
		for(count = 0; count < COM; count++)
			{
				if(gameCOM[count].tour != 3 || gamePlayer.tour != 3) 
				fini = 0;	
			}
		if(fini)
		{
	          Mix_FreeChunk(tut);
                  Mix_FreeChunk(tit);
                  Mix_FreeChunk(blabla);
                  Mix_FreeChunk(laps);
                  Mix_FreeChunk(check);
		  rep = classement(screen,gamePlayer.stat);
		  close = 1;
		}
   	}

/** FIN PLAYER ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------
-------------------------------------------------------
------------------------------
-------------------
-----------
----
--
-**/


if(start < 300)
  {
    start++;

  }
            SDL_FillRect(colision, NULL, SDL_MapRGB(colision->format, 254, 254, 254));
            SDL_BlitSurface(racePic, NULL, screen, NULL);

            for (count = 0 ; count < nbrCOM ; count++)
            {
                SDL_LockSurface(colorCars);
                for (i = 0; i < refCars->h ; i++)
                {
                    for (j = 0; j < refCars->w ; j++)
                    {
                        if(readColor(refCars, j, i).r == 192)
                            setPixel(colorCars, j, i, SDL_MapRGB(colorCars->format, COMColor1[count].r, COMColor1[count].g, COMColor1[count].b));
                        else if(readColor(refCars, j, i).r == 128)
                            setPixel(colorCars, j, i, SDL_MapRGB(colorCars->format, COMColor2[count].r, COMColor2[count].g, COMColor2[count].b));
                        else if(readColor(refCars, j, i).r == 255)
                            setPixel(colorCars, j, i, SDL_MapRGB(colorCars->format, COMColor3[count].r, COMColor3[count].g, COMColor3[count].b));
                    }
                }
                SDL_UnlockSurface(colorCars);

                gameCOM[count].px = gameCOM[count].px + cos((gameCOM[count].angleM - 90) * M_PI / 180) * gameCOM[count].speed;
                gameCOM[count].py = gameCOM[count].py + sin((gameCOM[count].angleM - 90) * M_PI / 180) * gameCOM[count].speed;

                SDL_FreeSurface(rotatedCars);

                rotatedCars = rotozoomSurface(colorCars, cont(gameCOM[count].angleR), 1.0, 1);

                rect.x = gameCOM[count].px - rotatedCars->w / 2;
                rect.y = gameCOM[count].py - rotatedCars->h / 2;

                SDL_BlitSurface(rotatedCars , NULL, screen, &rect);
                SDL_FreeSurface(rotatedCars);

                rotatedCars = rotozoomSurface(colorCars, cont(gameCOM[count].angleR), 1.0, 0);
                SDL_BlitSurface(rotatedCars , NULL, colision, &rect);

            }
                if(fabs(gamePlayer.angleM - gamePlayer.angleR) > 25)
                {
                    rect.x = (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    rect.y = (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    SDL_BlitSurface(trace , NULL, racePic, &rect);

                    rect.x = (gamePlayer.px + cos((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (cos((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    rect.y = (gamePlayer.py + sin((gamePlayer.angleR - 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (sin((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    SDL_BlitSurface(trace , NULL, racePic, &rect);

                    rect.x = (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (cos((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    rect.y = (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (sin((gamePlayer.angleR) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    SDL_BlitSurface(trace , NULL, racePic, &rect);

                    rect.x = (gamePlayer.px + cos((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (cos((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    rect.y = (gamePlayer.py + sin((gamePlayer.angleR + 90) * M_PI / 180) * ((refCars->h / 2) - 2) ) + (sin((gamePlayer.angleR - 180) * M_PI / 180) * ((refCars->w / 2) - 2) );
                    SDL_BlitSurface(trace , NULL, racePic, &rect);

                    Mix_PlayChannel(-1, skid,0);
                }


                SDL_LockSurface(colorCars);
                for (i = 0; i < refCars->h ; i++)
                {
                    for (j = 0; j < refCars->w ; j++)
                    {
                        if(readColor(refCars, j, i).r == 192)
                            setPixel(colorCars, j, i, SDL_MapRGB(colorCars->format, playerColor1[count].r, playerColor1[count].g, playerColor1[count].b));
                    }
                }
                SDL_UnlockSurface(colorCars);

                gamePlayer.px = gamePlayer.px + cos((gamePlayer.angleM - 90) * M_PI / 180) * gamePlayer.speed;
                gamePlayer.py = gamePlayer.py + sin((gamePlayer.angleM - 90) * M_PI / 180) * gamePlayer.speed;

		SDL_FreeSurface(rotatedCars);
                rotatedCars = rotozoomSurface(colorCars, cont(gamePlayer.angleR), 1.0, 1);

                rect.x = gamePlayer.px - rotatedCars->w / 2;
                rect.y = gamePlayer.py - rotatedCars->h / 2;

                SDL_BlitSurface(rotatedCars , NULL, screen, &rect);

		SDL_FreeSurface(rotatedCars);
                rotatedCars = rotozoomSurface(colorCars, cont(gamePlayer.angleR), 1.0, 0);
                SDL_BlitSurface(rotatedCars , NULL, colision, &rect);


                if(gamePlayer.count > 0)
                {
                    if(gamePlayer.tour == tour)
                    {
                        rect.x = gamePlayer.px;
                        rect.y = gamePlayer.py - refCars->h;
                        SDL_FreeSurface(txt);
                        txt = TTF_RenderText_Shaded(police, "FINISH", colorWhite, colorBlack);
                        SDL_BlitSurface(txt, NULL, screen, &rect);
                    }
                    else
                    {
                        sprintf(nbrTxt, "%d LAPS", gamePlayer.tour);
                        SDL_FreeSurface(txt);
                        txt = TTF_RenderText_Shaded(police, nbrTxt, colorWhite, colorBlack);
                        rect.x = gamePlayer.px;
                        rect.y = gamePlayer.py - refCars->h;
                        SDL_BlitSurface(txt, NULL, screen, &rect);
                    }
                }
		gamePlayer.count--;

		if(checkpoint)
			{
	                	rect.x = gamePlayer.px;
                        	rect.y = gamePlayer.py - refCars->h;
                       	        SDL_BlitSurface(checkpointxt, NULL, screen, &rect);
				checkpoint++;
				if(checkpoint > 15)		
					checkpoint = 0;
			}
                


            if(start <= 270)
		{
            rect.x = (screen->w / 2) - ((feu->w / 4) /2);
            rect.y = (screen->h / 2) - (feu->h / 2);

            clip.x = 0;
            if(start > 90 && start < 130){SDL_BlitSurface(feu, &clip, screen, &rect);}
            clip.x += feu->w / 4;
            if(start >= 130 && start <= 170){SDL_BlitSurface(feu, &clip, screen, &rect);}
            clip.x += feu->w / 4;
            if(start >= 170 && start <= 210){SDL_BlitSurface(feu, &clip, screen, &rect);}
            clip.x += feu->w / 4;
            if(start >= 210 && start <= 270){SDL_BlitSurface(feu, &clip, screen, &rect);}
		}	

            SDL_Flip(screen);

//technique du quick load !
	if(quick < 2)
		{
		if(quick == 1)
			{	
				police = TTF_OpenFont("data/font2.ttf", 12);
				laps = Mix_LoadWAV( "data/sounds/laps.wav");
				check = Mix_LoadWAV( "data/sounds/check.wav");
				tit = Mix_LoadWAV( "data/sounds/tit.wav");
				skid = Mix_LoadWAV("data/sounds/skid.wav");
			        music = Mix_LoadMUS("data/musics/1.wav");
				checkpointxt = TTF_RenderText_Shaded(police,"Checkpoint", colorWhite, colorBlack);
				quick = 2;
			}
		if(quick == 0)
			quick = 1;
		}
//fin de la technique du quick load
		
            prev = current;
        }
        else
        {
            SDL_Delay((1000/FPS) - (current - prev));
        }
   }
    TTF_CloseFont(police);
    SDL_FreeSurface(txt);
    SDL_FreeSurface(checkpointxt);
    SDL_FreeSurface(trace);
    SDL_FreeSurface(feu);
    SDL_FreeSurface(racePic);
    SDL_FreeSurface(raceMap);
    SDL_FreeSurface(racePicFG);
    SDL_FreeSurface(refCars);
    SDL_FreeSurface(colorCars);
    Mix_FreeMusic(music);
    Mix_FreeChunk(skid);


    return rep;
}
