#include "ast_printer.hpp"
#include <sstream>
#include <iostream>

AstPrinter::AstPrinter() {}

std::string AstPrinter::print(const Program &prog)
{
    AstPrinter p;
    return p.printProgram(prog);
}

std::string AstPrinter::printProgram(const Program &prog)
{
    std::ostringstream out;
    for (const auto &declPtr : prog.decls)
    {
        out << printDecl(declPtr.get(), 0);
        out << "\n";
    }
    return out.str();
}

std::string AstPrinter::printDecl(const Decl *d, int indent)
{
    if (!d)
        return std::string(indent, ' ') + "// <null decl>\n";
    // Try dynamic casts
    if (auto f = dynamic_cast<const FuncDecl *>(d))
        return printFunc(f, indent);
    if (auto b = dynamic_cast<const BlockDecl *>(d))
        return printBlockDecl(b, indent);
    // Unknown decl
    return std::string(indent, ' ') + "// <unknown decl>\n";
}

std::string AstPrinter::printFunc(const FuncDecl *f, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "@func " << f->name << "(";
    bool first = true;
    for (const auto &p : f->params)
    {
        if (!first)
            out << ", ";
        out << p.first << " " << p.second;
        first = false;
    }
    out << ") ";
    // body
    out << "{\n";
    for (auto it = f->body->begin(); it != f->body->end(); ++it)
    {
        out << printStmt(it->get(), indent + 4);
    }
    out << std::string(indent, ' ') << "}\n";
    return out.str();
}

std::string AstPrinter::printBlockDecl(const BlockDecl *b, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "Block " << b->name << " {\n";
    // fields
    for (const auto &f : b->fields)
    {
        out << printVarDecl(f.get(), indent + 4);
    }
    // methods
    for (const auto &m : b->methods)
    {
        out << printFunc(m.get(), indent + 4);
    }
    out << std::string(indent, ' ') << "}\n";
    return out.str();
}

std::string AstPrinter::printVarDecl(const VarDecl *v, int indent)
{
    std::ostringstream out;
    if (!v)
        return std::string(indent, ' ') + "// <null var>\n";
    out << std::string(indent, ' ') << v->typeName << " " << v->varName;
    if (v->initValue.has_value() && v->initValue.value())
    {
        out << " = " << printExpr(v->initValue.value().get());
    }
    out << ";\n";
    return out.str();
}

std::string AstPrinter::printStmt(const Stmt *s, int indent)
{
    if (!s)
        return std::string(indent, ' ') + "// <null stmt>\n";
    if (auto es = dynamic_cast<const ExprStmt *>(s))
        return printExprStmt(es, indent);
    if (auto rs = dynamic_cast<const ReturnStmt *>(s))
        return printReturn(rs, indent);
    if (auto is = dynamic_cast<const IfStmt *>(s))
        return printIf(is, indent);
    if (auto ws = dynamic_cast<const WhileStmt *>(s))
        return printWhile(ws, indent);
    if (auto ls = dynamic_cast<const LoopStmt *>(s))
        return printLoop(ls, indent);
    if (auto it = dynamic_cast<const IterStmt *>(s))
        return printIter(it, indent);
    if (auto vd = dynamic_cast<const VarDecl *>(s))
        return printVarDecl(vd, indent);
    // Unknown
    return std::string(indent, ' ') + "// <unknown stmt>\n";
}

std::string AstPrinter::printExprStmt(const ExprStmt *e, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << printExpr(e->expr.get()) << ";\n";
    return out.str();
}

std::string AstPrinter::printReturn(const ReturnStmt *r, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "return";
    if (r->value.has_value() && r->value.value())
    {
        out << " " << printExpr(r->value.value().get());
    }
    out << ";\n";
    return out.str();
}

