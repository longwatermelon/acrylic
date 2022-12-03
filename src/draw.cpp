#include "draw.h"
#include <stdexcept>
#include <iostream>
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
    Drawing d = draw::fn(root->comp_values[0].get());

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

        SDL_Rect r = { 100, 100, d.w, d.h };
        SDL_RenderCopy(g_rend, d.tex, 0, &r);

        SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
        SDL_RenderPresent(g_rend);
    }
}

Drawing draw::draw_expr(const Node *expr)
{
    switch (expr->type)
    {
    case NodeType::FN: return fn(expr);
    case NodeType::ID: return text(expr->id);
    default: throw std::runtime_error("error in draw::draw_expr");
    }
}

Drawing draw::fn(const Node *fn)
{
    if (fn->fn_name == "frac")
    {
        Drawing top = draw_expr(fn->fn_args[0].get());
        Drawing bot = draw_expr(fn->fn_args[1].get());
        int w = std::max(top.w, bot.w);
        int h = top.h + bot.h + 5;
        SDL_Texture *tex = SDL_CreateTexture(g_rend,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            w, h);
        SDL_SetRenderTarget(g_rend, tex);
        SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
        SDL_RenderFillRect(g_rend, 0);

        SDL_Rect rtop = { 0, 0, top.w, top.h };
        SDL_RenderCopy(g_rend, top.tex, 0, &rtop);
        SDL_Rect rbot = { 0, top.h + 5, bot.w, bot.h };
        SDL_RenderCopy(g_rend, bot.tex, 0, &rbot);

        SDL_SetRenderDrawColor(g_rend, 255, 255, 255, 255);
        SDL_Rect rdiv = { 0, top.h + 2, w, 2 };
        SDL_RenderFillRect(g_rend, &rdiv);
        return { tex, w, h };
    }

    std::cerr << "Function '" << fn->fn_name << "' does not exist.\n";
    exit(EXIT_FAILURE);
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

