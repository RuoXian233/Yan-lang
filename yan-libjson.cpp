#include "yan-lang.hpp"

const std::string moduleName = "json";


template<typename T>
static std::string ToString(const std::vector<T> &v) {
    if (v.empty()) {
        return "[]";
    }
    std::stringstream ss;
    ss << "[";
    int index = 0;
    for (const auto &i : v) {
        ss << i;
        index++;
        if (index != v.size()) {
            ss << ", ";
        } else {
            ss << "]";
        }
    }
    return ss.str();
}

static void Fatal(const std::string &errInfo, bool _) {
    std::cerr << errInfo << std::endl;
    assert(errInfo.empty());
}


template<typename T, typename U>
static std::string ToString(const std::map<T, U> &m) {
    if (m.empty()) {
        return "{}";
    }
    std::stringstream ss;
    std::stringstream cache;
    ss << "{";
    unsigned i = 0;
    for (auto [k, v] : m) {
        if constexpr (std::is_same_v<std::remove_const_t<decltype(k)>, std::string> || std::is_same_v<decltype(k), const char *>) {
            cache << k;
            ss << std::format("\"{}\"", cache.str());
        } else {
            ss << k;
        }
        ss << ": ";
        cache.str("");
        if constexpr (std::is_same_v<std::remove_const_t<decltype(v)>, std::string> || std::is_same_v<decltype(v), const char *>) {
            cache << v;
            ss << std::format("\"{}\"", cache.str());
        } else {
            ss << v;
        }
        ss << ((i + 1 == m.size()) ? "}" : ", ");
        cache.str("");
        i++;
    }
    return ss.str();
}


enum class JSONValueType {
    Null, Integer, Floating, String, Array, Boolean, Object
};

struct JSONValue {
private:
    JSONValueType type;
public:
    // union {
        int intValue;
        double floatValue;
        bool booleanValue;
        std::string stringValue;
        std::map<std::string, JSONValue> objectValue;
        std::vector<JSONValue> arrayValue;
    // };
    bool nullFlag;

    JSONValue();
    JSONValue(const JSONValue &val);
    JSONValue(int val);
    JSONValue(double val);
    JSONValue(const std::string &val);
    JSONValue(const char *val);
    JSONValue(bool val);
    JSONValue(std::initializer_list<JSONValue> val);
    JSONValue(std::map<std::string, JSONValue> val);

    void Clear(JSONValueType type);
    void SetValue(int val);
    void SetValue(double val);
    void SetValue(const std::string &val);
    void SetValue(bool val);
    void SetValue(decltype(arrayValue) val);
    void SetValue(decltype(objectValue) val);
    void SetValue();
    void SetValue(const JSONValue &val);

    void Add(JSONValue val);
    void Add(const std::string &k, JSONValue v);

    JSONValue &operator[](const std::string &s);
    JSONValue &operator[](int i);

    JSONValueType GetType() const;
};


std::ostream &operator<<(std::ostream &out, const JSONValue &v);
class JSONLoader final {
public:
    JSONLoader();
    JSONLoader(const std::string &file);
    JSONValue Parse();
    void Clear();
    void Load(const std::string &file);
    void Loads(const std::string &str);
    static std::string Dumps(const JSONValue &v);

    static std::string errorInfo;
    static int errorFlag;

    static Object *Wrap(const JSONValue &value);

private:
    JSONValue _Parse();
    JSONValue ParseString();
    JSONValue ParseNumber();
    JSONValue ParseArray();
    JSONValue ParseBoolean();
    JSONValue ParseObject();
    JSONValue ParseNull();

private:
    std::stringstream it;
    unsigned line, col;
};


void JSONValue::Clear(JSONValueType type) {
    this->arrayValue.clear();
    this->stringValue.clear();
    this->objectValue.clear();

    this->nullFlag = false;
    switch (type) {
    case JSONValueType::Integer:
        this->type = JSONValueType::Integer;
        this->intValue = 0;
        break;
    case JSONValueType::Floating:
        this->type = JSONValueType::Floating;
        this->floatValue = 0.0;
        break;
    case JSONValueType::Boolean:
        this->type = JSONValueType::Boolean;
        this->booleanValue = false;
        break;
    case JSONValueType::String:
        this->type = JSONValueType::String;
        this->stringValue = "";
        break;
    case JSONValueType::Array:
        this->type = JSONValueType::Array;
        this->arrayValue = decltype(this->arrayValue) {};
        break;
    case JSONValueType::Object:
        this->type = JSONValueType::Object;
        this->objectValue = decltype(this->objectValue) {};
        break;
    default:
        this->type = JSONValueType::Null;
        this->nullFlag = true;
        break;
    }
}

