#include "parser.h"
#include <stdexcept>
#include <unordered_map>

static std::unordered_map<std::string, size_t> g_fn_param_nums = {
    { "frac", 2 },
    { "_", 2 },
    { "^", 2 },
    { "sum", 2 },
    { "int", 0 },
    { "pi", 0 },
    { "theta", 0 },
    { "phi", 0 },
    { "delta", 0 },
    { "inf", 0 },
    { "to", 0 },
    { "lambda", 0 },
    { "mu", 0 },
    { "omega", 0 },
    { "lim", 1 }
};

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
            "Unexpected token '" + m_curr.value + "' on line " + std::to_string(m_line) +
            ", expected type " + std::to_string((int)type));
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

    std::unique_ptr<Node> n;

    switch (m_curr.type)
    {
    case TokenType::FN:
        n = parse_fn();
        break;
    case TokenType::ID:
        n = parse_id();
        break;
    case TokenType::LBRACKET:
        n = parse_brackets();
        break;
    default:
        n = nullptr;
        break;
    }

    if (m_curr.type == TokenType::INFIX_FN)
    {
        std::unique_ptr<Node> fn = std::make_unique<Node>(NodeType::FN);
        fn->fn_name = m_curr.value;
        fn->fn_args.emplace_back(std::move(n));
        expect(TokenType::INFIX_FN);
        fn->fn_args.emplace_back(parse_expr());
        return fn;
    }

    return n;
}

std::unique_ptr<Node> Parser::parse_id()
{
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::ID);
    n->id = m_curr.value;
    expect(TokenType::ID);
    return n;
}

std::unique_ptr<Node> Parser::parse_brackets()
{
    expect(TokenType::LBRACKET);
    std::unique_ptr<Node> n = std::make_unique<Node>(NodeType::COMPOUND);

    while (m_curr.type != TokenType::RBRACKET)
        n->comp_values.emplace_back(parse_expr());

    expect(TokenType::RBRACKET);

    if (n->comp_values.empty())
        n->comp_values.emplace_back(std::make_unique<Node>(NodeType::NOOP));

    return n;
}

std::unique_ptr<Node> Parser::parse_fn()
{
    std::unique_ptr<Node> fn = std::make_unique<Node>(NodeType::FN);
    fn->fn_name = m_curr.value;
    expect(TokenType::FN);

    while (fn->fn_args.size() < g_fn_param_nums[fn->fn_name])
        fn->fn_args.emplace_back(parse_expr());

    return fn;
}

