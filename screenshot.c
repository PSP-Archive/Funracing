
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

void screenshot(const char* filename)
{
        u32* vram32;
        u16* vram16;
        int bufferwidth;
        int pixelformat;
        int unknown;
        int i, x, y;
        png_structp png_ptr;
        png_infop info_ptr;
        FILE* fp;
        u8* line;
        
        fp = fopen(filename, "wb");
        if (!fp) return;
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png_ptr) return;
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr) {
                png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
                fclose(fp);
                return;
        }
        png_init_io(png_ptr, fp);
        png_set_IHDR(png_ptr, info_ptr, SCREEN_WIDTH, SCREEN_HEIGHT,
                8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png_ptr, info_ptr);
        line = (u8*) malloc(SCREEN_WIDTH * 3);
        sceDisplayWaitVblankStart();  // if framebuf was set with PSP_DISPLAY_SETBUF_NEXTFRAME, wait until it is changed
        sceDisplayGetFrameBuf((void**)&vram32, &bufferwidth, &pixelformat, &unknown);
        vram16 = (u16*) vram32;
        for (y = 0; y < SCREEN_HEIGHT; y++) {
                for (i = 0, x = 0; x < SCREEN_WIDTH; x++) {
                        u32 color = 0;
                        u8 r = 0, g = 0, b = 0;
                        switch (pixelformat) {
                                case PSP_DISPLAY_PIXEL_FORMAT_565:
                                        color = vram16[x + y * bufferwidth];
                                        r = (color & 0x1f) << 3; 
                                        g = ((color >> 5) & 0x3f) << 2 ;
                                        b = ((color >> 11) & 0x1f) << 3 ;
                                        break;
                                case PSP_DISPLAY_PIXEL_FORMAT_5551:
                                        color = vram16[x + y * bufferwidth];
                                        r = (color & 0x1f) << 3; 
                                        g = ((color >> 5) & 0x1f) << 3 ;
                                        b = ((color >> 10) & 0x1f) << 3 ;
                                        break;
                                case PSP_DISPLAY_PIXEL_FORMAT_4444:
                                        color = vram16[x + y * bufferwidth];
                                        r = (color & 0xf) << 4; 
                                        g = ((color >> 4) & 0xf) << 4 ;
                                        b = ((color >> 8) & 0xf) << 4 ;
                                        break;
                                case PSP_DISPLAY_PIXEL_FORMAT_8888:
                                        color = vram32[x + y * bufferwidth];
                                        r = color & 0xff; 
                                        g = (color >> 8) & 0xff;
                                        b = (color >> 16) & 0xff;
                                        break;
                        }
                        line[i++] = r;
                        line[i++] = g;
                        line[i++] = b;
                }
                png_write_row(png_ptr, line);
        }
        free(line);
        png_write_end(png_ptr, info_ptr);
        png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
        fclose(fp);
}

void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg)
{
        // ignore PNG warnings
}

int
nomduscreenshot(char* nomdufichier, int numeroduscreen ,char* extension)
{                     
  char filename[MAX_PATH];
  sprintf(filename, "./data/screenshot/%s.%d%s", nomdufichier, numeroduscreen, extension);
  screenshot(filename);
  return 1 ;
 }
