#include "parser.hpp"
#include <sstream>
#include <iostream>

/* Helper macros to simplify token names usage (optional) */
// no macros; explicit usage for clarity

Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

const Token &Parser::peek() const { return tokens[current]; }
const Token &Parser::previous() const { return tokens[current - 1]; }
const Token &Parser::advance()
{
    if (!isAtEnd())
        current++;
    return previous();
}
const Token &Parser::backward()
{
    if (current > 0)
        current--;
    return peek();
}
bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
bool Parser::check(TokenType t) const
{
    if (isAtEnd())
        return false;
    return peek().type == t;
}
bool Parser::match(TokenType t)
{
    if (check(t))
    {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType t, const std::string &errMsg)
{
    if (!check(t))
    {
        std::stringstream ss;
        ss << "Parse error at line " << peek().line << " col " << peek().column << ": " << errMsg;
        std::cout << ss.str() << "\n";
        m_errors.push_back(ss.str());
    }
    advance();
}

void Parser::logError(const std::string &errMsg)
{
    std::stringstream ss;
    ss << "Parse error at line " << peek().line << ", col " << peek().column << ": " << errMsg;
    std::cout << ss.str() << "\n";
    m_errors.push_back(ss.str());
}

std::unique_ptr<Program> Parser::parse()
{
    auto program = std::make_unique<Program>();
    while (!isAtEnd())
    {
        auto d = parseTopLevelDecl();
        if (d)
            program->decls.push_back(std::move(d));
        else
            continue;
    }
    return program;
}

// Top-level decl can be Block or @func or headers (ignored by parser for now)
DeclPtr Parser::parseTopLevelDecl()
{
    // if EOF, return null
    if (isAtEnd())
        return nullptr;

    if (match(TokenType::BLOCK))
    {
        return parseBlockDecl();
    }
    if (match(TokenType::AT))
    {
        if (match(TokenType::FUNC))
        {
            return parseFuncDecl();
        }
        else
        {
            logError("Expected 'func' after '@' for function declaration.");
        }
    }
    if (match(TokenType::FUNC))
    {
        return parseFuncDef();
    }
    if (match(TokenType::HEADER))
    {
        DeclPtr h = parseHeader();
        if (h)
            return h;
    }

    // otherwise attempt to parse as statement-level declaration (e.g., var decl)
    // If type keyword present (like 'number' etc), parse var decl as top-level
    if (isDtypeToken())
    {
        auto v = parseVarDeclStmt();
        // return v ? std::move(v) : nullptr;
        return nullptr;
    }
    // unknown top-level token -> error and skip
    if (!isAtEnd() && peek().type != TokenType::END_OF_FILE)
    {
        logError("Unexpected token at top-level: " + peek().lexeme);
    }
    advance();
    return parseTopLevelDecl(); // try next
}

DeclPtr Parser::parseHeader()
{
    consume(TokenType::LPAREN, "Expected '(' after 'header'");
    std::string headerName;
    if (check(TokenType::STRING_LITERAL))
    {
        headerName = peek().lexeme;
    }
    advance();
    consume(TokenType::RPAREN, "Expected ')' after header declaration");
    consume(TokenType::SEMICOLON, "Expected ';' after header declaration");
    auto hDecl = std::make_unique<HeaderDecl>();
    hDecl->name = headerName;
    return hDecl;
}

DeclPtr Parser::parseBlockDecl()
{
    // previously matched BLOCK
    std::string name;
    if (check(TokenType::IDENTIFIER))
    {
        name = peek().lexeme;
    }
    else
    {
        logError("Expected Block name after 'Block'");
    }
    advance();
    auto block = std::make_unique<BlockDecl>();
    block->name = name;

    // expect LBRACE or SEMICOLON
    if (!check(TokenType::LBRACE) && !check(TokenType::SEMICOLON))
    {
        consume(TokenType::SEMICOLON, "Expected '{' or ';' after Block name");
    }
    // parse fields and methods until RBRACE
    if (match(TokenType::LBRACE))
    {
        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            // if we see a type keyword -> var field
            if (isDtypeToken())
            {
                auto fld = parseVarDeclStmt();
                if (fld)
                    block->fields.push_back(std::unique_ptr<VarDecl>(static_cast<VarDecl *>(fld.release())));
                continue;
            }
            // methods: expect @func
            if (match(TokenType::AT) && check(TokenType::FUNC))
            {
                advance();
                DeclPtr method = parseFuncDecl();
                if (method)
                    block->methods.push_back(std::unique_ptr<FuncDecl>(static_cast<FuncDecl *>(method.release())));
                continue;
            }

            if (match(TokenType::FUNC))
            {
                DeclPtr method = parseFuncDef();
                if (method)
                    block->methods.push_back(std::unique_ptr<FuncDecl>(static_cast<FuncDecl *>(method.release())));
                continue;
            }

            // unknown inside Block
            logError("Unexpected token inside Block declaration: " + peek().lexeme);
            advance();
        }
        consume(TokenType::RBRACE, "Unterminated Block, expected '}'");
    }
    return block;
}

