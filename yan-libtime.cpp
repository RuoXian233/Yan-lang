#include "yan-lang.hpp"
#include <ctime>


const std::string moduleName = "time";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("Secs", {})
     ->AddSymbol("Fetch", {})
     ->AddSymbol("NanoSecs", {})
     ->AddSymbol("MiliSecs", {})
     ->AddSymbol("MicroSecs", {});
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Secs(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    time_t t;
    time(&t);
    return result->Success(new BigInt(BigInteger((long long) t)));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject NanoSecs(builtins::YanContext ctx) {
#ifdef __linux__
    auto result = new RuntimeResult;
    timespec s;
    clock_gettime(CLOCK_REALTIME, &s);
    return result->Success(new BigInt(BigInteger((long long)s.tv_nsec)));
#else

    auto result = new RuntimeResult;
    return result->Success(new String("[Platform not supported]"));
#endif
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject MiliSecs(builtins::YanObject ctx) {
    auto result = new RuntimeResult;
    auto now = std::chrono::system_clock::now();
    auto msecs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return result->Success(new BigInt(BigInteger((long long) msecs.count())));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject MicroSecs(builtins::YanObject ctx) {
    auto result = new RuntimeResult;
    auto now = std::chrono::system_clock::now();
    auto msecs = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch());
    return result->Success(new BigInt(BigInteger((long long) msecs.count())));
}
YAN_C_API_END

// YAN_C_API_START builtins::YanObject FormatTime(builtins::YanObject ctx) {

// }
// YAN_C_API_END


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
