#include "yan-translator.hpp"


int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Fatal: invilid argument" << std::endl;
        assert(false);
    } 
    std::string file = argv[1];
    std::ifstream ifs(file);
    if (!ifs.is_open()) {
        std::cerr << "Fatal: cannot open file " << file << std::endl;
        assert(false);
    }

    std::string code((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    std::vector<std::string> lines;

    for (auto &&line : Split(code, "\n")) {
        auto lineContent = line;
        // preprocessor -> python code embedding
        std::string strippedLine {};
        for (auto &&c : lineContent) {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                 continue;
            } else {
                strippedLine += c;
            }
        }
        if (strippedLine.starts_with("[[") && line.ends_with("]]")) {
            lineContent = "__yan_pyeval('" + strippedLine.substr(2, strippedLine.size() - 4) + "')";
        }
        lines.push_back(lineContent);
    }
    code = Join(lines, "\n");

    auto lexer = Lexer(file, ToWideString(code));
    auto tokens = lexer.MakeTokens();
    if (tokens.second) {
        std::cerr << tokens.second->ToString() << std::endl;
    }

    auto parser = Parser(tokens.first);
    auto ast = parser.Parse();
    if (ast->err) {
        std::cerr << ast->err->ToString() << std::endl;
    }

    auto codeGenreator = Translator(ast->ast);
    std::cout << "from builtins_py.common import *\n\n" << std::endl;
    std::cout << codeGenreator.ToPython() << std::endl;

    ifs.close();
}
