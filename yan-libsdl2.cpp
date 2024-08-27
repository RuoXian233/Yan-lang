#include "yan-lang.hpp"

namespace sdl2 {
    #include <SDL2/SDL.h>
}

const std::string moduleName = "sdl2";


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    return m;
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
