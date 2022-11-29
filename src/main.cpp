#include "lexer.h"
#include <fstream>
#include <sstream>

int main(int argc, char **argv)
{
    std::ifstream ifs(argv[1]);
    std::stringstream ss;
    std::string buf;

    while (std::getline(ifs, buf))
        ss << buf << "\n";

    Lexer l(ss.str());
    Token t;

    while ((t = l.next_tok()).type != TokenType::EOF_)
    {
        printf("%d | %s\n", (int)t.type, t.value.c_str());
    }

    return 0;
}

