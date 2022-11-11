#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <stdlib.h>
#include <stdio.h>#include <SDL/SDL.h>
#include <SDL/SDL_image.h>#include <SDL/SDL_ttf.h> 
#include <SDL/SDL_rotozoom.h>
#include <dirent.h>
#include <time.h>

#define printf pspDebugScreenPrintf
extern int SDL_main();

PSP_MODULE_INFO("FUNRACING", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);

int 
main(int argc, char *argv[])
{
  (void)SDL_main();
  return 0;
}
