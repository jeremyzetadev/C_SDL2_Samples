#header=global.h math_util.h render.h

files=main.c render.c global.c

libs=-lm `sdl2-config --cflags --libs` -lSDL2_mixer `pkg-config --libs glfw3` -ldl

build:
	gcc -g3 -O0 $(files) $(libs) -o mainrun

clean:
	rm *.o mainrun
