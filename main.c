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
#include <dirent.h>
#include <time.h>


int SDL_main()
{
SDL_Surface *screen = NULL, *title = NULL;
SDL_Rect pos0;
int continuer = 1;
SceCtrlData pad;


//les inits
init();
init_joy();

title = IMG_Load("./data/title.png");
screen = SDL_SetVideoMode(SCREENW, SCREENH, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);// surface ecran SDL
pos0.x = pos0.y = 0;

while(continuer)
	{
		sceCtrlPeekBufferPositive(&pad, 1);// init des touches de la psp 		
		if(pad.Buttons & PSP_CTRL_START)
			game_main(screen);
		SDL_BlitSurface(title,NULL,screen,&pos0);
		SDL_Flip(screen);
	}

 SDL_FreeSurface(title);
 quit();

 return 0;
}


int game_main(SDL_Surface *screen)
{
    SDL_Surface *background = NULL,*car = NULL,*msg1 = NULL,*msg2 = NULL,*msg3 = NULL, *msg4 = NULL,*refCars = NULL, *colorCars = NULL ;
    SDL_Rect posMsg1,pos0,posCar;
    color playerColor1[4];
    //SDL_Surface color[4] = {{NULL}};
    char race1[15] = "";
    char race2[15] = "";
    char race3[15] = "";
    //char tmp[100] = "";
   /// int tour1, tour2, tour3;
    int continuer = 1;
    TTF_Font *police = NULL;
    SDL_Color fontColor = {0, 0, 0};
    pos0.x = pos0.y = 0;
    SceCtrlData pad;
    int i = 0,j = 0; 
    int count;
    SDL_Rect StartPos[MAX_PATH];
    char pic[MAX_PATH] = "", map[MAX_PATH]= "";

 
    playerColor1[0].r = 255;
    playerColor1[0].g = 0;
    playerColor1[0].b = 0;
    playerColor1[1].r = 0;
    playerColor1[1].g = 255;
    playerColor1[1].b = 0;
    playerColor1[2].r = 0;
    playerColor1[2].g = 0;
    playerColor1[2].b = 255;
    playerColor1[3].r = 255;
    playerColor1[3].g = 255;
    playerColor1[3].b = 0;

    posMsg1.x = SCREENW/2;
    posMsg1.y = 10;
    posCar.x = SCREENW/2 - 10;
    posCar.y = 35;

    police = TTF_OpenFont("data/font.ttf", 15);
    background = IMG_Load("data/background.png");
    car = IMG_Load("data/bigcar.png");
    SDL_SetColorKey(car, SDL_SRCCOLORKEY, SDL_MapRGB(car->format, 0, 0, 255));
    refCars = SDL_DisplayFormatAlpha(car);
    colorCars = SDL_DisplayFormatAlpha(car);
    msg1 = TTF_RenderText_Blended(police, "Choose your car color", fontColor);
    msg2 = TTF_RenderText_Blended(police, "Color 1 :", fontColor);
    msg3 = TTF_RenderText_Blended(police, "Color 2 :", fontColor);
    msg4 = TTF_RenderText_Blended(police, "Color 3 :", fontColor);

while(continuer)
	{
           /*     SDL_LockSurface(colorCars);  NON INCLU DANS LA DEMO 
             for(count = 0; count < 4; count++)
		{
                for (i = 0; i < refCars->h ; i++)
                {
                    for (j = 0; j < refCars->w ; j++)
                    {
                        if(readColor(refCars, j, i).r == 192)
                        {
                            setPixel(colorCars, j, i, SDL_MapRGB(colorCars->format, playerColor1[count].r, playerColor1[count].g, playerColor1[count].b));
                        }
                    }
                  }
		}
                SDL_UnlockSurface(colorCars);


		sceCtrlPeekBufferPositive(&pad, 1);// init des touches de la psp 
		if(pad.Buttons != 0)
			{
			switch(pad.Buttons)
				{
					case PSP_CTRL_SELECT :
					sceKernelExitGame();
					break;
					case PSP_CTRL_START :*/
					for(count = 1 ; count < 4; count++)
				{
					initGameplay(1,count,pic,map,race1,race2,race3,StartPos);
					switch(gamePlay(screen,COM, playerColor1,pic,map,StartPos[1].x,StartPos[1].y, 3))
						{
							case 0 :
							count = 3;
							break;
							case 1 :
							count--;
							break;
						}
				}/*
					break;
				}
			}

		
		SDL_BlitSurface(background,NULL,screen,&pos0);
		SDL_BlitSurface(msg1,NULL,screen,&posMsg1);
		SDL_BlitSurface(colorCars , NULL, screen, &posCar);
		SDL_Flip(screen);*/
	}



SDL_FreeSurface(car);
SDL_FreeSurface(msg1);
SDL_FreeSurface(msg2);
SDL_FreeSurface(msg3);
SDL_FreeSurface(msg4);
SDL_FreeSurface(refCars);
SDL_FreeSurface(colorCars);
SDL_FreeSurface(background);
TTF_CloseFont(police);
return 0;
}

int initGameplay(int cup,int race,char *pic,char *map,char *race1, char *race2, char *race3, SDL_Rect *StartPos)
{
FILE* file = NULL;
char tmp[MAX_PATH];
sprintf(tmp,"./data/tracks/cup%d.txt",cup);
   file = fopen(tmp,"r");
int tour1,tour2,tour3;
int count = 0;
		 fgets(race1, MAX_PATH, file);
		 fgets(tmp, MAX_PATH, file);	
		 tour1 = atoi(tmp);
		 fgets(race2, MAX_PATH, file);
		 fgets(tmp, MAX_PATH, file);
		 tour2 = atoi(tmp);
		 fgets(race3, MAX_PATH, file);
		 fgets(tmp, MAX_PATH, file);
		 tour3 = atoi(tmp);
	 for(count = 0; count < MAX_PATH; count++)
                {
                    if(race1[count] == '\n')
                        race1[count] = 0;
		    if(race2[count] == '\n')
                        race2[count] = 0;
                    if(race3[count] == '\n')
                        race3[count] = 0;
		}
	fclose(file);

if (race == 1)
	{
	sprintf(tmp,"data/tracks/%s/base.txt",race1);
	sprintf(pic,"data/tracks/%s/MAP.png",race1);
	sprintf(map,"data/tracks/%s/PIC.png",race1);
	}

if (race == 2)
	{
	sprintf(tmp,"data/tracks/%s/base.txt",race2);
	sprintf(pic,"data/tracks/%s/MAP.png",race2);
	sprintf(map,"data/tracks/%s/PIC.png",race2);
	}

if (race == 3)
	{
	sprintf(tmp,"data/tracks/%s/base.txt",race3);
	sprintf(pic,"data/tracks/%s/MAP.png",race3);
	sprintf(map,"data/tracks/%s/PIC.png",race3);
	}

	file = fopen(tmp,"r");
	fgets(tmp, MAX_PATH, file);	
	fgets(tmp, MAX_PATH, file);	
        StartPos[1].x = atoi(tmp); 
	fgets(tmp, MAX_PATH, file);
	StartPos[1].y = atoi(tmp); 
	fclose(file);

return 0;
}
int init_joy()
 {
sceCtrlSetSamplingCycle(0);
sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
return 0;
 }

int quit()
{
 Mix_CloseAudio();
 TTF_Quit();
 SDL_Quit();
return 0;
}


int init()
 {
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
TTF_Init();
srand(time(NULL));//((rand() % (MAX - MIN + 1)) + MIN)
return 0;
 }
