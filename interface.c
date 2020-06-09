#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
int main(int argc,char * argv[])
{
    if(SDL_Init(SDL_INIT_VIDEO) == 1)
    {
        printf("SDL_Init %s",SDL_GetError());
        return 1;
    }
    SDL_Window *window = SDL_CreateWindow("A Window",SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480,SDL_WINDOW_FULLSCREEN);
    return 0;
}
