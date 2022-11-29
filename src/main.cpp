#include "parser.h"
#include <fstream>
#include <sstream>

int main(int argc, char **argv)
{
    std::ifstream ifs(argv[1]);
    std::stringstream ss;
    std::string buf;

    while (std::getline(ifs, buf))
        ss << buf << "\n";

    Parser p(ss.str());
    std::unique_ptr<Node> root = p.parse();

    return 0;
}

