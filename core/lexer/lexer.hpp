#pragma once
#include "token.hpp"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    explicit Lexer(const std::string& src);

    std::vector<Token> tokenize();

private:
    char peek() const;
    char advance();
    bool match(char expected);
    bool isAtEnd() const;

    Token makeToken(TokenType type, const std::string& lexeme);

    void skipWhitespace();
    void skipComment();

    Token identifier();
    Token number();
    Token stringLiteral();
    Token binLiteral();
    Token hexLiteral();

private:
    std::string src;
    size_t pos = 0;
    int line = 1;
    int column = 1;
};

static std::unordered_map<std::string, TokenType> keywords = {
        {"main", TokenType::MAIN},
        {"Block", TokenType::BLOCK},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"iter", TokenType::ITER},
        {"loop", TokenType::LOOP},
        {"while", TokenType::WHILE},
        {"@", TokenType::AT},
        {"func", TokenType::FUNC},
        {"header", TokenType::HEADER},
        {"as", TokenType::AS},
        {"number", TokenType::NUMBER},
        {"text", TokenType::TEXT},
        {"bin", TokenType::BIN},
        {"hex", TokenType::HEX},
        {"complex", TokenType::COMPLEX},
        {"vector", TokenType::VECTOR},
        {"sequence", TokenType::SEQUENCE},
        {"hash_map", TokenType::HASH_MAP},
        {"datetime", TokenType::DATETIME},
        {"bool", TokenType::BOOL},
        {"return", TokenType::RETURN}
    };