#pragma once
#include "node.h"
#include <SDL2/SDL.h>

struct Drawing
{
    SDL_Texture *tex{ nullptr };
    int w, h;

    void resize(float s)
    {
        w *= s;
        h *= s;
    }
};

namespace draw
{
    void init();
    void quit();

    void draw(const Node *root);
    Drawing draw_expr(const Node *expr);
    Drawing compound(const Node *cpd);
    Drawing fn(const Node *fn);
    Drawing text(const std::string &s);

    namespace functions
    {
        Drawing frac(const Node *fn);

        Drawing exponent(const Node *fn);
        Drawing subscript(const Node *fn);
    }
}