JSONValue::JSONValue() {
    Clear(JSONValueType::Null);
}

JSONValue::JSONValue(int val) {
    Clear(JSONValueType::Integer);
    SetValue(val);
}

JSONValue::JSONValue(double val) {
    Clear(JSONValueType::Floating);
    SetValue(val);
}

JSONValue::JSONValue(const std::string &val) {
    Clear(JSONValueType::String);
    SetValue(val);
}

JSONValue::JSONValue(bool val) {
    Clear(JSONValueType::Boolean);
    SetValue(val);
}

JSONValue::JSONValue(const char *val) : JSONValue(std::string(val)) {}

JSONValue::JSONValue(std::initializer_list<JSONValue> val) {
    Clear(JSONValueType::Array);
    std::vector<JSONValue> values { val };
    SetValue(values);
}

JSONValue::JSONValue(std::map<std::string, JSONValue> val) {
    Clear(JSONValueType::Object);
    SetValue(val);
}

void JSONValue::Add(JSONValue val) {
    if (this->type == JSONValueType::Array) {
        this->arrayValue.push_back(val);
    }
}

void JSONValue::Add(const std::string &k, JSONValue v) {
    if (this->type == JSONValueType::Object) {
        this->objectValue.insert(std::make_pair(k, v));
    }
}

