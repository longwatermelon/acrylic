#include "parser.h"
#include <stdexcept>

Parser::Parser(const std::string &prog)
    : m_lexer(prog)
{
    m_curr = m_lexer.next_tok();
}

Parser::~Parser()
{
}

std::unique_ptr<Node> Parser::parse()
{
    std::unique_ptr<Node> comp = std::make_unique<Node>(NodeType::COMPOUND);
    comp->comp_values.emplace_back(parse_expr());

    if (comp->comp_values[0])
    {
        while (true)
        {
            std::unique_ptr<Node> expr = parse_expr();
            if (!expr) break;

            comp->comp_values.emplace_back(std::move(expr));
        }
    }
    else
    {
        comp->comp_values[0] = std::make_unique<Node>(NodeType::NOOP);
    }

    return comp;
}

void Parser::expect(TokenType type)
{
    if (m_curr.type != type)
    {
        throw std::runtime_error(
            "Unexpected token '" + m_curr.value + "' on line " + std::to_string(m_line));
    }
    else
    {
        m_curr = m_lexer.next_tok();
    }
}

std::unique_ptr<Node> Parser::parse_expr()
{
    if (m_curr.type == TokenType::NEWLINE)
    {
        expect(TokenType::NEWLINE);
        ++m_line;
    }

    switch (m_curr.type)
    {
    case TokenType::FN:
        return parse_fn();
    case TokenType::ID:
        return parse_id();
    default:
        return nullptr;
    }
}

std::unique_ptr<Node> Parser::parse_id()
{
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::ID);
    n->id = m_curr.value;
    expect(TokenType::ID);
    return n;
}

std::unique_ptr<Node> Parser::parse_fn()
{
    std::unique_ptr<Node> fn = std::make_unique<Node>(NodeType::FN);
    fn->fn_name = m_curr.value;
    expect(TokenType::FN);

    while (m_curr.type == TokenType::LBRACKET)
    {
        expect(TokenType::LBRACKET);
        fn->fn_args.emplace_back(parse_expr());
        expect(TokenType::RBRACKET);
    }

    return fn;
}

