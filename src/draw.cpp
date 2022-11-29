#include "draw.h"
#include <SDL2/SDL.h>

SDL_Window *g_win{ nullptr };
SDL_Renderer *g_rend{ nullptr };

void draw::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_win = SDL_CreateWindow("Draw",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);
    g_rend = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void draw::quit()
{
    SDL_DestroyRenderer(g_rend);
    SDL_DestroyWindow(g_win);
    SDL_Quit();
}

void draw::draw(const Node *root)
{
}

