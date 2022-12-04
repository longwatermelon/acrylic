#include "parser.h"
#include "draw.h"
#include <fstream>
#include <sstream>
#include <iostream>
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

void run(const std::string &s, bool loop)
{
    std::unique_ptr<Node> root;

    try
    {
        Parser p(s);
        root = p.parse();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error parsing: " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }

    draw::draw(root.get(), loop);
}

void interactive()
{
#ifndef __EMSCRIPTEN__
    while (true)
    {
#endif
#ifdef __EMSCRIPTEN__
        std::string line = emscripten_run_script_string("prompt('Input formula:');");
#else
        std::cout << "Input (:q to quit): ";
        std::string line;
        std::getline(std::cin, line);

        if (line == ":q")
            break;
#endif

        run(line + '\n', false);
#ifndef __EMSCRIPTEN__
    }
#endif
}

int main(int argc, char **argv)
{
    draw::init();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(interactive, -1, 1);
#endif

    if (argc == 1)
        interactive();
    else
    {
        std::ifstream ifs(argv[1]);
        std::stringstream ss;
        std::string buf;

        while (std::getline(ifs, buf))
            ss << buf << "\n";

        run(ss.str(), false);
    }

    draw::quit();

    return 0;
}

