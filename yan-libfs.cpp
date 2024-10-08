#include "yan-lang.hpp"
#include <filesystem>


const std::string moduleName = "fs";
static std::map<std::string, std::shared_ptr<std::fstream>> openedFileStream;
BuiltinFunction *FileObject_Read = nullptr;
BuiltinFunction *FileObject_Write = nullptr;
BuiltinFunction *FileObject_Init = nullptr;
BuiltinFunction *FileObject_ReadBuf = nullptr;
BuiltinFunction *FileObject_Reverse = nullptr;
BuiltinFunction *FileObject_ReadLines = nullptr;
BuiltinFunction *FileObject_Length = nullptr;
BuiltinFunction *FileObject_Eof = nullptr;
ClassObject *FileObject = nullptr;


YAN_C_API_START
builtins::YanObject YanFs_FileObject_Init(builtins::YanContext ctx);
YAN_C_API_END

#define YANFS_NO_ERROR           0
#define YANFS_BROKEN_FILE_OBJECT 1
#define YANFS_DANGLING_FILE_REF  2
#define YANFS_FATAL_ERROR        3

const int YANFS_FILETYPE_ERR   = -1;
const int YANFS_FILETYPE_FILE  = 0;
const int YANFS_FILETYPE_DIR   = 1;
const int YANFS_FILETYPE_BLOCK = 2;
const int YANFS_FILETYPE_CHAR  = 3;
const int YANFS_FILETYPE_FIFO  = 4;
const int YANFS_FILETYPE_SOCK  = 5;
const int YANFS_FILETYPE_SYM   = 6;

const int YANFS_INT32_MAX = std::numeric_limits<int>::max();


static int YanFs_CheckFileObject_Internal(Object *arg) {
    if (arg->typeName != std::string("String")) {
        return YANFS_BROKEN_FILE_OBJECT;
    }

    auto filename = As<String>(arg)->s;
    if (openedFileStream.find(filename) == openedFileStream.end()) {
        return YANFS_BROKEN_FILE_OBJECT;
    }
    return YANFS_NO_ERROR;
}

static int YanFs_GetFileLength(std::fstream &fs) {
    fs.seekp(0, std::ios::beg);
    int st = fs.tellp();
    fs.seekp(0, std::ios::end);
    int et = fs.tellp();
    fs.seekp(0, std::ios::beg);
    return (int) et - st;
}

static Error *YanFs_ThrowExc(int excId, Context *ctx, Object *arg) {
    switch (excId) {
    case YANFS_BROKEN_FILE_OBJECT:
        return new RuntimeError("Broken file object", arg->startPos, arg->endPos, ctx);
    case YANFS_DANGLING_FILE_REF: {
        auto fname = As<String>(arg)->s;
        return new RuntimeError(std::format("Dangling file reference: '{}'", fname), arg->startPos, arg->endPos, ctx);
    }
    default:
        std::cerr << "Fatal: [yan::fs] Invilid exception id: " << excId << std::endl;
        assert(false);
    }
}

