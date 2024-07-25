#include "yan-lang.hpp"


const std::string moduleName = "fs";
static std::map<std::string, std::shared_ptr<std::fstream>> openedFileStream;
BuiltinFunction *FileObject_Read = nullptr;
BuiltinFunction *FileObject_Write = nullptr;
BuiltinFunction *FileObject_Init = nullptr;
ClassObject *FileObject = nullptr;


YAN_C_API_START
builtins::YanObject YanFs_FileObject_Init(builtins::YanContext ctx);
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Read(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Read);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    if (arg->typeName != std::string("String")) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto filename = As<String>(arg)->s;
    if (openedFileStream.find(filename) == openedFileStream.end()) {
        return result->Failure(new RuntimeError(
            std::format("Dangling file reference: '{}'", filename),
            arg->startPos, arg->endPos, ctx
        ));
    }
    auto fileStream = openedFileStream.at(filename);
    std::string content;
    std::stringstream ss;
    ss << fileStream->rdbuf();
    content = ss.str();
    return result->Success(new String(content));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Write(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Write);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    if (arg->typeName != std::string("String")) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto filename = As<String>(arg)->s;
    if (openedFileStream.find(filename) == openedFileStream.end()) {
        return result->Failure(new RuntimeError(
            std::format("Dangling file reference: '{}'", filename),
            arg->startPos, arg->endPos, ctx
        ));
    }
    auto fileStream = openedFileStream.at(filename);

    auto contentArg = ctx->symbols->Get("_str");
    if (contentArg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "Only string could be push into file stream",
            contentArg->startPos, contentArg->endPos, ctx
        ));
    }

    auto content = As<String>(contentArg)->s;
    *fileStream.get() << content;
    return result->Success(Number::null);
}
YAN_C_API_END


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    FileObject_Init = new BuiltinFunction("FileObject_Init");
    FileObject_Init->Bind({ "self", "name" }, YanFs_FileObject_Init);
    FileObject = new ClassObject({
            { new String("name"), nullptr },
            { new String("read"), nullptr },
            { new String("write"), nullptr },
            { new String("stat"), nullptr },
            { new String("__cls__"), new String("FileObject") },
            { new String("__init__"), FileObject_Init }
    });

    FileObject_Read = new BuiltinFunction("FileObject_Read");
    FileObject_Read->Bind({ "self" }, YanFs_FileObject_Read);
    FileObject_Write = new BuiltinFunction("FileObject_Write");
    FileObject_Write->Bind({ "self", "_str" }, YanFs_FileObject_Write);
    
    auto m = new builtins::YanModule(moduleName);
    m
    ->AddSymbol("Open", { "filename", "__mode__" })
    ->AddSymbol("Close", { "_fileObject" });
    return m;
}
YAN_C_API_END


YAN_C_API_START void _FileObject_Init_Internal(Object *self, const std::string &name) {
    self->SetAttr("name", new String(name));
    self->SetAttr("read", BuiltinMethod::FromBuiltinFunction(FileObject_Read, self));
    self->SetAttr("write", BuiltinMethod::FromBuiltinFunction(FileObject_Write, self));
    self->SetAttr("stat", Number::null);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Init(builtins::YanContext ctx) {
    auto self = ctx->symbols->Get("self");
    auto name = As<String>(ctx->symbols->Get("name"))->s;
    _FileObject_Init_Internal(self, name);
    return (new RuntimeResult)->Success(Number::null);
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject YanFs_FileObject_New(const std::string &name, Position *st, Position *et, Context *ctx) {
    auto result = new RuntimeResult;
    auto fileObject = FileObject->Instantiate({ new String(name) });
    fileObject->SetPos(st, et)->SetContext(ctx);
    if (result->ShouldReturn()) {
        return result;
    }
    return result->Success(fileObject);
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Open(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg1 = ctx->symbols->Get("filename");
    auto arg2 = ctx->symbols->Get("__mode__");
    std::ios_base::openmode mode;
    YAN_CONTEXT_DECORATION(Open);
    
    if (arg2 == nullptr) {
        mode = std::ios::out;
    } else {
        auto modeString = As<String>(arg2)->s;
        if (modeString == "r") {
            mode = std::ios::in;
        } else if (modeString == "w") {
            mode = std::ios::out;
        } else if (modeString == "wa") {
            mode = std::ios::out | std::ios::app;
        } else if (modeString == "wr") {
            mode = std::ios::in | std::ios::out;
        } else {
            return result->Failure(new ValueError(
                std::format("Invilid file open mode: '{}'", modeString),
                arg2->startPos, arg2->endPos, ctx
            ));
        }
    }

    if (arg1->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "Filename should be a string",
            arg1->startPos, arg1->endPos, ctx
        ));
    }
    auto filename = As<String>(arg1)->s;
    auto fs = std::make_shared<std::fstream>();
    fs->exceptions(std::fstream::failbit | std::fstream::badbit);

    try {
        fs->open(filename, mode);
    } catch (std::system_error &err) {
        return result->Failure(new OSError(
            std::format("Unable to open file '{}': {}", filename, err.code().message()),
            arg1->startPos, arg1->endPos, ctx
        ));
    }

    openedFileStream.insert(std::make_pair(filename, fs));
    auto fileOperationObject = YanFs_FileObject_New(filename, arg1->startPos, arg1->endPos, ctx)->value;
    return result->Success(fileOperationObject);
}
YAN_C_API_END


YAN_C_API_START builtins::YanObject Close(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(Close);    
    auto self = ctx->symbols->Get("_fileObject");
    auto arg = self->GetAttr("name").first;
    if (arg->typeName != std::string("String")) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            arg->startPos, arg->endPos, ctx
        ));
    }
    
    auto filename = As<String>(arg)->s;
    if (openedFileStream.find(filename) == openedFileStream.end()) {
        return result->Failure(new RuntimeError(
            std::format("Dangling file reference: '{}'", filename),
            arg->startPos, arg->endPos, ctx
        ));
    }

    openedFileStream.at(filename)->close();
    openedFileStream.erase(filename);
    return result->Success(Number::null);
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {
    for (auto &&[filename, fileStream] : openedFileStream) {
        fileStream->close();
    }
}
YAN_C_API_END
