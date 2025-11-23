#pragma once
#include "../lexer/token.hpp"
#include "ast.hpp"
#include <vector>
#include <optional>

class Parser
{
public:
    Parser(const std::vector<Token> &tokens);

    // parse whole program; returns nullptr on fatal error
    std::unique_ptr<Program> parse();

    // get errors
    const std::vector<std::string> &errors() const { return m_errors; }

private:
    // token helpers
    const Token &peek() const;
    const Token &previous() const;
    const Token &advance();
    const Token &backward();
    bool match(TokenType t);
    bool check(TokenType t) const;
    bool isAtEnd() const;
    void consume(TokenType t, const std::string &errMsg);
    void logError(const std::string &msg);

    // production rules
    DeclPtr parseTopLevelDecl();
    DeclPtr parseHeader();
    DeclPtr parseBlockDecl();
    DeclPtr parseFuncDecl();
    DeclPtr parseFuncDef();
    std::vector<std::pair<std::string, std::string>> parseArgList();
    std::unique_ptr<VarDecl> parseVarDeclStmt();
    StmtPtr parseStatement();
    StmtPtr parseReturnStmt();
    StmtPtr parseIfStmt();
    StmtPtr parseWhileStmt();
    StmtPtr parseLoopStmt();
    StmtPtr parseIterStmt();
    StmtPtr parseExprStmt();
    Block parseBlock();

    // expressions (precedence climbing)
    ExprPtr parseExpression();
    ExprPtr parseAssignment();
    ExprPtr parseOr();
    ExprPtr parseAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parseUnary();
    ExprPtr parsePrimary();

    // Helper functions
    bool isDtypeToken() const;
    bool isLiteralToken() const;

private:
    const std::vector<Token> &tokens;
    size_t current = 0;
    std::vector<std::string> m_errors;
};
