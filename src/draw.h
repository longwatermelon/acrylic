#pragma once
#include "node.h"
#include <SDL2/SDL.h>

struct Drawing
{
    SDL_Texture *tex{ nullptr };
    int w, h;
};

namespace draw
{
    void init();
    void quit();

    void draw(const Node *root);
    Drawing draw_expr(const Node *expr);
    Drawing fn(const Node *fn);
    Drawing text(const std::string &s);
}

