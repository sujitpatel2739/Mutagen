#include <iostream>
#include <fstream>
#include <string>

#include "core/lexer/lexer.hpp"
#include "core/parser/parser.hpp"
#include "core/parser/ast_printer.hpp"

int main()
{
    std::string source = R"(
        @funC main() {
            number x = 10;
            number y = 5;

            if(x > y) {
                x = x + 1;
            } else {
                y = y + 1;
            }

            return x;
        }
    )";

    std::cout << "=== SOURCE CODE ===\n"
              << source << "\n";

    // LEXING
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    std::cout << "=== TOKENS ===\n";
    for (auto &t : tokens)
    {
        std::cout << to_string(t.type) << "  '" << t.lexeme << "'  (line "
                  << t.line << ")\n";
    }

    // PARSING
    Parser parser(tokens);
    auto program = parser.parse();

    if (!program)
    {
        std::cout << "Parser returned null program.\n";
        return 1;
    }

    // AST PRINTING / RE-EMIT
    std::string reprinted = AstPrinter::print(*program);

    std::cout << "\n=== RECONSTRUCTED SOURCE ===\n";
    std::cout << reprinted << "\n";

    return 0;
}