std::string AstPrinter::printIf(const IfStmt *ifs, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "if(" << printExpr(ifs->ifBlock.first.get()) << ") {\n";
    for (auto it = ifs->ifBlock.second.begin(); it != ifs->ifBlock.second.end(); ++it)
        out << printStmt(it->get(), indent + 4);
    out << std::string(indent, ' ') << "}\n";

    if (ifs->elseBlock.has_value())
        for (auto it = ifs->elseIfs->begin(); it != ifs->elseIfs->end(); ++it)
        {
            out << std::string(indent, ' ') << "else(" << printExpr(it->first.get()) << ") {\n";
            for (auto sit = it->second.begin(); sit != it->second.end(); ++sit)
                out << printStmt(sit->get(), indent + 4);
            out << std::string(indent, ' ') << "}\n";
        }

    if (ifs->elseBlock.has_value() && ifs->elseBlock->size() > 0)
    {
        out << std::string(indent, ' ') << "else {\n";
        for (auto it = ifs->elseBlock->begin(); it != ifs->elseBlock->end(); it++)
            out << printStmt(it->get(), indent + 4);
        out << std::string(indent, ' ') << "}\n";
    }

    return out.str();
}

std::string AstPrinter::printWhile(const WhileStmt *w, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "while(" << printExpr(w->condition.get()) << ") {\n";
    for (const auto &s : w->body)
        out << printStmt(s.get(), indent + 4);
    out << std::string(indent, ' ') << "}\n";
    return out.str();
}

std::string AstPrinter::printLoop(const LoopStmt *l, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "loop(";
    if (l->init.get())
    {
        // init is usually ExprStmt
        if (auto es = dynamic_cast<const ExprStmt *>(l->init.get()))
        {
            out << printExpr(es->expr.get());
        }
        else
        {
            out << "//init";
        }
    }
    out << ", ";
    if (l->condition)
        out << printExpr(l->condition.get());
    else
        out << "true";
    out << ", ";
    if (l->step.get())
    {
        if (auto es = dynamic_cast<const ExprStmt *>(l->step.get()))
        {
            out << printExpr(es->expr.get());
        }
        else
        {
            out << "//step";
        }
    }
    out << ") {\n";
    for (const auto &s : l->body)
        out << printStmt(s.get(), indent + 4);
    out << std::string(indent, ' ') << "}\n";
    return out.str();
}

std::string AstPrinter::printIter(const IterStmt *it, int indent)
{
    std::ostringstream out;
    out << std::string(indent, ' ') << "iter(" << printExpr(it->iterable.get()) << ", " << it->varName << ") {\n";
    for (const auto &s : it->body)
        out << printStmt(s.get(), indent + 4);
    out << std::string(indent, ' ') << "}\n";
    return out.str();
}

std::string AstPrinter::printExpr(const Expr *e)
{
    if (!e)
        return "<null_expr>";
    if (auto lit = dynamic_cast<const LiteralExpr *>(e))
        return printLiteral(lit);
    if (auto id = dynamic_cast<const IdentifierExpr *>(e))
        return printIdentifier(id);
    if (auto u = dynamic_cast<const UnaryExpr *>(e))
        return printUnary(u);
    if (auto b = dynamic_cast<const BinaryExpr *>(e))
        return printBinary(b);
    if (auto c = dynamic_cast<const CallExpr *>(e))
        return printCall(c);
    // fallback
    return "<expr>";
}

std::string AstPrinter::printLiteral(const LiteralExpr *lit)
{
    return lit->value;
}

std::string AstPrinter::printIdentifier(const IdentifierExpr *id)
{
    return id->name;
}

std::string AstPrinter::printUnary(const UnaryExpr *u)
{
    std::ostringstream out;
    out << u->op;
    if (u->right)
        out << printExpr(u->right.get());
    else
        out << "<null>";
    return out.str();
}

std::string AstPrinter::printBinary(const BinaryExpr *b)
{
    std::ostringstream out;
    // simple parentheses to keep precedence safe
    out << "(" << (b->left ? printExpr(b->left.get()) : "<null>")
        << " " << b->op << " "
        << (b->right ? printExpr(b->right.get()) : "<null>") << ")";
    return out.str();
}

std::string AstPrinter::printCall(const CallExpr *c)
{
    std::ostringstream out;
    out << printExpr(c->callee.get()) << "(";
    bool first = true;
    for (const auto &a : c->args)
    {
        if (!first)
            out << ", ";
        out << printExpr(a.get());
        first = false;
    }
    out << ")";
    return out.str();
}

std::string AstPrinter::indentStr(int indent)
{
    return std::string(indent, ' ');
}
