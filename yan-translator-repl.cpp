#include "yan-translator.hpp"

int main(int argc, char *argv[]) {
    std::string input;

    do {
        std::cout << "yan2py >>> ";
        std::getline(std::cin, input);
        if (input.empty()) {
            continue;
        }
        auto lexer = Lexer("<stdin>", ToWideString(input));
        auto tokens = lexer.MakeTokens();
        if (tokens.second) {
            std::cerr << tokens.second->ToString() << std::endl;
            continue;
        }
        auto parser = Parser(tokens.first);
        auto ast = parser.Parse();
        if (ast->err) {
            std::cerr << ast->err->ToString() << std::endl;
            continue;
        }

        Translator translator(ast->ast);
        std::cout << translator.ToPython() << std::endl;
    }
    while (input != "exit");
}
