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
    Drawing text(const std::string &s);
}

