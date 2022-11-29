#pragma once
#include <string>

enum class TokenType
{
    ID,
    FN,
    NEWLINE,
    LBRACKET,
    RBRACKET,
    EOF_
};

struct Token
{
    Token() = default;
    Token(TokenType type, const std::string &value)
        : type(type), value(value) {}

    TokenType type{ TokenType::ID };
    std::string value;
};

