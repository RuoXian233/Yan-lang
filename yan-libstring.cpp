#include "yan-lang.hpp"

const std::string moduleName = "string";
YAN_INITIALIZE_CALL_STACK_INFO();

#define YAN_STRING_NO_ERROR             0
#define YAN_STRING_NOT_A_STRING         1
#define YAN_STRING_INDEX_OUT_OF_BOUNDS  2
#define YAN_STRING_INVILID_VALUE        3
#define YAN_STRING_NOT_A_LIST           4


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m
    ->AddSymbol("_Split", { "_src", "_splitter" })
    ->AddSymbol("Format", { "_fmt", "_args" })
    ->AddSymbol("ToCharArray", { "_src" })
    ->AddSymbol("Sub", { "_src", "_st", "_et" })
    ->AddSymbol("Replace", { "_src", "_sub", "_new" })
    ->AddSymbol("Substitute", { "_src", "_st", "_len", "_new" })
    ->AddSymbol("Repeat", { "_src", "_count" })
    ->AddSymbol("Find", { "_src", "_sub" })
    ->AddSymbol("StartsWith", { "_str1", "_str2" })
    ->AddSymbol("EndsWith", { "_str1", "_str2" });
    return m;
}
YAN_C_API_END

static bool CheckArg(Object *arg) {
    if (arg != nullptr) {
        return arg->typeName == std::string("String");
    }
    return false;
}

static bool CheckIndex(Object *arg) {
    if (arg != nullptr) {
        if (arg->typeName != std::string("Number")) {
            return false;
        }
        auto v = As<Number>(arg);
        if (!builtins::Math::HoldsInteger(v)) {
            return false;
        } 
        if (builtins::Math::GetInt(v) < 0) {
            return false;
        }
        return true;
    }
    return false;
}

static std::vector<Object *> YanString_PollArgs(Context *src, std::vector<std::string> &&argNames) {
    std::vector<Object *> result;
    for (std::string &argName : argNames) {
        result.emplace_back(src->symbols->Get(argName));
    }
    return result;
}

static Error *YanString_ThrowExc(int excId, Object *associatedArg, Context *ctx) {
    if (associatedArg == nullptr) {
        std::cerr << "Fatal: Unable to dereference a nil object" << std::endl;
        assert(false);
    }
    switch (excId) {
    case YAN_STRING_NOT_A_STRING:
        return new TypeError(
            "Argument should be a string",
            associatedArg->startPos, associatedArg->endPos, ctx
        );
    case YAN_STRING_INDEX_OUT_OF_BOUNDS:
        return new TypeError(
            "String index out of bounds",
            associatedArg->startPos, associatedArg->endPos, ctx
        );
    case YAN_STRING_NOT_A_LIST:
        return new TypeError(
            "Argument should be a list",
            associatedArg->startPos, associatedArg->endPos, ctx
        );
    case YAN_STRING_INVILID_VALUE:
        return new ValueError(
            "Argument should be a positive integer",
            associatedArg->startPos, associatedArg->endPos, ctx
        );
    default:
        std::cerr << "Fatal: Invilid use of YanString_ThrowExc()" << std::endl;
        assert(false);
    }
}