DeclPtr Parser::parseFuncDecl()
{
    // assume current token is function name (IDENTIFIER)
    std::string fname;
    if (check(TokenType::IDENTIFIER))
    {
        fname = peek().lexeme;
    }
    else
    {
        logError("Expected function name after 'func'");
    }
    advance();

    // expect '('
    std::vector<std::pair<std::string, std::string>> params;
    if (match(TokenType::LPAREN))
    {
        // parse params: type name, comma separated
        params = Parser::parseArgList();
    }
    else
    {
        consume(TokenType::LPAREN, "Expected '(' after function name");
    }

    // Function declaration ends with semicolon, NO body
    consume(TokenType::SEMICOLON, "Expected ';' after function declaration.");

    auto func = std::make_unique<FuncDecl>();
    func->name = fname;
    func->params = std::move(params);

    return func;
}

DeclPtr Parser::parseFuncDef()
{
    std::string fname;
    if (check(TokenType::IDENTIFIER))
    {
        fname = peek().lexeme;
    }
    else
    {
        logError("Expected function name after 'func'");
    }
    advance();

    std::vector<std::pair<std::string, std::string>> params;
    // expect '('
    if (match(TokenType::LPAREN))
    {
        // parse params: type name, comma separated
        params = Parser::parseArgList();
    }
    else
    {
        consume(TokenType::LPAREN, "Expected '(' after function name");
    }

    // parse function body (block)
    Block body;
    if (match(TokenType::LBRACE))
    {
        body = parseBlock();
    }
    else
    {
        consume(TokenType::LBRACE, "Expected '{' to start function body");
    }

    auto func = std::make_unique<FuncDecl>();
    func->name = fname;
    func->params = std::move(params);
    func->body = std::move(body);

    return func;
}

std::vector<std::pair<std::string, std::string>> Parser::parseArgList()
{
    // parse params: type name, comma separated
    std::vector<std::pair<std::string, std::string>> params;
    while (!match(TokenType::RPAREN) || match(TokenType::COMMA))
    {
        std::string ptype;
        if (!isDtypeToken())
        {
            logError("Expected parameter type in function *args or **kwargs");
        }
        else
        {
            ptype = peek().lexeme;
        }
        advance();
        std::string pname;
        if (match(TokenType::IDENTIFIER))
        {
            pname = peek().lexeme;
        }
        else
        {
            logError("Expected parameter name after type in function *args or **kwargs");
        }
        params.emplace_back(ptype, pname);
        if (!check(TokenType::RPAREN) && !check(TokenType::COMMA) || isAtEnd())
        {
            logError("Expected ',' or ')' after function parameter");
            break;
        }
    }
    return params;
}

