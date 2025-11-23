#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string &src)
    : src(src) {}

char Lexer::peek() const
{
    if (isAtEnd())
        return '\0';
    return src[pos];
}

char Lexer::advance()
{
    char c = peek();
    pos++;
    column++;
    return c;
}

bool Lexer::match(char expected)
{
    if (isAtEnd())
        return false;
    if (src[pos] != expected)
        return false;
    pos++;
    column++;
    return true;
}

bool Lexer::isAtEnd() const
{
    return pos >= src.size();
}

Token Lexer::makeToken(TokenType type, const std::string &lexeme)
{
    return Token{type, lexeme, line, column - static_cast<int>(lexeme.size())};
}

void Lexer::skipWhitespace()
{
    while (!isAtEnd())
    {
        char c = peek();

        if (c == ' ' || c == '\t' || c == '\r')
        {
            advance();
        }
        else if (c == '\n')
        {
            advance();
            line++;
            column = 1;
        }
        else
        {
            return;
        }
    }
}

void Lexer::skipComment()
{
    if (peek() == '/' && pos + 1 < src.size() && src[pos + 1] == '/')
    {
        while (!isAtEnd() && peek() != '\n')
            advance();
    }
}

Token Lexer::identifier()
{
    size_t start = pos;
    while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
        advance();

    std::string text = src.substr(start, pos - start);

    auto it = keywords.find(text);
    if (it != keywords.end())
        return makeToken(it->second, text);

    return makeToken(TokenType::IDENTIFIER, text);
}

Token Lexer::stringLiteral()
{
    size_t start = pos;
    advance();
    while (!isAtEnd() && peek() != '"')
    {
        if (peek() == '\n')
        {
            line++;
            column = 1;
        }
        advance();
    }

    if (isAtEnd())
        return makeToken(TokenType::STRING_LITERAL, "");

    advance();

    std::string value = src.substr(start, pos - start);
    return makeToken(TokenType::STRING_LITERAL, value);
}

Token Lexer::number()
{
    size_t start = pos;

    while (!isAtEnd() && std::isdigit(peek()))
        advance();

    if (!isAtEnd() && peek() == '.')
    {
        advance();
        while (!isAtEnd() && std::isdigit(peek()))
            advance();
    }

    std::string num = src.substr(start, pos - start);
    return makeToken(TokenType::FLOAT_LITERAL, num);
}

Token Lexer::binLiteral()
{
    size_t start = pos - 2;
    while (!isAtEnd() && (peek() == '0' || peek() == '1'))
        advance();
    return makeToken(TokenType::BIN_LITERAL, src.substr(start, pos - start));
}

Token Lexer::hexLiteral()
{
    size_t start = pos - 2;
    while (!isAtEnd() && std::isxdigit(peek()))
        advance();
    return makeToken(TokenType::HEX_LITERAL, src.substr(start, pos - start));
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;

    while (!isAtEnd())
    {
        skipWhitespace();
        skipComment();
        skipWhitespace();
        if (isAtEnd())
            break;

        char c = advance();

        switch (c)
        {
        case '{':
            tokens.push_back(makeToken(TokenType::LBRACE, "{"));
            break;
        case '}':
            tokens.push_back(makeToken(TokenType::RBRACE, "}"));
            break;
        case '(':
            tokens.push_back(makeToken(TokenType::LPAREN, "("));
            break;
        case ')':
            tokens.push_back(makeToken(TokenType::RPAREN, ")"));
            break;
        case '[':
            tokens.push_back(makeToken(TokenType::LBRACKET, "["));
            break;
        case ']':
            tokens.push_back(makeToken(TokenType::RBRACKET, "]"));
            break;
        case ';':
            tokens.push_back(makeToken(TokenType::SEMICOLON, ";"));
            break;
        case ',':
            tokens.push_back(makeToken(TokenType::COMMA, ","));
            break;
        case ':':
            tokens.push_back(makeToken(TokenType::COLON, ":"));
            break;
        case '.':
            tokens.push_back(makeToken(TokenType::DOT, "."));
            break;
        case '#':
            tokens.push_back(makeToken(TokenType::HASH, "#"));
            break;
        case '@':
            tokens.push_back(makeToken(TokenType::AT, "@"));
            break;

        case '+':
            tokens.push_back(makeToken(TokenType::PLUS, "+"));
            break;
        case '-':
            if (match('>'))
                tokens.push_back(makeToken(TokenType::ARROW, "->"));
            else
                tokens.push_back(makeToken(TokenType::MINUS, "-"));
            break;

        case '*':
            tokens.push_back(makeToken(TokenType::MUL, "*"));
            break;
        case '/':
            tokens.push_back(makeToken(TokenType::DIV, "/"));
            break;
        case '%':
            tokens.push_back(makeToken(TokenType::MOD, "%"));
            break;

        case '=':
            if (match('='))
                tokens.push_back(makeToken(TokenType::EQ, "=="));
            else
                tokens.push_back(makeToken(TokenType::ASSIGN, "="));
            break;

        case '!':
            if (match('='))
                tokens.push_back(makeToken(TokenType::NEQ, "!="));
            else
                tokens.push_back(makeToken(TokenType::NOT, "!"));
            break;

        case '<':
            if (match('='))
                tokens.push_back(makeToken(TokenType::LTE, "<="));
            else
                tokens.push_back(makeToken(TokenType::LT, "<"));
            break;

        case '>':
            if (match('='))
                tokens.push_back(makeToken(TokenType::GTE, ">="));
            else
                tokens.push_back(makeToken(TokenType::GT, ">"));
            break;

        case '"':
            tokens.push_back(stringLiteral());
            break;

        case '0':
            if (!isAtEnd() && peek() == 'b')
            {
                advance();
                tokens.push_back(binLiteral());
                break;
            }
            if (!isAtEnd() && peek() == 'x')
            {
                advance();
                tokens.push_back(hexLiteral());
                break;
            }
            tokens.push_back(number());
            break;

        default:
            if (std::isalpha(c) || c == '_' || c == '@')
            {
                pos--;
                column--;
                tokens.push_back(identifier());
            }
            else if (std::isdigit(c))
            {
                pos--;
                column--;
                tokens.push_back(number());
            }
            else
            {
                // Ignore unknowns or handle error
            }
            break;
        }
    }

    tokens.push_back(Token{TokenType::END_OF_FILE, "", line, column});
    return tokens;
}
