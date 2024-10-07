#include "yan-lang.hpp"

namespace sdl2 {
    extern "C" {
        #include <SDL2/SDL.h>
    }
}

const std::string moduleName = "sdl2";

Dictionary *EventStruct;

using IntPtr = std::string;

#define WRAP_PTR(p) (std::to_string((long) ((void *) p)))
#define UNWRAP_PTR(T, p) ((T *) ((void *) (atol(p.c_str()))))
#define ASSERT_TYPE_MATCH(o, tpStr) assert((o->typeName == std::string(#tpStr)) && ("Type mismatched: requires" #tpStr))
#define ASSERT_INT(o) assert(o->typeName == std::string("Number") && As<Number>(o)->ntype == NumberType::Int && "Requires integer");


class SDLError : public RuntimeError {
public:
    explicit SDLError(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) :
        RuntimeError("SDLError", details, st, et, ctx) {}
};


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);

    EventStruct = new Dictionary(
        {
            { new String("type"), Number::null }
        }
    );

    m->AddSymbol("Init", {});
    m->AddSymbol("Quit", {});
    m->AddSymbol("CreateWindow", { "title", "w", "h", "__flags__" } );
    m->AddSymbol("DestroyWindow", { "window" });
    m->AddSymbol("CreateRenderer", { "window", "index", "__flags__" });
    m->AddSymbol("DestroyRenderer", { "renderer" });
    m->AddSymbol("PollEvent", {});
    return m;
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Init(builtins::YanContext ctx) {
    int ret = sdl2::SDL_Init(SDL_INIT_EVERYTHING);
    return (new RuntimeResult)->Success(new Number(ret));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject CreateWindow(builtins::YanContext ctx) {
    auto arg1 = ctx->symbols->Get("title");
    auto arg2 = ctx->symbols->Get("w");
    auto arg3 = ctx->symbols->Get("h");
    auto arg4 = ctx->symbols->Get("__flags__");
    ASSERT_TYPE_MATCH(arg1, String);
    ASSERT_INT(arg2);
    ASSERT_INT(arg3);
    if (arg4) {
        ASSERT_INT(arg4);
    }

    auto title = As<String>(arg1)->s;
    auto w = builtins::Math::GetInt(As<Number>(arg2));
    auto h = builtins::Math::GetInt(As<Number>(arg3));
    sdl2::Uint32 flags = 0;
    if (arg4) {
        flags = (sdl2::Uint32) builtins::Math::GetInt(As<Number>(arg4));
    }

    auto window = sdl2::SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w, h, flags
    );
    if (!window) {
        return (new RuntimeResult)->Failure(new SDLError(
            std::format("Unable to create window: {}", sdl2::SDL_GetError()),
            arg1->startPos, arg3->startPos, ctx
        ));
    }

    return (new RuntimeResult)->Success(new String(WRAP_PTR(window)));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject DestroyWindow(builtins::YanContext ctx) {
    auto arg = ctx->symbols->Get("window");
    ASSERT_TYPE_MATCH(arg, String);
    auto window = UNWRAP_PTR(sdl2::SDL_Window, As<String>(arg)->s);
    sdl2::SDL_DestroyWindow(window);
    return (new RuntimeResult)->Success(Number::null);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject PollEvent(builtins::YanContext ctx) {
    sdl2::SDL_Event event;
    std::vector<Object *> eventQueue;
    while (sdl2::SDL_PollEvent(&event)) {
        auto eventStructInst = As<Dictionary>(EventStruct->Copy());
        // assign event value here
        // TODO: maybe overflow (int contains Uint32)
        eventStructInst->SetAttr("type", new Number((int) event.type));
        eventQueue.push_back(eventStructInst);
    }
    return (new RuntimeResult)->Success(new List(eventQueue));
}   
YAN_C_API_END

YAN_C_API_START builtins::YanObject DestroyRenderer(builtins::YanContext ctx) {
    auto arg = ctx->symbols->Get("renderer");
    ASSERT_TYPE_MATCH(arg, String);
    auto renderer = UNWRAP_PTR(sdl2::SDL_Renderer, As<String>(arg)->s);
    sdl2::SDL_DestroyRenderer(renderer);
    return (new RuntimeResult)->Success(Number::null);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject CreateRenderer(builtins::YanContext ctx) {
    auto arg1 = ctx->symbols->Get("window");
    auto arg2 = ctx->symbols->Get("index");
    auto arg3 = ctx->symbols->Get("__flags__");
    ASSERT_TYPE_MATCH(arg1, String);
    ASSERT_INT(arg2);
    if (arg3) {
        ASSERT_INT(arg3);
    }

    auto window = UNWRAP_PTR(sdl2::SDL_Window, As<String>(arg1)->s);
    auto index = builtins::Math::GetInt(As<Number>(arg2));
    sdl2::Uint32 flags = 0;
    if (arg3) {
        flags = (sdl2::Uint32) builtins::Math::GetInt(As<Number>(arg3));
    }

    auto renderer = sdl2::SDL_CreateRenderer(window, index, flags);
    if (!renderer) {
        return (new RuntimeResult)->Failure(new SDLError(
            std::format("Unable to create renderer: {}", sdl2::SDL_GetError()),
            arg1->startPos, arg2->startPos, ctx
        ));
    }

    return (new RuntimeResult)->Success(new String(WRAP_PTR(renderer)));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Quit(builtins::YanContext ctx) {
    sdl2::SDL_Quit();
    return (new RuntimeResult)->Success(Number::null);
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
