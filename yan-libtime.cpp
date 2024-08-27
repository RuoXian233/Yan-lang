#include "yan-lang.hpp"


const std::string moduleName = "time";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("Now", {})
     ->AddSymbol("Fetch", {});
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Now(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    // TODO: Implement time aftet we have BigInt
    return result->Success(Number::null);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Fetch(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time_t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return result->Success(new String(oss.str()));
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
