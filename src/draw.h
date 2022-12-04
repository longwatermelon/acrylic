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

    void draw(const Node *root, bool loop);
    Drawing draw_expr(const Node *expr);
    Drawing compound(const Node *cpd);
    Drawing fn(const Node *fn);
    Drawing text(std::string s);
    Drawing text_unicode(const std::wstring &s);

    namespace functions
    {
        Drawing frac(const Node *fn);
        Drawing sum(const Node *fn);
        Drawing integral(const Node *fn);
        Drawing ointegral(const Node *fn);
        Drawing lim(const Node *fn);

        Drawing exponent(const Node *fn);
        Drawing subscript(const Node *fn);
    }
}

