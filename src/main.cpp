#include "parser.h"
#include "draw.h"
#include <fstream>
#include <sstream>
#include <iostream>

int main(int argc, char **argv)
{
    if (argc == 1)
    {
        std::cerr << "Error: no file specified\n";
        exit(EXIT_FAILURE);
    }

    std::ifstream ifs(argv[1]);
    std::stringstream ss;
    std::string buf;

    while (std::getline(ifs, buf))
        ss << buf << "\n";

    /* Lexer l(ss.str()); */
    /* Token t; */

    /* while ((t = l.next_tok()).type != TokenType::EOF_) */
    /* { */
    /*     std::cout << t.value << "\n"; */
    /* } */

    std::unique_ptr<Node> root;

    try
    {
        Parser p(ss.str());
        root = p.parse();
    }
    catch (const std::runtime_error &e)
    {
        std::cerr << "Error parsing: " << e.what() << "\n";
        exit(EXIT_FAILURE);
    }

    draw::init();
    draw::draw(root.get());
    draw::quit();

    return 0;
}