std::unique_ptr<VarDecl> Parser::parseVarDeclStmt()
{
    // first token is a type keyword
    std::string typeName = peek().lexeme;
    std::string varName;
    if (!check(TokenType::IDENTIFIER))
    {
        varName = peek().lexeme;
    }
    else
    {
        logError("Expected variable name after type");
    }
    advance();
    std::optional<ExprPtr> init;
    if (match(TokenType::ASSIGN))
    {
        init = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

    auto v = std::make_unique<VarDecl>();
    v->typeName = typeName;
    v->varName = varName;
    v->initValue = std::move(init);
    return v;
}

StmtPtr Parser::parseStatement()
{
    // dispatch based on current token
    if (match(TokenType::RETURN))
        return parseReturnStmt();
    if (match(TokenType::IF))
        return parseIfStmt();
    if (match(TokenType::WHILE))
        return parseWhileStmt();
    if (match(TokenType::LOOP))
        return parseLoopStmt();
    if (match(TokenType::ITER))
        return parseIterStmt();

    // variable decl as statement (rare)
    if (isDtypeToken())
    {
        auto v = parseVarDeclStmt();
        return v ? std::move(v) : nullptr;
    }
    if (check(TokenType::IDENTIFIER))
    {
        return parseExprStmt();
    }
    logError("Unexpected token in statement: " + peek().lexeme);
    advance();
    return nullptr;
}

StmtPtr Parser::parseReturnStmt()
{
    // we consumed 'return'
    std::optional<ExprPtr> value;
    if (!check(TokenType::SEMICOLON))
    {
        value = parseExpression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' at the end of return statement");
    return std::make_unique<ReturnStmt>(std::move(value));
}

StmtPtr Parser::parseIfStmt()
{
    ExprPtr cond;
    if (match(TokenType::RPAREN))
    {
        cond = parseExpression();
    }
    else
    {
        logError("Expected '(' after if");
    }
    Block thenBlock;
    if (check(TokenType::LBRACE))
    {
        thenBlock = parseBlock();
    }
    else if (!check(TokenType::SEMICOLON))
    {
        consume(TokenType::SEMICOLON, "Expected '{' or ';' after if(condition)");
    }
    auto ifstmt = std::make_unique<IfStmt>();
    ifstmt->ifBlock.first = std::move(cond);
    ifstmt->ifBlock.second = std::move(thenBlock);

    // else-if and else chain
    if (match(TokenType::ELSE) && check(TokenType::LPAREN))
        while (match(TokenType::ELSE))
        {
            if (check(TokenType::LBRACE))
            {
                auto elseBlk = parseBlock();
                ifstmt->elseBlock = std::move(elseBlk);
            }
            else if (check(TokenType::LPAREN))
            {
                ExprPtr econd = parseExpression();
                Block elseBlock;
                if (match(TokenType::LBRACE))
                {
                    elseBlock = parseBlock();
                }
                else if (!check(TokenType::SEMICOLON))
                {
                    consume(TokenType::SEMICOLON, "Expected '{' or ';' after else(condition)");
                    continue;
                }
                if (!ifstmt->elseIfs)
                    ifstmt->elseIfs.emplace();
                ifstmt->elseIfs->push_back(std::make_pair(std::move(econd), std::move(elseBlock)));
            }
            else
            {
                logError("Expected '{' or '(' after else");
            }
        }
    return ifstmt;
}

StmtPtr Parser::parseWhileStmt()
{
    ExprPtr cond;
    Block body;
    if (check(TokenType::LPAREN))
    {
        cond = parseExpression();
    }
    Block elseBlock;
    if (match(TokenType::LBRACE))
    {
        body = parseBlock();
    }
    else if (!check(TokenType::SEMICOLON))
    {
        consume(TokenType::SEMICOLON, "Expected '{' or ';' after while(condition)");
    }
    auto ws = std::make_unique<WhileStmt>();
    ws->condition = std::move(cond);
    ws->body = std::move(body);
    return ws;
}

StmtPtr Parser::parseLoopStmt()
{
    auto loop = std::make_unique<LoopStmt>();
    if (!match(TokenType::LPAREN))
    {
        logError("Expected '(' after loop");
    }
    else
    {
        if (isDtypeToken())
        {
            loop->dtype = std::move(peek().lexeme);
        }
        if (check(TokenType::IDENTIFIER))
        {
            auto e = parseExpression();
            loop->init = std::make_unique<ExprStmt>(std::move(e));
        }
        if (!loop->init)
        {
            logError("Expected loop initialization statement");
        }
        consume(TokenType::COMMA, "Expected ',' after loop init");
        if (check(TokenType::IDENTIFIER) || isLiteralToken())
        {
            loop->condition = std::move(parseExpression());
        }
        consume(TokenType::COMMA, "Expected ',' after loop condition");
        if (check(TokenType::IDENTIFIER) || isLiteralToken())
        {
            auto stepExpr = parseExpression();
            loop->step = std::make_unique<ExprStmt>(std::move(stepExpr));
        }

        consume(TokenType::RPAREN, "Expected ')' after loop parameters");
    }
    if (match(TokenType::LBRACE))
    {
        loop->body = parseBlock();
    }
    else if (!check(TokenType::SEMICOLON))
    {
        consume(TokenType::SEMICOLON, "Expected '{' or ';' after loop(params)");
    }

    return loop;
}

StmtPtr Parser::parseIterStmt()
{
    auto iter = std::make_unique<IterStmt>();
    if (!match(TokenType::LPAREN))
    {
        logError("Expected '(' after iter");
    }
    else
    {
        if (check(TokenType::IDENTIFIER))
        {
            auto e = parseExpression();
            iter->iterable = std::move(e);
        }
        if (!iter->iterable)
        {
            logError("Expected iterable expression in iter statement");
        }
        consume(TokenType::COMMA, "Expected ',' after loop init");
        if (isDtypeToken() && check(TokenType::IDENTIFIER))
        {
            iter->varName = std::move(peek().lexeme);
        }
        consume(TokenType::RPAREN, "Expected ')' after loop parameters");
    }
    if (match(TokenType::LBRACE))
    {
        iter->body = parseBlock();
    }
    else if (!check(TokenType::SEMICOLON))
    {
        consume(TokenType::SEMICOLON, "Expected '{' or ';' after loop(params)");
    }
    return iter;
}

Block Parser::parseBlock()
{
    Block block;
    while (!check(TokenType::RBRACE) && !isAtEnd())
    {
        StmtPtr s = parseStatement();
        if (s)
            block.addStatement(std::move(s));
    }
    consume(TokenType::RBRACE, "Expected '}' to end block");
    return block;
}

StmtPtr Parser::parseExprStmt()
{
    ExprPtr e = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression");
    return std::make_unique<ExprStmt>(std::move(e));
}

//////////////////////////////////////////////////////////////////////////
// Expressions (precedence climbing implemented as chain of functions)
//////////////////////////////////////////////////////////////////////////

ExprPtr Parser::parseExpression()
{
    return parseAssignment();
}

ExprPtr Parser::parseAssignment()
{
    auto left = parseOr();
    // only simple assignment operator '=' is supported
    if (match(TokenType::ASSIGN))
    {
        ExprPtr value = parseAssignment();
        // represent as BinaryExpr with op "="
        return std::make_unique<BinaryExpr>(std::move(left), "=", std::move(value));
    }
    return left;
}

ExprPtr Parser::parseOr()
{
    auto expr = parseAnd();
    while (match(TokenType::OR))
    {
        std::string op = previous().lexeme.empty() ? "||" : previous().lexeme;
        auto right = parseAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseAnd()
{
    auto expr = parseEquality();
    while (match(TokenType::AND))
    {
        std::string op = previous().lexeme.empty() ? "&&" : previous().lexeme;
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseEquality()
{
    auto expr = parseComparison();
    while (match(TokenType::EQ) || match(TokenType::NEQ))
    {
        std::string op = previous().lexeme;
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseComparison()
{
    auto expr = parseTerm();
    while (match(TokenType::LT) || match(TokenType::LTE) || match(TokenType::GT) || match(TokenType::GTE))
    {
        std::string op = previous().lexeme;
        auto right = parseTerm();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseTerm()
{
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS))
    {
        std::string op = previous().lexeme;
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseFactor()
{
    auto expr = parseUnary();
    while (match(TokenType::MUL) || match(TokenType::DIV) || match(TokenType::MOD))
    {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprPtr Parser::parseUnary()
{
    if (match(TokenType::NOT) || match(TokenType::MINUS) || match(TokenType::PLUS))
    {
        std::string op = previous().lexeme;
        auto right = parseUnary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parsePrimary();
}

ExprPtr Parser::parsePrimary()
{
    if (isLiteralToken())
    {
        return std::make_unique<LiteralExpr>(previous().lexeme, previous().type);
    }

    if (match(TokenType::IDENTIFIER))
    {
        // could be a call
        std::string name = previous().lexeme;
        ExprPtr left = std::make_unique<IdentifierExpr>(name);
        if (match(TokenType::LPAREN))
        {
            // parse args
            std::vector<ExprPtr> args;
            if (!check(TokenType::RPAREN))
            {
                do
                {
                    args.push_back(parseExpression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RPAREN, "Expected ')' after call arguments");
            return std::make_unique<CallExpr>(std::move(left), std::move(args));
        }
        return left;
    }

    if (match(TokenType::LPAREN))
    {
        ExprPtr expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    // fallback: error literal of empty
    m_errors.push_back("Unexpected token in expression: " + peek().lexeme);
    // attempt recovery
    advance();
    return std::make_unique<LiteralExpr>("0", TokenType::INT_LITERAL);
}

bool Parser::isDtypeToken() const
{
    TokenType t = peek().type;
    return t == TokenType::NUMBER || t == TokenType::TEXT || t == TokenType::BIN ||
           t == TokenType::HEX || t == TokenType::COMPLEX || t == TokenType::VECTOR ||
           t == TokenType::DATETIME || t == TokenType::BOOL || t == TokenType::BLOCK ||
           t == TokenType::SEQUENCE || t == TokenType::HASH_MAP;
}

bool Parser::isLiteralToken() const
{
    TokenType t = peek().type;
    return t == TokenType::INT_LITERAL || t == TokenType::FLOAT_LITERAL ||
           t == TokenType::STRING_LITERAL || t == TokenType::BIN_LITERAL ||
           t == TokenType::HEX_LITERAL || t == TokenType::BOOL_LITERAL ||
           t == TokenType::COMPLEX_LITERAL || t == TokenType::VECTOR_LITERAL ||
           t == TokenType::DATETIME_LITERAL;
}