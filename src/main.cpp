#include "parser.h"
#include "draw.h"
#include <fstream>
#include <sstream>
#include <iostream>

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
    while (true)
    {
        std::cout << "Input (:q to quit): ";
        std::string line;
        std::getline(std::cin, line);

        if (line == ":q")
            break;

        run(line + '\n', false);
    }
}

int main(int argc, char **argv)
{
    draw::init();

    if (argc == 1)
        interactive();
    else
    {
        std::ifstream ifs(argv[1]);
        std::stringstream ss;
        std::string buf;

        while (std::getline(ifs, buf))
            ss << buf << "\n";

        run(ss.str(), true);
    }

    draw::quit();

    return 0;
}

