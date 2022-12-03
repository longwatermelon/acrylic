#include "draw.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

SDL_Window *g_win{ nullptr };
SDL_Renderer *g_rend{ nullptr };

TTF_Font *g_font{ nullptr };

void draw::init()
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    g_win = SDL_CreateWindow("Draw",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600, SDL_WINDOW_SHOWN);
    g_rend = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    g_font = TTF_OpenFont("res/font.ttf", 16);

    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderClear(g_rend);
}

void draw::quit()
{
    SDL_DestroyRenderer(g_rend);
    SDL_DestroyWindow(g_win);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void draw::draw(const Node *root)
{
    Drawing d = text(root->comp_values[0]->id);

    SDL_SetRenderTarget(g_rend, 0);
    SDL_Event evt;
    bool running = true;

    while (running)
    {
        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                running = false;
            }
        }

        SDL_RenderClear(g_rend);

        SDL_RenderCopy(g_rend, d.tex, 0, 0);

        SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
        SDL_RenderPresent(g_rend);
    }
}

Drawing draw::text(const std::string &s)
{
    if (s.empty()) return { nullptr };
    SDL_Surface *surf = TTF_RenderText_Blended(g_font, s.c_str(), { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g_rend, surf);
    SDL_FreeSurface(surf);

    int w, h;
    SDL_QueryTexture(tex, 0, 0, &w, &h);
    return { tex, w, h };
}