void JSONValue::SetValue(int val) {
    if (this->type == JSONValueType::Integer) {
        this->intValue = val;
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(double val) {
    if (this->type == JSONValueType::Floating) {
        this->floatValue = val;
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(const std::string &val) {
    if (this->type == JSONValueType::String) {
        this->stringValue = val;
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(bool val) {
    if (this->type == JSONValueType::Boolean) {
        this->booleanValue = val;
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(decltype(arrayValue) val) {
    if (this->type == JSONValueType::Array) {
        if (!this->arrayValue.empty()) {
            this->arrayValue.clear();
        }
        for (auto &&element : val) {
            this->arrayValue.push_back(element);
        }
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(decltype(objectValue) val) {
    if (this->type == JSONValueType::Object) {
        if (!this->objectValue.empty()) {
            this->objectValue.clear();
        }
        for (auto &&[k, v] : val) {
            this->objectValue.insert(std::make_pair(k, v));
        }
    } else {
        assert("JSONValue type mismatched" && false);
    }
}

void JSONValue::SetValue(const JSONValue &val) {
    switch (val.GetType()) {
    case JSONValueType::Integer:
        this->SetValue(val.intValue);
        break;
    case JSONValueType::Floating:
        this->SetValue(val.floatValue);
        break;
    case JSONValueType::Boolean:
        this->SetValue(val.booleanValue);
        break;
    case JSONValueType::String:
        this->SetValue(val.stringValue);
        break;
    case JSONValueType::Array:
        this->SetValue(val.arrayValue);
        break;
    case JSONValueType::Object:
        this->SetValue(val.objectValue);
        break;
    case JSONValueType::Null:
        this->SetValue();
        break;
    default:
        assert(false);
        break;
    }
}

void JSONValue::SetValue() {
    this->Clear(JSONValueType::Null);
}

JSONValueType JSONValue::GetType() const {
    return type;
}

JSONValue::JSONValue(const JSONValue &val) {
    this->Clear(val.GetType());
    this->SetValue(val);
}

std::ostream &operator<<(std::ostream &out, const JSONValue &v) {
    switch (v.GetType()) {
    case JSONValueType::Null:
        out << "(null)";
        break;
    case JSONValueType::Integer:
        out << v.intValue;
        break;
    case JSONValueType::String:
        out << "\"" << v.stringValue << "\"";
        break;
    case JSONValueType::Boolean:
        out << (v.booleanValue ? "true" : "false");
        break;
    case JSONValueType::Array:
        out << ToString(v.arrayValue);
        break;
    case JSONValueType::Object:
        out << ToString(v.objectValue);
        break;
    case JSONValueType::Floating:
        out << v.floatValue;
        break;
    default:
        assert(false);
        break;
    }

    return out;
}


JSONLoader::JSONLoader(const std::string &file) {
    line = col = 1;
    Load(file);
}

JSONLoader::JSONLoader() {
    line = col = 1;
}

void JSONLoader::Loads(const std::string &str) {
    Clear();
    it << str;
}

void JSONLoader::Clear() {
    it.str("");
    line = col = 1;
}

void JSONLoader::Load(const std::string &file) {
    Clear();
    std::ifstream f(file);
    if (f.fail()) {
        Fatal(std::format("JSONLoader: Cannot open file \"{}\"", file), false);
    }
    it << f.rdbuf();
    f.close();
}

JSONValue JSONLoader::ParseBoolean() {
    std::string validation;
    
    if (it.peek() == 'f') {
        for (int i = 0; i < 5; i++) {
            validation.push_back(it.get());
            col++;
        }
        if (validation != "false") {
            Fatal(std::format("JSONLoader (line {} col {}): Invalid identifier: \"{}\"", line, col, validation), false);
        }
        return JSONValue(false);
    } else {
        for (int i = 0; i < 4; i++) {
            validation.push_back(it.get());
            col++;
        }
        if (validation != "true") {
            Fatal(std::format("JSONLoader (line {} col {}): Invalid identifier: \"{}\"", line, col, validation), false);
        }
        return JSONValue(true);
    }
}

JSONValue JSONLoader::ParseString() {
    it.get();
    col++;
    std::string s;
    while (it.peek() != '"') {
        if (it.peek() == '\n') {
            line++;
            col = 1;
        }
        s.push_back(it.get());
        col++;
    }
    it.get();
    col++;
    return JSONValue(s);
}

JSONValue JSONLoader::ParseNull() {
    std::string validation;
    for (int i = 0; i < 4; i++) {
        validation.push_back(it.get());
        col++;
    }
    if (validation != "null") {
        Fatal(std::format("JSONLoader (line {} col {}): Invalid identifier: \"{}\"", line, col, validation), false); 
    }
    return JSONValue();
}

JSONValue JSONLoader::ParseNumber() {
    std::string s;
    while (isdigit(it.peek()) || it.peek() == 'e' || it.peek() == '.' || it.peek() == '-') {
        s.push_back(it.get());
        col++;
    }
    if (std::count(s.begin(), s.end(), '.') || count(s.begin(), s.end(), 'e') || std::count(s.begin(), s.end(), '-')) {
        if (std::count(s.begin(), s.end(), '-') > 1 || std::count(s.begin(), s.end(), 'e') > 1) {
            Fatal(std::format("JSONLoader (line {} col {}): Invalid number representaion", line, col), false);
        }
        return stof(s);
    }
    else {
        return stoi(s);
    }
}

JSONValue JSONLoader::ParseArray() {
    it.get();
    col++;
    JSONValue v;
    v.Clear(JSONValueType::Array);
    while (it.peek() != ']') {
        v.Add(_Parse());
        while (it.peek() != ']' && (it.peek() == ' ' || it.peek() == '\t' || it.peek() == '\n' || it.peek() == ',')) {
            if (it.peek() == '\n') {
                line++;
                col = 1;
            }
            it.get();
            col++;
        }
    }
    it.get();
    col++;
    return v;
}

JSONValue JSONLoader::ParseObject() {
    it.get();
    col++;
    JSONValue object;
    object.Clear(JSONValueType::Object);
    while (it.peek() != '}') {
        while (it.peek() == ' ' || it.peek() == '\t' || it.peek() == '\n') {
            if (it.peek() == '\n') {
                line++;
                col = 1;
            }
            it.get();
            col++;
        }
        if (it.peek() == '}') {
            break;
        }

        JSONValue key = _Parse();
        if (key.GetType() != JSONValueType::String) {
            Fatal(std::format("JSONLoader (line {} col {}): JSON key should be a string", line, col), false);
        }
        while (it.peek() == ' ' || it.peek() == ':') {
            it.get();
            col++;
        }

        JSONValue val = _Parse();
        object.Add(key.stringValue, val);
        while (it.peek() != '}' && (it.peek() == ' ' || it.peek() == '\t' || it.peek() == '\n')) {
            if (it.peek() == '\n') {
                line++;
                col = 1;
            }
            it.get();
            col++;
        }
        auto x = it.peek();
        if (it.peek() == ',') {
            it.get();
            col++;
        } else if (it.peek() == '}') {
            continue;
        } else {
            Fatal(std::format("JSONLoader (line {} col {}): Key-Value pair must be seperated by ','", line, col), false);
        }
    }

    it.get();
    col++;
    return object;
}

JSONValue &JSONValue::operator[](const std::string &s) {
    if (GetType() == JSONValueType::Object) {
        if (this->objectValue.find(s) == this->objectValue.end()) {
            Fatal(std::format("Key \"{}\" does not exist", s), false);
        }
        return this->objectValue[s];
    }
    Fatal("Unable to subscript object", false);
}

JSONValue &JSONValue::operator[](int i) {
    if (GetType() == JSONValueType::Array) {
        if (i >= this->arrayValue.size()) {
            Fatal("Array index out of range", false);
        }
        return this->arrayValue[i];
    }
    Fatal("Unable to subscript object", false);
}

JSONValue JSONLoader::Parse() {
    auto data = _Parse();
    while (it.peek() != -1) {
        if (it.peek() != -1 && it.peek() != '\n' && it.peek() != ' ' && it.peek() != '\t') {
            Fatal(std::format("JSONLoader (line {} col {}): Extra data", line, col), false);
        }
        it.get();
    }
    return data;
}

JSONValue JSONLoader::_Parse() {
    if (!it.str().empty()) {
        while (it.peek() != -1) {
            if (it.peek() == '\n' || it.peek() == '\t' || it.peek() == ' ') {
                if (it.peek() == '\n') {
                    line++;
                    col = 1;
                }
                it.get();
            } else if (it.peek() == '"') {
                return ParseString();
            } else if (it.peek() == 'f' || it.peek() == 't') {
                return ParseBoolean();
            } else if (it.peek() == '[') {
                return ParseArray();
            } else if (it.peek() == '{') {
                return ParseObject();
            } else if (it.peek() == 'n') {
                return ParseNull();
            } else if (isdigit(it.peek()) || it.peek() == '-' || it.peek() == '+') {
                return ParseNumber();
            } else {
                Fatal(std::format("JSONLoader (line {} col {}): Invalid character in JSON: '{}'", line, col, (char) it.peek()), false);
            }
        }
    }
    return 0;
}

std::string JSONLoader::Dumps(const JSONValue &v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}


YAN_C_API_START builtins::YanModuleDeclearation YanModule_OnLoad() {
    auto m = new builtins::YanModule(moduleName);
    m->AddSymbol("Load", { "_file" });
    return m;
}
YAN_C_API_END


Object *JSONLoader::Wrap(const JSONValue &value) {
    // assert(value.GetType() == JSONValueType::Object);
    if (value.GetType() == JSONValueType::Integer) {
        return new Number(value.intValue);
    } else if (value.GetType() == JSONValueType::Floating) {
        return new Number(value.floatValue);
    } else if (value.GetType() == JSONValueType::Boolean) {
        return new Number(static_cast<int>(value.booleanValue));
    } else if (value.GetType() == JSONValueType::Null) {
        return new Number(0);
    } else if (value.GetType() == JSONValueType::String) {
        return new String(value.stringValue);
    } else if (value.GetType() == JSONValueType::Array) {
        std::vector<Object *> arr;
        for (const auto i : value.arrayValue) {
            arr.push_back(Wrap(i));
        }
        return new List(arr);
    }

    auto rdct = value.objectValue;
    std::map<Object *, Object *> internal;

    for (const auto [k, v] : rdct) {
        switch (v.GetType()) {
        case JSONValueType::Integer:
            internal[new String(k)] = new Number(v.intValue);
            break;
        case JSONValueType::Floating:
            internal[new String(k)] = new Number(v.floatValue);
            break;
        case JSONValueType::String:
            internal[new String(k)] = new String(v.stringValue);
            break;
        case JSONValueType::Boolean:
            internal[new String(k)] = new Number(static_cast<int>(v.booleanValue));
            break;
        case JSONValueType::Null:
            internal[new String(k)] = new Number(0);
            break;
        case JSONValueType::Array: 
            {
                std::vector<Object *> arr;
                for (const auto x : v.arrayValue) {
                    arr.push_back(Wrap(x));
                }
                internal[new String(k)] = new List(arr);
            }
            break;
        case JSONValueType::Object:
            {
                std::map<Object *, Object *> o;
                for (const auto [kk, vv]: v.objectValue) {
                    o[new String(kk)] = Wrap(vv);
                }
                internal[new String(k)] = new Dictionary(o);
            }
            break;
        default:
            assert(false);
            break;
        }
    }
    return new Dictionary(internal);
}


YAN_C_API_START builtins::YanObject Load(builtins::YanContext ctx) {
    auto result = new RuntimeResult;

    auto arg = ctx->symbols->Get("_file");
    if (arg->typeName != std::string("String")) {
        return result->Failure(
            new TypeError("Expected a string path", arg->startPos, arg->endPos, arg->ctx)
        );
    }
    auto path = As<String>(arg);
    auto val = JSONLoader(path->s).Parse();
    return result->Success(JSONLoader::Wrap(val));
}
YAN_C_API_END

YAN_C_API_START void YanModule_OnDestroy() {}
YAN_C_API_END
