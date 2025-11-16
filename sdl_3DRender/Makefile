#header=global.h math_util.h render.h

files=main.c render.c global.c geometry_util.c math_util.c arraylist.c

libs=-lm `sdl2-config --cflags --libs` -lSDL2_mixer `pkg-config --libs glfw3` -ldl

build:
	gcc -g3 -O0 $(files) $(libs) -o mainrun -g

clean:
	rm *.o mainrun
