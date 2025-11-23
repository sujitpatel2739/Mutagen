#pragma once
#include <string>

enum class TokenType {
    // Keywords
    MAIN, FUNC, BLOCK, IF, ELSE, ITER, LOOP, WHILE, HEADER, AS,
    NUMBER, TEXT, BIN, HEX, COMPLEX, VECTOR, SEQUENCE, HASH_MAP, DATETIME, BOOL, RETURN,

    // Symbols
    LBRACE, RBRACE, LPAREN, RPAREN, LBRACKET, RBRACKET, LSQUARE, RSQUARE,
    SEMICOLON, COMMA, COLON, DOT, HASH, ARROW, AT,

    // Operators
    PLUS, MINUS, MUL, DIV, MOD,
    AND, OR, NOT,
    EQ, NEQ, LT, GT, LTE, GTE,
    ASSIGN,

    // Literals
    INT_LITERAL, FLOAT_LITERAL, STRING_LITERAL,
    BIN_LITERAL, HEX_LITERAL, BOOL_LITERAL,
    COMPLEX_LITERAL, VECTOR_LITERAL, DATETIME_LITERAL,
    SEQUENCE_INST, HASH_MAP_INST, BLOCK_INST,

    IDENTIFIER,
    END_OF_FILE
};

inline const char* to_string(TokenType t) {
    switch (t) {
        // Keywords
        case TokenType::MAIN: return "MAIN";
        case TokenType::FUNC: return "FUNC";
        case TokenType::BLOCK: return "BLOCK";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::ITER: return "ITER";
        case TokenType::LOOP: return "LOOP";
        case TokenType::WHILE: return "WHILE";
        case TokenType::HEADER: return "HEADER";
        case TokenType::AS: return "AS";
        case TokenType::NUMBER: return "NUMBER";
        case TokenType::TEXT: return "TEXT";
        case TokenType::BIN: return "BIN";
        case TokenType::HEX: return "HEX";
        case TokenType::COMPLEX: return "COMPLEX";
        case TokenType::VECTOR: return "VECTOR";
        case TokenType::SEQUENCE: return "SEQUENCE";
        case TokenType::HASH_MAP: return "HASH_MAP";
        case TokenType::DATETIME: return "DATETIME";
        case TokenType::BOOL: return "BOOL";
        case TokenType::RETURN: return "RETURN";

        // Symbols
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";
        case TokenType::LSQUARE: return "LSQUARE";
        case TokenType::RSQUARE: return "RSQUARE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::DOT: return "DOT";
        case TokenType::HASH: return "HASH";
        case TokenType::ARROW: return "ARROW";
        case TokenType::AT: return "AT";

        // Operators
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MUL: return "MUL";
        case TokenType::DIV: return "DIV";
        case TokenType::MOD: return "MOD";

        case TokenType::AND: return "AND";
        case TokenType::OR:  return "OR";
        case TokenType::NOT: return "NOT";
        case TokenType::EQ:  return "EQ";
        case TokenType::NEQ: return "NEQ";
        case TokenType::LT:  return "LT";
        case TokenType::GT:  return "GT";
        case TokenType::LTE: return "LTE";
        case TokenType::GTE: return "GTE";
        case TokenType::ASSIGN: return "ASSIGN";

        // Literals
        case TokenType::INT_LITERAL: return "INT_LITERAL";
        case TokenType::FLOAT_LITERAL: return "FLOAT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::BIN_LITERAL: return "BIN_LITERAL";
        case TokenType::HEX_LITERAL: return "HEX_LITERAL";
        case TokenType::BOOL_LITERAL: return "BOOL_LITERAL";
        case TokenType::COMPLEX_LITERAL: return "COMPLEX_LITERAL";
        case TokenType::VECTOR_LITERAL: return "VECTOR_LITERAL";
        case TokenType::DATETIME_LITERAL: return "DATETIME_LITERAL";
        case TokenType::SEQUENCE_INST: return "SEQUENCE_INST";
        case TokenType::HASH_MAP_INST: return "HASH_MAP_INST";
        case TokenType::BLOCK_INST: return "BLOCK_INST";

        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        default: return "UNKNOWN";
    }
}

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
};
