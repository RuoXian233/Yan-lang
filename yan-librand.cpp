#include "yan-lang.hpp"
#include <random>

const std::string moduleName = "rand"; 
YAN_INITIALIZE_CALL_STACK_INFO();


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m
    ->AddSymbol("Random", {})
    ->AddSymbol("RandInt", { "a", "b" });
    
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Random(builtins::YanContext ctx) {
    auto randNum = rand();
    if (randNum == 0) {
        return (new RuntimeResult)->Success(new Number(0.0));
    } else if (randNum == 1) {
        return (new RuntimeResult)->Success(new Number(1.0));
    }
    return (new RuntimeResult)->Success(new Number(1.0 / randNum));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject RandInt(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(RandInt);
    auto arg1 = ctx->symbols->Get("a");
    auto arg2 = ctx->symbols->Get("b");
    if (arg1->typeName != std::string("Number") || arg2->typeName != std::string("Number")) {
        return result->Failure(new TypeError(
            "Interval endpoints should be numbers",
            arg1->startPos, arg2->endPos, ctx
        ));
    }

    auto a = As<Number>(arg1);
    auto b = As<Number>(arg2);
    if (!builtins::Math::HoldsInteger(a) || !builtins::Math::HoldsInteger(b)) {
        return result->Failure(new TypeError(
            "Interval endpoints should be integers",
            a->startPos, b->endPos, ctx
        ));
    }

    std::random_device rd;
    std::mt19937 generator(rd());
    auto va = builtins::Math::GetInt(a);
    auto vb = builtins::Math::GetInt(b);
    std::uniform_int_distribution<> dist(std::min(va, vb), std::max(va, vb));
    return result->Success(new Number(dist(generator)));
}
YAN_C_API_END

YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
