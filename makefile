all:
	gcc main.c vector.c -o main `sdl2-config --cflags --libs` -lSDL2_image -g
