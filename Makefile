PREFIX?=/usr/X11R6
CFLAGS?=-Os -pedantic -Wall
XFT_CFLAGS?=-I$(PREFIX)/include/X11/Xft
XFT_LIBS?=-L$(PREFIX)/lib -lXft
FREETYPE_CFLAGS?=-I/usr/include/freetype2
FREETYPE_LIBS?=-L/usr/lib -lfreetype

CFLAGS += $(XFT_CFLAGS) $(FREETYPE_CFLAGS)
LIBS = $(XFT_LIBS) $(FREETYPE_LIBS)

all: m1wm

m1wm: m1wm.o 
	$(CC) $(CFLAGS) -o m1wm m1wm.o $(LIBS) -L$(PREFIX)/lib -lX11

m1wm.o: m1wm.c 
	$(CC) $(CFLAGS) -I$(PREFIX)/include -c m1wm.c

clean:
	rm -f m1wm m1wm.o 