YAN_C_API_START builtins::YanObject _Split(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(_Split);
    auto arg = ctx->symbols->Get("_src");
    auto arg2 = ctx->symbols->Get("_splitter");
    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    if (!CheckArg(arg2)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg2, ctx));
    }
    auto str = As<String>(arg)->s;
    auto splitter = As<String>(arg2)->s;

    std::vector<Object *> l;
    for (auto &&s : Split(str, splitter)) {
        l.emplace_back(new String(std::move(s)));
    }
    return result->Success(new List(l));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Format(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(Format);
    auto arg = ctx->symbols->Get("_fmt");
    auto args = ctx->symbols->Get("_args");
    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    if (args->typeName != std::string("List")) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_LIST, args, ctx));
    }

    bool startFmt = false;
    size_t argIndex = 0;
    size_t fmtArgs = 0;
    std::wstringstream wss;
    auto fmtArg = As<List>(args)->elements;
    auto fmt = ToWideString(As<String>(arg)->s);
    for (auto c : fmt) {
        if (c == L'%') {
            if (!startFmt) {
                startFmt = true;
                continue;
            }
        }
        
        if (startFmt) {
            if (c == L'%') {
                wss << L'%';
                startFmt = false;
                continue;
            } else {
                if (argIndex >= fmtArg.size()) {
                    return result->Failure(new RuntimeError(
                        "The amound of format args is less than the amount of placeholders in format string",
                        args->startPos, args->endPos, ctx
                    ));
                }
                switch (c) {
                case L'd': {
                    if (fmtArg[argIndex]->typeName != std::string("Number")) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%d' requires an integer (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    auto num = As<Number>(fmtArg[argIndex]);
                    if (!builtins::Math::HoldsInteger(num)) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%d' requires an integer (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    wss << std::format(L"{}", builtins::Math::GetInt(num));
                    break;
                }                    
                case L'f': {
                    if (fmtArg[argIndex]->typeName != std::string("Number")) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%f' requires an float number (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    auto num = As<Number>(fmtArg[argIndex]);
                    if (builtins::Math::HoldsInteger(num)) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%f' requires an float number (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    wss << std::format(L"{}", builtins::Math::GetFloat(num));                                     
                    break;
                }
                case L'l': {
                    if (fmtArg[argIndex]->typeName != std::string("List")) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%l' requires an list (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    wss << std::format(L"{}", ToWideString(fmtArg[argIndex]->ToString()));
                    break;
                }
                case L'm': {
                    if (fmtArg[argIndex]->typeName != std::string("Dictionary") && fmtArg[argIndex]->typeName != std::string("ClassObject")) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%m' requires an mapping [ClassObject or Dictionary] (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    wss << std::format(L"{}", ToWideString(fmtArg[argIndex]->ToString()));                    ;
                    break;
                }
                case L's': {
                    if (fmtArg[argIndex]->typeName != std::string("String")) {
                        return result->Failure(new ValueError(
                            std::format("Format placeholder '%s' requires an string (pos {})", argIndex + 1),
                            args->startPos, args->endPos, ctx
                        ));
                    }
                    wss << std::format(L"{}", ToWideString(fmtArg[argIndex]->ToString()));                    
                    break;
                }
                case L'x':
                    wss << std::format(L"{}", ToWideString(fmtArg[argIndex]->ToString()));                    
                    break;                    
                default:
                    return result->Failure(new ValueError(
                        ToByteString(std::format(L"Invilid format placeholder: '%{}'", c)),
                        arg->startPos, arg->endPos, ctx
                    ));
                    // startFmt = false;
                    // continue;
                }
                fmtArgs++;
                startFmt = false;
            }
            argIndex++;
            continue;
        }
        wss << c;
    }

    if (fmtArgs < fmtArg.size()) {
        return result->Failure(new ValueError(
            "Trailing format argument(s)",
            arg->startPos, arg->endPos, ctx
        ));
    }

    return result->Success(new String(ToByteString(wss.str())));
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject ToCharArray(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(Format);
    auto arg = ctx->symbols->Get("_src");
    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    
    auto wstr = ToWideString(As<String>(arg)->s);
    std::vector<Object *> r;
    for (auto c : wstr) {
        r.emplace_back(new String(ToByteString(std::wstring(1, c))));
    }
    return result->Success(new List(r));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Sub(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(Sub);
    auto arg = ctx->symbols->Get("_src");
    auto startArg = ctx->symbols->Get("_st");
    auto endArg = ctx->symbols->Get("_et");

    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    } else if (!CheckIndex(startArg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_INVILID_VALUE, startArg, ctx));
    } else if (!CheckIndex(endArg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_INVILID_VALUE, endArg, ctx));
    }

    auto src = ToWideString(As<String>(arg)->s);
    auto start = builtins::Math::GetInt(As<Number>(startArg));
    auto end = builtins::Math::GetInt(As<Number>(endArg));

    if (start > src.size() || end > src.size()) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_INDEX_OUT_OF_BOUNDS, startArg, ctx));
    } else if (start > end) {
        return result->Failure(new RuntimeError(
            "Start index should be less than end index",
            startArg->startPos, startArg->endPos, ctx
        ));
    }
    return result->Success(new String(ToByteString(src.substr(start, end - start))));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Replace(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(Replace);
    auto arg = ctx->symbols->Get("_src");
    auto subArg = ctx->symbols->Get("_sub");
    auto newArg = ctx->symbols->Get("_new");
    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    } else if (!CheckArg(subArg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, subArg, ctx));
    } else if (!CheckArg(newArg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, newArg, ctx));
    }

    std::wstring src = ToWideString(As<String>(arg)->s);
    std::wstring orig = ToWideString(As<String>(subArg)->s);
    std::wstring replacement = ToWideString(As<String>(newArg)->s);
    std::wstring dest {};

    std::wstring::size_type pos = 0;
    std::wstring subTmp {};

    while ((pos = src.find(orig)) != std::string::npos) {
        auto ol = orig.length();
        subTmp = src.substr(0, pos + ol);
        subTmp.replace(pos, ol, replacement);
        dest += subTmp;
        if (pos + ol <= src.length()) {
            src = src.substr(pos + ol);
        } else break;
    }
    if (src != L"") {
        dest += src;
    }
    return result->Success(new String(ToByteString(dest)));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Substitute(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto args = YanString_PollArgs(ctx, { "_src", "_st", "_len", "_new" });
    if (!CheckArg(args[0])) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, args[0], ctx));
    }
    if (!CheckIndex(args[1])) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_INVILID_VALUE, args[1], ctx));
    }
    if (!CheckIndex(args[2])) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_INVILID_VALUE, args[2], ctx));
    }
    if (!CheckArg(args[3])) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, args[3], ctx));
    }

    auto src = ToWideString(As<String>(args[0])->s);
    auto st = builtins::Math::GetInt(As<Number>(args[1]));
    auto len = builtins::Math::GetInt(As<Number>(args[2]));
    auto newString = ToWideString(As<String>(args[3])->s);

    std::wstringstream wss;
    if (st >= src.length()) {
        return result->Failure(new RuntimeError("String index out of range", args[1]->startPos, args[1]->endPos, ctx));
    }

    std::size_t index = 0;
    for (const auto c : src) {
        if (index == st) {
            for (const auto sc : newString) {
                wss << sc;
            }
        }
        wss << c;
        index++;
    }
    return result->Success(new String(ToByteString(wss.str())));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Repeat(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_src");
    auto arg2 = ctx->symbols->Get("_count");

    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    if (!CheckIndex(arg2)) {
        return result->Failure(new TypeError(
            "Repeat count should be a positive (or 0) integer",
            arg2->startPos, arg2->endPos, ctx
        ));
    }

    auto src = As<String>(arg);
    auto count = builtins::Math::GetInt(As<Number>(arg2));
    std::stringstream ss;

    for (int i = 0; i < count; i++) {
        ss << src;
    }
    return result->Success(new String(ss.str()));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Find(builtins::YanContext ctx) {
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject StartsWith(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_str1");
    auto arg2 = ctx->symbols->Get("_str2");

    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    if (!CheckArg(arg2)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg2, ctx));
    }

    auto src = As<String>(arg)->s;
    auto match = As<String>(arg2)->s;
    bool val = src.starts_with(match);    
    return result->Success(new Number((int) val));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject EndsWith(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_str1");
    auto arg2 = ctx->symbols->Get("_str2");

    if (!CheckArg(arg)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg, ctx));
    }
    if (!CheckArg(arg2)) {
        return result->Failure(YanString_ThrowExc(YAN_STRING_NOT_A_STRING, arg2, ctx));
    }

    auto src = As<String>(arg)->s;
    auto match = As<String>(arg2)->s;
    bool val = src.ends_with(match);
    return result->Success(new Number((int) val));
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END

