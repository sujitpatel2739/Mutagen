#pragma once
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include "../lexer/token.hpp"

// forward
struct Expr;
struct Stmt;
struct Decl;

using ExprPtr = std::unique_ptr<Expr>;
using StmtPtr = std::unique_ptr<Stmt>;
using DeclPtr = std::unique_ptr<Decl>;

//////////////////////////////////////////////////////////////////////////
// Base AST nodes
struct Node
{
    virtual ~Node() = default;
};

//////////////////////////////////////////////////////////////////////////
// Expressions
struct Expr : Node
{
    virtual ~Expr() = default;
};

struct IdentifierExpr : Expr
{
    std::string name;
    IdentifierExpr(std::string n) : name(std::move(n)) {}
};

struct LiteralExpr : Expr
{
    std::string value; // keep lexeme; parser/type-checker will interpret
    TokenType litType;
    LiteralExpr(std::string v, TokenType t) : value(std::move(v)), litType(t) {}
};

struct UnaryExpr : Expr
{
    std::string op;
    ExprPtr right;
    UnaryExpr(std::string op_, ExprPtr r) : op(std::move(op_)), right(std::move(r)) {}
};

struct BinaryExpr : Expr
{
    ExprPtr left;
    std::string op;
    ExprPtr right;
    BinaryExpr(ExprPtr l, std::string op_, ExprPtr r)
        : left(std::move(l)), op(std::move(op_)), right(std::move(r)) {}
};

struct CallExpr : Expr
{
    ExprPtr callee;
    std::vector<ExprPtr> args;
    CallExpr(ExprPtr c, std::vector<ExprPtr> a) : callee(std::move(c)), args(std::move(a)) {}
};

//////////////////////////////////////////////////////////////////////////
// Statements
struct Stmt : Node
{
    virtual ~Stmt() = default;
};

struct Block {
private:
    std::vector<StmtPtr> statements;

public:
    // Basic immutability for internal use; mutations go through these:
    void addStatement(StmtPtr&& s) { statements.push_back(std::move(s)); }

    // Example mutation: remove last N statements safely
    void removeLast(size_t n = 1) {
        if (n == 0) return;
        if (n > statements.size()) n = statements.size();
        statements.erase(statements.end() - n, statements.end());
    }

    // Accessors without exposing the vector directly
    size_t size() const { return statements.size(); }
    bool empty() const { return statements.empty(); }

    // Read-only access for iteration
    auto begin() const { return statements.begin(); }
    auto end() const { return statements.end(); }

    // Optional: a non-modifying accessor for clients that need a view
    const std::vector<StmtPtr>& view() const { return statements; }

    // Optional: operator() to fetch a read-only view (as a const reference)
    const std::vector<StmtPtr>& operator()() const { return statements; }
};

struct ExprStmt : Stmt
{
    ExprPtr expr;
    ExprStmt(ExprPtr e) : expr(std::move(e)) {}
};

struct ReturnStmt : Stmt
{
    std::optional<ExprPtr> value;
    ReturnStmt(std::optional<ExprPtr> v) : value(std::move(v)) {}
};

struct IfStmt : Stmt
{
    std::pair<ExprPtr, Block> ifBlock;
    // else if chain: vector of pairs (cond, block)
    std::optional<std::vector<std::pair<ExprPtr, Block>>> elseIfs;
    std::optional<Block> elseBlock;
};

struct WhileStmt : Stmt
{
    ExprPtr condition;
    Block body;
};

struct LoopStmt : Stmt
{
    // loop(var = init, cond, step) { body }
    std::optional<std::string> dtype;
    StmtPtr init; // usually ExprStmt for var assignment
    ExprPtr condition;
    StmtPtr step; // usually ExprStmt
    Block body;
};

struct IterStmt : Stmt
{
    ExprPtr iterable;
    std::string varName;
    Block body;
};

//////////////////////////////////////////////////////////////////////////
// Declarations
struct Decl : Node
{
    virtual ~Decl() = default;
};

struct HeaderDecl : Decl
{
    std::string name;// (type, name)
};

struct FuncDecl : Decl
{
    std::string name;
    std::vector<std::pair<std::string, std::string>> params; // (type, name)
    std::optional<Block> body;
};

struct VarDecl : Stmt
{
    std::string typeName;
    std::string varName;
    std::optional<ExprPtr> initValue;
};

struct BlockDecl : Decl
{
    std::string name;
    std::vector<std::unique_ptr<VarDecl>> fields;
    std::vector<std::unique_ptr<FuncDecl>> methods;
};

//////////////////////////////////////////////////////////////////////////
// Program root
struct Program
{
    std::vector<DeclPtr> decls;
    std::vector<ExprPtr> globalExprs;
    std::vector<std::unique_ptr<VarDecl>> globalVars;
};

struct TopLevelItem {
    enum Kind { DECL, GLOBAL_VAR, GLOBAL_EXPR, NONE } kind;
    DeclPtr decl;
    StmtPtr globalVar;
    ExprPtr globalExpr;
};