YAN_C_API_START builtins::YanObject YanFs_FileObject_Eof(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Read);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);

    bool isEof = fileStream->eof();
    return result->Success(new Number((int) isEof));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Length(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Length);
    
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);
    return result->Success(new Number(YanFs_GetFileLength(*fileStream.get())));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Read(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Read);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);
    std::string content;
    std::stringstream ss;
    ss << fileStream->rdbuf();
    content = ss.str();
    return result->Success(new String(content));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_ReadBuf(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.ReadBuf);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    auto _currentPos = self->GetAttr("pos");
    
    if (_currentPos.first == nullptr) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            self->startPos, self->endPos, ctx
        ), _currentPos.second);
    }
    if (_currentPos.first->typeName != std::string("Number")) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            self->startPos, self->endPos, ctx
        ));
    }
    int currentPos = builtins::Math::GetInt(As<Number>(_currentPos.first));
    auto size = ctx->symbols->Get("__size__");

    size_t s = 1;
    if (size != nullptr) {
        if (size->typeName != std::string("Number")) {
            return result->Failure(new TypeError(
                "Buffer size should be a positive integer",
                size->startPos, size->endPos, ctx
            ));
        }
        auto sn = As<Number>(size);
        if (!builtins::Math::HoldsInteger(sn)) {
            return result->Failure(new TypeError(
                "Buffer size should be a positive integer",
                size->startPos, size->endPos, ctx
            ));
        }
        s = builtins::Math::GetInt(sn);
        if (s < 1) {
            return result->Failure(new ValueError(
                "Buffer size should be a positive integer",
                size->startPos, size->endPos, ctx
            ));
        }
    }
    
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);
    auto fileLength = YanFs_GetFileLength(*fileStream.get());

    if (currentPos + s > fileLength) {
        return result->Failure(new OSError(
            std::format("EOF: {}", filename),
            size->startPos, size->endPos, ctx
        ));
    }
    
    fileStream->seekg(currentPos);
    char *buf = new char[s + 1];
    fileStream->read(buf, s);
    fileStream->seekg(currentPos + s);
    buf[s] = '\0';
    auto str = std::string(buf);
    delete buf;

    self->SetAttr("pos", new Number((int) (currentPos + s)));
    return result->Success(new String(str));
}

YAN_C_API_END

static inline bool YanFs_IsPositiveInteger(Object *o, bool zeroExcluded = true) {
    if (o != nullptr) {
        if (o->typeName == std::string("Number")) {
            auto n = As<Number>(o);
            if (builtins::Math::HoldsInteger(n)) {
                auto v = builtins::Math::GetInt(n);
                if (!(v < 0 || (v == 0 && zeroExcluded))) {
                    return true;
                }
            }
        }
    }
    return false;
}

