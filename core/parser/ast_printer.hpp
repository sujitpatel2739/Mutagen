#pragma once
#include "ast.hpp"
#include <string>

class AstPrinter {
public:
    // Print a full program to source text
    static std::string print(const Program& prog);

private:
    AstPrinter();
    std::string printProgram(const Program& prog);
    std::string printDecl(const Decl* d, int indent);
    std::string printFunc(const FuncDecl* f, int indent);
    std::string printBlockDecl(const BlockDecl* b, int indent);
    std::string printVarDecl(const VarDecl* v, int indent);
    std::string printStmt(const Stmt* s, int indent);
    std::string printExprStmt(const ExprStmt* e, int indent);
    std::string printReturn(const ReturnStmt* r, int indent);
    std::string printIf(const IfStmt* ifs, int indent);
    std::string printWhile(const WhileStmt* w, int indent);
    std::string printLoop(const LoopStmt* l, int indent);
    std::string printIter(const IterStmt* it, int indent);
    std::string printExpr(const Expr* e);
    std::string printLiteral(const LiteralExpr* lit);
    std::string printIdentifier(const IdentifierExpr* id);
    std::string printUnary(const UnaryExpr* u);
    std::string printBinary(const BinaryExpr* b);
    std::string printCall(const CallExpr* c);
    std::string indentStr(int n);
};
