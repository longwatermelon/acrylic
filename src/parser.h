#pragma once
#include "node.h"
#include "lexer.h"

class Parser
{
public:
    Parser(const std::string &prog);
    ~Parser();

    std::unique_ptr<Node> parse();

private:
    void expect(TokenType type);

    std::unique_ptr<Node> parse_expr();
    std::unique_ptr<Node> parse_id();
    std::unique_ptr<Node> parse_brackets();
    std::unique_ptr<Node> parse_fn();

private:
    Lexer m_lexer;
    Token m_curr;
    size_t m_line{ 1 };
};

