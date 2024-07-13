#include "yan-lang.hpp"


int main(int argc, char **argv) {
    globalSymbolTable = new SymbolTable;

    Initialize();
    std::vector<std::string> args;
    for (int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }

    if (argc >= 2) {
        CopyCommandLineArgs(argc, argv, InterpreterStartMode::File, globalSymbolTable);
        InterpreteFile(std::string(argv[1]));
        Finalize();
        return EXIT_SUCCESS;
    }

    startAsShell = true;
    CopyCommandLineArgs(argc, argv, InterpreterStartMode::Repl, globalSymbolTable);

    std::string versionInfo = std::format("Yan {} {} {} on {}", YAN_LANG_VERSION, compilationTimeStamp, compilerInfo, platform);
    std::cout << versionInfo << std::endl;
    std::string code;
    if (debug) {
        std::cout << "NOTICE: Currently in debug mode!" << std::endl;
    }
    while (code != "$exit") {
        std::cout << ">>> ";
        std::getline(std::cin, code);
        if (code == "$exit") {
            break;
        } else if (code == "$cls") {
            system("cls");
            continue;
        } else if (code == "$clear") {
            system("clear");
            continue;
        } else if (code == "$yan") {
            std::cout << versionInfo << std::endl;
            continue;
        } else if (code == "$help") {
            std::cout << "REPL Commands (Start with '$'):\n  "
            "$exit: Terminate the session\n  $module: Prints current imported module and symbols\n"
            "  $external: Prints currently loaded external symbols\n"
            "  $(cls|clear): Clear screen (Win32|POSIX)\n  $yan: Print the version string\n  $globals: Print the global symbol table\n  $help: Print help message" << std::endl;
            continue;
        } else if (code == "$globals") {
            std::cout << globalSymbolTable->ToString() << std::endl;
            continue;
        } else if (code == "$module") {
            std::cout << "Imported modules:" << std::endl;
            for (auto &[k, v] : moduleContextCache) {
                std::cout << "  '" << k << "'" << ": " << v << std::endl;
                // std::cout << "  " << v->symbols->ToString(false) << std::endl;
            }
            std::cout << "External symbols:" << std::endl;
            for (auto &[k, v] : symbolsModuleLocation) {
                std::cout << "  " << k << ": " << v << std::endl;
            }
            continue;
        } else if (code == "$external") {
            std::cout << "Native C modules:" << std::endl;
            for (auto &[k, v] : dylibs) {
                std::cout << "  '" << k << "'" << ": " << v << std::endl;
                // std::cout << "  " << v->symbols->ToString(false) << std::endl;
            }
            std::cout << "External (native) symbols:" << std::endl;
            for (auto &[k, v] : dynamicLoadedSymbol) {
                std::cout << "  " << k << ": " << v.first << " (" << v.second << ")" << std::endl;
            }
            continue;
        } else if (code.starts_with("$")) {
            std::cerr << std::format("Fatal: Invilid command: \"{}\"", code) << std::endl;
            continue;
        } else if (code == "") {
            continue;
        }
        Interprete("<stdin>", code, InterpreterStartMode::Repl);
    }
    Finalize();
}
