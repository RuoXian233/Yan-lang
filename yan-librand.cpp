#include "yan-lang.hpp"

const std::string moduleName = "rand";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m
    ->AddSymbol("Random", {})
    ->AddSymbol("RandInt", { "a", "b" });
    
    return m;
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Random(builtins::YanContext ctx) {
    return (new RuntimeResult)->Success(new Number(rand()));
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

    return result->Success(new Number(rand() % builtins::Math::GetInt(b) + builtins::Math::GetInt(a)));
}
YAN_C_API_END
