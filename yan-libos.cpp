#include "yan-lang.hpp"


const std::string moduleName = "os";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("System", { "_cmd" });
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject System(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_cmd");

    if (arg->typeName != std::string("String")) {
        return result->Failure(
            new TypeError("Expected a string command", arg->startPos, arg->endPos, arg->ctx)
        );
    }
    int ret = system(As<String>(arg)->s.c_str());
    return result->Success(new Number(ret));
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
