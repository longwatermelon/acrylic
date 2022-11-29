#pragma once
#include "token.h"
#include <functional>

class Lexer
{
public:
    Lexer(const std::string &prog);
    ~Lexer();

    Token next_tok();

private:
    void advance();
    std::string collect_id();

private:
    std::string m_contents;
    char m_ch{ 0 };
    size_t m_idx{ 0 }, m_line{ 1 };
};

