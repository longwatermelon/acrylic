#include "lexer.h"
#include <vector>
#include <algorithm>

static std::vector<char> g_reserved = { '{', '}', '\n', '\\', '^', '_', ' ' };

Lexer::Lexer(const std::string &prog)
    : m_contents(prog)
{
    m_ch = m_contents[0];
}

Lexer::~Lexer()
{
}

Token Lexer::next_tok()
{
    while (std::isspace(m_ch) && m_ch != '\n')
        advance();

    if (m_idx == m_contents.size() - 1)
        return Token(TokenType::EOF_, "");

    switch (m_ch)
    {
    case '{': advance(); return Token(TokenType::LBRACKET, "{");
    case '}': advance(); return Token(TokenType::RBRACKET, "}");
    case '\\': advance(); return Token(TokenType::FN, collect_id());
    case '^': advance(); return Token(TokenType::INFIX_FN, "^");
    case '_': advance(); return Token(TokenType::INFIX_FN, "_");
    case '\n': advance(); return Token(TokenType::NEWLINE, "\n");
    }

    return Token(TokenType::ID, collect_id());
}

void Lexer::advance()
{
    if (m_idx < m_contents.size())
        m_ch = m_contents[++m_idx];
}

std::string Lexer::collect_id()
{
    size_t begin = m_idx;
    while (std::find(g_reserved.begin(), g_reserved.end(), m_ch) == g_reserved.end())
        advance();

    return m_contents.substr(begin, m_idx - begin);
}

