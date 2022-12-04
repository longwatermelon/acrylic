#include "draw.h"
#include <stdexcept>
#include <iostream>
#include <unordered_map>
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
    g_win = SDL_CreateWindow("Acrylic",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        800, 600,
#ifdef __EMSCRIPTEN__
        SDL_WINDOW_SHOWN
#else
        SDL_WINDOW_HIDDEN
#endif
    );
    g_rend = SDL_CreateRenderer(g_win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    g_font = TTF_OpenFont("res/font.ttf", 64);

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

static void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

void draw::draw(const Node *root, bool loop)
{
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderClear(g_rend);

    Drawing d = compound(root);

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

        SDL_Rect r = { 400 - d.w / 2, 300 - d.h / 2, d.w, d.h };
        SDL_RenderCopy(g_rend, d.tex, 0, &r);

        SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
        SDL_RenderPresent(g_rend);

        if (!loop) break;
    }

#ifndef __EMSCRIPTEN__
    std::cout << "Save file as: ";
    std::string out;
    std::getline(std::cin, out);
    save_texture(out.c_str(), g_rend, d.tex);
#endif
    SDL_DestroyTexture(d.tex);
}

Drawing draw::draw_expr(const Node *expr)
{
    switch (expr->type)
    {
    case NodeType::FN: return fn(expr);
    case NodeType::ID: return text(expr->id);
    case NodeType::COMPOUND: return compound(expr);
    case NodeType::NOOP: return text(" ");
    default: throw std::runtime_error("error in draw::draw_expr");
    }
}

Drawing draw::compound(const Node *cpd)
{
    std::vector<Drawing> drawings;
    for (const auto &e : cpd->comp_values)
        drawings.emplace_back(draw_expr(e.get()));

    int w = 0,
        h = 0;

    for (const auto &d : drawings)
    {
        if (d.h > h)
            h = d.h;

        w += d.w + 10;
    }

    w -= 10;

    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    int x = 0;
    for (auto &d : drawings)
    {
        SDL_Rect r = { x, h / 2 - d.h / 2, d.w, d.h };
        SDL_RenderCopy(g_rend, d.tex, 0, &r);
        x += d.w + 10;

        SDL_DestroyTexture(d.tex);
    }

    return { tex, w, h };
}

Drawing draw::fn(const Node *fn)
{
    if (fn->fn_name == "frac") return functions::frac(fn);
    if (fn->fn_name == "sum") return functions::sum(fn);
    if (fn->fn_name == "int") return functions::integral(fn);
    if (fn->fn_name == "oint") return functions::ointegral(fn);
    if (fn->fn_name == "lim") return functions::lim(fn);
    if (fn->fn_name == "vec") return functions::vec(fn);
    if (fn->fn_name == "sqrt") return functions::sqrt(fn);

    if (fn->fn_name == "^") return functions::exponent(fn);
    if (fn->fn_name == "_") return functions::subscript(fn);

    std::unordered_map<std::string, std::wstring> unicode_chars = {
        { "pi", L"π" },
        { "theta", L"θ" },
        { "phi", L"φ" },
        { "inf", L"∞" },
        { "to", L"→" },
        { "delta", L"Δ" },
        { "epsilon", L"ε" },
        { "omega", L"ω" },
        { "lambda", L"λ" },
        { "mu", L"μ" },
        { "plusminus", L"±" },
        { "cross", L"×" },
        { "dot", L"∙" },
        { "le", L"≤" },
        { "ge", L"≥" }
    };

    if (unicode_chars.find(fn->fn_name) != unicode_chars.end())
        return text_unicode(unicode_chars[fn->fn_name]);

    std::cerr << "Function '" << fn->fn_name << "' does not exist.\n";
    exit(EXIT_FAILURE);
}

