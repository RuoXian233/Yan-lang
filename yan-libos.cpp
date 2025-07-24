#include "yan-lang.hpp"


const std::string moduleName = "os";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("System", { "_cmd" });
    m->AddSymbol("GetPlatformInfo", { "void" });
    m->AddSymbol("GetInterpreterConfigurations", { "void" });
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

YAN_C_API_START builtins::YanObject GetPlatformInfo(builtins::YanContext ctx) {
    auto result = new RuntimeResult;

    std::map<Object *, Object *> infoRaw {
        { new String("version"), new String(YAN_LANG_VERSION) },
        { new String("platform"), new String(platform) },
        { new String("architecture"), new String(platformInfo) },
        { new String("compilerInfo"), new String(compilerInfo) },
        { new String("compilationTime"), new String(compilationTimeStamp) },
        { new String("compilerDescription"), new String(__VERSION__) }
    };
    auto info = new Dictionary(infoRaw);
    return result->Success(info);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetInterpreterConfigurations(builtins::YanContext ctx) {
    auto result = new RuntimeResult;

    std::map<Object *, Object *> infoRaw {
        { new String("maxCallStackDepth"), new String(std::to_string(MAX_CALLSTACK_DEPTH)) },
        { new String("maxOverflowTolerance"), new String(std::to_string(INVILID_OVERFLOW_TOLERANCE)) }
    };
    auto info = new Dictionary(infoRaw);
    return result->Success(info);
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