YAN_C_API_START builtins::YanObject YanFs_FileObject_Reverse(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto self = ctx->symbols->Get("self");

    auto arg = self->GetAttr("name").first;
    auto _currentPos = self->GetAttr("pos");

    if (_currentPos.first == nullptr) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            self->startPos, self->endPos, ctx
        ), _currentPos.second);
    }
    if (_currentPos.first->typeName != std::string("Number")) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            self->startPos, self->endPos, ctx
        ));
    }
    int currentPos = builtins::Math::GetInt(As<Number>(_currentPos.first));

    auto _reverseCount = ctx->symbols->Get("_num");
    if (!YanFs_IsPositiveInteger(_reverseCount)) {
        return result->Failure(new ValueError(
            "Reverse count should be a positive integer",
            _reverseCount->startPos, _reverseCount->endPos, ctx
        ));
    }

    auto reverseCount = builtins::Math::GetInt(As<Number>(_reverseCount));
    if (currentPos - reverseCount < 0) {
        return result->Failure(new OSError(
            "Already reached start of file",
            self->startPos, self->endPos, ctx
        ));
    }

    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);
    fileStream->seekg(currentPos - reverseCount);
    fileStream->seekp(currentPos - reverseCount);
    self->SetAttr("pos", new Number((int) (currentPos - reverseCount)));
    return result->Success(Number::null);
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_ReadLines(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Read);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
    
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
    auto fileStream = openedFileStream.at(filename);
    std::string content;
    std::stringstream ss;
    ss << fileStream->rdbuf();
    content = ss.str();
    
    std::vector<Object *> r;
    auto lines = Split(content, "\n");
    for (const auto &line : lines) {
        r.emplace_back(new String(line));
    }
    return result->Success(new List(r));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject YanFs_FileObject_Write(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    YAN_CONTEXT_DECORATION(FileObject.Write);
    auto self = ctx->symbols->Get("self");
    auto arg = self->GetAttr("name").first;
   
    int status = YanFs_CheckFileObject_Internal(arg);
    if (status) {
        return result->Failure(YanFs_ThrowExc(status, ctx, arg));
    }

    auto filename = As<String>(arg)->s;
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

YAN_C_API_START builtins::YanModule *YanModule_OnLoad() {
    FileObject_Init = new BuiltinFunction("FileObject_Init");
    FileObject_Init->Bind({ "self", "name" }, YanFs_FileObject_Init);
    FileObject = new ClassObject({
            { new String("name"), nullptr },
            { new String("pos"), nullptr },
            { new String("eof"), nullptr },
            { new String("read"), nullptr },
            { new String("length"), nullptr },
            { new String("readBuf"), nullptr },
            { new String("reverse"), nullptr },
            { new String("readLines"), nullptr },
            { new String("write"), nullptr },
            { new String("__cls__"), new String("FileObject") },
            { new String("__init__"), FileObject_Init }
    });

    FileObject_Read = new BuiltinFunction("FileObject_Read");
    FileObject_Read->Bind({ "self" }, YanFs_FileObject_Read);
    FileObject_Write = new BuiltinFunction("FileObject_Write");
    FileObject_Write->Bind({ "self", "_str" }, YanFs_FileObject_Write);
    FileObject_ReadBuf = new BuiltinFunction("FileObject_ReadBuf");
    FileObject_ReadBuf->Bind({ "self", "__size__" }, YanFs_FileObject_ReadBuf);
    FileObject_Reverse = new BuiltinFunction("FileObject_Reverse");
    FileObject_Reverse->Bind({ "self", "_num" }, YanFs_FileObject_Reverse);
    FileObject_Eof = new BuiltinFunction("FileObject_Eof");
    FileObject_Eof->Bind({ "self" }, YanFs_FileObject_Eof);
    FileObject_Length = new BuiltinFunction("FileObject_Length");
    FileObject_Length->Bind({ "self" }, YanFs_FileObject_Length);
    FileObject_ReadLines = new BuiltinFunction("FileObject_ReadLines");
    FileObject_ReadLines->Bind({ "self" }, YanFs_FileObject_ReadLines);

    auto m = new builtins::YanModule(moduleName);
    m
    ->AddSymbol("Open", { "filename", "__mode__" })
    ->AddSymbol("Close", { "_fileObject" })
    ->AddSymbol("GetFileType", { "_path" })
    ->AddSymbol("Exists", { "_path" })
    ->AddSymbol("GetFilePermissions", { "_path" })
    ->AddSymbol("_GetFreeSpace", { "_path" })
    ->AddSymbol("GetFileSize", { "_file" })
    ->AddSymbol("FormatSize", { "_num" })
    ->AddSymbol("GetLastWriteTime", { "_path" })
    ->AddSymbol("GetHardLinksCount", { "_path" })
    ->AddSymbol("ListDirectory", { "_path" });
    return m;
}
YAN_C_API_END


YAN_C_API_START void _FileObject_Init_Internal(Object *self, const std::string &name) {
    self->SetAttr("name", new String(name));
    self->SetAttr("read", BuiltinMethod::FromBuiltinFunction(FileObject_Read, self));
    self->SetAttr("write", BuiltinMethod::FromBuiltinFunction(FileObject_Write, self));
    self->SetAttr("readBuf", BuiltinMethod::FromBuiltinFunction(FileObject_ReadBuf, self));
    self->SetAttr("reverse", BuiltinMethod::FromBuiltinFunction(FileObject_Reverse, self));
    self->SetAttr("length", BuiltinMethod::FromBuiltinFunction(FileObject_Length, self));
    self->SetAttr("readLines", BuiltinMethod::FromBuiltinFunction(FileObject_ReadLines, self));
    self->SetAttr("eof", BuiltinMethod::FromBuiltinFunction(FileObject_Eof, self));
    int p = openedFileStream[name]->tellp();
    self->SetAttr("pos", new Number(p));
    openedFileStream[name]->seekp(p, std::ios::beg);
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
    auto fileObject = FileObject->Instantiate({ new String(name) }, ctx, st, et);
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
        mode = std::ios::in;
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
        } else if (modeString == "wb") {
            return result->Failure(new RuntimeError(
                "Binary IO not implemented yet",
                arg2->startPos, arg2->endPos, ctx
            ));
        } else if (modeString == "rb") {
            return result->Failure(new RuntimeError(
                "Binary IO not implemented yet",
                arg2->startPos, arg2->endPos, ctx
            ));
        } else if (modeString == "wba") {
            return result->Failure(new RuntimeError(
                "Binary IO not implemented yet",
                arg2->startPos, arg2->endPos, ctx
            ));
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
    if (self->typeName != std::string("ClassObject")) {
        return result->Failure(new TypeError(
            "Argument should be a `FileObject` created by fs.Open()",
            self->startPos, self->endPos, ctx
        ));
    }
    std::string clsName = As<ClassObject>(self)->className;
    if (clsName != "FileObject") {
        return result->Failure(new TypeError(
            std::format("Argument should be a `FileObject` created by fs.Open() [got {} object]", clsName),
            self->startPos, self->endPos, ctx
        ));
    }
    auto [arg, err] = self->GetAttr("name");
    if (arg == nullptr) {
        return result->Failure(new RuntimeError(
            "Broken file object",
            self->startPos, self->endPos, ctx
        ), err);
    }
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


YAN_C_API_START builtins::YanObject GetFileType(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }
    
    std::filesystem::path p = As<String>(arg)->s;
    auto s = std::filesystem::status(p);
    int type = 0;

    namespace fs = std::filesystem;

    if (fs::is_regular_file(s)) {
        type = YANFS_FILETYPE_FILE;
    } else if (fs::is_directory(s)) {
        type = YANFS_FILETYPE_DIR;
    } else if (fs::is_block_file(s)) {
        type = YANFS_FILETYPE_BLOCK;
    } else if (fs::is_character_file(s)) {
        type = YANFS_FILETYPE_CHAR;
    } else if (fs::is_fifo(s)) {
        type = YANFS_FILETYPE_FIFO;
    } else if (fs::is_socket(s)) {
        type = YANFS_FILETYPE_SOCK;
    } else if (fs::is_symlink(s)) {
        type = YANFS_FILETYPE_SYM;
    } else {
        type = YANFS_FILETYPE_ERR;
    }

    return (new RuntimeResult)->Success(new Number(type));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetFilePermissions(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }
    
    try {
        std::filesystem::path filePath = As<String>(arg)->s;
        auto stat = std::filesystem::status(filePath);  
        if (!std::filesystem::exists(stat)) {
            return result->Failure(new OSError(
            std::format("Path '{}' does not exists", filePath.string()),
            arg->startPos, arg->endPos, ctx
            ));
        }

        auto p = stat.permissions();
        std::string permissionString = "";

        using std::filesystem::perms;
        auto show = [=, &permissionString](char op, perms perm)
        {
            permissionString += (perms::none == (perm & p) ? '-' : op);
        };

        show('r', perms::owner_read);
        show('w', perms::owner_write);
        show('x', perms::owner_exec);
        show('r', perms::group_read);
        show('w', perms::group_write);
        show('x', perms::group_exec);
        show('r', perms::others_read);
        show('w', perms::others_write);
        show('x', perms::others_exec);

        return result->Success(new String(permissionString));
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    }
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject Exists(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto path = As<String>(arg);
    std::filesystem::path p = path->s;
    return result->Success(new Number((int) std::filesystem::exists(p)));
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject _GetFreeSpace(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    std::filesystem::path filePath = As<String>(arg)->s;
    try {
        auto stat = std::filesystem::status(filePath);
        if (!std::filesystem::exists(stat)) {
            return result->Failure(new OSError(
                std::format("Path '{}' does not exists", filePath.string()),
                arg->startPos, arg->endPos, ctx
            ));
        }

        auto spaceInfo = std::filesystem::space(filePath);
        std::vector<Object *> wrappedResult;

        auto PushValue = [=, &wrappedResult](std::uintmax_t val) {
            if (val > static_cast<std::uintmax_t>(YANFS_INT32_MAX)) {
                wrappedResult.push_back(new String(std::format("{}", val)));
            } else {
                wrappedResult.push_back(new Number(static_cast<int>(val)));
            }
        };
    
        PushValue(spaceInfo.capacity);
        PushValue(spaceInfo.available);
        PushValue(spaceInfo.free);
        return result->Success(new List(wrappedResult));
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    }
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetFileSize(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_file");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto filename = As<String>(arg)->s;
    try {
        std::filesystem::path p = filename;
        if (std::filesystem::is_directory(p)) {
            return result->Failure(new OSError(
                std::format("Is a directory [{}]", filename),
                arg->startPos, arg->endPos, ctx
            ));
        }
        auto size = std::filesystem::file_size(p);
        if (size > static_cast<std::uintmax_t>(YANFS_INT32_MAX)) {
            return result->Success(new String(std::format("{}", size)));
        } else {
            return result->Success(new Number(static_cast<int>(size)));
        }
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    } 
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject FormatSize(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_num");
    std::stringstream ss;

    if (arg->typeName != std::string("Number")) {
        return result->Failure(new TypeError(
            "_num should be a integer",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto num_ = As<Number>(arg);
    if (!builtins::Math::HoldsInteger(num_)) {
        return result->Failure(new TypeError(
            "_num should be a integer",
            arg->startPos, arg->endPos, ctx
        ));
    }
    auto num = builtins::Math::GetInt(num_);

    int o {};
    double mantissa = num;
    for (; mantissa >= 1024.; mantissa /= 1024., ++o);
    ss << std::ceil(mantissa * 10.) / 10. << "BKMGTPE"[o];
    if (!o) {
        // return result->Success(new String("0"));
        return result->Success(new String(ss.str()));    
    } else {
        ss << "B (" << num << ')';
        return result->Success(new String(ss.str()));
    }
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject ListDirectory(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto directory = As<String>(arg)->s;
    try {
        std::filesystem::path p = directory;
        if (!std::filesystem::is_directory(p)) {
            return result->Failure(new OSError(
                "_path should be a directory",
                arg->startPos, arg->endPos, ctx
            ));
        }

        std::vector<Object *> fileList;
        for (auto v : std::filesystem::directory_iterator(p)) {
            fileList.emplace_back(new String(v.path().string()));
        }
        return result->Success(new List(fileList));
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    } 
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetLastWriteTime(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_file should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto file = As<String>(arg)->s;
    try {
        std::filesystem::path p = file;
        auto ftime = std::filesystem::last_write_time(p);
        return result->Success(new String(std::format("{}", ftime)));
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    } 
}
YAN_C_API_END

YAN_C_API_START builtins::YanObject GetHardLinksCount(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("_path");
    if (arg->typeName != std::string("String")) {
        return result->Failure(new TypeError(
            "_path should be a string",
            arg->startPos, arg->endPos, ctx
        ));
    }

    auto filename = As<String>(arg)->s;
    try {
        std::filesystem::path p = filename;
        auto count = std::filesystem::hard_link_count(p);
        if (count > static_cast<std::uintmax_t>(YANFS_INT32_MAX)) {
            return result->Success(new String(std::format("{}", count)));
        } else {
            return result->Success(new Number(static_cast<int>(count)));
        }
    } catch (std::filesystem::filesystem_error &e) {
        std::cerr << "Fatal: [yan::fs] " << e.what() << std::endl;
        assert(false);
    }
}
YAN_C_API_END


YAN_C_API_START void YanModule_OnDestroy() {
    std::cout << openedFileStream.size() << std::endl;
    for (auto &&[filename, fileStream] : openedFileStream) {
        fileStream->close();
    }
}
YAN_C_API_END
