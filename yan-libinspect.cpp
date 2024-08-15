#include "yan-lang.hpp"

const std::string moduleName = "inspect";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("GetNativeCallStackInfo", {});
    m->AddSymbol("GetCallStackInfo", {});
    m->AddSymbol("GetLocals", {});
    m->AddSymbol("PrintLocals", {});
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject GetNativeCallStackInfo(builtins::YanContext ctx) {
    return (new RuntimeResult)->Success(new String(RuntimeError::GetNativeCallStackInfo()));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetCallStackInfo(builtins::YanContext ctx) {
    std::stringstream ss;
    auto callStack = ctx->parent;
    auto p = ctx->parentEntry;
    int i = 0;
    while (callStack != nullptr) {
        if (p != nullptr) {
            ss << std::format("#{}: {}{}{} [{}{}{}:{}:{}]\n", i, CYAN, callStack->ctxLabel, RESET, YELLOW, p->filename, RESET, p->line + 1, p->column + 1);
        } else {
            ss << std::format("#{} ?? (not avalible) [+{}{}{}]", i, GREEN, (void *) p, RESET) << std::endl;
        }
        i++;
        p = callStack->parentEntry;
        callStack = callStack->parent;
    }
    return (new RuntimeResult)->Success(new String(ss.str()));
}   
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetLocals(builtins::YanContext ctx) {
    auto result = new RuntimeResult;

    if (ctx->parent == nullptr) {
        std::cerr << "Fatal: Invilid context" << std::endl;
        std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;        
        std::cerr << RuntimeError::GetNativeCallStackInfo() << std::endl;
        assert(false);
    }
    auto pc = ctx->parent->symbols;
    std::map<Object *, Object *> localsCache;
    for (const auto &[symName, sym] : pc->symbols) {
        localsCache.insert(std::make_pair(new String(symName), sym));
    }
    return result->Success(new Dictionary(localsCache));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject PrintLocals(builtins::YanContext ctx) {
    auto result = new RuntimeResult;

    if (ctx->parent == nullptr) {
        std::cerr << "Fatal: Invilid context" << std::endl;
        std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;        
        std::cerr << RuntimeError::GetNativeCallStackInfo() << std::endl;
        assert(false);
    }
    auto pc = ctx->parent->symbols;

    std::cout << pc->ToString() << std::endl;
    return result->Success(Number::null);
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