Drawing draw::text(std::string s)
{
    if (s.empty()) s = " ";
    SDL_Surface *surf = TTF_RenderText_Blended(g_font, s.c_str(), { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g_rend, surf);
    SDL_FreeSurface(surf);

    int w, h;
    SDL_QueryTexture(tex, 0, 0, &w, &h);
    return { tex, w, h };
}

Drawing draw::text_unicode(const std::wstring &s)
{
    if (s.empty()) return { nullptr };
    SDL_Surface *surf = TTF_RenderUNICODE_Blended(g_font, (const Uint16*)s.c_str(), { 255, 255, 255 });
    SDL_Texture *tex = SDL_CreateTextureFromSurface(g_rend, surf);
    SDL_FreeSurface(surf);

    int w, h;
    SDL_QueryTexture(tex, 0, 0, &w, &h);
    return { tex, w, h };
}

Drawing draw::functions::frac(const Node *fn)
{
    Drawing top = draw_expr(fn->fn_args[0].get());
    Drawing bot = draw_expr(fn->fn_args[1].get());
    top.resize(.5f);
    bot.resize(.5f);

    int w = std::max(top.w, bot.w);
    int h = top.h + bot.h + 5;
    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rtop = { (w - top.w) / 2, 0, top.w, top.h };
    SDL_RenderCopy(g_rend, top.tex, 0, &rtop);
    SDL_Rect rbot = { (w - bot.w) / 2, top.h + 5, bot.w, bot.h };
    SDL_RenderCopy(g_rend, bot.tex, 0, &rbot);

    SDL_SetRenderDrawColor(g_rend, 255, 255, 255, 255);
    SDL_Rect rdiv = { 0, top.h + 2, w, 2 };
    SDL_RenderFillRect(g_rend, &rdiv);

    SDL_DestroyTexture(top.tex);
    SDL_DestroyTexture(bot.tex);
    return { tex, w, h };
}

Drawing draw::functions::sum(const Node *fn)
{
    Drawing bot = draw_expr(fn->fn_args[0].get());
    Drawing top = draw_expr(fn->fn_args[1].get());
    bot.resize(.5f);
    top.resize(.5f);

    int maxw = std::max(70, std::max(top.w, bot.w));
    SDL_Rect rsigma = {
        maxw / 2 - 70 / 2,
        top.h, 70, 70 };
    int w = std::max(rsigma.w, std::max(bot.w, top.w));
    int h = rsigma.h + bot.h + top.h;
    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_SetRenderDrawColor(g_rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(g_rend, rsigma.x, rsigma.y, rsigma.x + rsigma.w, rsigma.y);
    SDL_RenderDrawLine(g_rend, rsigma.x, rsigma.y, rsigma.x + (rsigma.w * 3 / 4), rsigma.y + rsigma.h / 2);
    SDL_RenderDrawLine(g_rend, rsigma.x, rsigma.y + rsigma.h - 1, rsigma.x + (rsigma.w * 3 / 4), rsigma.y + rsigma.h / 2);
    SDL_RenderDrawLine(g_rend, rsigma.x, rsigma.y + rsigma.h - 1, rsigma.x + rsigma.w, rsigma.y + rsigma.h - 1);

    SDL_Rect rtop = { maxw / 2 - top.w / 2, 0, top.w, top.h };
    SDL_RenderCopy(g_rend, top.tex, 0, &rtop);
    SDL_Rect rbot = { maxw / 2 - bot.w / 2, top.h + rsigma.h, bot.w, bot.h };
    SDL_RenderCopy(g_rend, bot.tex, 0, &rbot);

    SDL_DestroyTexture(bot.tex);
    SDL_DestroyTexture(top.tex);

    return { tex, w, h };
}

Drawing draw::functions::integral(const Node *fn)
{
    Drawing sign = { IMG_LoadTexture(g_rend, "res/integral.png"), 0, 0 };
    SDL_QueryTexture(sign.tex, 0, 0, &sign.w, &sign.h);
    sign.resize(2.f);
    return sign;
}

Drawing draw::functions::ointegral(const Node *fn)
{
    Drawing sign = { IMG_LoadTexture(g_rend, "res/ointegral.png"), 0, 0 };
    SDL_QueryTexture(sign.tex, 0, 0, &sign.w, &sign.h);
    sign.resize(2.f);
    return sign;
}

Drawing draw::functions::lim(const Node *fn)
{
    Drawing lim = text("lim");
    Drawing bot = draw_expr(fn->fn_args[0].get());
    lim.resize(.6f);
    bot.resize(.4f);

    int w = std::max(lim.w, bot.w);
    int h = lim.h + bot.h;
    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rbot = { lim.w < bot.w ? 0 : lim.w / 2 - bot.w / 2, lim.h - bot.h / 2, bot.w, bot.h };
    SDL_RenderCopy(g_rend, bot.tex, 0, &rbot);
    SDL_Rect rlim = { lim.w < bot.w ? bot.w / 2 - lim.w / 2 : 0, 0, lim.w, lim.h };
    SDL_RenderCopy(g_rend, lim.tex, 0, &rlim);

    SDL_DestroyTexture(lim.tex);
    SDL_DestroyTexture(bot.tex);

    return { tex, w, h };
}

Drawing draw::functions::vec(const Node *fn)
{
    Drawing term = draw_expr(fn->fn_args[0].get());
    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        term.w, term.h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rterm = { 0, 0, term.w, term.h };
    SDL_RenderCopy(g_rend, term.tex, 0, &rterm);

    SDL_SetRenderDrawColor(g_rend, 255, 255, 255, 255);
    int w = term.w;
    SDL_RenderDrawLine(g_rend, 0, 4, w, 4);
    SDL_RenderDrawLine(g_rend, 0, 5, w, 5);
    SDL_RenderDrawLine(g_rend, w, 4, w - 4, 0);
    SDL_RenderDrawLine(g_rend, w, 5, w - 5, 0);
    SDL_RenderDrawLine(g_rend, w, 4, w - 4, 8);
    SDL_RenderDrawLine(g_rend, w, 5, w - 5, 10);

    SDL_DestroyTexture(term.tex);
    return { tex, term.w, term.h };
}

Drawing draw::functions::sqrt(const Node *fn)
{
    Drawing term = draw_expr(fn->fn_args[0].get());
    int w = term.w + 10;
    int h = term.h;
    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rterm = { 10, 0, term.w, term.h };
    SDL_RenderCopy(g_rend, term.tex, 0, &rterm);

    SDL_SetRenderDrawColor(g_rend, 255, 255, 255, 255);
    SDL_RenderDrawLine(g_rend, 0, h - 20, 8, h);
    SDL_RenderDrawLine(g_rend, 0, h - 19, 9, h);
    SDL_RenderDrawLine(g_rend, 8, 0, 8, h);
    SDL_RenderDrawLine(g_rend, 7, 0, 7, h);
    SDL_RenderDrawLine(g_rend, 8, 0, w, 0);
    SDL_RenderDrawLine(g_rend, 8, 1, w, 1);

    SDL_DestroyTexture(term.tex);
    return { tex, w, h };
}

Drawing draw::functions::exponent(const Node *fn)
{
    Drawing base = draw_expr(fn->fn_args[0].get());
    Drawing exp = draw_expr(fn->fn_args[1].get());

    exp.resize(.5f);
    int w = base.w + exp.w;
    int h = base.h;

    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rbase = { 0, 0, base.w, base.h };
    SDL_RenderCopy(g_rend, base.tex, 0, &rbase);
    SDL_Rect rexp = { base.w, 0, exp.w, exp.h };
    SDL_RenderCopy(g_rend, exp.tex, 0, &rexp);

    SDL_DestroyTexture(base.tex);
    SDL_DestroyTexture(exp.tex);

    return { tex, w, h };
}

Drawing draw::functions::subscript(const Node *fn)
{
    Drawing base = draw_expr(fn->fn_args[0].get());
    Drawing sub = draw_expr(fn->fn_args[1].get());

    sub.resize(.5f);
    int w = base.w + sub.w;
    int h = base.h;

    SDL_Texture *tex = SDL_CreateTexture(g_rend,
        SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
        w, h);
    SDL_SetRenderTarget(g_rend, tex);
    SDL_SetRenderDrawColor(g_rend, 0, 0, 0, 255);
    SDL_RenderFillRect(g_rend, 0);

    SDL_Rect rbase = { 0, 0, base.w, base.h };
    SDL_RenderCopy(g_rend, base.tex, 0, &rbase);
    SDL_Rect rsub = { base.w, base.h - sub.h, sub.w, sub.h };
    SDL_RenderCopy(g_rend, sub.tex, 0, &rsub);

    SDL_DestroyTexture(base.tex);
    SDL_DestroyTexture(sub.tex);

    return { tex, w, h };
}

