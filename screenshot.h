
#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#define MAX_PATH2 256

#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272


void screenshot(const char* filename);
int nomduscreenshot(char* nomdufichier, int numeroduscreen ,char* extension);
void user_warning_fn(png_structp png_ptr, png_const_charp warning_msg);#endif
