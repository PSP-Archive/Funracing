
TARGET = pspkb0
OBJS = main.o psp_main.o game.o function.o

INCDIR = 
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ../lib
LDFLAGS = 
LIBS=  -lSDLmain  -lSDL_mixer -lSDL_image   -lSDL  -lSDL_ttf -lSDL_gfx -lfreetype -lvorbisfile -lvorbis  -logg -lpng -ljpeg -lm  -lz \
		-lpspsdk -lpspctrl  -lpsprtc -lpsppower -lpspgu -lpspaudiolib -lpspaudio -lpsphprm -lpspkubridge

PSP_FW_VERSION = 371 

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = FunRacing

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

