#include <utility>
#ifdef _MSC_VER
    #define _CRT_SECURE_NO_WARNINGS
#endif

#define YAN_C_API_START extern "C" {
#define YAN_C_API_END }
#define YAN_CONTEXT_DECORATION(current) ctx->ctxLabel = std::format("@{}." #current " (aka '{}')", moduleName, ctx->ctxLabel)
#define YAN_METHOD_CONTEXT_DECORATION(name) ctx->ctxLabel = std::format("{}.{}", this->className, name)
// deprecated macro
#define YAN_INITIALIZE_CALL_STACK_INFO() 
// deprecated 

#define RESET "\033[0m"
#define BLACK "\033[30m"  /* Black */
#define RED "\033[31m"    /* Red */
#define GREEN "\033[32m"  /* Green */
#define YELLOW "\033[33m" /* Yellow */
#define BLUE "\033[34m"   /* Blue */
#define PURPLE "\033[35m" /* Purple */
#define CYAN "\033[36m"   /* Cyan */
#define WHITE "\033[37m"  /* White */
#define BOLD_WHITE "\033[1;37m"

#define NORMAL() (std::cout << RESET)
#define INFO() (std::cout << GREEN)
#define WARN() (std::cout << YELLOW)
#define ERROR() (std::cout << RED)
#define DEBUG() (std::cout << CYAN)


#include <cstring>
#include <iostream>
#include <string>
#include <format>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <functional>
#include <variant>
#include <cmath>
#include <map>
#include <memory>
#include <chrono>
#include <algorithm>
#include <cstdio>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <cstring>


const char *YAN_LANG_VERSION = "2.0";
const char *TOKEN_TYPE_TAGS[] {"Int", "Float", "OP_Plus", "OP_Minus", "OP_Mul", "OP_Div", "OP_Pow", 
                            "Lparen", "Rparen", "LSquare", "RSquare", "Identifier", "Keyword", 
                            "OP_Eq", "OP_Equal", "OP_Nequal", "OP_Lt", "OP_Gt", "OP_Lte", "OP_Gte", "Comma", "Arrow", "String", "Newline", "Dot", "Colon", "LStart", "RStart",
                             "Invilid", "EOF"};
const std::string EOF_ = "EOF";
const std::wstring EOFW_ = L"EOF";
const std::vector<std::string> SPACES { " ", "\t" };
const std::vector<std::string> DIGITS { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
const std::vector<std::string> LETTERS {
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};

const std::vector<std::string> LETTERS_WITH_DIGITS {
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
    "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
    "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};

const std::vector<std::string> KEYWORDS {
    "var", "and", "or", "not", "if", "elif", "then", "else", "for", "while", "step", "to", "function", "end", "return", "continue", "break", "in", "new", "nonlocal",
    "defer"
};

const std::vector<std::string> STATEMENT_SEPERATORS {
    "\n", ";"
};

const std::map<std::string, std::string> ESCAPED_CHARACTERS {
    { "n", "\n" }, { "t", "\t" }, { "b", "\b" }, { "r", "\r" },
    { "'", "\'" }
};

const std::map<std::string, std::string> REVERSED_ESCAPE_CHARACTERS {
    { "\n", "n" }, { "\t", "t" }, { "\b", "b" }, { "\r", "r" },
    { "\'", "'" }
};

#if defined(__LP64__) || defined(_WIN64)
    const char *platformInfo = "AMD64";
#else
    const char *platformInfo = "i386";
#endif

#ifdef __clang__
    const std::string compilerInfo =  std::format("[Clang {}.{}.{} ({})]", __clang_major__, __clang_minor__, __clang_patchlevel__, platformInfo);
#endif

#ifdef __GNUC__
    #ifndef __clang__
    const std::string compilerInfo = std::format("[GCC {}.{} ({})]", __GNUC__, __GNUC_PATCHLEVEL__, platformInfo);
    #endif
#endif

#ifdef _MSC_VER
    const std::string compilerInfo = std::format("[MSC v.{} ({})]", _MSC_VER, platformInfo);
#endif

#ifdef _WIN32
    namespace win32 {
        #include <windows.h>
        #include <dbghelp.h>
    }
    const char *platform = "win32";
#elif defined(__linux__)
    #include <dlfcn.h>
    #include <execinfo.h>
    #include <cxxabi.h>
    const char *platform = "linux";
#elif defined(__APPLE__)
    const char *platform = "darwin"
#else
    const char *platform = "unknown";
#endif

const std::string compilationTimeStamp = std::format("({}, {})", __DATE__, __TIME__);
bool debug = false;
const unsigned MAX_CALLSTACK_DEPTH = 50;
long currentCallStackDepth = 0;
unsigned overflowCount = 0;
const unsigned INVILID_OVERFLOW_TOLERANCE = 10;


class BigInteger {
private:
    static const int BASE = 100000000;
    static const int WIDTH = 8;
    bool sign;
    size_t length;
    std::vector<int> num;

    void CutLeadingZero() {
        while (num.back() == 0 && num.size() != 1) {
            num.pop_back();
        }
    }

    void SetLength() {
        this->CutLeadingZero();
        int tmp = num.back();
        if (tmp == 0) {
            length = 1;
        } else {
            length = (num.size() - 1) * 8;
            while (tmp > 0) {
                ++length;
                tmp /= 10;
            }
        }
    }

public:
    BigInteger(int n = 0) { *this = n; }
    BigInteger(long long n) { *this = n; }
    BigInteger(const char *n) { *this = n; }
    BigInteger(const BigInteger &n) { *this = n; }
    BigInteger(const std::string &n) { *this = n; }

    const BigInteger& operator=(int n) {
         *this = (long long)n;
        return *this;
    }

    const BigInteger& operator=(long long n) {
        num.clear();
        if (n == 0) {
            num.push_back(0);
        }
        if (n >= 0) {
            sign = true;
        } else if (n == LONG_LONG_MIN) {
            *this = "-9223372036854775808";
            return *this;
        } else if (n < 0) {
            sign = false;
            n = -n;
        }

        while (n != 0) {
            num.push_back(n % BASE);
            n /= BASE;
        }
        this->SetLength();
        return *this;
    }

    const BigInteger& operator=(const char *n) {
        int len = strlen(n);
        int tmp = 0;
        int ten = 1;
        int stop = 0;
        num.clear();
        sign = (n[0] != '-');
        if (!sign) {
            stop = 1;
        }
        for (int i = len; i > stop; --i) {
            tmp += (n[i - 1] - '0') * ten;
            ten *= 10;
            if ((len - i) % 8 == 7) {
                num.push_back(tmp);
                tmp = 0;
                ten = 1;
            }
        }
        if ((len - stop) % WIDTH != 0) {
            num.push_back(tmp);
        }
        SetLength();
        return *this;
    }

    const BigInteger &operator=(const std::string &n) {
        *this = n.c_str();
        return *this;
    }

    const BigInteger& operator=(const BigInteger &n) {
        num = n.num;
        sign = n.sign;
        length = n.length;
        return *this;
    }

    size_t size() const { return this->length; }
    BigInteger e(size_t n) const {
        int tmp = n % 8;
        BigInteger ans;
        ans.length = n + 1;
        n /= 8;

        while (ans.num.size() <= n) {
            ans.num.push_back(0);
        }
        ans.num[n] = 1;
        while (tmp--) {
            ans.num[n] *= 10;
        }
        return ans * (*this);
    }

    BigInteger abs() const {
        BigInteger ans(*this);
        ans.sign = true;
        return ans;
    }

    const BigInteger& operator+() const {
        return *this;
    }

    friend BigInteger operator+(const BigInteger &a, const BigInteger &b) {
        if (!b.sign) {
            return a - (-b);
        }
        if (!a.sign) {
            return b - (-a);
        }
        BigInteger ans;
        int carry = 0;
        int aa, bb;
        size_t lena = a.num.size();
        size_t lenb = b.num.size();
        size_t len = std::max(lena, lenb);
        ans.num.clear();

        for (size_t i = 0; i < len; ++i) {
            if (lena <= i) {
                aa = 0;
            } else {
                aa = a.num[i];
            }
            if (lenb <= i) {
                bb = 0;
            } else {
                bb = b.num[i];
            }
            ans.num.push_back((aa + bb + carry) % BigInteger::BASE);
            carry = (aa + bb + carry) / BigInteger::BASE;
        }
        if (carry > 0) {
            ans.num.push_back(carry);
        }
        ans.SetLength();
        return ans;
    }

    const BigInteger& operator+=(const BigInteger &n) {
        *this = *this + n;
        return *this;
    }

    const BigInteger& operator++() {
        *this = *this + 1;
        return *this;
    }

    BigInteger operator++(int) {
        BigInteger ans(*this);
        *this = *this + 1;
        return ans;
    }

    BigInteger operator-() const {
        BigInteger ans(*this);
        if(ans != 0) {
            ans.sign = !ans.sign;
        }
        return ans;
    }

    friend BigInteger operator-(const BigInteger &a, const BigInteger &b) {
        if(!b.sign) {
            return a + (-b);
        }
        if(!a.sign) {
            return -((-a) + b);
        }
        if(a < b) {
            return -(b - a);
        }

        BigInteger ans;
        int carry = 0;
        int aa, bb;
        size_t lena = a.num.size();
        size_t lenb = b.num.size();
        size_t len = std::max(lena, lenb);
        ans.num.clear();

        for (size_t i = 0; i < len; ++i) {
            aa = a.num[i];
            if (i >= lenb) {
                bb = 0;
            } else {
                bb = b.num[i];
            }
            ans.num.push_back((aa - bb - carry + BigInteger::BASE) % BigInteger::BASE);
            if (aa < bb + carry) {
                carry = 1;
            } else {
                carry = 0;
            }
        }
     
        ans.SetLength();
        return ans;
    }

    const BigInteger& operator-=(const BigInteger &n) {
        *this = *this - n;
        return *this;
    }

    const BigInteger& operator--() {
        *this = *this - 1;
        return *this;
    }

    BigInteger operator--(int) {
        BigInteger ans(*this);
        *this = *this - 1;
        return ans;
    }

    friend BigInteger operator*(const BigInteger &a, const BigInteger &b) {
        size_t lena = a.num.size();
        size_t lenb = b.num.size();
        std::vector<long long> ansLL;
        for (size_t i = 0; i < lena; ++i) {
            for (size_t j = 0; j < lenb; ++j) {
                if (i + j >= ansLL.size()) {
                    ansLL.push_back((long long)a.num[i] * (long long)b.num[j]);
                } else {
                    ansLL[i + j] += (long long)a.num[i] * (long long)b.num[j];
                }
            }
        }

        while (ansLL.back() == 0 && ansLL.size() != 1) {
            ansLL.pop_back();
        }
        size_t len = ansLL.size();
        long long carry = 0;
        long long tmp;
        BigInteger ans;
        ans.sign = (ansLL.size() == 1 && ansLL[0] == 0) || (a.sign == b.sign);
        ans.num.clear();
        for (size_t i = 0; i < len; ++i) {
            tmp = ansLL[i];
            ans.num.push_back((tmp + carry) % BigInteger::BASE);
            carry = (tmp + carry) / BigInteger::BASE;
        }
        if (carry > 0) {
            ans.num.push_back(carry);
        }
        ans.SetLength();
        return ans;
    }

    const BigInteger& operator*=(const BigInteger &n) {
        *this = *this * n;
        return *this;
    }

    friend BigInteger operator/(const BigInteger &a, const BigInteger &b) {
        BigInteger aa(a.abs());
        BigInteger bb(b.abs());
        if (aa < bb) {
            return 0;
        }
        char *str = new char[aa.size() + 1];
        memset(str, 0, sizeof(char) * (aa.size() + 1));
        BigInteger tmp;
        int lena = aa.length;
        int lenb = bb.length;
        for (int i = 0; i <= lena - lenb; ++i) {
            tmp = bb.e(lena - lenb - i);
            while(aa >= tmp) {
                ++str[i];
                aa = aa - tmp;
            }
            str[i] += '0';
        }

        BigInteger ans(str);
        delete[] str;
        ans.sign = (ans == 0 || a.sign == b.sign);
        return ans;
    }

    const BigInteger& operator/=(const BigInteger &n) {
        *this = *this / n;
        return *this;
    }

    friend BigInteger operator%(const BigInteger &a, const BigInteger &b) {
        return a - a / b * b;
    }

    const BigInteger& operator%=(const BigInteger &n) {
        *this = *this - *this / n * n;
        return *this;
    }

    friend bool operator<(const BigInteger &a, const BigInteger &b) {
        if (a.sign && !b.sign) {
            return false;
        } else if (!a.sign && b.sign) {
            return true;
        } else if (a.sign && b.sign) {
            if (a.length < b.length) {
                return true;
            } else if (a.length > b.length) {
                return false;
            } else {
                size_t lena = a.num.size();
                for (int i = lena - 1; i >= 0; --i) {
                    if (a.num[i] < b.num[i]) {
                        return true;
                    } else if (a.num[i] > b.num[i]) {
                        return false;
                    }
                }
                return false;
            }
        } else {
            return -b < -a;
        }
    }
    
    friend bool operator<=(const BigInteger &a, const BigInteger &b) {
        return !(b < a);
    }

    friend bool operator>(const BigInteger &a, const BigInteger &b) {
        return b < a;
    }

    friend bool operator>=(const BigInteger &a, const BigInteger &b) {
        return !(a < b);
    }

    friend bool operator==(const BigInteger &a, const BigInteger &b) {
        return !(a < b) && !(b < a);
    }

    friend bool operator!=(const BigInteger &a, const BigInteger &b) {
        return (a < b) || (b < a);
    }

    friend bool operator||(const BigInteger &a, const BigInteger &b) {
        return a != 0 || b != 0;
    }

    friend bool operator&&(const BigInteger &a, const BigInteger &b) {
        return a != 0 && b != 0;
    }

    bool operator!() {
        return *this == 0;
    }

    friend std::ostream& operator<<(std::ostream &out, const BigInteger &n) {
         size_t len = n.num.size();
        if(!n.sign) {
            out << '-';
        }
        out << n.num.back();
        for(int i = len - 2; i >= 0; --i) {
            out << std::setw(BigInteger::WIDTH) << std::setfill('0') << n.num[i];
        }
        return out;
    }

    friend std::istream& operator>>(std::istream &in, BigInteger &n) {
        std::string str;
        in >> str;
        size_t len = str.length();
        size_t i, start = 0;
        if(str[0] == '-') {
            start = 1;
        }
        if(str[start] == '\0') {
            return in;
        }
        for(i = start; i < len; ++i) {
            if(str[i] < '0' || str[i] > '9') {
                return in;
            }
        }
        n = str.c_str();
        return in;
    }
};


enum class TokenType {
    Int,
    Float, 
    OP_Plus,
    OP_Minus,
    OP_Mul,
    OP_Div,
    OP_Pow,
    Lparen,
    Rparen,
    LSquare,
    RSquare,
    Identifier,
    Keyword,
    OP_Eq,
    OP_Equal,
    OP_Nequal,
    OP_Lt,
    OP_Gt,
    OP_Lte,
    OP_Gte,
    Comma,
    Arrow,
    String,
    NewLine,
    Dot,
    Colon,
    LStart,
    RStart,
    Invilid,
    __EOF__
};

const std::vector<TokenType> NUMERIC_TOKEN_TYPES { TokenType::Int, TokenType::Float };
const std::vector<TokenType> FIRST_LEVEL_OPERANDS { TokenType::OP_Plus, TokenType::OP_Minus };
const std::vector<TokenType> SECOND_LEVEL_OPERANDS { TokenType::OP_Mul, TokenType::OP_Div };
const std::vector<TokenType> OPERANDS { TokenType::OP_Mul, TokenType::OP_Div, TokenType::OP_Plus, TokenType::OP_Minus };
const std::vector<TokenType> THIRD_LEVEL_OPERAND { TokenType::OP_Pow };
const std::vector<std::pair<TokenType, std::string>> LOGIC_OPERANDS { { TokenType::Keyword, "and" }, { TokenType::Keyword, "or" }, { TokenType::Keyword, "not" } };
const std::vector<TokenType> COMPARE_OPERANDS { TokenType::OP_Equal, TokenType::OP_Nequal, TokenType::OP_Gt, TokenType::OP_Lt, TokenType::OP_Gte, TokenType::OP_Lte };


inline std::wstring ToWideString(const std::string& input) {
    try {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.from_bytes(input);
    } catch (std::range_error &re) {
        std::cerr << "Fatal: Input string '" << input << "' contains invilid character(s)" << std::endl;
        return L"0";
    }
}

inline std::string ToByteString(const std::wstring& input) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input);
}


std::string DemangleSymbol(const char *symbol) {
    const std::unique_ptr<char, decltype(&free)> demangled(abi::__cxa_demangle(symbol, 0, 0, 0), &free);
    if (demangled) {
        return demangled.get();
    }
    else {
        return symbol;
	}
}


template<typename T>
concept StringAble = requires(T t) {
    t.ToString();
};


template<StringAble T>
inline void PrintSequence(const std::vector<T> &seq) {
    std::cout << "[";
    for (int i = 0; i < seq.size(); i++) {
        std::cout << seq[i].ToString();
        if (i + 1 != seq.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

template<StringAble T>
inline void PrintSequence(const std::vector<T *> &seq) {
    std::cout << "[";
    for (int i = 0; i < seq.size(); i++) {
        std::cout << seq[i]->ToString();
        if (i + 1 != seq.size()) {
            std::cout << ", ";
        }
    }
    std::cout << "]";
}

template<typename T>
inline std::string StringifySequence(const std::vector<T *> &seq) {
    std::string result = "[";
    for (int i = 0; i < seq.size(); i++) {
        result += seq[i]->ToString();
        if (i + 1 != seq.size()) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}

inline std::string StringifyStringSequence(const std::vector<std::string> &seq) {
    std::string result = "[";
    for (int i = 0; i < seq.size(); i++) {
        result += seq[i];
        if (i + 1 != seq.size()) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}


struct Position {
    int index, line, column;
    std::string filename, fileContent;
    Position(int idx, int ln, int col, const std::string &fn, const std::string &text) :
    index(idx), line(ln), column(col), filename(fn), fileContent(text) {}
    
    inline Position *Advance(const std::wstring &ch = L"") {
        this->index++;
        this->column++;
        if (ch == L"\n") {
            this->line++;
            this->column = 0;
        }
        return this;
    }

    inline Position *Copy() {
        return new Position(this->index, this->line, this->column, this->filename, this->fileContent);
    }

    inline std::string ToString() {
        return std::format("Position(line:{}, col:{}) [{}:{}]", this->line + 1, this->column, this->filename, this->index);
    }
};


struct SymbolTable;
struct NodeBase;
class Interpreter;
SymbolTable *globalSymbolTable;

struct Context final {
    std::string ctxLabel;
    Context *parent;
    Position *parentEntry;
    SymbolTable *symbols;
    SymbolTable *global;
    SymbolTable *nonlocals;
    std::vector<NodeBase *> deferNodes;
    Interpreter *interpreter;
    bool external;
    Context *exFrame;

    explicit Context(const std::string &ctxName, Context *parent = nullptr, Position *parentEntry = nullptr) : ctxLabel(ctxName),
        parent(parent), parentEntry(parentEntry)
    {
        if (this->parent && this->parent->interpreter) {
            this->interpreter = this->parent->interpreter;
        }
        this->SetGlobalSymbolTable(globalSymbolTable);
    }

    void SetGlobalSymbolTable(SymbolTable *st) {
        this->global = st;
    }
    
    inline void SetExternal(bool e) {
        this->external = e;
    }
};

struct Token {
    void *value;
    TokenType type;
    Position *st;
    Position *et;

    Token() : value(nullptr), type(TokenType::Invilid) {}
    explicit Token(TokenType t, void *v = nullptr, Position *st = nullptr, Position *et = nullptr) : 
        type(t), value(v), st(st), et(et) {
            if (st != nullptr) {
                this->st = st->Copy();
                this->et = st->Copy();
                this->et->Advance();
            }
            if (et != nullptr) {
                this->et = et->Copy();
            }
        }

    inline std::string ToString() const {
        auto tokenTypeName = TOKEN_TYPE_TAGS[static_cast<int>(this->type)];
        if (this->value != nullptr) {
            if (this->type == TokenType::Int) {
                return std::format("{}: {} ({})", tokenTypeName, this->value, *(long *) this->value);
            } else if (this->type == TokenType::Float) {
                return std::format("{}: {} ({})", tokenTypeName, this->value, *(double *) this->value);
            } else if (this->type == TokenType::Identifier || this->type == TokenType::Keyword) {
                return std::format("{}: {} ('{}')", tokenTypeName, this->value, *(std::string *) this->value);
            } else if (this->type == TokenType::String) {
                return std::format("{}: {} ('{}')", tokenTypeName, this->value, *(std::string *) this->value);
            }
        }
        return std::string(tokenTypeName);
    }

    template<typename VT>
    bool Matches(TokenType tt, const VT &value) {
        return this->type == tt && (*(VT *) this->value) == value;
    }

    bool Matches(TokenType tt) {
        return this->type == tt;
    }
};

using Tokens = std::vector<Token>;

std::string SubReplace(const std::string &resource_str, const std::string &sub_str, const std::string &new_str) {
    std::string dst_str = resource_str;
    std::string::size_type pos = 0;

    while((pos = dst_str.find(sub_str)) != std::string::npos) {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    return dst_str;
}

static inline std::vector<std::string> Split(const std::string &str, const std::string &splitter) {
    std::vector<std::string> tokens;
    // if (str != splitter && str.length() >= splitter.length()) {
    //     char *src = new char[str.length() + 1];
    //     strcpy(src, str.c_str());

    //     char *token = strtok(src, splitter.c_str());
    //     tokens.push_back(std::string(token));
    //     while (true) {
    //         token = strtok(nullptr, splitter.c_str());
    //         if (token == nullptr) {
    //             break;
    //         }
    //         tokens.push_back(std::string(token));
    //     }
    //     return tokens;
    // } else {
    //     tokens.push_back(str);
    //     return tokens;
    // }
    if (str != splitter && str.length() >= splitter.length()) {
        std::wstring wsplitter = ToWideString(splitter);        
        std::wstring wstr = ToWideString(str);
        wstr += wsplitter;

        size_t pos = wstr.find(wsplitter);
        int step = wsplitter.length();

        while (pos != wstr.npos) {
            std::wstring temp = wstr.substr(0, pos);
            tokens.emplace_back(ToByteString(temp));
            wstr = wstr.substr(pos + step, wstr.length());
            pos = wstr.find(wsplitter);
        
        }
        return tokens;
    } else {
        tokens.push_back(str);
        return tokens;
    }
}

std::string Join(const std::vector<std::string> &tokens, const std::string &splitter) {
    std::stringstream ss;
    int index = 1;
    for (const auto &token : tokens) {
        ss << token;
        if (index != tokens.size()) {
            ss << splitter;
        }
        index++;
    }
    return ss.str();
}



class Error {
public:
    Error(const std::string &name, const std::string &details, Position *st = nullptr, Position *et = nullptr) 
    : name(name), details(details), st(st), et(et) {}
 
    #define deprecated 
    [[deprecated]] static std::string StringWithArrow(const std::string &s, Position *st, Position *et, int offset = 2, char ch = '^') {
        if (st == nullptr || et == nullptr) {
            return std::format("\n  {} (internal)", s);
        }
        std::string result;
        int indexStart = std::max((int) s.rfind("\n", 0, st->index), 0);
        int indexEnd = s.find("\n", indexStart + 1);
        indexEnd = indexEnd < 0 ? s.length() : indexEnd;
        int lineCount = et->line - st->line + 1;
        
        for (int line = 0; line < lineCount; line++) {
            auto ln = s.substr(indexStart, indexEnd - indexStart);
            auto columnStart = line == 0 ? st->column : 0;
            auto columnEnd = line == lineCount - 1 ? et->column : ln.length() - 1;

            result += ln + '\n';
            result += std::string(columnStart + offset, ' ') + std::string(columnEnd - columnStart, ch);

            indexStart = indexEnd;
            indexEnd = s.find("\n", indexStart + 1);
            indexEnd = indexEnd < 0 ? s.length() : indexEnd;
        }
        return SubReplace(result, "\t", "");
    }

    virtual std::string ToString() {
        // Fix code line display
        // auto content = Error::StringWithArrow(Split(this->st->fileContent, "\n")[this->st->line], this->st, this->et);
        return std::format("File \"{}\", line {}\n  {}: {}", this->st->filename, this->st->line + 1, this->name, this->details);
    }

public:
    std::string name;
    std::string details;
    Position *st;
    Position *et;
};

struct Context;

class IllegalCharacterError final : public Error {
public:
    IllegalCharacterError(const std::string &details, Position *st, Position *et) : 
        Error("IllegalCharacterError", details, st, et) {}
};

class SyntaxError final : public Error {
public:
    SyntaxError(const std::string &details, Position *st, Position *et) : 
        Error("SyntaxError", details, st, et) {}
};

class RuntimeError : public Error {
public:
    RuntimeError(const std::string &details, Position *st, Position *et, Context *ctx = nullptr) : 
        Error("RuntimeError", details, st, et) {
            this->ctx = ctx;
        }
    
    RuntimeError(const std::string& errName, const std::string& details, Position *st, Position *et, Context *ctx = nullptr) :
        Error(errName, details, st, et) {
        this->ctx = ctx;
    }

    virtual std::string GenerateTraceBack() {
        std::string result = "Stack Traceback:\n";
        auto pos = this->st;
        auto rt = this->ctx;
        if (rt == nullptr || pos == nullptr) {
            return "Stack Traceback:\n  [U] ?? (not avalible)";
        }
        while (rt != nullptr) {
            // result += std::format("  at {}  [{}:{}] <+{}>\n", rt->ctxLabel, pos->filename, pos->line + 1, (void *) pos);
            result += std::format("  at {}{}{} [{}{}{}:{}:{}]\n", CYAN, rt->ctxLabel, RESET, YELLOW, pos->filename, RESET, pos->line + 1, pos->column + 1);
            pos = rt->parentEntry;
            rt = rt->parent;
        }
        result = result.substr(0, result.length() - 1);

        return result;
    }

    virtual std::string ToString() override {
        // auto content = Error::StringWithArrow(Split(this->st->fileContent, "\n")[this->st->line], this->st, this->et);        
        // Fix me: Error content display
        auto basicErrorInfo = std::format("{}: {}", this->name, this->details);
        auto traceback = this->GenerateTraceBack();
        if (!this->st || !this->et) {
            return std::format("{}{}{}\n {}", RED, basicErrorInfo, RESET, traceback);
        }
        try {
            auto possibleSource = ToWideString(Split(this->st->fileContent, "\n")[this->st->line]);
            std::wstringstream wss;
            const auto red = ToWideString(RED);
            const auto reset = ToWideString(RESET);
            int col = 0;

            for (auto c : possibleSource) {
                if (col == this->st->column - 1) {
                    wss << ToWideString(RED);
                }

                if (this->et->line == this->st->line && col == this->et->column) {
                    wss << ToWideString(RESET);
                }
                wss << c;
                col++;
            }
            possibleSource = wss.str() + reset;
            return std::format("{}{}{}\n  (Possibly related source: `{}{}{}`)\n{}", RED, basicErrorInfo, RESET, BOLD_WHITE, ToByteString(possibleSource), RESET, traceback);    
        } catch (std::exception &e) {
            return std::format("{}{}{}\n  {}{}{}\n{}", RED, basicErrorInfo, RESET, BOLD_WHITE, "(Source is not avaliable)", RESET, traceback);
        }
    }

    static std::string GetNativeCallStackInfo(bool demangle = true, bool colored = true) {
        std::stringstream ss;
        void* addresses[256];
        const int n = backtrace(addresses, std::extent<decltype(addresses)>::value);
        const std::unique_ptr<char*, decltype(&free)> symbols(backtrace_symbols(addresses, n), &free);
        for (int i = 0; i < n; i++) {
            char *symbol = symbols.get()[i];
            char *end = symbol;

            // demangle 
            if (demangle) {
                int p = 0;
                int q = 0;
                while (*end) {
                    ++end;
                }
                while (end != symbol && *end != '+') {
                    --end;
                }
                char *begin = end;
                while (begin != symbol && *begin != '(') {
                    --begin;
                }

                if (begin != symbol) {
                    auto str1 = std::string(symbol, ++begin - symbol);
                    *end++ = '\0';
                    std::string result = str1 + DemangleSymbol(begin) + '+' + end;
                    ss << "#" << i + 1 << ": " << result << std::endl;
                }
                else {
                    ss << "#" << i + 1 << ": " << symbol << std::endl;
                }
            } else {
                ss << "#" << i + 1 << ": " << symbol << std::endl;
            }
        }
        
        auto r = ss.str();
        std::stringstream rs;
        for (auto &&c : std::move(r)) {
            rs << c;            
            if (colored) {
                if (c == '(') {
                    rs << CYAN;
                } else if (c == ')') {
                    rs << RESET;
                } else if (c == '[') {
                    rs << GREEN;
                } else if(c == ']') {
                    rs << RESET;
                }
            } 
        }
        return rs.str();
    }

    inline Context *GetContext() {
        return this->ctx;
    }

    inline void SetContext(Context *ctx) {
        this->ctx = ctx;
    }

private:
    Context *ctx;
};

class TypeError : public RuntimeError {
public:
    explicit TypeError(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) :
        RuntimeError("TypeError", details, st, et, ctx) {}
};

class ValueError : public RuntimeError {
public:
    explicit ValueError(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) :
        RuntimeError("ValueError", details, st, et, ctx) {}
};

class OSError : public RuntimeError {
public:
    explicit OSError(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) : 
        RuntimeError("OSError", details, st, et, ctx) {}
};

class Panic : public RuntimeError {
public:
    explicit Panic(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) :
        RuntimeError("Panic", details, st, et, ctx) {}
};

class AttributeError : public RuntimeError {
public:
    explicit AttributeError(const std::string& details, Position* st, Position* et, Context* ctx = nullptr) :
        RuntimeError("AttributeError", details, st, et, ctx) {}
};

class StopIteration : public RuntimeError {
public:
    explicit StopIteration() : StopIteration(nullptr, nullptr, nullptr) {}
    explicit StopIteration(Position *st, Position *et, Context *ctx) :
        RuntimeError("StopIteration", "",  st, et, ctx) {}
};


class Lexer final {
public:
    explicit Lexer(const std::string &filename, const std::wstring &t) : 
    text(t), filename(filename), pos(new Position(-1, 0, -1, filename, ToByteString(text))) {
        this->Advance();
    }

    static inline std::wstring CharAt(const std::wstring &s, unsigned index) {
        return s.substr(index, 1);
    }

    template<typename T>    
    static inline bool Contains(const std::vector<T> &c, const T &v) {
        for (auto e : c) {
            if (v == e) {
                return true;
            }
        }
        return false;
    }

    static inline long ParseInt(const std::string &numStr) {
        std::istringstream is(numStr);
        long num;
        is >> num;
        return num;
    }

    static inline std::pair<long, Error *> ParseIntWithError(const std::string &numStr, Position *st, Position *et) {
        for (auto c : numStr) {
            if (!isdigit(c)) {
                return std::make_pair((long) 0, new Error(
                    "ParseError",
                    "Invilid characters in integer",
                    st, et
                ));
            }
        }
        return std::make_pair(ParseInt(numStr), nullptr);
    }

    static inline std::pair<long, Error *> ParseFloatWithError(const std::string &numStr, Position *st, Position *et) {
        for (auto c : numStr) {
            if (!isdigit(c)) {
                return std::make_pair((long) 0, new Error(
                    "ParseError",
                    "Invilid characters in a floating point number",
                    st, et
                ));
            }
        }
        return std::make_pair(ParseFloat(numStr), nullptr);
    }

    static inline double ParseFloat(const std::string &numStr) {
        std::istringstream is(numStr);
        double num;
        is >> num;
        return num;
    }

    void Advance() {
        this->pos->Advance(this->currentChar);
        this->currentChar = this->pos->index < (int) this->text.length() ? Lexer::CharAt(this->text, this->pos->index) : ToWideString(EOF_);
    }

    Token MakeNumber() {
        std::string numberString;
        int dots = 0; // floating or integer
        auto posStart = this->pos->Copy();


        while (this->currentChar != ToWideString(EOF_) && (Lexer::Contains(DIGITS, ToByteString(this->currentChar)) || this->currentChar == L".")) {
            if (this->currentChar == L".") {
                if (dots == 1) {
                    break;
                }
                dots++;
                numberString += ".";
            } else {
                numberString += ToByteString(this->currentChar);
            }
            this->Advance();
        }

        if (dots == 0) {
            auto v = new long;
            *v = Lexer::ParseInt(numberString);
            this->tokenValueRefs.push_back(v);
            return Token(TokenType::Int, (void *) v, posStart, this->pos);
        } else {
            auto v = new double;
            *v = Lexer::ParseFloat(numberString);
            this->tokenValueRefs.push_back(v);
            return Token(TokenType::Float, (void *) v, posStart, this->pos);
        }
    }

    Token MakeIdentifier() {
        std::wstring identifier;
        auto posStart = this->pos->Copy();

        // while (this->currentChar != EOFW_ && ((Lexer::Contains(LETTERS_WITH_DIGITS, ToByteString(this->currentChar))) || this->currentChar == L"_")) {
        while (this->currentChar != EOFW_ && this->currentChar != L"\n" && !Lexer::Contains(Lexer::OP, ToByteString(this->currentChar)) && !Lexer::Contains(SPACES, ToByteString(this->currentChar))) {
            identifier += this->currentChar;
            this->Advance();
        }

        auto bs = ToByteString(identifier);
        auto isKeyword = Lexer::Contains(KEYWORDS, bs);
        auto identifierLabel = new std::string(bs);
        return Token(isKeyword ? TokenType::Keyword : TokenType::Identifier, (void *) identifierLabel, posStart, this->pos);
    }

    std::pair<Token, Error *> MakeNeqToken() {
        auto posStart = this->pos->Copy();
        this->Advance();
        if (this->currentChar == L"=") {
            this->Advance();
            return std::make_pair(Token(TokenType::OP_Nequal, nullptr, posStart, this->pos), nullptr);
        }
        this->Advance();
        return std::make_pair(Token(TokenType::Invilid, nullptr, nullptr, nullptr), new SyntaxError("Expected '=' (after '!' as '!=')", posStart, this->pos));
    }

    Token MakeEquals() {
        auto posStart = this->pos->Copy();
        TokenType tt = TokenType::OP_Eq;
        this->Advance();
        if (this->currentChar == L"=") {
            this->Advance();
            tt = TokenType::OP_Equal;
        }
        return Token(tt, nullptr, posStart, this->pos);
    }

    Token MakeLtToken() {
        auto posStart = this->pos->Copy();
        TokenType tt = TokenType::OP_Lt;
        this->Advance();
        if (this->currentChar == L"=") {
            this->Advance();
            tt = TokenType::OP_Lte;
        }
        return Token(tt, nullptr, posStart, this->pos);
    }

    Token MakeGtToken() {
        auto posStart = this->pos->Copy();
        TokenType tt = TokenType::OP_Gt;
        this->Advance();
        if (this->currentChar == L"=") {
            this->Advance();
            tt = TokenType::OP_Gte;
        }
        return Token(tt, nullptr, posStart, this->pos);
    }

    Token MakeMinusOrArrowToken() {
        auto ttype = TokenType::OP_Minus;
        auto posStart = this->pos->Copy();
        this->Advance();

        if (this->currentChar == L">") {
            this->Advance();
            ttype = TokenType::Arrow;
        }
        return Token(ttype, nullptr, posStart, this->pos);
    }

    Token MakeString() {
        std::string content;
        auto posStart = this->pos->Copy();
        bool escaped = false;
        this->Advance();
        /**/
        if (this->currentChar == EOFW_) {
              return Token(TokenType::Invilid);
        }

        while (this->currentChar != EOFW_ && (this->currentChar != L"'" || escaped)) {
            auto wchar = ToByteString(this->currentChar);
            if (escaped) {
                if (ESCAPED_CHARACTERS.find(wchar) != ESCAPED_CHARACTERS.end()) {
                    content += ESCAPED_CHARACTERS.at(wchar);
                } else {
                    content += wchar;
                }
            } else {
                if (this->currentChar == L"\\") {
                    escaped = true;
                    this->Advance();
                    continue;
                } else {
                    content += wchar;
                }
            }
            this->Advance();
            escaped = false;
        }

        if (escaped && content.size() == 0 || this->currentChar == EOFW_) {
            return Token(TokenType::Invilid);
        }
        this->Advance();
        auto str = new std::string(content);
        return Token(TokenType::String, (void *) str, posStart, this->pos);
    }

    const std::vector<std::string> OP = {
        " ", "%", "&", "|", "\\", "+", "-", "*", "/", "^", ":", "[", "]", "{", "}", "(", ")", "'", "=", ">", "<", "!", ",", ";", ".", "?", "#", "@", "\"", "~", "`"
    };

    std::pair<Tokens, Error*> MakeTokens() {
        Tokens tokens;
        while (this->currentChar != ToWideString(EOF_)) {
            if (Lexer::Contains(SPACES, ToByteString(this->currentChar))) {
                this->Advance();
            } else if (Lexer::Contains(STATEMENT_SEPERATORS, ToByteString(this->currentChar))) {
                tokens.push_back(Token(TokenType::NewLine, nullptr, this->pos));
                this->Advance();
            } else if (Lexer::Contains(DIGITS, ToByteString(this->currentChar))) {
                tokens.push_back(this->MakeNumber());
            // } else if (Lexer::Contains(LETTERS, ToByteString(this->currentChar)) || this->currentChar == L"_") {
            } else if (!Lexer::Contains(DIGITS, ToByteString(this->currentChar)) && !Lexer::Contains(Lexer::OP, ToByteString(this->currentChar))) {
                tokens.push_back(this->MakeIdentifier());
            } else if (this->currentChar == L"+") {
                tokens.push_back(Token(TokenType::OP_Plus, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L"-") {
                tokens.push_back(this->MakeMinusOrArrowToken());
            } else if (this->currentChar == L"*") {
                tokens.push_back(Token(TokenType::OP_Mul, nullptr, this->pos));
                this->Advance();            
            } else if (this->currentChar == L"/") {
                tokens.push_back(Token(TokenType::OP_Div, nullptr, this->pos));
                this->Advance();        
            } else if (this->currentChar == L"(") {
                tokens.push_back(Token(TokenType::Lparen, nullptr, this->pos));
                this->Advance();  
            } else if (this->currentChar == L")") {
                tokens.push_back(Token(TokenType::Rparen, nullptr, this->pos));
                this->Advance();  
            } else if (this->currentChar == L"[") {
                tokens.push_back(Token(TokenType::LSquare, nullptr, this->pos));
                this->Advance();  
            }  else if (this->currentChar == L"]") {
                tokens.push_back(Token(TokenType::RSquare, nullptr, this->pos));
                this->Advance();  
            } else if (this->currentChar == L"'") {
                auto errorPos = this->pos->Copy();
                auto tk = this->MakeString();
                if (tk.type == TokenType::Invilid) {
                    Tokens emptyTokens;
                    return std::make_pair(emptyTokens, new SyntaxError("Mismatched `'` in string literal", errorPos, this->pos));
                }
                tokens.push_back(tk);
            } else if (this->currentChar == L"=") {
                // tokens.push_back(Token(TokenType::OP_Eq, nullptr, this->pos));
                // this->Advance();
                tokens.push_back(this->MakeEquals());
            } else if (this->currentChar == L"<") {
                tokens.push_back(this->MakeLtToken());
            } else if (this->currentChar == L">") {
                tokens.push_back(this->MakeGtToken());
            } else if (this->currentChar == L"^") {
                tokens.push_back(Token(TokenType::OP_Pow, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L"!") {
                auto [token, error] = this->MakeNeqToken();
                if (error != nullptr) {
                    return std::make_pair(std::vector<Token>(), error);
                }
                tokens.push_back(token);
            } else if (this->currentChar == L",") {
                tokens.push_back(Token(TokenType::Comma, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L".") {
                tokens.push_back(Token(TokenType::Dot, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L":") {
                tokens.push_back(Token(TokenType::Colon, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L"{") {
                tokens.push_back(Token(TokenType::LStart, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L"}") {
                tokens.push_back(Token(TokenType::RStart, nullptr, this->pos));
                this->Advance();
            } else if (this->currentChar == L"\\") {
                if (Lexer::CharAt(this->text, this->pos->index + 1) != L"\n") {
                    return std::make_pair(Tokens(), new IllegalCharacterError(
                        "Trailing characters after line continuation character",
                        this->pos, this->pos
                    ));
                }
                this->Advance();
                this->Advance();
            } else {
                auto errorChar = this->currentChar;
                auto errorPos = this->pos->Copy();
                this->Advance();
                Tokens emptyTokens;
                return std::make_pair(emptyTokens, new IllegalCharacterError(std::format("Found unexpected \'{}\'", ToByteString(errorChar)), errorPos, this->pos));
            }
        }

        tokens.push_back(Token(TokenType::__EOF__, nullptr, this->pos));
        return std::make_pair(tokens, nullptr);
    }

    ~Lexer() {
        // for (void *p : this->tokenValueRefs) {
        //     delete p;
        //     // free(p);
        // }
    }

private:
    std::wstring text;
    std::wstring currentChar;
    std::string filename;
    Position *pos;
    std::vector<void *> tokenValueRefs;
};


enum class NodeType {
    Number,
    Expression,
    SingleExpression,
    VarAccess,
    VarAssign,
    IfExpression,
    ForExpression,
    WhileExpression,
    FunctionDefinition,
    FunctionCall,
    String,
    List,
    Return,
    Break,
    Continue,
    Subscription,
    Dictionary,
    Attribution,
    AdvancedVarAccess,
    NewExpression,
    AttributionCall,
    SubscriptionCall,
    NonlocalStatement,
    Defer,
    Invilid
};

const std::vector<std::string> NODE_TYPE_TAGS { 
    "Number", "Expression", "SingleExpression", 
    "VarAccess", "VarAssign", "IfExpression", "ForExpression", "WhileExpression",
    "FunctionDefinition", "FunctionCall",
    "String", "List",
    "Return", "Break", "Continue",
    "Subscription",
    "Dictionary", "Attribution", "AdvancedVarAccess", 
    "NewExpression",
    "AttributionCall", "SubscriptionCall",
    "NonlocalStatement",
    "Invilid" 
};


struct NodeBase {
    NodeType nodeType;
    NodeBase *left, *right;
    Position *st, *et;

    explicit NodeBase(NodeType tp, NodeBase *l, NodeBase *r) :
        nodeType(tp), left(l), right(r), st(nullptr), et(nullptr) {}
    explicit NodeBase(NodeType tp) : 
        NodeBase(tp, nullptr, nullptr) {}    
    explicit NodeBase() : 
        NodeBase(NodeType::Invilid) {}
    
    virtual ~NodeBase() = default;

    virtual inline std::string ToString() {
        return std::format("<Node {} at {}>", NODE_TYPE_TAGS[static_cast<int>(this->nodeType)], static_cast<void *>(this));
    }

};

struct NumberNode : public NodeBase {
    Token numberToken;
    explicit NumberNode(Token nt) : NodeBase(NodeType::Number), numberToken(nt) {
        this->st = this->numberToken.st;
        this->et = this->numberToken.et;
    }

    inline std::string ToString() override {
        return std::format("Number({})", this->numberToken.ToString());
    }
};

std::pair<Position *, Position *> GetPosition(NodeBase *node);
struct BinaryOperationNode : public NodeBase {
    Token binaryOperator;
    
    explicit BinaryOperationNode(NodeBase *lt, Token op, NodeBase *rt) : 
        NodeBase(NodeType::Expression, lt, rt), binaryOperator(op) {
            this->st = GetPosition(lt).first;
            this->et = GetPosition(rt).second;
        }
    explicit BinaryOperationNode() = default;


    inline std::string ToString() override {
        auto ls = this->left == nullptr ? "null" : this->left->ToString();
        auto rs = this->right == nullptr ? "null" : this->right->ToString();
        return std::format("BinOp({}, {}, {})", ls, this->binaryOperator.ToString(), rs);
    } 

    inline std::string Representation() {
        return "";
    }

    ~BinaryOperationNode() = default;
};


struct UnaryOperationNode : public NodeBase {
    Token unaryOperator;
    NodeBase *node;
    explicit UnaryOperationNode(Token op, NodeBase *node) : unaryOperator(op), node(node), NodeBase(NodeType::SingleExpression) {
        assert(node != nullptr);
        this->st = this->unaryOperator.st;
        this->et = GetPosition(node).second;
    }

    inline std::string ToString() override {
        return std::format("UnaOp({}, {})", this->unaryOperator.ToString(), this->node->ToString());
    }

    inline std::string Representation() {
        return "<UnaryOperation>";
    }

    ~UnaryOperationNode() = default;
};


struct VariableAssignNode : public NodeBase {
    NodeBase *valueNode;
    Token variableNameToken;
    explicit VariableAssignNode(Token var, NodeBase *value) : variableNameToken(var), valueNode(value), NodeBase(NodeType::VarAssign) {
        this->st = this->variableNameToken.st;
        this->et = this->variableNameToken.et;
    }
};

struct VariableAccessNode : public NodeBase {
    Token variableNameToken;
    explicit VariableAccessNode(Token var) : variableNameToken(var), NodeBase(NodeType::VarAccess) {
        this->st = this->variableNameToken.st;
        this->et = this->variableNameToken.et;
    }
};

struct ListNode : public NodeBase {
    bool isStatements;
    
    std::vector<NodeBase *> elements;
    NodeBase *subscripting;
    NodeBase *newVal;
    explicit ListNode(const std::vector<NodeBase *> &elements, Position *st, Position *et) : elements(elements), NodeBase(NodeType::List) {
        this->st = st;
        this->et = et;
        this->subscripting = nullptr;
        this->newVal = nullptr;
    }

    explicit ListNode(const std::vector<NodeBase *> &elements, Position *st, Position *et, NodeBase *sub, NodeBase *newVal = nullptr) :
        elements(elements), NodeBase(NodeType::List) {
            this->st = st;
            this->et = et;
            this->subscripting = sub;
            this->newVal = newVal;
        }
};

struct ParseResult final {
    Error *err;
    NodeBase *ast;
    size_t advancedCount;
    unsigned reverseCount;

    explicit ParseResult() : err(nullptr), ast(nullptr), advancedCount(0), reverseCount(0) {}
    NodeBase *Register(ParseResult *pr) {
        this->advancedCount += pr->advancedCount;
        if (pr->err != nullptr) {
            this->err = pr->err;
        }
        return pr->ast;
    }

    void RegisterAdvance() {
        this->advancedCount++;
    }

    NodeBase *Register(NodeBase *node) {
        this->RegisterAdvance();
        return node;
    }

    Token Register(Token tk) {
        this->RegisterAdvance();
        return tk;
    }

    NodeBase *TryRegister(ParseResult *res) {
        if (res->err != nullptr) {
            this->reverseCount = res->reverseCount;
            this->advancedCount += res->advancedCount;
            return nullptr;
        }
        return this->Register(res);
    }

    ParseResult *Success(NodeBase *node) {
        this->ast = node;
        return this;
    }

    ParseResult *Failure(Error *err) {
        if (this->err == nullptr || this->advancedCount == 0) {
            this->err = err;
        }
        return this;
    }
};


struct IfExpressionNode final : public NodeBase {
    std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>> cases;
    std::pair<NodeBase *, bool> elseCase;
    Position *st, *et;

    explicit IfExpressionNode(std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>> &&cases, std::pair<NodeBase *, bool> elseCase)
        : cases(cases), elseCase(elseCase), NodeBase(NodeType::IfExpression)
    {
        this->st = this->cases[0].first.first->st;
        if (this->elseCase.first != nullptr) {
            this->et = this->elseCase.first->et;
        } else {
            this->et = this->cases[this->cases.size() - 1].first.first->et;
        }
    }
};

struct WhileExpressionNode final : public NodeBase {
    NodeBase *conditionNode, *body;
    bool shouldReturnNull;
    explicit WhileExpressionNode(NodeBase *cond, NodeBase *body, bool shouldReturnNull) 
        : conditionNode(cond), body(body),shouldReturnNull(shouldReturnNull), NodeBase(NodeType::WhileExpression)
    {
        this->st = this->conditionNode->st;
        this->et = this->body->et;
    }
};

struct ForExpressionNode final : public NodeBase {
    bool rangeBasedLoop;
    NodeBase *stvNode, *etvNode, *stepvNode, *body;
    NodeBase *range;
    Token var;
    bool shouldReturnNull;   
    explicit ForExpressionNode(Token var, NodeBase *stvNode, NodeBase *etvNode, NodeBase *stepvNode, NodeBase *body, bool shouldReturnNull) 
        : var(var), stvNode(stvNode), etvNode(etvNode), stepvNode(stepvNode), body(body), shouldReturnNull(shouldReturnNull), NodeBase(NodeType::ForExpression)
    {
        this->st = this->var.st;
        this->et = this->body->et;
        this->rangeBasedLoop = false;
    }

    explicit ForExpressionNode(Token var, NodeBase *range, NodeBase *body, bool shouldReturnNull)
        : var(var), range(range), body(body), shouldReturnNull(shouldReturnNull), NodeBase(NodeType::ForExpression)
    {
        this->st = this->var.st;
        this->et = this->body->et;
        this->rangeBasedLoop = true;
    }
};

struct NonlocalStatementNode : public NodeBase {
    Token freeVar;

    explicit NonlocalStatementNode(Token freeVar) : freeVar(freeVar), NodeBase(NodeType::NonlocalStatement) {
        this->st = freeVar.st;
        this->et = freeVar.et;
    }
};

struct FunctionDefinitionNode final : public NodeBase {
    Token fun;
    Tokens parameters;
    NodeBase *body;
    std::vector<NonlocalStatementNode *> freeVars;
    std::vector<NonlocalStatementNode *> cellVars;
    bool shouldAutoReturn;
    std::vector<NodeBase *> defers;

    explicit FunctionDefinitionNode(Token fun, Tokens parameters, NodeBase *body, bool shouldAutoReturn) :
        fun(fun), parameters(parameters), body(body), shouldAutoReturn(shouldAutoReturn),  NodeBase(NodeType::FunctionDefinition)
    {
        if (this->fun.type != TokenType::Invilid) {
            this->st = this->fun.st;
        } else if (this->parameters.size() > 0) {
            this->st = this->parameters[0].st;
        } else {
            this->st = this->body->st;
        }
        this->et = this->body->et;
    }

    void SetClosure(const std::vector<NonlocalStatementNode *> &f) {
        this->freeVars = f;
    }

    void SetCell(const std::vector<NonlocalStatementNode *> &c) {
        this->cellVars = c;
    }

    explicit FunctionDefinitionNode(Tokens parameters, NodeBase *body) : FunctionDefinitionNode(Token(TokenType::Invilid), parameters, body, true)
    {}
};

struct FunctionCallNode final : public NodeBase {
    std::vector<NodeBase *> arguments;
    NodeBase *target;
    
    explicit FunctionCallNode(NodeBase *target, std::vector<NodeBase *> arguments) :
        arguments(arguments), target(target), NodeBase(NodeType::FunctionCall)
    {
        this->st = this->target->st;
        if (this->arguments.size() > 0) {
            this->et = this->arguments[this->arguments.size() - 1]->et;
        } else {
            this->et = this->target->et;
        }
    }
};


struct StringNode final : public NodeBase {
    Token stringToken;
    explicit StringNode(Token st) : NodeBase(NodeType::String), stringToken(st) {
        this->st = this->stringToken.st;
        this->et = this->stringToken.et;
    }
    
    inline std::string ToString() override {
        return std::format("String({})", this->stringToken.ToString());
    }
};

struct ReturnStatementNode final : public NodeBase {
    NodeBase *nodeToReturn;
    explicit ReturnStatementNode(NodeBase *nodeToReturn, Position *st, Position *et) :
        nodeToReturn(nodeToReturn), NodeBase(NodeType::Return) {
        this->st = st;
        this->et = et;
    }
};

struct BreakStatementNode final : public NodeBase {
    explicit BreakStatementNode(Position *st, Position *et) : NodeBase(NodeType::Break) {
        this->st = st;
        this->et = et;
    }
};

struct ContinueStatementNode final : public NodeBase {
    explicit ContinueStatementNode(Position *st, Position *et) : NodeBase(NodeType::Continue) {
        this->st = st;
        this->et = et;
    }
};

struct SubscriptionNode final : public NodeBase {
    NodeBase *index;
    std::vector<NodeBase *> subIndexes;
    std::map<int, std::vector<NodeBase *>> calls;
    NodeBase *assignment;
    NodeBase *target;
    explicit SubscriptionNode(NodeBase *t, NodeBase *idxExpr, NodeBase *assignment = nullptr) :
        target(t), index(idxExpr), assignment(assignment), NodeBase(NodeType::Subscription)
    {
        this->st = this->index->st;
        if (this->assignment != nullptr) {
            this->et = this->assignment->et;
        } else {
            this->et = this->index->et;
        }
    }

    void AddSubIndex(NodeBase *nd) {
        this->subIndexes.push_back(nd);
    }

    void SetCalls(const std::map<int, std::vector<NodeBase *>> &calls) {
        this->calls = calls;
    }
};


struct AttributionNode final : public NodeBase {
    Token attr;
    std::vector<Token> subAttrs;
    std::map<int, NodeBase *> calls;
    NodeBase *assignment;
    NodeBase *target;

    explicit AttributionNode(NodeBase *t, Token a, NodeBase *ass = nullptr) :
        target(t), attr(a), assignment(ass), NodeBase(NodeType::Attribution) 
    {
        this->st = this->attr.st;
        if (this->assignment != nullptr) {
            this->et = this->assignment->et;
        } else {
            this->et = this->attr.et;
        }
    }

    inline void SetCalls(const std::map<int, NodeBase *> &calls) {
        this->calls = calls;
    }

    inline void SetSubAttrs(const std::vector<Token> &attrs) {
        this->subAttrs = attrs;
    }
};

struct AttributionCallNode final : public NodeBase {
    FunctionCallNode *call;
    explicit AttributionCallNode(FunctionCallNode *call) : call(call), NodeBase(NodeType::AttributionCall) {
        this->st = this->call->st;
        this->et = this->call->et;
    }
};

struct SubscriptionCallNode final : public NodeBase {
    std::vector<NodeBase *> arguments;
    explicit SubscriptionCallNode(std::vector<NodeBase *> arguments, Position *st, Position *et) : arguments(arguments), NodeBase(NodeType::SubscriptionCall) {
        this->st = st;
        this->et = et;
    }
};

struct AdvancedVarAccessNode : public NodeBase {
    std::vector<NodeBase *> advancedAccess;

    explicit AdvancedVarAccessNode(const std::vector<NodeBase *> &aa) :
        advancedAccess(aa), NodeBase(NodeType::AdvancedVarAccess) 
    {
        this->st = this->advancedAccess[0]->st;
        this->et = this->advancedAccess[this->advancedAccess.size() - 1]->et;
    }
 };

struct DictionaryNode : public NodeBase {
    std::map<NodeBase *, NodeBase *> elements;
    
    explicit DictionaryNode(const std::map<NodeBase *, NodeBase *> &elements, Position *st, Position *et) 
        : NodeBase(NodeType::Dictionary), elements(elements)
    {
        this->st = st;
        this->et = et;
    }
};

struct NewExprNode : public NodeBase {
    NodeBase *newExpr;

    explicit NewExprNode(NodeBase *newE) : newExpr(newE), NodeBase(NodeType::NewExpression) {
        this->st = newE->st;
        this->et = newE->et;
    }
};

struct DeferNode : public NodeBase {
    NodeBase *deferExpr;

    explicit DeferNode(NodeBase *de, Position *st, Position *et) : NodeBase(NodeType::Defer) {
        this->deferExpr = de;
        this->st = st;
        this->et = et;
    }
};


std::pair<Position *, Position *> GetPosition(NodeBase *node) {
    if (node->nodeType == NodeType::Number) {
        return std::make_pair(dynamic_cast<NumberNode *>(node)->st, dynamic_cast<NumberNode *>(node)->et);
    } else if (node->nodeType == NodeType::Expression) {
        return std::make_pair(dynamic_cast<BinaryOperationNode *>(node)->st, dynamic_cast<BinaryOperationNode *>(node)->et);
    } else if (node->nodeType == NodeType::SingleExpression) {
        return std::make_pair(dynamic_cast<UnaryOperationNode *>(node)->st, dynamic_cast<UnaryOperationNode *>(node)->et);
    } else if (node->nodeType == NodeType::VarAccess) {
        return std::make_pair(dynamic_cast<VariableAccessNode *>(node)->st, dynamic_cast<VariableAccessNode *>(node)->et);
    } else if (node->nodeType == NodeType::VarAssign) {
        return std::make_pair(dynamic_cast<VariableAssignNode *>(node)->st, dynamic_cast<VariableAssignNode *>(node)->et);        
    } else if (node->nodeType == NodeType::ForExpression) {
        return std::make_pair(dynamic_cast<ForExpressionNode *>(node)->st, dynamic_cast<ForExpressionNode *>(node)->et);        
    } else if (node->nodeType == NodeType::WhileExpression) {
        return std::make_pair(dynamic_cast<WhileExpressionNode *>(node)->st, dynamic_cast<WhileExpressionNode *>(node)->et);        
    } else if (node->nodeType == NodeType::IfExpression) {
        return std::make_pair(dynamic_cast<IfExpressionNode *>(node)->st, dynamic_cast<IfExpressionNode *>(node)->et);        
    } else if (node->nodeType == NodeType::FunctionCall) {
        return std::make_pair(dynamic_cast<FunctionCallNode *>(node)->st, dynamic_cast<FunctionCallNode *>(node)->et);        
    } else if (node->nodeType == NodeType::FunctionDefinition) {
        return std::make_pair(dynamic_cast<FunctionDefinitionNode *>(node)->st, dynamic_cast<FunctionDefinitionNode *>(node)->et);        
    } else if (node->nodeType == NodeType::String) {
        return std::make_pair(dynamic_cast<StringNode *>(node)->st, dynamic_cast<StringNode *>(node)->et);        
    } else if (node->nodeType == NodeType::Return) {
        return std::make_pair(dynamic_cast<ReturnStatementNode *>(node)->st, dynamic_cast<ReturnStatementNode *>(node)->et);                
    } else if (node->nodeType == NodeType::Break) {
        return std::make_pair(dynamic_cast<BreakStatementNode *>(node)->st, dynamic_cast<BreakStatementNode *>(node)->et);                        
    } else if (node->nodeType == NodeType::Continue) {
        return std::make_pair(dynamic_cast<ContinueStatementNode *>(node)->st, dynamic_cast<ContinueStatementNode *>(node)->et);                                
    } else if (node->nodeType == NodeType::Subscription) {
        return std::make_pair(dynamic_cast<SubscriptionNode *>(node)->st, dynamic_cast<SubscriptionNode *>(node)->et);                                
    } else if (node->nodeType == NodeType::Dictionary) {
        return std::make_pair(dynamic_cast<DictionaryNode *>(node)->st, dynamic_cast<DictionaryNode *>(node)->et);                                        
    } else if (node->nodeType == NodeType::Attribution) {
        return std::make_pair(dynamic_cast<AttributionNode *>(node)->st, dynamic_cast<AttributionNode *>(node)->et);
    } else if (node->nodeType == NodeType::AdvancedVarAccess) {
        return std::make_pair(dynamic_cast<AdvancedVarAccessNode *>(node)->st, dynamic_cast<AdvancedVarAccessNode *>(node)->et);
    } else if (node->nodeType == NodeType::NewExpression) {
        return std::make_pair(dynamic_cast<NewExprNode *>(node)->st, dynamic_cast<NewExprNode *>(node)->et);
    }
    return std::make_pair(nullptr, nullptr);
}


class Parser final {
public:
    explicit Parser(Tokens tks) : tokenIndex(-1) {        
        this->tokens = tks;
        this->Advance();
        this->functionLayers = 0;
    }

    Token Reverse(unsigned amount = 1) {
        this->tokenIndex -= amount;
        this->UpdateCurrentToken();
        return this->currentToken;
    }

    Token Advance() {
        this->tokenIndex++;
        this->UpdateCurrentToken();
        return this->currentToken;
    }

    void UpdateCurrentToken() {
        if (this->tokenIndex < (int) this->tokens.size()) {
            this->currentToken = this->tokens[this->tokenIndex];
        }
    }

    ParseResult *Statement() {
        auto result = new ParseResult;
        auto posStart = this->currentToken.st->Copy();

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "return")) {
            if (this->functionLayers == 0) {
                return result->Failure(new SyntaxError(
                    "'return' outside a user-defined function",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();
            auto returnExpression = result->TryRegister(this->Expr());
            if (returnExpression == nullptr) {
                this->Reverse(result->reverseCount);
            }
            return result->Success(new ReturnStatementNode(returnExpression, posStart, this->currentToken.et->Copy()));
        }

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "nonlocal")) {
            if (this->functionLayers < 2) {
                return result->Failure(new SyntaxError(
                    "'nonlocal' outside a closure function",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            this->clousureLayer = this->functionLayers;
            result->RegisterAdvance();
            this->Advance();
            if (this->currentToken.type != TokenType::Identifier) {
                return result->Failure(new SyntaxError(
                    "Nonlocal name must be a identifier",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            auto nd = new NonlocalStatementNode(this->currentToken);
            this->closureNodes[this->functionLayers].push_back(nd);
            result->RegisterAdvance();
            this->Advance();
            return result->Success(nd);
        } else if (this->currentToken.Matches<std::string>(TokenType::Keyword, "defer")) {
            if (this->functionLayers == 0) {
                return result->Failure(new SyntaxError(
                    "'defer' outside a user-defined function",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();

            auto deferExpr = result->Register(this->Expr());
            return result->Success(new DeferNode(deferExpr, posStart, this->currentToken.st));
        }

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "continue")) {
            result->RegisterAdvance();
            this->Advance();
            return result->Success(new ContinueStatementNode(posStart, this->currentToken.et->Copy()));
        }
    
        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "break")) {
            result->RegisterAdvance();
            this->Advance();
            return result->Success(new BreakStatementNode(posStart, this->currentToken.et->Copy()));
        }

        auto expr = result->Register(this->Expr());
        if (result->err != nullptr) {
            // return result->Failure(new SyntaxError(
            //     "Invilid statement", this->currentToken.st, this->currentToken.et
            // ));
            return result;
        }
        return result->Success(expr);
    }

    ParseResult *Statements() {
        auto result = new ParseResult;
        std::vector<NodeBase *> statements;
        auto posStart = this->currentToken.st->Copy();

        while (this->currentToken.type == TokenType::NewLine) {
            result->RegisterAdvance();
            this->Advance();
        }

        auto statement = result->Register(this->Statement());
        if (result->err != nullptr) {
            return result;
        }
        statements.push_back(statement);
        unsigned newLineCount = 0;
        bool multiStatements = true;

        while (true) {
            newLineCount = 0;
            while (this->currentToken.type == TokenType::NewLine) {
                result->RegisterAdvance();
                this->Advance();
                newLineCount++;
            }
            if (newLineCount == 0) {
                multiStatements = false;
            }
            if (!multiStatements) {
                break;
            }

            auto statementNode = result->TryRegister(this->Statement());
            if (statementNode == nullptr) {
                this->Reverse(result->reverseCount);
                multiStatements = false;
                continue;
            }
            statements.push_back(statementNode);        
        }

        auto n = new ListNode(statements, posStart, this->currentToken.et->Copy());
        n->isStatements = true;
        return result->Success(n);
    }

    ParseResult *Factor() {
        auto result = new ParseResult;
        auto token = this->currentToken;

        if (Lexer::Contains(FIRST_LEVEL_OPERANDS, token.type)) {
            result->Register(this->Advance());
            auto fac = result->Register(this->Factor());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(new UnaryOperationNode(token, fac));
        }

        return this->Power();
    }

    ParseResult *Power() {
        return this->BinaryOperation(&Parser::FunctionCall, THIRD_LEVEL_OPERAND, &Parser::Factor);
    }

    ParseResult *FunctionCall() {
        auto result = new ParseResult;
        auto atomNode = result->Register(this->Atom());
        if (result->err != nullptr) {
            return result;
        }
        std::vector<NodeBase *> args;
        if (this->currentToken.type == TokenType::Lparen) {
            result->RegisterAdvance();
            this->Advance();

            if (this->currentToken.type == TokenType::Rparen) {
                result->RegisterAdvance();
                this->Advance();
            } else {
                args.push_back(result->Register(this->Expr()));
                if (result->err != nullptr) {
                    // return result->Failure(new SyntaxError(
                    //     "Invilid function call",
                    //     this->currentToken.st, this->currentToken.et
                    // ));
                    return result;
                }

                while (this->currentToken.type == TokenType::Comma) {
                    result->RegisterAdvance();
                    this->Advance();
                    args.push_back(result->Register(this->Expr()));
                    if (result->err != nullptr) {
                        return result;
                    }
                }

                if (this->currentToken.type != TokenType::Rparen) {
                    return result->Failure(new SyntaxError(
                        "Mismatched '(' while calling function (unfinished)",
                        this->currentToken.st, this->currentToken.et
                    ));
                }

                result->RegisterAdvance();
                this->Advance();
            }

            auto nd = new FunctionCallNode(atomNode, args);
            NodeBase *_node;
            if (this->currentToken.type == TokenType::Dot) {
                _node = result->Register(this->Attribution(nd));
                if (result->err != nullptr) {
                    return result;
                }
            } else {
                _node = nd;
            }
            return result->Success(_node);
        }
        return result->Success(atomNode);
    }

    // TODO: Attribute access after function call is invaild? f().a -> error [Temp solved]

    ParseResult *Term() {
        return this->BinaryOperation(&Parser::Factor, SECOND_LEVEL_OPERANDS);
    }

    ParseResult *ListSetItem() {
        auto result = new ParseResult;
        if (this->currentToken.type != TokenType::OP_Eq) {
            return result->Failure(new SyntaxError(
                "Expected '=' in list-setitem", 
                this->currentToken.st, this->currentToken.et
            ));
        }
         
        result->RegisterAdvance();
        this->Advance();
        auto valueExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(valueExpr);
    }

    std::pair<ParseResult *, NodeBase *> ListSubscription() {
        auto result = new ParseResult;
        if (this->currentToken.type != TokenType::LSquare) {
            return std::make_pair(result->Failure(new SyntaxError(
                "Expected '['", this->currentToken.st, this->currentToken.et
            )), nullptr);
        }

        result->RegisterAdvance();
        this->Advance();
        auto subscriptionExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return std::make_pair(result, nullptr);
        }
    
        if (this->currentToken.type != TokenType::RSquare) {
            return std::make_pair(result->Failure(new SyntaxError(
                "'[' mismatched in list-subsciption",
                this->currentToken.st, this->currentToken.et
            )), nullptr);
        }
        result->RegisterAdvance();
        this->Advance();

        if (this->currentToken.type == TokenType::OP_Eq) {
            auto valueExpr = result->Register(this->ListSetItem());
            if (result->err != nullptr) {
                return std::make_pair(result, nullptr);
            }
            return std::make_pair(result->Success(subscriptionExpr), valueExpr);
        } else {
            return std::make_pair(result->Success(subscriptionExpr), nullptr);
        }
    }

    ParseResult *List() {
        auto result = new ParseResult;
        std::vector<NodeBase *> elementNodes;
        auto posStart = this->currentToken.st->Copy();
        if (this->currentToken.type != TokenType::LSquare) {
            return result->Failure(new SyntaxError(
                "Expected '[' when declaring a list",
                posStart, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();

        if (this->currentToken.type == TokenType::RSquare) {
            result->RegisterAdvance();
            this->Advance();
        } else {
            elementNodes.push_back(result->Register(this->Expr()));
            if (result->err != nullptr) {
                return result->Failure(new SyntaxError(
                    "Invilid list definition",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            while (this->currentToken.type == TokenType::Comma) {
                result->RegisterAdvance();
                this->Advance();
                elementNodes.push_back(result->Register(this->Expr()));
                if (result->err != nullptr) {
                    return result;
                }
            }

            if (this->currentToken.type != TokenType::RSquare) {
                return result->Failure(new SyntaxError(
                    "Mismatched ']' while giving initalizations to a list (unfinished)",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            result->RegisterAdvance();
            this->Advance();
        }

        if (this->currentToken.type == TokenType::LSquare) {
            auto [subsciptionExprWrapper, newValue] = this->ListSubscription();
            if (subsciptionExprWrapper->err != nullptr) {
                return result->Failure(subsciptionExprWrapper->err);
            }
            auto n = new ListNode(elementNodes, posStart, this->currentToken.et->Copy(), subsciptionExprWrapper->ast, newValue);
            n->isStatements = false;
            return result->Success(n);
        } else {
            auto n = new ListNode(elementNodes, posStart, this->currentToken.et->Copy());
            n->isStatements = false;
            return result->Success(n);
        }
    }

    ParseResult *Dictionary() {
        auto result = new ParseResult;
        std::map<NodeBase *, NodeBase *> elementNodes;
        NodeBase *currentKey = nullptr, *currentValue = nullptr;
        auto posStart = this->currentToken.st->Copy();
        if (this->currentToken.type != TokenType::LStart) {
            return result->Failure(new SyntaxError(
                "Expected '{' when declaring a dictionary",
                posStart, this->currentToken.et
            ));
        }
        result->RegisterAdvance();
        this->Advance();
    
        if (this->currentToken.type == TokenType::RStart) {
            result->RegisterAdvance();
            this->Advance();
        } else {
            currentKey = result->Register(this->Expr());
            if (result->err != nullptr) {
                // return result->Failure(new SyntaxError(
                //     "Invilid dictionary definition",
                //     this->currentToken.st, this->currentToken.et
                // ));
                return result;
            }
            if (this->currentToken.type != TokenType::Colon) {
                return result->Failure(new SyntaxError(
                    "Expected ':' to seperate key and value",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();
            currentValue = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }
            elementNodes.insert(std::make_pair(currentKey, currentValue));

            while (this->currentToken.type == TokenType::Comma) {
                result->RegisterAdvance();
                this->Advance();
                currentKey = result->Register(this->Expr());
                if (result->err != nullptr) {
                    return result;
                }
                if (this->currentToken.type != TokenType::Colon) {
                    return result->Failure(new SyntaxError(
                        "Expected ':' to seperate key and value",
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                result->RegisterAdvance();
                this->Advance();
                currentValue = result->Register(this->Expr());
                if (result->err != nullptr) {
                    return result;
                }
                elementNodes.insert(std::make_pair(currentKey, currentValue));            
            }

            if (this->currentToken.type != TokenType::RStart) {
                return result->Failure(new SyntaxError(
                    "Mismatched '}' while initalizing a dictionary (unfinished)",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            result->RegisterAdvance();
            this->Advance();
        }            
        return result->Success(new DictionaryNode(elementNodes, posStart, this->currentToken.et->Copy()));
    }

    std::pair<std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>>, std::pair<ParseResult *, bool>> IfExprCases(const std::string &type) {
        auto result = new ParseResult;
        using CaseInfo = std::pair<std::pair<NodeBase *, NodeBase *>, bool>;
        std::vector<CaseInfo> cases;
        std::pair<ParseResult *, bool> elseCase;

        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, type)) {
            return std::make_pair(decltype(cases)(), std::make_pair(result->Failure(new SyntaxError(
                std::format("Expected '{}'", type), this->currentToken.st, this->currentToken.et
            )), false));
        }
        result->RegisterAdvance();
        this->Advance();

        auto condition = result->Register(this->Expr());
        if (result->err != nullptr) {
            return std::make_pair(decltype(cases)(), std::make_pair(result, false));
        }

        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "then")) {
            return std::make_pair(decltype(cases)(), std::make_pair(result->Failure(new SyntaxError(
                "Expected 'then' in if-statement",
                this->currentToken.st, this->currentToken.et
            )), false));
        }
        result->RegisterAdvance();
        this->Advance();

        if (this->currentToken.type == TokenType::NewLine) {
            result->RegisterAdvance();
            auto statements = result->Register(this->Statements());
            if (result->err != nullptr) {
                return std::make_pair(decltype(cases)(), std::make_pair(result, false));
            }

            cases.push_back(std::make_pair(std::make_pair(condition, statements), true));

            if (this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
                result->RegisterAdvance();
                this->Advance();
            } else {
                auto allCases = this->IfExprBOrC();
                if (result->err != nullptr) {
                    return std::make_pair(decltype(cases)(), std::make_pair(result, false));
                }
                auto newCases = allCases.first;
                elseCase = allCases.second;
                for (auto &&otherCases : newCases) {
                    cases.push_back(otherCases);
                }
            }
        } else {
            auto statement = result->Register(this->Statement());
            if (result->err != nullptr) {
                return std::make_pair(decltype(cases)(), std::make_pair(result, false));
            }
            cases.push_back(std::make_pair(std::make_pair(condition, statement), false));

            auto allCases = this->IfExprBOrC();
            if (allCases.second.first != nullptr && allCases.second.first->err != nullptr) {
                return std::make_pair(decltype(cases)(), std::make_pair(result->Failure(allCases.second.first->err), false));
            }
            auto newCases = allCases.first;
            elseCase = allCases.second;
            for (auto &&otherCases : newCases) {
                cases.push_back(otherCases);
            }
        }

        return std::make_pair(cases, elseCase);
    } 

    std::pair<std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>>, std::pair<ParseResult *, bool>> IfExprB() {
        return this->IfExprCases("elif"); 
    }

    std::pair<ParseResult *, bool> *IfExprC() {
        auto result = new ParseResult;
        std::pair<ParseResult *, bool> *elseCase = nullptr;

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "else")) {
            result->RegisterAdvance();
            this->Advance();

            if (this->currentToken.type == TokenType::NewLine) {
                result->RegisterAdvance();
                this->Advance();

                elseCase = new std::pair<ParseResult *, bool>;                
                auto statements = result->Register(this->Statements());
                if (result->err != nullptr) {
                    *elseCase = std::make_pair(nullptr, false);
                    return elseCase;
                }
                *elseCase = std::make_pair(result->Success(statements), true);

                if (this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
                    result->RegisterAdvance();
                    this->Advance();
                } else {
                    elseCase = new std::pair<ParseResult *, bool>;               
                    *elseCase = std::make_pair(result->Failure(new SyntaxError(
                        "Expected 'end' after multi-line statement",
                        this->currentToken.st, this->currentToken.et
                    )), false);
                    return elseCase;
                }
            } else {
                auto statement = result->Register(this->Statement());
                if (result->err != nullptr) {
                    elseCase = new std::pair<ParseResult *, bool>;
                    *elseCase = std::make_pair(result, false);
                    return elseCase;
                }
                elseCase = new std::pair<ParseResult *, bool>;
                *elseCase = std::make_pair(result->Success(statement), false);
                return elseCase;
            }
        }
        return elseCase;
    }

    std::pair<std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>>, std::pair<ParseResult *, bool>> IfExprBOrC() {
        auto result = new ParseResult;
        std::vector<std::pair<std::pair<NodeBase *, NodeBase *>, bool>> cases;
        std::pair<ParseResult *, bool> elseCase;

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "elif")) {
            auto allCases = this->IfExprB();
            if (allCases.second.first) {
                if (allCases.second.first->err != nullptr) {
                    return std::make_pair(decltype(cases)(), std::make_pair(result->Failure(allCases.second.first->err), false));
                }
            }
            cases = allCases.first;
            elseCase = allCases.second;
        } else {
            auto elseCase_ = this->IfExprC();
            if (elseCase_ == nullptr) {
                 return std::make_pair(cases, std::make_pair(result->Success(nullptr), false));
            } else {
                elseCase = *elseCase_;
            }
            if (elseCase.first != nullptr && elseCase.first->err != nullptr) {
                return std::make_pair(decltype(cases)(), std::make_pair(result->Failure(elseCase.first->err), false));
            }
        }
        return std::make_pair(cases, elseCase);
    }

    ParseResult *IfExpr() {
        auto result = new ParseResult;
        auto ifExprSub = this->IfExprCases("if");
        if (ifExprSub.second.first != nullptr && ifExprSub.second.first->err != nullptr) {
            return result->Failure(ifExprSub.second.first->err);
        }
        // if (result->err != nullptr) {
        //     return result;
        // }
        auto [cases, elseCase] = ifExprSub;
        if (elseCase.first == nullptr) {
            return result->Success(new IfExpressionNode(std::move(cases), std::make_pair(nullptr, false)));
        }
        return result->Success(new IfExpressionNode(std::move(cases), std::make_pair(elseCase.first->ast, elseCase.second)));
    }

    ParseResult *SubsciptiveAssignment() {
        auto result = new ParseResult;
        auto valueExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(valueExpr);
    }

    ParseResult *Subscription(NodeBase *target) {
        auto result = new ParseResult;
        result->RegisterAdvance();
        this->Advance();
        
        auto indexExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }

        if (this->currentToken.type != TokenType::RSquare) {
            return result->Failure(new SyntaxError(
                "'[' mismatched",
                this->currentToken.st, this->currentToken.et
            ));
        }
        result->RegisterAdvance();
        this->Advance();
        std::map<int, std::vector<NodeBase *>> calls;
        if (currentToken.type == TokenType::Lparen) {
            result->RegisterAdvance();
            this->Advance();
            std::vector<NodeBase *> args;

            if (this->currentToken.type == TokenType::Rparen) {
                calls.insert(std::make_pair(0, args));
                result->RegisterAdvance();
                this->Advance();
            } else {
                auto arg = result->Register(this->Expr());
                if (result->err != nullptr) {
                    return result;
                }
                args.push_back(arg);

                while (this->currentToken.type == TokenType::Comma) {
                    result->RegisterAdvance();
                    this->Advance();
                    auto arg = result->Register(this->Expr());
                    if (result->err != nullptr) {
                        return result;
                    }
                    args.push_back(arg);
                }

                if (this->currentToken.type != TokenType::Rparen) {
                    return result->Failure(new SyntaxError(
                        "Mismatched ')' in function call",
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                calls.insert(std::make_pair(0, args));
                result->RegisterAdvance();
                this->Advance();
            }
        }

        std::vector<NodeBase *> subNodes;
        int index = 1;
        while (this->currentToken.type == TokenType::LSquare) {
            result->RegisterAdvance();
            this->Advance();
            auto subNode = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }
            subNodes.push_back(subNode);
            if (this->currentToken.type != TokenType::RSquare) {
                return result->Failure(new SyntaxError(
                    "'[' mismatched",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();

            if (currentToken.type == TokenType::Lparen) {
                result->RegisterAdvance();
                this->Advance();
                std::vector<NodeBase *> args;

                if (this->currentToken.type == TokenType::Rparen) {
                    calls.insert(std::make_pair(index, args));
                } else {
                    auto arg = result->Register(this->Expr());
                    if (result->err != nullptr) {
                        return result;
                    }
                    args.push_back(arg);

                    while (this->currentToken.type == TokenType::Comma) {
                        result->RegisterAdvance();
                        this->Advance();
                        auto arg = result->Register(this->Expr());
                        if (result->err != nullptr) {
                            return result;
                        }
                        args.push_back(arg);
                    }

                    if (this->currentToken.type != TokenType::Rparen) {
                        return result->Failure(new SyntaxError(
                            "Mismatched ')' in function call",
                            this->currentToken.st, this->currentToken.et
                        ));
                    }
                    calls.insert(std::make_pair(index, args));
                }
                result->RegisterAdvance();
                this->Advance();
            }
            index++;
        }

        // result->RegisterAdvance();
        // this->Advance();

        if (this->currentToken.type == TokenType::OP_Eq) {
            result->RegisterAdvance();
            this->Advance();
            auto valueExpr = result->Register(this->SubsciptiveAssignment());
            if (result->err != nullptr) {
                return result;
            }
            auto sn = new SubscriptionNode(target, indexExpr, valueExpr);
            for (auto node : subNodes) {
                sn->AddSubIndex(node);
            }
            sn->SetCalls(calls);
            return result->Success(sn);
        }
        auto sn = new SubscriptionNode(target, indexExpr);
        for (auto node : subNodes) {
            sn->AddSubIndex(node);
        }
        sn->SetCalls(calls);
        return result->Success(sn);
    }

    ParseResult *Attribution(NodeBase *varAccess) {
        auto result = new ParseResult;
        result->RegisterAdvance();
        this->Advance();
        if (this->currentToken.type != TokenType::Identifier) {
            return result->Failure(new SyntaxError(
                "Expected an identifier", 
                this->currentToken.st, this->currentToken.et
            ));
        }
        auto attr = this->currentToken;
        result->RegisterAdvance();
        this->Advance();
        std::vector<Token> attrs;
        std::map<int, NodeBase *> calls;

        int attributionIndex = 0;
        while (this->currentToken.type == TokenType::Dot || this->currentToken.type == TokenType::Lparen) {

            if (this->currentToken.type == TokenType::Lparen) {
                this->Reverse();
                auto token = this->currentToken;
                result->RegisterAdvance();
                this->Advance();
                result->RegisterAdvance();
                this->Advance();
                std::vector<NodeBase *> args;

                if (this->currentToken.type == TokenType::Rparen) {
                    result->RegisterAdvance();
                    this->Advance();
                } else {
                    args.push_back(result->Register(this->Expr()));
                    if (result->err != nullptr) {
                        // return result->Failure(new SyntaxError(
                        //     "Invilid function call",
                        //     this->currentToken.st, this->currentToken.et
                        // ));
                        return result;
                    }
    
                    while (this->currentToken.type == TokenType::Comma) {
                        result->RegisterAdvance();
                        this->Advance();
                        args.push_back(result->Register(this->Expr()));
                        if (result->err != nullptr) {
                            return result;
                        }
                    }
    
                    if (this->currentToken.type != TokenType::Rparen) {
                        return result->Failure(new SyntaxError(
                            "Mismatched '(' while calling function (unfinished)",
                            this->currentToken.st, this->currentToken.et
                        ));
                    }
    
                    result->RegisterAdvance();
                    this->Advance();
                }
                calls.insert(std::make_pair(
                    attributionIndex,
                    new AttributionCallNode(new FunctionCallNode(new VariableAccessNode(token), args))
                ));
            } else {
                result->RegisterAdvance();
                this->Advance();

                if (this->currentToken.type == TokenType::Identifier) {
                    attrs.push_back(this->currentToken);
                } else {
                    return result->Failure(new SyntaxError(
                        "Invilid attribute access", 
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                result->RegisterAdvance();
                this->Advance();
                attributionIndex++;            
            }
        }

        if (this->currentToken.type == TokenType::OP_Eq) {
            result->RegisterAdvance();
            this->Advance();
            auto assExpr = result->Register(this->AttributionAssignment());
            if (result->err != nullptr) {
                return result;
            } 
            auto an = new AttributionNode(varAccess, attr, assExpr);
            an->SetCalls(calls);
            if (attrs.size() > 0) {
                an->SetSubAttrs(attrs);
            }
            return result->Success(an);
        }
        auto an = new AttributionNode(varAccess, attr);
        an->SetCalls(calls);
        if (attrs.size() > 0) {
            an->SetSubAttrs(attrs);
        }
        return result->Success(an);
    }

    ParseResult *AttributionAssignment() {
        auto result = new ParseResult;
        auto valueExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(valueExpr);
    }

    ParseResult *Atom() {
        auto result = new ParseResult;
        auto token = this->currentToken;

        if (Lexer::Contains(NUMERIC_TOKEN_TYPES, token.type)) {
            result->Register(this->Advance());
            return result->Success(dynamic_cast<NodeBase *>(new NumberNode(token)));
        } else if (token.type == TokenType::String) {
            result->RegisterAdvance();
            this->Advance();
            return result->Success(new StringNode(token));
        } else if (token.type == TokenType::Identifier) {
            result->Register(this->Advance());
            std::vector<NodeBase *> varSubAccess;
            NodeBase *prev = new VariableAccessNode(token);
            while (this->currentToken.type == TokenType::LSquare || this->currentToken.type == TokenType::Dot) {
                if (this->currentToken.type == TokenType::LSquare) {
                    auto subNode = result->Register(this->Subscription(prev));
                    if (result->err != nullptr) {
                        return result;
                    } 
                    prev = subNode;
                    varSubAccess.push_back(subNode);
                } else if (this->currentToken.type == TokenType::Dot) {
                    auto subNode = result->Register(this->Attribution(prev));
                    if (result->err != nullptr) {
                        return result;
                    }
                    prev = subNode;
                    varSubAccess.push_back(subNode);
                }
            }
            if (varSubAccess.size() > 0) {
                return result->Success(new AdvancedVarAccessNode(varSubAccess));
            }
            
            return result->Success(new VariableAccessNode(token));
        } else if (token.type == TokenType::Lparen) {
            result->Register(this->Advance());
            auto expr = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }
            if (this->currentToken.type == TokenType::Rparen) {
                result->Register(this->Advance());
                return result->Success(expr);
            } else {
                return result->Failure(new SyntaxError(
                    "Expected ')'",
                    this->currentToken.st, this->currentToken.et
                ));
            }
        } else if (token.type == TokenType::LSquare) {
            auto list = result->Register(this->List());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(list);
        } else if (token.type == TokenType::LStart) {
            auto dict = result->Register(this->Dictionary());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(dict);
        } else if (token.Matches<std::string>(TokenType::Keyword, "if")) {
            auto ifExpression = result->Register(this->IfExpr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(ifExpression);
        } else if (token.Matches<std::string>(TokenType::Keyword, "for")) {
            auto forExpression = result->Register(this->ForExpr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(forExpression);
        } else if (token.Matches<std::string>(TokenType::Keyword, "while")) {
            auto whileExpression = result->Register(this->WhileExpr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(whileExpression);
        } else if (token.Matches<std::string>(TokenType::Keyword, "function")) {
            auto functionDefinition = result->Register(this->FunctionDefinition());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(functionDefinition);
        } else if (token.Matches<std::string>(TokenType::Keyword, "new")) {
            auto newExpr = result->Register(this->NewExpr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(newExpr);
        } else if (token.type == TokenType::__EOF__ && this->RealSize() == 1) {
            return result->Success(new ListNode({}, this->currentToken.st, this->currentToken.et));
        }

        return result->Failure(new SyntaxError(
            "Invilid syntax", token.st, token.et
        ));
    }

    inline unsigned RealSize() {
        unsigned size = 0;
        for (auto &tk : this->tokens) {
            size += (int) (tk.type != TokenType::NewLine);
        }
        return size;
    }

    ParseResult *NewExpr() {
        auto result = new ParseResult;
        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "new")) {
            return result->Failure(new SyntaxError("Expected 'new'", this->currentToken.st, this->currentToken.et));
        }
        result->RegisterAdvance();
        this->Advance();

        auto newExpr = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(new NewExprNode(newExpr));
    }

    ParseResult *ForExpr() {
        auto result = new ParseResult;
        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "for")) {
            return result->Failure(new SyntaxError(
                "Expected 'for'",
                this->currentToken.st, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();

        if (this->currentToken.type != TokenType::Identifier) {
            return result->Failure(new SyntaxError(
                "Expected an identifier in for-statement",
                this->currentToken.st, this->currentToken.et
            ));
        }

        auto loopVariable = this->currentToken;
        result->RegisterAdvance();
        this->Advance();

        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "in")) {
            result->RegisterAdvance();
            this->Advance();

            auto range = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }

            if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "then")) {
                return result->Failure(new SyntaxError(
                    "Expected 'then' after identifier in range-based for-loop",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();

            if (this->currentToken.type == TokenType::NewLine) {
                result->RegisterAdvance();
                this->Advance();

                auto body = result->Register(this->Statements());
                if (result->err != nullptr) {
                    return result;
                }

                if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
                    return result->Failure(new SyntaxError(
                        "Expected 'end' after multi-line statement",
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                result->RegisterAdvance();
                this->Advance();

                return result->Success(new ForExpressionNode(
                    loopVariable, range, body, true
                ));
            }

            auto body = result->Register(this->Statement());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(new ForExpressionNode(
                loopVariable, range, body, false
            ));

        } else if (this->currentToken.type == TokenType::OP_Eq) {
            result->RegisterAdvance();
            this->Advance();

            auto startValue = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }

            if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "to")) {
                return result->Failure(new SyntaxError(
                    "Expected 'to' after the start of the initial expression in for-statement",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            result->RegisterAdvance();
            this->Advance();
            auto endValue = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }

            NodeBase *stepValue = nullptr;
            if (this->currentToken.Matches<std::string>(TokenType::Keyword, "step")) {
                result->RegisterAdvance();
                this->Advance();

                stepValue = result->Register(this->Expr());
                if (result->err != nullptr) {
                    return result;
                }
            }

            if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "then")) {
                return result->Failure(new SyntaxError(
                    "Expected 'then' after the end of the initial expression or 'step' in for-statement",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();

            if (this->currentToken.type == TokenType::NewLine) {
                result->RegisterAdvance();
                this->Advance();

                auto body = result->Register(this->Statements());
                if (result->err != nullptr) {
                    return result;
                }

                if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
                    return result->Failure(new SyntaxError(
                        "Expected 'end' after multi-line statement",
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                result->RegisterAdvance();
                this->Advance();

                return result->Success(new ForExpressionNode(
                    loopVariable, startValue, endValue, stepValue, body, true
                ));
            }

            auto body = result->Register(this->Statement());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(new ForExpressionNode(
                loopVariable, startValue, endValue, stepValue, body, false
            ));
        } else {
            return result->Failure(new SyntaxError(
                "Expected '=' (initialization for loop var) or 'in' (range-based loop) in for-stmt",
                this->currentToken.st, this->currentToken.et
            ));
        }
    }

    ParseResult *WhileExpr() {
        auto result = new ParseResult;

        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "while")) {
            return result->Failure(new SyntaxError(
                "Expected 'while'",
                this->currentToken.st, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();
        auto condition = result->Register(this->Expr());
        if (result->err != nullptr) {
            return result;
        }

        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "then")) {
            return result->Failure(new SyntaxError(
                "Expected 'then' after condition expression of while-statement",
                this->currentToken.st, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();

         if (this->currentToken.type == TokenType::NewLine) {
            result->RegisterAdvance();
            this->Advance();

            auto body = result->Register(this->Statements());
            if (result->err != nullptr) {
                return result;
            }

            if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
                return result->Failure(new SyntaxError(
                    "Expected 'end' after multi-line statement",
                    this->currentToken.st, this->currentToken.et
                ));
            }
            result->RegisterAdvance();
            this->Advance();

            return result->Success(new WhileExpressionNode(condition, body, true));
         }

        auto body = result->Register(this->Statement());
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(new WhileExpressionNode(condition, body, false));
    }

    ParseResult *CompExpr() {
        auto result = new ParseResult;
        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "not")) {
            auto opToken = this->currentToken;
            result->RegisterAdvance();
            this->Advance();

            auto node = result->Register(this->CompExpr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(new UnaryOperationNode(opToken, node));
        }

        auto node = result->Register(this->BinaryOperation(&Parser::ArithExpr, COMPARE_OPERANDS));
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(node);
    }

    ParseResult *ArithExpr() {
        return this->BinaryOperation(&Parser::Term, FIRST_LEVEL_OPERANDS);
    }

    ParseResult *Expr() {
        auto result = new ParseResult;
        if (this->currentToken.Matches<std::string>(TokenType::Keyword, "var")) {
            result->Register(this->Advance());
            if (this->currentToken.type != TokenType::Identifier) {
                return result->Failure(new SyntaxError(
                    "Expected an identifier",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            auto variableName = this->currentToken;
            result->Register(this->Advance());

            if (this->currentToken.type != TokenType::OP_Eq) {
                return result->Failure(new SyntaxError(
                    "Expected '='",
                    this->currentToken.st, this->currentToken.et
                ));
            }

            result->Register(this->Advance());
            auto expResult = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }
            return result->Success(new VariableAssignNode(variableName, expResult));
        }

        auto node = result->Register(this->BinaryOperation(&Parser::CompExpr, LOGIC_OPERANDS));
        if (result->err != nullptr) {
            return result;
        }
        return result->Success(node);
    }

    template<typename T>
    ParseResult *BinaryOperation(T func, std::vector<TokenType> validTokenTypes, T rightFunc = nullptr) {
        if (rightFunc == nullptr) {
            rightFunc = func;
        }
        auto result = new ParseResult;
        auto leftNode = result->Register((this->*func)());
        if (result->err != nullptr) {
            return result;
        }

        while (Lexer::Contains(validTokenTypes, this->currentToken.type)) {
            auto operationToken = this->currentToken;
            result->Register(this->Advance());
            auto rightNode = result->Register((this->*rightFunc)());
            if (result->err != nullptr) {
                return result;
            }
            leftNode = dynamic_cast<NodeBase *>(
                new BinaryOperationNode(leftNode, operationToken, rightNode)
            );
        }
        // return leftNode->nodeType == NodeType::Expression ?
        //     dynamic_cast<BinaryOperationNode *>(leftNode) : dynamic_cast<NumberNode *>(leftNode);
        return result->Success(leftNode);
    }

    template<typename T>
    // Only for use to check keywords
    ParseResult *BinaryOperation(T func, std::vector<std::pair<TokenType, std::string>> validTokens, T rightFunc = nullptr) {
        if (rightFunc == nullptr) {
            rightFunc = func;
        }
        auto result = new ParseResult;
        auto leftNode = result->Register((this->*func)());
        if (result->err != nullptr) {
            return result;
        }
    
        while (this->currentToken.value != nullptr && this->currentToken.type == TokenType::Keyword && Lexer::Contains(validTokens, std::make_pair(this->currentToken.type, *(std::string *) this->currentToken.value))) {
            auto operationToken = this->currentToken;
            result->Register(this->Advance());
            auto rightNode = result->Register((this->*rightFunc)());
            if (result->err != nullptr) {
                return result;
            }
            leftNode = dynamic_cast<NodeBase *>(
                new BinaryOperationNode(leftNode, operationToken, rightNode)
            );
        }
        // return leftNode->nodeType == NodeType::Expression ?
        //     dynamic_cast<BinaryOperationNode *>(leftNode) : dynamic_cast<NumberNode *>(leftNode);
        return result->Success(leftNode);
    }

    ParseResult *FunctionDefinition() {
        auto result = new ParseResult;
        // deprecated:  this->insideAFunction = true;
        this->functionLayers++;
        this->closureNodes.insert(std::make_pair(this->functionLayers, std::vector<NonlocalStatementNode *>()));
        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "function")) {
            return result->Failure(new SyntaxError(
                "Expected 'function'", this->currentToken.st, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();
        Token funcNameToken;
        if (this->currentToken.type == TokenType::Identifier) {
            funcNameToken = this->currentToken;
            result->RegisterAdvance();
            this->Advance();

            if (this->currentToken.type != TokenType::Lparen) {
                return result->Failure(new SyntaxError(
                    "Expected '(' after function name",
                    this->currentToken.st, this->currentToken.et
                ));
            }
        } else {
            funcNameToken = Token(TokenType::Invilid);
             if (this->currentToken.type != TokenType::Lparen) {
                return result->Failure(new SyntaxError(
                    "Expected an identifier or '(' after 'function'",
                    this->currentToken.st, this->currentToken.et
                ));
            }
        }

        result->RegisterAdvance();
        this->Advance();
        Tokens args;

        if (this->currentToken.type == TokenType::Identifier) {
            args.push_back(this->currentToken);
            result->RegisterAdvance();
            this->Advance();

            while (this->currentToken.type == TokenType::Comma) {
                result->RegisterAdvance();
                this->Advance();

                if (this->currentToken.type != TokenType::Identifier) {
                    return result->Failure(new SyntaxError(
                        "Expected an identifier after ',' in parameter list",
                        this->currentToken.st, this->currentToken.et
                    ));
                }
                args.push_back(this->currentToken);
                result->RegisterAdvance();
                this->Advance();
            }

            if (this->currentToken.type != TokenType::Rparen) {
                return result->Failure(new SyntaxError(
                    "Mismatched '(' found in the end of the parameter list (unfinished)",
                    this->currentToken.st, this->currentToken.et
                ));
            }
        } else {
            if (this->currentToken.type != TokenType::Rparen) {
                return result->Failure(new SyntaxError(
                    "Expected ')'",
                    this->currentToken.st, this->currentToken.et
                ));
            }
        }
        
        result->RegisterAdvance();
        this->Advance();
        if (this->currentToken.type == TokenType::Arrow) {
            result->RegisterAdvance();
            this->Advance();
    
            auto bodyNode = result->Register(this->Expr());
            if (result->err != nullptr) {
                return result;
            }
            auto fd = new FunctionDefinitionNode(funcNameToken, args, bodyNode, true);
            this->functionLayers--;
            return result->Success(fd);
        }

        if (this->currentToken.type != TokenType::NewLine) {
            return result->Failure(new SyntaxError(
                    "Expected '->' or new-lines after parameter list in function definition",
                    this->currentToken.st, this->currentToken.et
            ));
        }

        result->RegisterAdvance();
        this->Advance();

        auto body = result->Register(this->Statements());
        if (result->err != nullptr) {
            return result;
        }

        if (!this->currentToken.Matches<std::string>(TokenType::Keyword, "end")) {
            return result->Failure(new SyntaxError(
                "Expected 'end' after multi-line statement",
                this->currentToken.st, this->currentToken.et
            ));
        }
        result->RegisterAdvance();
        this->Advance();

        assert(functionLayers > 0);
        auto fsd = new FunctionDefinitionNode(funcNameToken, args, body, false);
        
        if (this->functionLayers == this->closureNodes.size()) {
            fsd->SetClosure(this->closureNodes[this->functionLayers]);
            fsd->SetCell({});
        } else if (this->functionLayers == 1) {
            if (closureNodes.find(2) != closureNodes.end()) {
                fsd->SetCell(this->closureNodes[2]);
            } else {
                fsd->SetCell({});
            }
            fsd->SetClosure({});
        } else {
            fsd->SetClosure(this->closureNodes[this->functionLayers]);
            fsd->SetCell(this->closureNodes[this->functionLayers + 1]);
        }

        this->functionLayers--;
        return result->Success(fsd);
    }

    ParseResult *Parse() {
        auto result = this->Statements();
        if (result->err == nullptr && this->currentToken.type != TokenType::__EOF__) {
            return result->Failure(new SyntaxError("Unexpected end of input", this->currentToken.st, this->currentToken.et));
        }
        return result;
    }

private:
    Token currentToken;
    Tokens tokens;
    int tokenIndex;
    bool insideAFunction; // deprecated
    int functionLayers;
    int clousureLayer;
    std::map<int, std::vector<NonlocalStatementNode *>> closureNodes;
};


enum class NumberType { Int, Float };
struct RuntimeResult;
struct Object;

std::vector<RuntimeResult *> contextResultCache;

struct RuntimeResult final {
    Object *value;
    Error *error;
    Object *funcReturnValue;
    // for loops
    bool shouldContinue;
    bool shouldBreak;
    std::string extraInfo = "";

    std::vector<RuntimeResult *> *resultCache;
    Error *cause;

    void Reset() {
        this->value = nullptr;
        this->error = nullptr;
        this->shouldContinue = false;
        this->shouldBreak = false;
        this->funcReturnValue = nullptr;
        this->cause = nullptr;
    }

    explicit RuntimeResult() : error(nullptr) {
        this->Reset();
        resultCache = &contextResultCache;
        contextResultCache.push_back(this);
    }

    Object *Register(RuntimeResult *res) {
        // if (res->error != nullptr) {
            this->error = res->error;
        // }
        this->funcReturnValue = res->funcReturnValue;
        this->shouldContinue = res->shouldContinue;
        this->shouldBreak = res->shouldBreak;
        this->cause = res->cause;
        this->extraInfo = res->extraInfo;
        return res->value;
    }

    void SetCause(Error *err) {
        if (err != nullptr) {
            this->cause = err;
        }
    }

    void ClearCause() {
        this->cause = nullptr;
    }

    RuntimeResult *Success(Object *value) {
        this->Reset();
        this->value = value;
        return this;
    }

    RuntimeResult *SuccessReturn(Object *value) {
        this->Reset();
        this->funcReturnValue = value;
        return this;
    }

    RuntimeResult *SuccessContinue() {
        this->Reset();
        this->shouldContinue = true;
        return this;
    }

    RuntimeResult *SuccessBreak() {
        this->Reset();
        this->shouldBreak = true;
        return this;
    }

    RuntimeResult *Failure(Error *err, Error *cause = nullptr) {
        this->Reset();
        this->error = err;
        this->SetCause(cause);
        return this; 
    }

    RuntimeResult *Failure(Error *err, Error *cause, const std::string &extra) {
        this->Reset();
        this->error = err;
        this->extraInfo = extra;
        this->SetCause(cause);
        return this; 
    }

    bool ShouldReturn() {
        return this->error != nullptr || this->shouldBreak || this->shouldContinue || this->funcReturnValue != nullptr;
    }
};


struct Object {
    const char *typeName;
    Context *ctx;
    Position *startPos;
    Position *endPos;

    explicit Object(const char *n) : typeName(n), ctx(nullptr) {}
    explicit Object() : Object("Object") {}

    virtual inline std::string ToString() {
        return std::format("<object {} at {}>", this->typeName, static_cast<void *>(this));
    }

    virtual Object *SetContext(Context *ctx = nullptr) {
        this->ctx = ctx;
        if (ctx && ctx->exFrame) {
            this->ctx->exFrame = ctx->exFrame;
        }
        return this;
    }

    virtual Object *Copy() {
        // Object *o = new Object;
        // *o = *this;
        // return o;
        assert(false);
        return nullptr;
    }

    virtual Object *SetPos(Position *st = nullptr, Position *et = nullptr) {
        this->startPos = st;
        this->endPos = et;
        return this;
    }

    virtual ~Object() = default;

    virtual bool AsBool() {
        return true;
    }

    virtual std::pair<Object *, Error *> AddTo(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "+"));
    }

    virtual std::pair<Object *, Error *> SubstractedBy(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "-"));
    }
    
    virtual std::pair<Object *, Error *> PoweredBy(Object *other)  {
        return std::make_pair(nullptr, this->IllegalOperation(other, "^"));
    }

    virtual std::pair<Object *, Error *> MultipliedBy(Object *other)  {
        return std::make_pair(nullptr, this->IllegalOperation(other, "*"));
    }

    virtual std::pair<Object *, Error *> DividedBy(Object *other)  {
        return std::make_pair(nullptr, this->IllegalOperation(other, "/"));
    }

    virtual std::pair<Object *, Error *> GetCompEquals(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "=="));
    }

    virtual std::pair<Object *, Error *> GetCompNequals(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "!="));   
    }
    
    virtual std::pair<Object *, Error *> GetCompLt(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "<"));   
    }

    virtual std::pair<Object *, Error *> GetCompGt(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, ">"));   
    }

    virtual std::pair<Object *, Error *> GetCompLte(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "<="));    
    }

    virtual std::pair<Object *, Error *> GetCompGte(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, ">="));   
    }

    virtual std::pair<Object *, Error *> And(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "and"));      
    }

    virtual std::pair<Object *, Error *> Or(Object *other) {
        return std::make_pair(nullptr, this->IllegalOperation(other, "or"));     
    }

    virtual std::pair<Object *, Error *> Not(Object *other) {
         return std::make_pair(nullptr, this->IllegalOperation(nullptr, "not"));
    }

    virtual std::pair<Object *, Error *> Subsciption(Object *other) {
        return std::make_pair(nullptr, new TypeError(
            std::format("'{}' object is not subscriptable", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> SubsciptionAssignment(Object *other, Object *value) {
        return std::make_pair(nullptr, new TypeError(
            std::format("'{}' object cannot be assigned through subscription", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> Len() {
        return std::make_pair(nullptr, new TypeError(
            std::format("Cannot perform 'len' on object '{}'", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> GetAttr(const std::string &attr) {
         return std::make_pair(nullptr, new TypeError(
            std::format("'{}' has no user-defined attribute", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> SetAttr(const std::string &attr, Object *newVal) {
        return std::make_pair(nullptr, new TypeError(
            std::format("'{}' object has no user-defined attribute", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> Iter() {
        return std::make_pair(nullptr, new TypeError(
            std::format("'{}' object is not iterable", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual std::pair<Object *, Error *> Next() {
        return std::make_pair(nullptr, new TypeError(
            std::format("'{}' object is not a iterator", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual RuntimeResult *Execute(std::vector<Object *> args) {
        auto result = new RuntimeResult;
        return result->Failure(new TypeError(
            std::format("'{}' object is not callable", this->typeName),
            this->startPos, this->endPos, this->ctx
        ));
    }

    virtual Error *IllegalOperation(Object *other, const std::string &operationId) {
        Object *o = other == nullptr ? this : other;
        return new TypeError(
            std::format("Invilid operation '{}' between '{}' and '{}'", operationId, this->typeName, o->typeName),
            this->startPos, this->endPos, this->ctx
        );
    }
};

template<typename T>
T *As(Object *o) {
    return dynamic_cast<T *>(o);
}

struct Number : public Object {
    std::variant<int, double> value;
    // union {
    //     int value;
    //     double value;
    // };
    NumberType ntype;
    static Number *null;

    
    explicit Number(int v) : ntype(NumberType::Int), value(v), Object("Number") {
        this->SetPos();
    }
    explicit Number(double v) : ntype(NumberType::Float), value(v), Object("Number") {
        this->SetPos();
    }

    explicit Number(std::variant<int, double> v) : value(v), Object("Number") {
        this->SetPos();
        if (std::holds_alternative<int>(v)) {
            this->ntype = NumberType::Int;
        } else {
            this->ntype = NumberType::Float;
        }
    }

    explicit Number() : ntype(NumberType::Float), value(0), Object("Number") {}

    Number *SetContext(Context *ctx = nullptr) override {
        Object::SetContext(ctx);
        return this;
    } 

    Number *SetPos(Position *st = nullptr, Position *et = nullptr) override {
        Object::SetPos(st, et);
        return this;
    }

    Number *Copy() override {
        return (new Number(this->value))->SetPos(this->startPos, this->endPos)->SetContext(this->ctx);
    }

    // template<typename T>
    // static bool IsSameVariant(const std::variant<int, double> &a, const std::variant<int, double> &b) {
    //     return std::holds_alternative<T>(a) && std::holds_alternative<T>(b);
    // }

    std::pair<Object *, Error *> AddTo(Object *other) override {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '+' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->BinaryOperation(b, a, ob->startPos, ob->endPos, TokenType::OP_Plus);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "+"));
    }

    std::pair<Object *, Error *> SubstractedBy(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '-' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->BinaryOperation(b, a, ob->startPos, ob->endPos, TokenType::OP_Minus);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "-"));
    }

    std::pair<Object *, Error *> PoweredBy(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '^' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->BinaryOperation(b, a, ob->startPos, ob->endPos, TokenType::OP_Pow);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "^"));
    }

    std::pair<Object *, Error *> GetCompEquals(Object *other) override {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '==' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Equal);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "=="));
    }

    std::pair<Object *, Error *> GetCompNequals(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '!=' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Nequal);
        }
       return std::make_pair(nullptr, Object::IllegalOperation(other, "!="));
    }

    std::pair<Object *, Error *> GetCompLt(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '<' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Lt);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "<"));
    }

    std::pair<Object *, Error *> GetCompGt(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '>' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Gt);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, ">"));
    }

    std::pair<Object *, Error *> GetCompLte(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '<=' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Lte);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "<="));
    }
    
    std::pair<Object *, Error *> GetCompGte(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '>=' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->ComparisonOperation(a, b, ob->startPos, ob->endPos, TokenType::OP_Gte);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, ">="));
    }

    std::pair<Number *, Error *> BinaryOperation(std::variant<int, double> a, std::variant<int, double> b, Position *ost, Position *oet, TokenType opType) {
        bool aIsInt = std::holds_alternative<int>(a), bIsInt = std::holds_alternative<int>(b);
        // Shit code :(
        
        if (aIsInt && bIsInt) {
            switch (opType) {
            case TokenType::OP_Plus:
                return std::make_pair((new Number(std::get<int>(a) + std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Minus:
                return std::make_pair((new Number(std::get<int>(a) - std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Mul:
                return std::make_pair((new Number(std::get<int>(a) * std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Div:
                if (std::get<int>(b) == 0) {
                    return std::make_pair(nullptr, new RuntimeError("Division by zero", ost, oet, this->ctx));
                }
                return std::make_pair((new Number(std::get<int>(a) / std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Pow:
                return std::make_pair((new Number(pow(std::get<int>(a), std::get<int>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        } else if (aIsInt && !bIsInt) {
            switch (opType) {
            case TokenType::OP_Plus:
                return std::make_pair((new Number(std::get<int>(a) + std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Minus:
                return std::make_pair((new Number(std::get<int>(a) - std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Mul:
                return std::make_pair((new Number(std::get<int>(a) * std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Div:
                if (std::get<double>(b) == 0) {
                    return std::make_pair(nullptr, new RuntimeError("Division by zero", ost, oet, this->ctx));
                }
                return std::make_pair((new Number(std::get<int>(a) / std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Pow:
                return std::make_pair((new Number(pow(std::get<int>(a), std::get<double>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        } else if (!aIsInt && bIsInt) {
            switch (opType) {
            case TokenType::OP_Plus:
                return std::make_pair((new Number(std::get<double>(a) + std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Minus:
                return std::make_pair((new Number(std::get<double>(a) - std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Mul:
                return std::make_pair((new Number(std::get<double>(a) * std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Div:
                if (std::get<int>(b) == 0) {
                    return std::make_pair(nullptr, new RuntimeError("Division by zero", ost, oet, this->ctx));
                }
                return std::make_pair((new Number(std::get<double>(a) / std::get<int>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Pow:
                return std::make_pair((new Number(pow(std::get<double>(a), std::get<int>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }   
        } else {
            switch (opType) {
            case TokenType::OP_Plus:
                return std::make_pair((new Number(std::get<double>(a) + std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Minus:
                return std::make_pair((new Number(std::get<double>(a) - std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Mul:
                return std::make_pair((new Number(std::get<double>(a) * std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Div:
                if (std::get<double>(b) == 0) {
                    return std::make_pair(nullptr, new RuntimeError("Division by zero", ost, oet, this->ctx));
                }
                return std::make_pair((new Number(std::get<double>(a) / std::get<double>(b)))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Pow:
                return std::make_pair((new Number(pow(std::get<double>(a), std::get<double>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        }
    }

    std::pair<Number *, Error *> ComparisonOperation(std::variant<int, double> a, std::variant<int, double> b, Position *ost, Position *oet, TokenType opType) {
        bool aIsInt = std::holds_alternative<int>(a), bIsInt = std::holds_alternative<int>(b);
        // Shit code again :(
                
        if (aIsInt && bIsInt) {
            switch (opType) {
            case TokenType::OP_Lt:
                return std::make_pair((new Number((int) (std::get<int>(a) > std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gt:
                return std::make_pair((new Number((int) (std::get<int>(a) < std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Lte:
                return std::make_pair((new Number((int) (std::get<int>(a) >= std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gte:
                return std::make_pair((new Number((int) (std::get<int>(a) <= std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Equal:
                return std::make_pair((new Number((int) (std::get<int>(a) == std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Nequal:
                return std::make_pair((new Number((int) (std::get<int>(a) != std::get<int>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        } else if (aIsInt && !bIsInt) {
           switch (opType) {
            case TokenType::OP_Lt:
                return std::make_pair((new Number((int) (std::get<int>(a) > std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gt:
                return std::make_pair((new Number((int) (std::get<int>(a) < std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Lte:
                return std::make_pair((new Number((int) (std::get<int>(a) <= std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gte:
                return std::make_pair((new Number((int) (std::get<int>(a) >= std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Equal:
                return std::make_pair((new Number((int) (std::get<int>(a) == std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Nequal:
                return std::make_pair((new Number((int) (std::get<int>(a) != std::get<double>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        } else if (!aIsInt && bIsInt) {
            switch (opType) {
            case TokenType::OP_Lt:
                return std::make_pair((new Number((int) (std::get<double>(a) > std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gt:
                return std::make_pair((new Number((int) (std::get<double>(a) < std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Lte:
                return std::make_pair((new Number((int) (std::get<double>(a) <= std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gte:
                return std::make_pair((new Number((int) (std::get<double>(a) >= std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Equal:
                return std::make_pair((new Number((int) (std::get<double>(a) == std::get<int>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Nequal:
                return std::make_pair((new Number((int) (std::get<double>(a) != std::get<int>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }      
        } else {
            switch (opType) {
            case TokenType::OP_Lt:
                return std::make_pair((new Number((int) (std::get<double>(a) > std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gt:
                return std::make_pair((new Number((int) (std::get<double>(a) < std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Lte:
                return std::make_pair((new Number((int) (std::get<double>(a) <= std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Gte:
                return std::make_pair((new Number((int) (std::get<double>(a) >= std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Equal:
                return std::make_pair((new Number((int) (std::get<double>(a) == std::get<double>(b))))->SetContext(this->ctx), nullptr);
            case TokenType::OP_Nequal:
                return std::make_pair((new Number((int) (std::get<double>(a) != std::get<double>(b))))->SetContext(this->ctx), nullptr);
            default:
                assert(false);
            }
        }
    }

    // 0 for and, 1 for or
    std::pair<Number *, Error *> LogicalOperation(std::variant<int, double> a, std::variant<int, double> b, Position *ost, Position *oet, int op) {\
        bool aIsInt = std::holds_alternative<int>(a), bIsInt = std::holds_alternative<int>(b);
        // Shit code again again :(
                        
        if (aIsInt && bIsInt) {
            if (op == 0) {
                return std::make_pair((new Number((int) ((bool) std::get<int>(a) && (bool) std::get<int>(b))))->SetContext(this->ctx), nullptr);
            } else if (op == 1) {
                return std::make_pair((new Number((int) ((bool) std::get<int>(a) || (bool) std::get<int>(b))))->SetContext(this->ctx), nullptr);
            } else assert(false);
        } else if (aIsInt && !bIsInt) {
            if (op == 0) {
                return std::make_pair((new Number((int) ((bool) std::get<int>(a) && (bool) std::get<double>(b))))->SetContext(this->ctx), nullptr);
            } else if (op == 1) {
                return std::make_pair((new Number((int) ((bool) std::get<int>(a) || (bool) std::get<double>(b))))->SetContext(this->ctx), nullptr);
            } else assert(false);
        } else if (!aIsInt && bIsInt) {
            if (op == 0) {
                return std::make_pair((new Number((int) ((bool) std::get<double>(a) && (bool) std::get<int>(b))))->SetContext(this->ctx), nullptr);
            } else if (op == 1) {
                return std::make_pair((new Number((int) ((bool) std::get<double>(a) || (bool) std::get<int>(b))))->SetContext(this->ctx), nullptr);
            } else assert(false);
        } else {
           if (op == 0) {
                return std::make_pair((new Number((int) ((bool) std::get<double>(a) && (bool) std::get<double>(b))))->SetContext(this->ctx), nullptr);
            } else if (op == 1) {
                return std::make_pair((new Number((int) ((bool) std::get<double>(a) || (bool) std::get<double>(b))))->SetContext(this->ctx), nullptr);
            } else assert(false);
        }
    }

    std::pair<Object *, Error *> And(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Logical operation 'and' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->LogicalOperation(b, a, ob->startPos, ob->endPos, 0);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "and"));        
    } 


    std::pair<Object *, Error *> Or(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Logical operation 'or' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->LogicalOperation(b, a, ob->startPos, ob->endPos, 1);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "or"));
    } 

    std::pair<Object *, Error *> MultipliedBy(Object *other) override {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '*' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->BinaryOperation(b, a, ob->startPos, ob->endPos, TokenType::OP_Mul);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "*"));
    }

    std::pair<Object *, Error *> DividedBy(Object *other) {
        if (other == nullptr) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Operator '/' not supported between '{}' and null", this->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (std::string(other->typeName) == std::string(this->typeName)) {
            auto ob = dynamic_cast<Number *>(other);
            auto a = ob->value;
            auto b = this->value;
            return this->BinaryOperation(b, a, ob->startPos, ob->endPos, TokenType::OP_Div);
        }
        return std::make_pair(nullptr, Object::IllegalOperation(other, "/"));
    }

    std::pair<Object *, Error *> Not() {
        if (this->ntype == NumberType::Float) {
            return std::make_pair((new Number(std::get<double>(this->value) == 0.0))->SetContext(this->ctx), nullptr);
        } else {
            return std::make_pair((new Number(std::get<int>(this->value) == 0))->SetContext(this->ctx), nullptr);
        }
    }

    inline std::string ToString() override {
        if (std::holds_alternative<int>(this->value)) {
            return std::format("{}", std::get<int>(this->value));
        } else {
            return std::format("{}", std::get<double>(this->value));
        }
    }

    bool AsBool() override {
        if (this->ntype == NumberType::Int) {
            return std::get<int>(this->value) != 0;
        } else {
            return std::get<double>(this->value) != 0.0;
        }
    }

    ~Number() {

    }
};


struct SymbolTable final {
    std::map<std::string, Object *> symbols;
    SymbolTable *parentFieldSymbols;

    explicit SymbolTable() {
        this->parentFieldSymbols = nullptr;
    }

    explicit SymbolTable(SymbolTable *parant) {
        this->parentFieldSymbols = parant;
    }

    inline Object *Get(const std::string &name) {
        if (this->symbols.find(name) == this->symbols.end()) {
            if (this->parentFieldSymbols != nullptr) {
                SymbolTable *st = this->parentFieldSymbols;
                while (st != nullptr) {
                    if (st->symbols.find(name) != st->symbols.end()) {
                        return st->symbols[name];
                    }
                    st = st->parentFieldSymbols;
                }
            }
        } else {
            return this->symbols[name];
        }
        return nullptr;
    }

    inline void Set(const std::string &name, Object *newValue) {
        if (this->symbols.find(name) == this->symbols.end()) {
            this->symbols.emplace(std::make_pair(name, newValue));
        } else {
            this->symbols[name] = newValue;
        }
    } 

    inline void Remove(const std::string &name) {
        this->symbols.erase(name);
    }

    inline std::string ToString(bool showBuiltins = true) const {
        std::stringstream ss;
        ss << "{\n";
        unsigned nums = 0;
        for (auto [n, v] : this->symbols) {
            nums++;            
            if (!showBuiltins && v->typeName == "BuiltinFunction") {
                if (nums == this->symbols.size() - 1) {
                    auto s = showBuiltins ? "" : "\n";                
                    ss << s << "}";
                }
                continue;
            }
            ss << std::format("  [{}] \'{}\':  {} ({})", v->typeName, n, v->ToString(), (void *) v);
            if (nums != this->symbols.size()) {
                ss << ",\n";
            } else {
                ss << "\n}";
            }
        }
        return ss.str();
    }
};

struct String : public Object {
    using ObjectWithError = std::pair<Object *, Error *>;

    struct StringIterator : public Object {
        std::string content;
        std::wstring iterContent;
        long n;
        explicit StringIterator(const std::string &content) : content(content), n(-1) {
            this->iterContent = ToWideString(this->content);
        }

        ObjectWithError Next() override {
            this->n++;
            if (n < this->iterContent.length()) {
                auto s = ToByteString(Lexer::CharAt(this->iterContent, n));
                return std::make_pair(new String(s), nullptr);
            } else {
                return std::make_pair(nullptr, new StopIteration);
            }
        }
    };

    static inline std::string Repeat(const std::string &s, int times) {
        std::string result;
        for (int i = 0; i < times; i++) {
            result += s;
        }
        return result;
    }

    std::string s;
    explicit String(const std::string &value) : s(value), Object("String") {}

    std::pair<Object *, Error *> AddTo(Object *other) override {
        if (other->typeName == std::string("String")) {
            auto *o = dynamic_cast<String *>(other);
            return std::make_pair((new String(this->s + o->s))->SetContext(this->ctx)->SetPos(this->startPos, this->endPos), nullptr);
        } else {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "+"));
        }
    }

    std::pair<Object *, Error *> MultipliedBy(Object *other) override {
        if (other->typeName == "Number") {
            auto *o = dynamic_cast<Number *>(other);
            if (o->ntype != NumberType::Int) {
                return std::make_pair(nullptr, new TypeError(
                    "The right of '*' in string multiplication must be an integer",
                    other->startPos, other->endPos, this->ctx
                ));
            }
            return std::make_pair((new String(String::Repeat(this->s, std::get<int>(o->value))))->SetContext(this->ctx)->SetPos(this->startPos, this->endPos), nullptr);
        } else {
           return std::make_pair(nullptr, Object::IllegalOperation(other, "*"));
        }
    }

    std::pair<Object *, Error *> GetCompEquals(Object* other) override {
        if (other->typeName == "String") {
            return this->s == dynamic_cast<String *>(other)->s ? std::make_pair(new Number(1), nullptr) : std::make_pair(new Number(0), nullptr);
        } else {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "=="));
        }
    }

    std::pair<Object *, Error *> GetCompNequals(Object* other) override {
        if (other->typeName == "String") {
            return this->s == dynamic_cast<String *>(other)->s ? std::make_pair(new Number(0), nullptr) : std::make_pair(new Number(1), nullptr);
        } else {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "!="));
        }
    }

    bool AsBool() override {
        return this->s.size() > 0;
    }

    Object *Copy() override {
        return (new String(this->s))->SetContext(this->ctx)->SetPos(this->startPos, this->endPos);
    }

    inline std::string ToString() override {
        return this->s;
    }

    std::pair<Object *, Error *> Len() override {
        return std::make_pair(new Number((int) ToWideString(this->s).length()), nullptr);
    }

    inline void Representation() {
        std::string repr;
        std::cout << "'";
        std::wstring ws = ToWideString(this->s);
        for (long i = 0; i < ws.length(); i++) {
            auto c = ToByteString(Lexer::CharAt(ws, i));
            if (REVERSED_ESCAPE_CHARACTERS.find(c) != REVERSED_ESCAPE_CHARACTERS.end()) {
                std::cout << "\\";
                std::cout << REVERSED_ESCAPE_CHARACTERS.at(c);
                continue;
            }
            std::cout << c;
        }
        std::cout << "'" << std::endl;
    }

    inline void Representation(std::ostream &o, bool nl = false) {
        std::string repr;
        std::wstring ws = ToWideString(this->s);        
        o << "'";
        for (long i = 0; i < ws.length(); i++) {
            auto c = ToByteString(Lexer::CharAt(ws, i));
            if (REVERSED_ESCAPE_CHARACTERS.find(c) != REVERSED_ESCAPE_CHARACTERS.end()) {
                o << "\\";
                o << REVERSED_ESCAPE_CHARACTERS.at(c);
                continue;
            }
            o << c;
        }
        o << "'";
        if (nl) {
            o << std::endl;
        }
    }

    ObjectWithError Subsciption(Object *other) override {
        if (other->typeName != std::string("Number")) {
            return std::make_pair(nullptr, new TypeError(
                "String index should be a number",
                other->startPos, other->endPos, this->ctx 
            ));
        }

        auto index = As<Number>(other);
        if (!std::holds_alternative<int>(index->value)) {
            return std::make_pair(nullptr, new TypeError(
                "String index should be a positive integer",
                other->startPos, other->endPos, this->ctx
            ));
        }

        auto indexNum = std::get<int>(index->value);
        auto ws = ToWideString(this->s);
        if (indexNum < 0) {
            return std::make_pair(nullptr, new TypeError(
                "String index should be a positive integer",
                other->startPos, other->endPos, this->ctx
            ));
        }

        if (indexNum + 1 > ws.length()) {
            return std::make_pair(nullptr, new TypeError(
                std::format("String index out of range (maximum is {} but got {})", this->s.length() - 1, indexNum),
                other->startPos, other->endPos, this->ctx
            ));
        }

        return std::make_pair(new String(ToByteString(Lexer::CharAt(ws, indexNum))), nullptr);
    }

    ObjectWithError Iter() {
        return std::make_pair(new StringIterator(this->s), nullptr);
    }

    ~String() = default;
};


template <>
inline std::string StringifySequence(const std::vector<Object *> &seq) {
    std::string result = "[";
    for (int i = 0; i < seq.size(); i++) {
        if (seq[i]->typeName == "String") {
            std::ostringstream oss;
            As<String>(seq[i])->Representation(oss);
            result += oss.str();
        } else {
            result += seq[i]->ToString();
        }
        if (i + 1 != seq.size()) {
            result += ", ";
        }
    }
    result += "]";
    return result;
}


struct List : public Object {
    std::vector<Object *> elements;
    using ObjectWithError = std::pair<Object *, Error *>;
    bool isParameterPack = false;
    bool isMutable = true;

    List *AsParameterPack() { 
        auto c = As<List>(this->Copy());
        c->isParameterPack = true;
        return c;
    }
    List *SetUnmutable(bool v = false) { this->isMutable = v; return this; }

    explicit List(const std::vector<Object *> &elements) : elements(elements), Object("List") {}

    // immutable operation for operators of list
    ObjectWithError AddTo(Object *other) override {
        auto copy = dynamic_cast<List *>(this->Copy());
        copy->elements.push_back(other);
        return std::make_pair(copy, nullptr);
    }

    ObjectWithError MultipliedBy(Object *other) override {
        if (other->typeName != "List") {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "<list-concat '*'>"));
        }
        auto newList = dynamic_cast<List *>(this->Copy());
        // for (auto e : this->elements) {
        //     newList->elements.push_back(e);
        // }
        for (auto e : dynamic_cast<List *>(other)->elements) {
            newList->elements.push_back(e);
        }
        return std::make_pair(newList, nullptr);
    }

    ObjectWithError SubstractedBy(Object *other) override {
        if (other->typeName != "Number") {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "<list-remove '-'>"));
        } else {
            auto index = dynamic_cast<Number *>(other);
            if (index->ntype != NumberType::Int) {
                return std::make_pair(nullptr, new TypeError(
                    "List index must be an integer",
                    this->startPos, this->endPos, this->ctx
                ));
            } else {
                auto indexValue = std::get<int>(index->value);      
                auto newList = dynamic_cast<List *>(this->Copy());
                if (indexValue < 0) {
                    indexValue = newList->elements.size() + indexValue;
                }
                if (indexValue + 1 > newList->elements.size()) {
                    return std::make_pair(nullptr, new RuntimeError(
                        std::format("List index out of range (given {} but maximum is {})", indexValue, newList->elements.size()),
                        this->startPos, this->endPos, this->ctx
                    ));
                }
                decltype(newList->elements)::iterator it = newList->elements.begin() + indexValue;
                newList->elements.erase(it);
                return std::make_pair(newList, nullptr);
            }
        }
    }

    ObjectWithError DividedBy(Object *other) override {
        if (other->typeName != std::string("Number")) {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "<list-access '/'>"));
        } else {
            auto index = dynamic_cast<Number *>(other);
            if (index->ntype != NumberType::Int) {
                return std::make_pair(nullptr, new TypeError(
                    "List index must be an integer",
                    this->startPos, this->endPos, this->ctx
                ));
            } else {
                auto indexValue = std::get<int>(index->value);      
                if (indexValue < 0) {
                    indexValue = this->elements.size() + indexValue;
                }
                if (indexValue + 1 > this->elements.size() || indexValue < 0) {
                    return std::make_pair(nullptr, new RuntimeError(
                        std::format("List index out of range (given {} but maximum is {})", indexValue, this->elements.size()),
                        this->startPos, this->endPos, this->ctx
                    ));
                }
                return std::make_pair(this->elements[indexValue], nullptr);
            }
        }
    }

    ObjectWithError Subsciption(Object *other) override {
        return this->DividedBy(other);
    }

    ObjectWithError SubsciptionAssignment(Object *other, Object *newVal) override {
        if (other->typeName != "Number") {
            return std::make_pair(nullptr, Object::IllegalOperation(other, "<list-access '[]'>"));
        } else {
            auto index = dynamic_cast<Number *>(other);
            if (index->ntype != NumberType::Int) {
                return std::make_pair(nullptr, new TypeError(
                    "List index must be an integer",
                    this->startPos, this->endPos, this->ctx
                ));
            } else {
                auto indexValue = std::get<int>(index->value);      
                if (indexValue < 0) {
                    indexValue = this->elements.size() + indexValue;
                }
                if (indexValue + 1 > this->elements.size() || indexValue < 0) {
                    return std::make_pair(nullptr, new RuntimeError(
                        std::format("List index out of range (given {} but maximum is {})", indexValue, this->elements.size()),
                        this->startPos, this->endPos, this->ctx
                    ));
                }
                this->elements[indexValue] = newVal;
            }
        }
        return std::make_pair(Number::null, nullptr);
    }

    Object *Copy() override {
        auto copy = new List(this->elements);
        copy->SetPos(this->startPos, this->endPos);
        copy->SetContext(this->ctx);
        // copy->isParameterPack = this->isParameterPack;
        // copy->isMutable = this->isMutable;
        return copy;
    }

     std::pair<Object *, Error *> Len() override {
        return std::make_pair(new Number((int) this->elements.size()), nullptr);
    }
    

    inline std::string ToString() override {
        return StringifySequence(this->elements);
    }
};

std::string StringifyMapping(const std::map<Object *, Object *> &mapping, std::string prefIgnore = "", std::string suffIgnore = "") {
    std::string result = "{";
    int i = 0;
    for (auto &[k, v] : mapping) {
        std::string str = "";
        if (prefIgnore != "") {
            if (k->ToString().starts_with(prefIgnore)) {
                i++;
                continue;
            }
        }        
        if (suffIgnore != "") {
            if (k->ToString().ends_with(suffIgnore)) {
                i++;
                continue;
            }
        }
        if (k->typeName == "String") {
            str = std::string("'") + k->ToString() + std::string("'");
        } else {
            str = k->ToString();
        }
        result += str;
        result += ": ";
        if (v->typeName == "String") {
            str = std::string("'") + v->ToString() + std::string("'");
        } else {
            str = v->ToString();
        }
        result += str;
        if (i + 1 != mapping.size()) {
            result += ", ";
        }
        i++;
    }
    return result + "}";
}


struct Dictionary : public Object {
    std::map<Object *, Object *> elements;
    using ObjectWithError = std::pair<Object *, Error *>;

    explicit Dictionary(const std::map<Object *, Object *> &elements) : elements(elements), Object("Dictionary") {}

    std::vector<Object *> GetKeys() {
        std::vector<Object *> result;
        for (auto [k, _] : this->elements) {
            result.push_back(k);
        }
        return result;
    }

    ObjectWithError Subsciption(Object *other) override {
        for (auto k : this->GetKeys()) {
            auto compResult = other->GetCompEquals(k);
            if (compResult.first == nullptr) {
                return std::make_pair(nullptr, new TypeError(
                    std::format("Invilid key type (which is not defined `GetCompEq` with type '{}'): '{}'", k->typeName, other->typeName),
                    this->startPos, this->endPos, this->ctx
                ));
            }
            if (compResult.second != nullptr) {
                return std::make_pair(nullptr, compResult.second);
            }
            if (compResult.first->AsBool()) {
                return std::make_pair(this->elements.at(k), nullptr);
            }
        }
        return std::make_pair(nullptr, new AttributeError(
            std::format("Dictionary has no attribute `{}`", other->ToString()),
            other->startPos, other->endPos, this->ctx
        ));
    }

    ObjectWithError SubsciptionAssignment(Object *other, Object *newVal) override {
        for (auto k : this->GetKeys()) {
            auto compResult = other->GetCompEquals(k);
            if (compResult.first == nullptr) {
                return std::make_pair(nullptr, new TypeError(
                    std::format("Invilid key type (which is not defined `GetCompEq` with type '{}'): '{}'", k->typeName, other->typeName),
                    this->startPos, this->endPos, this->ctx
                ));
            }
            if (compResult.second != nullptr) {
                return std::make_pair(nullptr, compResult.second);
            }
            if (compResult.first->AsBool()) {
                this->elements[k] = newVal;        
                return std::make_pair(Number::null, nullptr);
            }
        }
        if (other->typeName != std::string("String") && other->typeName != std::string("Number")) {
            return std::make_pair(nullptr, new TypeError(
                std::format("Invilid key type: '{}'", other->typeName),
                other->startPos, other->endPos, this->ctx
            ));
        }
        this->elements[other] = newVal;
        return std::make_pair(Number::null, nullptr);
    }

    ObjectWithError GetAttr(const std::string &attr) override {
        for (auto [attrName, value] : this->elements) {
            if (attrName->typeName != std::string("String")) {
                continue;
            }
            auto key = As<String>(attrName);
            if (attr == key->s) {
                return std::make_pair(value, nullptr);
            }
        }
        return std::make_pair(nullptr, new AttributeError(
           std::format("Could not found attribute '.{}'", attr),
           this->startPos, this->endPos, this->ctx
        ));
    }

    ObjectWithError SetAttr(const std::string &attr, Object *newVal) override {
        return this->SubsciptionAssignment((new String(attr))->SetContext(this->ctx)->SetPos(newVal->startPos, newVal->endPos), newVal);
    }

    Object *Copy() override {
        auto copy = new Dictionary(this->elements);
        copy->SetContext(this->ctx)->SetPos(this->startPos, this->endPos);
        return copy;
    }

    ObjectWithError Len() override {
        return std::make_pair(new Number((int) this->elements.size()), nullptr);
    }

    inline std::string ToString() override {
        return StringifyMapping(this->elements);
    }
};


enum class InterpreterStartMode {
    Repl,
    File,
    Evaluation
};

class Interpreter final {
public:
    explicit Interpreter();
    RuntimeResult *Visit(NodeBase *node, Context *ctx);
    RuntimeResult *VisitExpression(NodeBase *node, Context *ctx);
    RuntimeResult *VisitSingleExpression(NodeBase *node, Context *ctx);
    RuntimeResult *VisitVarAccessNode(NodeBase *node, Context *ctx);
    RuntimeResult *VisitVarAssignNode(NodeBase *node, Context *ctx);
    RuntimeResult *VisitNumber(NodeBase *node, Context *ctx);
    RuntimeResult *VisitIfExpressionNode(NodeBase *node, Context *ctx);
    RuntimeResult *VisitForExpression(NodeBase *node, Context *ctx);
    RuntimeResult *VisitWhileExpression(NodeBase *node, Context *ctx);
    RuntimeResult *VisitFunctionCall(NodeBase *node, Context *ctx);
    RuntimeResult *VisitFunctionDefinition(NodeBase *node, Context *ctx);
    RuntimeResult *VisitString(NodeBase *node, Context *ctx);
    RuntimeResult *VisitList(NodeBase *node, Context *ctx);
    RuntimeResult *VisitReturn(NodeBase *node, Context *ctx);
    RuntimeResult *VisitContinue(NodeBase *node, Context *ctx);
    RuntimeResult *VisitBreak(NodeBase *node, Context *ctx);
    RuntimeResult *VisitSubscription(NodeBase *node, Context *ctx);
    RuntimeResult *VisitDictionary(NodeBase *node, Context *ctx);
    RuntimeResult *VisitAttribution(NodeBase *node, Context *ctx);
    RuntimeResult *VisitAdvancedVarAccess(NodeBase *node, Context *ctx);
    RuntimeResult *VisitNewExpression(NodeBase *node, Context *ctx);
    RuntimeResult *VisitAttributionCall(NodeBase *node, Context *ctx);
    RuntimeResult *VisitNonlocal(NodeBase *node, Context *ctx);
    RuntimeResult *VisitDefer(NodeBase *node, Context *ctx);
    std::vector<Context *> *GetCallStack() { return this->callStack; }
    [[noreturn]] RuntimeResult *VisitEmpty(NodeBase *node, Context *ctx);
    ~Interpreter();

    std::vector<Context *> *callStack;
};


std::map<std::string, Context *> moduleContextCache;
std::map<std::string, std::string> symbolsModuleLocation;
struct FunctionBase : virtual public Object {
    std::string functionName;
    bool hasMutableArgument = false;
    explicit FunctionBase(const std::string &name) : functionName(name), Object("FunctionBase")
    {}

    explicit FunctionBase() : FunctionBase("<anonymous>") {}

    Context *GenerateNewContext() {
        if (symbolsModuleLocation.find(this->functionName) == symbolsModuleLocation.end()) {
            auto frameContext = new Context(this->functionName, this->ctx, this->startPos);
            // frameContext->symbols = new SymbolTable(frameContext->global);
            if (this->ctx && this->ctx->symbols) {
                frameContext->symbols = new SymbolTable(this->ctx->symbols);
            } else {
                frameContext->symbols = new SymbolTable(frameContext->global);
            }
            frameContext->nonlocals = new SymbolTable;
            // frameContext->symbols->Set(frameContext->parent->ctxLabel, this->ctx->symbols->Get(frameContext->parent->ctxLabel));        
            return frameContext;
        }
        auto ctxTmpl = moduleContextCache.at(symbolsModuleLocation.at(this->functionName));
        Context *frameContext = nullptr;
        if (ctxTmpl->external) {
            frameContext = new Context(this->functionName, this->ctx, this->startPos);
        } else {
            frameContext = new Context(this->functionName, ctxTmpl, this->startPos);
        }

        frameContext->symbols = new SymbolTable(ctxTmpl->symbols);
        frameContext->nonlocals = new SymbolTable;

        return frameContext;
    }

    virtual RuntimeResult *CheckArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args) {
        auto result = new RuntimeResult;
        if (args.size() > argNames.size()) {
            return result->Failure(new TypeError(
                std::format("Too many arguments given to function '{}' (Expected {} but got {})", this->functionName, argNames.size(), args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        } else if (args.size() < argNames.size()) {
            return result->Failure(new TypeError(
                std::format("Too few arguments given to function '{}' (Expected {} but got {})", this->functionName, argNames.size(), args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        }
        return result->Success(nullptr);
    }

    virtual RuntimeResult *PopulateArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args, Context *execCtx) {
        for (int i = 0; i < args.size(); i++) {
            auto argValue = args[i];
            auto argName = argNames[i];
            argValue->SetContext(execCtx);
            execCtx->symbols->Set(argName, argValue);
        }
        return (new RuntimeResult)->Success(nullptr);
    }

    virtual RuntimeResult *CheckAndPopulate(const std::vector<std::string> &argNames, std::vector<Object *> &args, Context *execCtx) {
        auto result = new RuntimeResult;
        result->Register(this->CheckArguments(argNames, args));
        if (result->ShouldReturn()) {
            return result;
        }
        result->Register(this->PopulateArguments(argNames, args, execCtx));
        if (result->ShouldReturn()) {
            return result;
        }
        return result->Success(nullptr);
    }
};


struct Function : public FunctionBase {
    NodeBase *body;
    std::vector<std::string> parameters;
    bool shouldAutoReturn;
    std::string mutableArgName;
    std::vector<std::string> cellVars;
    SymbolTable *closureVarsTable;

    explicit Function(const std::string &name, NodeBase *body, std::vector<std::string> args, bool shouldAutoReturn) :
        FunctionBase(name), body(body), parameters(args), shouldAutoReturn(shouldAutoReturn), Object("Function")
    {
        this->closureVarsTable = new SymbolTable;
    }

    explicit Function(NodeBase *body, std::vector<std::string> args, bool shouldAutoReturn) : 
        Function("<anonymous>", body, args, shouldAutoReturn) {}

    RuntimeResult *CheckMutableArgument(const std::vector<std::string> &argNames, std::vector<Object *> &args, bool isMethod = false) {
        auto result = new RuntimeResult;
        std::vector<std::string> mutableArgNames;
        for (auto &argName : argNames) {
            if (argName.starts_with("_") && argName.ends_with("_") && !argName.starts_with("__") && !argName.ends_with("__")) {
                this->hasMutableArgument = true;
                mutableArgNames.push_back(argName);
            }
        }
        if (!this->hasMutableArgument && !isMethod) {
            return FunctionBase::CheckArguments(argNames, args);
        }
        if (this->hasMutableArgument) {
            if (mutableArgNames.size() > 1) {
                return result->Failure(new RuntimeError(
                    "Too many mutable arguments",
                    this->startPos, this->endPos, this->ctx
                ));
            }

            if (argNames[argNames.size() - 1] != mutableArgNames[0]) {
                return result->Failure(new RuntimeError(
                    "Mutable argument appeared before positional arguments",
                    this->startPos, this->endPos, this->ctx
                ));
            }        
            this->mutableArgName = mutableArgNames[0];
            return result->Success(Number::null);
        }
        return result->Success(nullptr);
    }

    RuntimeResult *CheckArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args) override {
        auto result = new RuntimeResult;
        result->Register(this->CheckMutableArgument(argNames, args));
        if (result->ShouldReturn()) {
            return result;
        }
        if (argNames.size() == 0) {
            return result->Success(nullptr);
        }

        if (args.size() < argNames.size() - 1) {
             return result->Failure(new RuntimeError(
                std::format("Too few arguments given to function '{}' (Expected at least {} but got {})", this->functionName, argNames.size() - 1, args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        }
        return result->Success(nullptr);
    }

    virtual RuntimeResult *PopulateArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args, Context *execCtx) override {
        if (!this->hasMutableArgument) {
            return FunctionBase::PopulateArguments(argNames, args, execCtx);
        }
        auto mutableArgumentValue = new List({});
        bool mutableArgumentReplaced = false;
        int i;
        for (i = 0; i < argNames.size() - 1; i++) {
            auto argValue = args[i];
            auto argName = argNames[i];
            argValue->SetContext(execCtx);
            execCtx->symbols->Set(argName, argValue);
        }

        if (!mutableArgumentReplaced) {
            for (; i < args.size(); i++) {
                auto argValue = args[i];
                argValue->SetContext(execCtx);

                if (argValue->typeName == std::string("List")) {
                    if (As<List>(argValue)->isParameterPack) {
                        for (auto &&unpackedVal : As<List>(argValue)->elements) {
                            mutableArgumentValue->elements.push_back(unpackedVal);
                        }
                        continue;
                    }
                }
                mutableArgumentValue->elements.push_back(argValue);
            }
            execCtx->symbols->Set(this->mutableArgName.substr(1, this->mutableArgName.size() - 2), mutableArgumentValue);
            if (!mutableArgumentValue->elements.empty()) {
                execCtx->symbols->Set("$" + this->mutableArgName.substr(1, this->mutableArgName.size() - 2), mutableArgumentValue->AsParameterPack());
            }
        }

        return (new RuntimeResult)->Success(nullptr);
    }

    RuntimeResult *Execute(std::vector<Object *> args) {
        auto result = new RuntimeResult;
        auto interpreter = new Interpreter;
        auto frameContext = this->GenerateNewContext();

        for (auto [name, value] : this->closureVarsTable->symbols) {
            frameContext->nonlocals->Set(name, value);
        }

        result->Register(this->CheckAndPopulate(this->parameters, args, frameContext));
        if (result->ShouldReturn()) {
            return result;
        }

        auto returnValue = result->Register(interpreter->Visit(this->body, frameContext));
        if (result->ShouldReturn() && result->funcReturnValue == nullptr) {
            Error *lastError = result->error;
            if (frameContext->deferNodes.size() != 0) {
                for (auto node : frameContext->deferNodes) {
                    auto expr = dynamic_cast<DeferNode *>(node)->deferExpr;
                    if (currentCallStackDepth <= MAX_CALLSTACK_DEPTH) {

                        std::map<Object *, Object *> excInfo {
                            { new String("category"), new String(lastError->name) },
                            { new String("details"), new String(lastError->details) },
                            { new String("line"), new Number(lastError->st->line + 1) },
                            { new String("column"), new Number(lastError->st->column + 1) },
                            { new String("filename"), new String(lastError->st->filename) }
                        };
                        frameContext->symbols->Set("__lastexc__", new Dictionary(excInfo));
                        result->Register(interpreter->Visit(expr, frameContext));        
                        if (result->ShouldReturn()) {
                            return result->Failure(
                                result->error, lastError, "During handling the above exception, another exception occurred:"
                            );                        
                        }
                    } else {
                         return result->Failure(new RuntimeError(
                            "`defer` terminated due to stack overflow",
                            this->startPos, this->endPos, this->ctx
                        ));
                    }
                }

                auto frameState = frameContext->symbols->Get("__recovered__");
                if (!frameState) {
                    return result->Failure(lastError);
                } else {
                    return result->Success(frameState);
                }
            } else {
                return result;
            }
        }
        Object *returns = nullptr;
        if (shouldAutoReturn && result->funcReturnValue == nullptr) {
            returns = returnValue;
        } else if (result->funcReturnValue != nullptr) {
            returns = result->funcReturnValue;
        } else {
            returns = Number::null;
        }

        if (frameContext->deferNodes.size() != 0) {
            for (auto node : frameContext->deferNodes) {
                auto expr = dynamic_cast<DeferNode *>(node)->deferExpr;
                if (currentCallStackDepth <= MAX_CALLSTACK_DEPTH) {
                    frameContext->symbols->Set("__lastexc__", Number::null);
                    result->Register(interpreter->Visit(expr, frameContext));        
                    if (result->ShouldReturn()) {
                        return result;                    
                    }
                } else {
                    return result->Failure(new RuntimeError(
                        "`defer` terminated due to stack overflow",
                        this->startPos, this->endPos, this->ctx
                    ));
                }
            }
        }
        return result->Success(returns);
    }

    Object *Copy() override {
        auto copiedFunction = new Function(this->functionName, this->body, this->parameters, this->shouldAutoReturn);
        copiedFunction->cellVars = this->cellVars;
        copiedFunction->closureVarsTable = this->closureVarsTable;
        copiedFunction->SetContext(this->ctx);
        copiedFunction->SetPos(this->startPos, this->endPos);
        return copiedFunction;        
    }

    Object *SetContext(Context *ctx) override {
        Object::SetContext(ctx);
        return this;
    }

    Object *SetPos(Position *st, Position *et) override {
        Object::SetPos(st, et);
        return this;
    }

    inline std::string ToString() override {
        return std::format("<function {} at {}>", this->functionName, (void *) this);
    }
};


struct ClassObject : public Dictionary {
    explicit ClassObject(const std::map<Object *, Object *> &elements) : Dictionary(elements) {}
    std::string className;
    bool isProto = true;

    RuntimeResult *BuildClass() {
        auto result = new RuntimeResult;
        auto [className, error] = this->GetAttr("__cls__");
        if (error != nullptr) {
            return result->Failure(new ValueError(
                "Class prototype without an '__cls__' attribute",
                this->startPos, this->endPos, this->ctx
            ));
        }
        if (className->typeName != "String") {
            return result->Failure(new TypeError(
                std::format("Invilid type for '__cls__': '{}'", className->typeName),
                this->startPos, this->endPos, this->ctx
            ));
        }
        this->className = As<String>(className)->s.c_str();
        this->typeName = "ClassObject";
        return result->Success(nullptr);
    }


    inline std::string ToString() override {
        if (isProto) {
            return std::format("<Prototype of object '{}' at {}>", this->className, (void *) this);
        } else {
            std::stringstream ss;
            ss << RED << this->className << RESET << " {\n  ";
            int idx = 0;
            for (const auto &[k, v] : this->elements) {
                std::string s;
                if (idx + 1 == this->elements.size()) {
                    s = "\n}";
                } else {
                    s = ",\n  ";
                }
                std::string vs;
                if (v->typeName == std::string("Function")) {
                    vs = "[Function]";
                } else if (v->typeName == std::string("BuiltinFunction")) {
                    vs = "[Native Function]";
                } else if (v->typeName == std::string("BuiltinMethod")) {
                    vs = "[Native Method]";
                } else if (v->typeName == std::string("String")) {
                    vs = std::format("'{}'", v->ToString());
                } else {
                    vs = v->ToString();
                }
                ss << std::format("'{}': ", k->ToString());
                ss << CYAN;
                ss << std::format("{}", vs);
                ss << RESET;
                ss << std::format("{}", s);
                idx++;
            }
            return ss.str();
        }
    }

    Object *Copy() override {
        auto copy = new ClassObject(this->elements);
        copy->className = this->className;
        copy->isProto = this->isProto;
        copy->SetContext(this->ctx)->SetPos(this->startPos, this->endPos);
        copy->BuildClass();
        return copy;
    }

    Object *Instantiate(const std::vector<Object *> &args, Context *ctx, Position *st, Position *et);
};

struct Method : public Function {
    Object *self;

    static Method *FromFunction(Function *f, Object *self, const std::string &name = "<anonymous>") {
        auto m = new Method(f->functionName, f->body, f->parameters, f->shouldAutoReturn);
        m->SetPos(f->startPos, f->endPos);
        m->SetContext(f->ctx);
        m->self = self;
        m->functionName = name;
        return m;
    }

    explicit Method(const std::string &name, NodeBase *body, std::vector<std::string> args, bool shouldAutoReturn)
        : Function(name, body, args, shouldAutoReturn), Object("Method") {}

    RuntimeResult *CheckArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args) override {
        auto result = new RuntimeResult;
        result->Register(this->CheckMutableArgument(argNames, args, true));
        if (result->ShouldReturn()) {
            return result;
        }

         if (args.size() + 1 < argNames.size()) {
                return result->Failure(new RuntimeError(
                std::format("Too few arguments given to method '{}' (Expected at least {} but got {})", this->functionName, argNames.size() - 1, args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        }

        return result->Success(nullptr);
    }
    
    RuntimeResult *PopulateArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args, Context *execCtx) override {
        if (argNames.size() == 0) {
            return Function::PopulateArguments(argNames, args, execCtx);
        }
        if ((args.size() < argNames.size() - 1 && hasMutableArgument) || (args.size() != argNames.size() - 1 && !hasMutableArgument)) {
            return (new RuntimeResult)->Failure(new TypeError(
                std::format("Invilid argument sequence length of method '{}': {} (requires {})", this->functionName, args.size(), argNames.size() - 1),
                this->startPos, this->endPos, this->ctx
            ));
        }
        execCtx->symbols->Set(argNames[0], this->self);
        std::vector<std::string> remain;
        for (int i = 1; i < argNames.size(); i++) {
            remain.push_back(argNames[i]);
        }

        return Function::PopulateArguments(remain, args, execCtx);
    }

    Object *Copy() override {
        auto copiedMethod = new Method(this->functionName, this->body, this->parameters, this->shouldAutoReturn);
        copiedMethod->self = this->self;
        copiedMethod->SetContext(this->ctx);
        copiedMethod->SetPos(this->startPos, this->endPos);
        return copiedMethod;
    }

    inline std::string ToString() override {
        std::string tpName;
        if (this->self->typeName == std::string("ClassObject")) {
            tpName = As<ClassObject>(this->self)->className;
        } else {
            tpName = this->self->typeName;
        }
        return std::format("<bound-method {} at {} of object '{}' at {}>", this->functionName, (void *) this, tpName, (void *) self);
    }
};


void Interprete(const std::string &, const std::string &, InterpreterStartMode, const std::string & = "", Context * = nullptr, Position *parentEntry = nullptr);
void SetBuiltins(SymbolTable *global);
const std::vector<std::string> builtinNames { 
    "print", "println", "typeof", "readLine", "len", "parseInt", "parseFloat", "str", "eval",
    "sin", "cos", "tan", "abs", "log", "ln", "sqrt", "isFloating", "isInteger",
    "input", "import", "set", "require",
    "readFile", "writeFile", "append", "concat", "remove", "builtins", "panic", "del",
    "range", "addressOf", "keys", "values", "global", "recover", "BigInteger"
};

std::map<std::string, std::string> *envVars = nullptr;
const std::map<std::string, std::string> requiredEnvVars = { 
    { "native-lib-path", "lib/" },
    { "builtins-import-path", "builtins/" },
    { "version", YAN_LANG_VERSION }
};


namespace builtins {
    using YanArgumentListType = std::vector<Object *> &;
    using YanContext = Context *;
    using YanObject = RuntimeResult *;
    using BuiltinFunctionImplementation = std::function<RuntimeResult *(YanContext)>;
    
    void AllocEnvVars() {
        ::envVars = new std::map<std::string, std::string>;
    }

    bool HasEnvVar(const std::string &key) {
        if (envVars == nullptr) {
            std::cerr << "Fatal: Env has not been allocated, builtins::AllocEnv() should be called before operation" << std::endl;
            assert(false);
        }
        return ::envVars->find(key) != ::envVars->end();
    }

    void SetEnvVar(const std::string &key, const std::string &val) {
        if (envVars == nullptr) {
            std::cerr << "Fatal: Env has not been allocated, builtins::AllocEnv() should be called before operation" << std::endl;
            assert(false);
        }
        if (!HasEnvVar(key)) {
            ::envVars->insert(std::make_pair(key, val));
        } else {
            (*::envVars)[key] = val;
        }
    }

    std::string GetEnvVar(const std::string &key) {
        if (envVars == nullptr) {
            std::cerr << "Fatal: Env has not been allocated, builtins::AllocEnv() should be called before operation" << std::endl;
            assert(false);
        }
        if (!HasEnvVar(key)) {
            std::cerr << "Fatal: Invilid env key: " << key << std::endl;
            assert(false);
        }
        return (*::envVars)[key];
    }

    // default
    void RestoreEnvVar() {
        if (envVars == nullptr) {
            std::cerr << "Fatal: Env has not been allocated, builtins::AllocEnv() should be called before operation" << std::endl;
            assert(false);
        }
        std::fstream file;
        file.open("./env.def", std::ios::in);
        std::string buf;

        if (file.is_open()) {
            while (std::getline(file, buf)) {
                auto def = Split(buf, "=");
                if (def.size() != 2) {
                    std::cerr << "Fatal: Invilid env definition: " << buf << std::endl;
                    assert(false); 
                }

                SetEnvVar(def[0], def[1]);
            }      
            file.close();        
        }

        for (auto [requiredKey, defaultVal] : requiredEnvVars) {
            if (!HasEnvVar(requiredKey)) {
                SetEnvVar(requiredKey, defaultVal);
            }
        }
    }

    std::map<std::string, std::string> GetVars() {
        auto vars = *::envVars;
        return vars;
    }

    struct YanModule {
        YanModule(const std::string &moduleName) : moduleName(moduleName) {}
        YanModule *AddSymbol(const std::string &symbolName, std::vector<std::string> &&decl) {
            if (decl.size() != 0) {
                this->functionArgumentDeclearation.insert(std::make_pair(symbolName, decl));
            }
            this->symbols.push_back(symbolName);
            return this;
        }

        std::string moduleName;
        std::vector<std::string> symbols;
        std::map<std::string, std::vector<std::string>> functionArgumentDeclearation;
    };

    using YanModuleDeclearation = YanModule *;
    using ModuleLoaderFunc = YanModuleDeclearation (*)();

    
    [[noreturn]] YanObject Invilid(YanContext ctx) {
        std::cerr << "Fatal: Unknown builtin access" << std::endl;
        assert(false);
    }

    Error *AssertYanTypeMatches(YanContext ctx, Object *obj, const std::string &argName, const std::vector<std::string> &validTypes) {
        if (!Lexer::Contains(validTypes, std::string(obj->typeName))) {
            return new TypeError(
                std::format("Type of argument '{}' mismatched: Requires one of {} but got {}", argName, StringifyStringSequence(validTypes), obj->typeName),
                obj->startPos, obj->endPos, ctx
            );
        }
        return nullptr;
    }

    namespace IO {
        YanObject Println(YanContext ctx) {
            std::cout << ctx->symbols->Get("_str")->ToString() << std::endl;
            return (new RuntimeResult)->Success(Number::null);
        }

        YanObject Readline(YanContext ctx) {
            std::string s;
            std::getline(std::cin, s);
            return (new RuntimeResult)->Success(new String(s));
        }

        YanObject Input(YanContext ctx) {
            auto prompt = ctx->symbols->Get("__prompt__");
            if (prompt != nullptr) {
                std::cout << prompt->ToString();
            }
            std::string s;
            std::getline(std::cin, s);
            return (new RuntimeResult)->Success(new String(s));
        }

        YanObject ReadFile(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_filename");
            auto err = AssertYanTypeMatches(ctx, arg, "_filename", { "String" });
            if (err != nullptr) {
                return result->Failure(err);
            }
            auto file = As<String>(arg)->s;

            std::ifstream ifs;
            ifs.open(file, std::ios::in);
            if (!ifs.is_open()) {
                return result->Failure(new OSError(
                    std::format("Failed to open file: '{}'", file),
                    arg->startPos, arg->endPos, ctx
                ));
            }
            std::ostringstream oss;
            oss << ifs.rdbuf();
            auto content = oss.str();
            ifs.close();

            return result->Success(new String(content));
        }

        YanObject WriteFile(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg1 = ctx->symbols->Get("_filename");
            auto arg2 = ctx->symbols->Get("_str");
            auto arg3 = ctx->symbols->Get("__mode__");
            std::fstream fileStream;
            std::ios_base::openmode mode;

            if (arg3 != nullptr) {
                if (arg3->typeName != std::string("String")) {
                    return result->Failure(new TypeError(
                        "File open mode should be a string",
                        arg3->startPos, arg3->endPos, ctx
                    ));
                }
                auto modeString = As<String>(arg3)->s;
                if (modeString == "w") {
                    mode = std::ios::out;
                } else if (modeString == "wa") {
                    mode = std::ios::out | std::ios::app;
                } else {
                    return result->Failure(new ValueError(
                        std::format("Invilid file open mode: \'{}\'", modeString),
                        arg3->startPos, arg3->endPos, ctx
                    ));
                }
            } else {
                mode = std::ios::out;
            }

            if (arg1->typeName != std::string("String")) {
                return result->Failure(new TypeError(
                    "Expected a path with string type",
                    arg3->startPos, arg3->endPos, ctx
                ));
            }
            if (arg2->typeName != std::string("String")) {
                return result->Failure(new TypeError(
                    "Content should be a string",
                    arg2->startPos, arg2->endPos, ctx
                ));
            }

            auto filePath = As<String>(arg1)->s;
            auto content = As<String>(arg2)->s;
            fileStream.open(filePath, mode);
            if (!fileStream.is_open()) {
                return result->Failure(new OSError(
                    std::format("Failed to open file: '{}'", filePath),
                    arg1->startPos, arg1->endPos, ctx
                ));
            }
            fileStream << content;
            fileStream.close();
            return result->Success(Number::null);
        }

        YanObject Print(YanContext ctx) {
            std::cout << ctx->symbols->Get("_str")->ToString();
            return (new RuntimeResult)->Success(Number::null);
        }
    }

    namespace Math {
        bool HoldsInteger(Number *n) {
            return std::holds_alternative<int>(n->value);
        }
        
        int GetInt(Number *n) {
            assert(HoldsInteger(n));
            return std::get<int>(n->value);
        }

        double GetFloat(Number *n) {
            assert(!HoldsInteger(n));
            return std::get<double>(n->value);
        }

        // For only 1 argument
        template<typename T>
        YanObject _MathFunc(YanContext ctx, YanObject o, T func) {
            auto arg = ctx->symbols->Get("_x");
            auto err = AssertYanTypeMatches(ctx, arg, "_x", { "Number" });
            if (err != nullptr) {
                return o->Failure(err);
            }
            auto num = dynamic_cast<Number *>(arg);
            
            return o->Success(new Number(func(HoldsInteger(num) ? GetInt(num) : GetFloat(num))));
        }

        YanObject Sin(YanContext ctx) {
            auto result = new RuntimeResult;
            auto sin_ = [](double x) { return sin(x); };
            return _MathFunc(ctx, result, sin_);
        }

        YanObject Cos(YanContext ctx) {
            auto result = new RuntimeResult;
            auto cos_ = [](double x) { return cos(x); };
            return _MathFunc(ctx, result, cos_);
        }

        YanObject Tan(YanContext ctx) {
            auto result = new RuntimeResult;
            auto tan_ = [](double x) { return tan(x); };
            return _MathFunc(ctx, result, tan_);
        }

        YanObject Abs(YanContext ctx) {
            auto result = new RuntimeResult;
            auto abs_ = [](double x) { return x > 0 ? x : -x; };
            return _MathFunc(ctx, result, abs_);
        }

        YanObject Log(YanContext ctx) {
            auto result = new RuntimeResult;
            auto log_ = [](double x) { return log10(x); };
            return _MathFunc(ctx, result, log_);
        }

        YanObject Ln(YanContext ctx) {
            auto result = new RuntimeResult;
            auto ln_ = [](double x) { return log(x); };
            return _MathFunc(ctx, result, ln_);
        }

        YanObject Sqrt(YanContext ctx) {
            auto result = new RuntimeResult;
            auto sqrt_ = [](double x) { return sqrt(x); };
            return _MathFunc(ctx, result, sqrt_);
        }

        YanObject IsFloating(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_num");
            auto err = AssertYanTypeMatches(ctx, arg, "_num", { "Number" });
            if (err != nullptr) {
                return result->Failure(err);
            }

            auto num = As<Number>(arg);
            int v = (int) !HoldsInteger(num);
            return result->Success(new Number(v));
        }

        YanObject IsInteger(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_num");
            auto err = AssertYanTypeMatches(ctx, arg, "_num", { "Number" });
            if (err != nullptr) {
                return result->Failure(err);
            }

            auto num = As<Number>(arg);
            int v = (int) HoldsInteger(num);
            return result->Success(new Number(v));
        }
    }

    YanObject Len(YanContext ctx) {
        auto result = new RuntimeResult;
        auto len = ctx->symbols->Get("_seq")->Len();
        if (len.first == nullptr) {
            return result->Failure(len.second);
        }
        return result->Success(len.first);
    }

    namespace List_ {
        YanObject Append(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_lst");
            auto o = ctx->symbols->Get("_o");
            auto err = AssertYanTypeMatches(ctx, arg, "_lst", { "List" });
            if (err != nullptr) {
                return result->Failure(err);
            }
            auto lst = As<List>(arg);
            lst->elements.push_back(o);
            return result->Success(Number::null);
        } 

        YanObject Remove(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_lst");
            auto arg2 = ctx->symbols->Get("_idx");
            auto err = AssertYanTypeMatches(ctx, arg, "_lst", { "List" });
            auto err2 = AssertYanTypeMatches(ctx, arg2, "_idx", { "Number" });
            if (err != nullptr) {
                return result->Failure(err);
            }
            if (err2 != nullptr) {
                return result->Failure(err2);
            }

            auto lst = As<List>(arg);
            auto idx = As<Number>(arg2);
            if (!Math::HoldsInteger(idx)) {
                return result->Failure(new TypeError(
                    "List index must be an integer",
                    idx->startPos, idx->endPos, ctx
                ));
            }
            auto indexReal = Math::GetInt(idx);
            if (indexReal < 0) {
                return result->Failure(new TypeError(
                    "List index must be a positive integer",
                    idx->startPos, idx->endPos, ctx
                ));  
            }
            lst->elements.erase(lst->elements.begin() + indexReal);
            return result->Success(Number::null);
        }

        YanObject Concat(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_lst1");
            auto arg2 = ctx->symbols->Get("_lst2");
            auto err = AssertYanTypeMatches(ctx, arg, "_lst1", { "List" });
            auto err2 = AssertYanTypeMatches(ctx, arg2, "_lst2", { "List" });
            if (err != nullptr) {
                return result->Failure(err);
            }
            if (err2 != nullptr) {
                return result->Failure(err2);
            }

            auto lst = As<List>(arg);
            auto idx = As<List>(arg2);
            for (auto &e : idx->elements) {
                lst->elements.push_back(e);
            }
            return result->Success(Number::null);
        }

        YanObject Range(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg1 =  ctx->symbols->Get("_a");
            auto arg2 = ctx->symbols->Get("__b__");
            auto arg3 = ctx->symbols->Get("__c__");
            Number *a = nullptr;
            Number *b = nullptr;
            Number *c = nullptr;

            auto checkArg = [](Number *n) { 
                return ::builtins::Math::HoldsInteger(n) && n->ntype == NumberType::Int && ::builtins::Math::GetInt(n) >= 0;
            };

            if (arg1->typeName == "Number") {
                a = As<Number>(arg1);
                if (!checkArg(a)) {
                    return result->Failure(new ValueError(
                        "range(x) requires an positive integer",
                        arg1->startPos, arg1->endPos, ctx
                    ));
                }
            } else {
                return result->Failure(new TypeError(
                    "range() requires number as parameters",
                    arg1->startPos, arg1->endPos, ctx
                ));
            }
            assert(!(arg2 == nullptr && arg3 != nullptr));

            auto check = [ctx, checkArg](Object *arg1) {
                auto result = new RuntimeResult;
                if (arg1->typeName == "Number") {
                    auto n = As<Number>(arg1);
                    if (!checkArg(n)) {
                        return result->Failure(new ValueError(
                            "range(x) requires an positive integer",
                            arg1->startPos, arg1->endPos, ctx
                        ));
                    }
                    return result->Success(nullptr);
                } else {
                    return result->Failure(new TypeError(
                        "range() requires number as parameters",
                        arg1->startPos, arg1->endPos, ctx
                    ));
                }
            };

            if (arg2 != nullptr && arg3 == nullptr) {
                result->Register(check(arg2));
                if (result->ShouldReturn()) {
                    return result;
                }
                b = As<Number>(arg2);
                std::vector<Object *> iter;
                for (int i = ::builtins::Math::GetInt(a); i < ::builtins::Math::GetInt(b); i++) {
                    iter.push_back(new Number(i));
                } 
                return result->Success(new List(iter));
            } else if (arg2 != nullptr && arg3 != nullptr) {
                result->Register(check(arg2));
                if (result->ShouldReturn()) {
                    return result;
                }
                result->Register(check(arg3));
                if (result->ShouldReturn()) {
                    return result;
                }
                b = As<Number>(arg2);
                c = As<Number>(arg3);
                
                std::vector<Object *> iter;
                for (int i = ::builtins::Math::GetInt(a); i < ::builtins::Math::GetInt(b); i += ::builtins::Math::GetInt(c)) {
                    iter.push_back(new Number(i));
                } 
                return result->Success(new List(iter));
            } else {
                std::vector<Object *> iter;
                for (int i = 0; i < ::builtins::Math::GetInt(a); i++) {
                    iter.push_back(new Number(i));
                } 
                return result->Success(new List(iter));
            }
        }

        YanObject Keys(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_dict");
            auto err = AssertYanTypeMatches(ctx, arg, "_dict", { "Dictionary" });
            if (err != nullptr) {
                return result->Failure(err);
            }
            auto dict = As<Dictionary>(arg);
            return result->Success(new List(dict->GetKeys()));
        }

        YanObject Values(YanContext ctx) {
            auto result = new RuntimeResult;
            auto arg = ctx->symbols->Get("_dict");
            auto err = AssertYanTypeMatches(ctx, arg, "_dict", { "Dictionary" });
            if (err != nullptr) {
                return result->Failure(err);
            }

            auto dict = As<Dictionary>(arg);
            std::vector<Object *> values;
            for (auto &[k, v] : dict->elements) {
                values.push_back(v);
            }
            return result->Success(new List(values));
        }
    }
    
    YanObject Set(YanContext ctx) {
        auto result = new RuntimeResult;
        auto argLst = ctx->symbols->Get("_lst");
        auto argIdx = ctx->symbols->Get("_idx");
        auto argValue = ctx->symbols->Get("_value");
        auto err1 = AssertYanTypeMatches(ctx, argLst, "_lst", { "List" });
        if (err1 != nullptr) {
            return result->Failure(err1);
        }
        auto err2 = AssertYanTypeMatches(ctx, argIdx, "_idx", { "Number" });
        if (err2 != nullptr) {
            return result->Failure(err2);
        }

        auto lst = As<List>(argLst);
        auto index = As<Number>(argIdx);
        if (index->ntype != NumberType::Int) {
            return result->Failure(new ValueError(
                "List index must be an integer",
                index->startPos, index->endPos, ctx
            ));
        } else if (Math::GetInt(index) < 0) {
            return result->Failure(new ValueError(
                "Negative index not allowed here",
                index->startPos, index->endPos, ctx
            ));
        }

        lst->elements[Math::GetInt(index)] = argValue;
        return result->Success(Number::null);
    }

    YanObject _InterpreteModule(const std::string &moduleFile, const std::string &symbolName, Position *st, Position *et, YanContext ctx) {
        auto result = new RuntimeResult;
        std::ifstream ifs;
        ifs.open(moduleFile, std::ios::in);
        if (!ifs.is_open()) {
            ifs.clear();
            ifs.open(std::format("{}/{}", GetEnvVar("builtins-import-path"), moduleFile), std::ios::in);
            if (!ifs.is_open()) {
                return result->Failure(new OSError(
                    std::format("Failed to open module: '{}'", moduleFile),
                    st, et, ctx
                ));
            }
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        auto code = oss.str();

        auto lexer = new Lexer(moduleFile, ToWideString(code));
        auto [tokens, err] = lexer->MakeTokens();
        if (err != nullptr) {
            return result->Failure(err);
        }
        auto parser = new Parser(tokens);
        auto parseR = parser->Parse();
        if (parseR->err != nullptr) {
            return result->Failure(parseR->err);
        }
        auto moduleContext = new Context(std::format("<module '{}'>", moduleFile));
        moduleContext->parent = ctx;
        moduleContext->parentEntry = st;
        moduleContext->symbols = new SymbolTable;
        SetBuiltins(moduleContext->symbols);
        auto interpreter = new Interpreter;
        result->Register(interpreter->Visit(parseR->ast, moduleContext));
        if (result->ShouldReturn()) {
            return result->Failure(new RuntimeError(
                std::format("Failed to load module: '{}'", moduleFile),
                st, et, ctx
            ), result->error);
        }

        auto symbol = moduleContext->symbols->Get(symbolName);
        if (symbol == nullptr) {
            return result->Failure(new RuntimeError(
                std::format("Module '{}' has no symbol '{}'", moduleFile, symbolName),
                st, et, ctx
            ));
        }
        // if (symbol->typeName != "Function") {
        //     return result->Failure(new ValueError(
        //         "Only functions can be imported", st, et, ctx
        //     ));
        // }

        auto symbolCopy = symbol->Copy();
        moduleContextCache.insert(std::make_pair(moduleFile, moduleContext));        
        symbolsModuleLocation.insert(std::make_pair(symbolName, moduleFile));
        delete lexer;
        delete parser;
        delete parseR;
        delete interpreter;
        return result->Success(symbolCopy);
    }

    YanObject _ImportModule(const std::string &moduleFile, YanContext dest, Position *st, Position *et, YanContext ctx) {
        auto result = new RuntimeResult;
        std::ifstream ifs;
        ifs.open(moduleFile, std::ios::in);
        if (!ifs.is_open()) {
            ifs.clear();
            ifs.open(std::format("{}/{}", GetEnvVar("builtins-import-path"), moduleFile), std::ios::in);
            if (!ifs.is_open()) {
                return result->Failure(new OSError(
                    std::format("Failed to open module: '{}'", moduleFile),
                    st, et, ctx
                ));
            }
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        auto code = oss.str();

        auto lexer = new Lexer(moduleFile, ToWideString(code));
        auto [tokens, err] = lexer->MakeTokens();
        if (err != nullptr) {
            return result->Failure(err);
        }
        auto parser = new Parser(tokens);
        auto parseR = parser->Parse();
        if (parseR->err != nullptr) {
            return result->Failure(parseR->err);
        }
        auto moduleContext = new Context(std::format("<module '{}'>", moduleFile));
        moduleContext->parent = ctx;   
        moduleContext->parentEntry = st;             
        moduleContext->symbols = new SymbolTable;
        SetBuiltins(moduleContext->symbols);
        auto interpreter = new Interpreter;
        result->Register(interpreter->Visit(parseR->ast, moduleContext));
        if (result->ShouldReturn()) {
            return result->Failure(new RuntimeError(
                std::format("Failed to load module: '{}'", moduleFile),
                st, et, ctx
            ), result->error);
        }
        moduleContextCache.insert(std::make_pair(moduleFile, moduleContext));

        for (auto &[symbolName, symbol] : moduleContext->symbols->symbols) {
            if (symbol->typeName != "BuiltinFunction" || !Lexer::Contains(builtinNames, symbolName)) {
                dest->symbols->Set(symbolName, symbol->Copy());
                symbolsModuleLocation.insert(std::make_pair(symbolName, moduleFile));
            }
        }

        return result->Success(Number::null);
    }
    
    YanObject _useModule(const std::string &moduleFile, YanContext dest, Position *st, Position *et, YanContext ctx) {
        auto result = new RuntimeResult;
        std::ifstream ifs;
        ifs.open(moduleFile + ".yan", std::ios::in);
        if (!ifs.is_open()) {
            ifs.clear();
            ifs.open(std::format("{}/{}", GetEnvVar("builtins-import-path"), moduleFile + ".yan"), std::ios::in);
            if (!ifs.is_open()) {
                return result->Failure(new OSError(
                    std::format("Failed to open module: '{}'", moduleFile),
                    st, et, ctx
                ));
            }
        }
        std::ostringstream oss;
        oss << ifs.rdbuf();
        auto code = oss.str();

        auto lexer = new Lexer(moduleFile, ToWideString(code));
        auto [tokens, err] = lexer->MakeTokens();
        if (err != nullptr) {
            return result->Failure(err);
        }
        auto parser = new Parser(tokens);
        auto parseR = parser->Parse();
        if (parseR->err != nullptr) {
            return result->Failure(parseR->err);
        }
        auto moduleContext = new Context(std::format("<module '{}'>", moduleFile));
        moduleContext->parent = ctx;        
        moduleContext->parentEntry = st;        
        moduleContext->symbols = new SymbolTable;
        moduleContext->SetExternal(true);
        SetBuiltins(moduleContext->symbols);
        auto interpreter = new Interpreter;
        result->Register(interpreter->Visit(parseR->ast, moduleContext));
        if (result->ShouldReturn()) {
            return result->Failure(new RuntimeError(
                std::format("Failed to load module: '{}'", moduleFile),
                st, et, ctx
            ), result->error);
        }
        moduleContextCache.insert(std::make_pair(moduleFile, moduleContext));

        std::map<Object *, Object *> moduleSymbols;
        for (auto &[symbolName, symbol] : moduleContext->symbols->symbols) {
            if (symbol->typeName != "BuiltinFunction" || !Lexer::Contains(builtinNames, symbolName)) {
                moduleSymbols.insert(std::make_pair(new String(symbolName), symbol));
                symbolsModuleLocation.insert(std::make_pair(symbolName, moduleFile));
            }
        }

        auto p = Split(moduleFile, "/");
        std::string importName {};
        if (p.size() != 1) {
            importName = p[p.size() - 1];
        } else {
            importName = moduleFile;
        }
        dest->symbols->Set(importName, new Dictionary(moduleSymbols));
        return result->Success(Number::null);
    }

    // YanObject _LoadNativeModuleFromStub(const std::string &moduleStub, YanContext dest, Position *st, Position *et, YanContext ctx) {

    // }

    // auto ::LoadNativeFunctionImplementation(const std::string &dynamicLib, const std::string &name) -> RuntimeResult *(*)(Context *);
    YanObject _LoadNativeSymbol(const std::string &mod, const std::string &symbolName, Position *st, Position *et, YanContext ctx);

    YanObject Import(YanContext ctx) {
        auto result = new RuntimeResult;
        auto arg = ctx->symbols->Get("_symbol");
        auto err = AssertYanTypeMatches(ctx, arg, "_symbol", { "String" });
        if (err != nullptr) {
            return result->Failure(err);
        }
        auto argString = As<String>(arg)->s;

        if (!argString.starts_with("@")) {
            auto symbolInfo = Split(argString, ".");
            if (symbolInfo.size() != 1 && symbolInfo.size() != 2) {
                return result->Failure(new ValueError(
                    std::format("Invilid import specification: '{}'", arg->ToString()),
                    arg->startPos, arg->endPos, ctx
                ));
            }
            if (symbolInfo.size() == 2) {
                auto symbolSourceFile = symbolInfo[0] + ".yan";
                auto symbolName = symbolInfo[1];
                auto symbol = _InterpreteModule(symbolSourceFile, symbolName, arg->startPos, arg->endPos, ctx);
                if (symbol->error != nullptr) {
                    return result->Failure(symbol->error, symbol->cause);
                }

                return result->Success(symbol->value);
            } else {
                auto destCtx = ctx->parent;
                auto status = _useModule(symbolInfo[0], destCtx, arg->startPos, arg->endPos, ctx);
                if (status->error != nullptr) {
                    return result->Failure(status->error, status->cause);
                }
                return result->Success(Number::null);
            }
        } else {
            return result->Failure(new ValueError(
                "\'import\' does not support to load an native implementation",
                arg->startPos, arg->endPos, ctx
            ));
        }
    }

    YanObject Require(YanContext ctx) {
        auto result = new RuntimeResult;
        auto arg = ctx->symbols->Get("_module");
        auto err = AssertYanTypeMatches(ctx, arg, "_module", { "String" });
        if (err != nullptr) {
            return result->Failure(err);
        }

        auto argString = As<String>(arg)->s;
        if (!argString.starts_with("@")) {
            auto moduleName = argString + ".yan";
            auto destCtx = ctx->parent;
            auto status = _ImportModule(moduleName, destCtx, arg->startPos, arg->endPos, ctx);
            if (status->error != nullptr) {
                return result->Failure(status->error, status->cause);
            }

            return result->Success(Number::null);
        } else {
            auto moduleDescr = Split(argString.substr(1), ".");
            if (moduleDescr.size() != 2) {
                return result->Failure(new ValueError(
                    "Invilid symbol location",
                    arg->startPos, arg->endPos, ctx
                ));
            }
            auto moduleName = moduleDescr[0];
            auto symbol = moduleDescr[1];

            auto destCtx = ctx->parent;
            auto symbolValue = _LoadNativeSymbol(moduleName, symbol, arg->startPos, arg->endPos, ctx);
            if (symbolValue->error != nullptr) {
                return result->Failure(symbolValue->error);
            }
            return result->Success(symbolValue->value);
        }
    }

    YanObject Eval(YanContext ctx) {
        auto result = new RuntimeResult;
        auto code = ctx->symbols->Get("_code");
        if (code->typeName != std::string("String")) {
            return result->Failure(new TypeError(
                std::format("Argument '_code' must be a String (got {})", code->typeName),
                code->startPos, code->endPos, ctx
            ));
        }

        auto evaluationFrameId = std::format("<eval frame at {}>", (void *) ctx);
        Interprete(evaluationFrameId, dynamic_cast<String *>(code)->s, InterpreterStartMode::Evaluation, evaluationFrameId, ctx, code->startPos);
        return result->Success(Number::null);
    }

    YanObject ParseInt(YanContext ctx) {
        auto result = new RuntimeResult;
        auto yanStr = ctx->symbols->Get("_str");
        if (yanStr->typeName != std::string("String")) {
            return result->Failure(new TypeError(
                std::format("Argument '_str' must be a String (got {})", yanStr->typeName),
                yanStr->startPos, yanStr->endPos, ctx
            ));
        }
        return (new RuntimeResult)->Success(new Number((int) Lexer::ParseInt(dynamic_cast<String *>(yanStr)->s)));
    }

    YanObject ParseFloat(YanContext ctx) {
        auto result = new RuntimeResult;
        auto yanStr = ctx->symbols->Get("_str");
        if (yanStr->typeName != std::string("String")) {
            return result->Failure(new TypeError(
                std::format("Argument '_str' must be a String (got {})", yanStr->typeName),
                yanStr->startPos, yanStr->endPos, ctx
            ));
        }
        return (new RuntimeResult)->Success(new Number((double) Lexer::ParseFloat(dynamic_cast<String *>(yanStr)->s)));
    }

    YanObject ToString(YanContext ctx) {
        return (new RuntimeResult)->Success(new String(ctx->symbols->Get("_object")->ToString()));
    }

    YanObject TypeNameOf(YanContext ctx) {
        auto obj = ctx->symbols->Get("_object");
        if (obj->typeName == std::string("ClassObject")) {
            return (new RuntimeResult)->Success(new String(std::format("[Class {}]", dynamic_cast<ClassObject *>(obj)->className)));
        } else {
            return (new RuntimeResult())->Success(new String(std::format("[builtins.{}]", std::string(obj->typeName))));
        }
    }

    YanObject Builtins(YanContext ctx) {
        auto builtinLst = new List({});
        for (const auto &n : builtinNames)  {
            builtinLst->elements.push_back(new String(n));
        }
        return (new RuntimeResult)->Success(builtinLst);
    }

    YanObject Panic(YanContext ctx) {
        auto err = ctx->symbols->Get("_err");
        return (new RuntimeResult)->Failure(new ::Panic(err->ToString(), err->startPos, err->endPos, ctx));
    }

    YanObject Del(YanContext ctx) {
        auto result = new RuntimeResult;
        auto arg = ctx->symbols->Get("_varName");
        if (arg->typeName != "String") {
            return result->Failure(new TypeError(
                "del() requires a string name of variable",
                arg->startPos, arg->endPos, ctx
            ));
        }

        auto delVarName = As<String>(arg);
        if (ctx->symbols->Get(delVarName->s) == nullptr) {
            return result->Failure(new ValueError(
                std::format("del('{}'): not defined", delVarName->s),
                arg->startPos, arg->endPos, ctx
            ));
        }

        auto var = ctx->symbols->Get(delVarName->s);
        if (var->typeName == "BuiltinFunction") {
            return result->Failure(new RuntimeError(
                std::format("Attempted to delete non-user defined function: '{}'", delVarName->s),
                arg->startPos, arg->endPos, ctx
            ));
        }
        ctx->parent->symbols->Remove(delVarName->s);
        delete var;
        return result->Success(Number::null);
    }

    YanObject AddressOf(YanContext ctx) {
        return (new RuntimeResult)->Success(
            new String(std::format("{}", static_cast<void *>(ctx->symbols->Get("_object"))))
        );
    }

    YanObject Global(YanContext ctx) {
        auto result = new RuntimeResult;
        auto arg = ctx->symbols->Get("_varName");
        auto value = ctx->symbols->Get("_value");

        if (arg->typeName != std::string("String")) {
            return result->Failure(new TypeError(
                "Variable name must be a string",
                arg->startPos, arg->endPos, ctx
            ));
        }

        auto varName = As<String>(arg)->s;
        if (ctx->global->Get(varName) == nullptr)  {
            return result->Failure(new ValueError(
                std::format("Variable '{}' is not declared in global scope", varName),
                arg->startPos, arg->endPos, ctx
            ));
        }

        ctx->global->Set(varName, value);
        return result->Success(Number::null);
    }

    YanObject Recover(YanContext ctx) {
        auto result = new RuntimeResult;
        auto arg = ctx->parent->symbols->Get("__lastexc__");

        if (!ctx->parent || !ctx->parent->parent) {
            return result->Failure(new ValueError(
                "Cannot recover from top-level scope or a normal function",
                nullptr, nullptr, ctx
            ));
        }
        auto ret = ctx->symbols->Get("__ret__");

        if (arg != nullptr) {
            ctx->parent->symbols->Remove("__lastexc__");
            if (!ret) {
                ctx->parent->parent->symbols->Set("__recovered__", Number::null);            
            } else {
                ctx->parent->parent->symbols->Set("__recovered__", ret);            
            }
        }
        return result->Success(Number::null);
    }

    YanObject BigInteger(YanContext ctx);
}

const std::map<std::string, builtins::BuiltinFunctionImplementation> builtinFuncIndexes {
    { "print", builtins::IO::Print },
    { "println", builtins::IO::Println },
    { "readLine", builtins::IO::Readline },
    { "typeof", builtins::TypeNameOf },
    { "len", builtins::Len },
    { "parseInt", builtins::ParseInt },
    { "parseFloat", builtins::ParseFloat },
    { "str", builtins::ToString },
    { "eval", builtins::Eval },
    { "sin", builtins::Math::Sin },
    { "cos", builtins::Math::Cos },
    { "tan", builtins::Math::Tan },
    { "abs", builtins::Math::Abs },
    { "log", builtins::Math::Log },
    { "ln", builtins::Math::Ln },
    { "sqrt", builtins::Math::Sqrt },
    { "input", builtins::IO::Input },
    { "import", builtins::Import },
    { "set", builtins::Set },
    { "require", builtins::Require },
    { "readFile", builtins::IO::ReadFile },
    { "writeFile", builtins::IO::WriteFile },
    { "append", builtins::List_::Append },
    { "concat", builtins::List_::Concat },
    { "remove", builtins::List_::Remove },
    { "builtins", builtins::Builtins },
    { "panic", builtins::Panic },
    { "isFloating", builtins::Math::IsFloating },
    { "isInteger", builtins::Math::IsInteger },
    { "del", builtins::Del },
    { "range", builtins::List_::Range },
    { "addressOf", builtins::AddressOf },
    { "keys", builtins::List_::Keys },
    { "values", builtins::List_::Values },
    { "global", builtins::Global },
    { "recover", builtins::Recover },
    { "BigInteger", builtins::BigInteger }
};

const std::map<std::string, std::vector<std::string>> builtinFuncParamsRegistry {
    { "print" , { "_str" } },
    { "println", { "_str" } },
    { "typeof", { "_object" } },
    { "len", { "_seq" } },
    { "parseInt", { "_str" } },
    { "parseFloat", { "_str" } },
    { "str", { "_object" } },
    { "eval", { "_code" } },
    { "sin", { "_x" } },
    { "cos", { "_x" } },
    { "tan", { "_x" } },
    { "abs", { "_x" } },
    { "log", { "_x" } },
    { "ln", { "_x" } },
    { "sqrt", { "_x" } },
    { "input", { "__prompt__" } },
    { "import", { "_symbol" } },
    { "set", { "_lst", "_idx", "_value" } },
    { "require", { "_module" } },
    { "readFile", { "_filename" } },
    { "writeFile", { "_filename", "_str", "__mode__" } },
    { "append", { "_lst", "_o" } },
    { "concat", { "_lst1", "_lst2" } },
    { "remove", { "_lst", "_idx" } },
    { "panic", { "_err" } },
    { "isFloating", { "_num" } },
    { "isInteger", { "_num" } },
    { "del", { "_varName" } },
    { "range", { "_a", "__b__", "__c__" } },
    { "addressOf", { "_object" } },
    { "keys", { "_dict" } },
    { "values", { "_dict" } },
    { "global", { "_varName", "_value" } },
    { "recover", { "__ret__" } },
    { "BigInteger", { "__val__" } }
};

#ifdef __linux__
    using DylibType = void *;
#elif defined(_WIN32)
    using DylibType = win32::HMODULE;
#endif
static std::map<std::string, std::pair<std::string, void *>> dynamicLoadedSymbol;
std::map<std::string, DylibType> dylibs;
std::map<std::string, builtins::YanModuleDeclearation> nativeModules;

struct BuiltinFunction : public FunctionBase {
    explicit BuiltinFunction(const std::string &name) : FunctionBase(name), Object("BuiltinFunction") {}
    std::vector<std::string> argDeclearation;
    bool dynamicBind = false;
    builtins::BuiltinFunctionImplementation dynamicImpl = nullptr;

    RuntimeResult *Execute(std::vector<Object *> args) {
        auto result = new RuntimeResult;
        auto frameContext = this->GenerateNewContext();
        auto builtinFunctionName = this->functionName;
        builtins::BuiltinFunctionImplementation func;

        if (currentCallStackDepth > 1) {
             currentCallStackDepth--; // built-in call does not take times of max calls
        }
        if (!this->dynamicBind) {
            if (builtinFuncIndexes.find(builtinFunctionName) == builtinFuncIndexes.end()) {
                if (dynamicLoadedSymbol.find(builtinFunctionName) != dynamicLoadedSymbol.end()) {
                    func = (RuntimeResult *(*)(Context *)) dynamicLoadedSymbol.at(builtinFunctionName).second;
                } else {
                    func = builtins::Invilid;
                }
            } else {
                func = builtinFuncIndexes.at(builtinFunctionName);
            }
        } else {
            func = this->dynamicImpl;
        }
        // std::cout << std::boolalpha << this->dynamicBind << std::endl;
        assert(func != nullptr);

        if (builtinFuncParamsRegistry.find(builtinFunctionName) != builtinFuncParamsRegistry.end() && this->argDeclearation.size() == 0) {
            result->Register(this->CheckAndPopulate(builtinFuncParamsRegistry.at(builtinFunctionName), args, frameContext));
        } else if (this->argDeclearation.size() != 0) {
            if (this->argDeclearation.size() == 1) {
                if (this->argDeclearation[0] == std::string("void")) {
                    if (args.size() != 0) {
                        result->Register((new RuntimeResult)->Failure(new RuntimeError(
                            std::format("Function '{}' decleared as `void` should not take argument(s) [got {}]", functionName, args.size()),
                            this->startPos, this->endPos, this->ctx
                        )));
                        return result;
                    }
                }
            }
            result->Register(this->CheckAndPopulate(this->argDeclearation, args, frameContext));
        }
        if (result->ShouldReturn()) {
            return result;
        }

        auto returnValue = result->Register(func(frameContext));
        if (result->ShouldReturn()) {
            return result;
        }
        return result->Success(returnValue);
    }

    void Bind(const std::vector<std::string> &argDeclearation, builtins::BuiltinFunctionImplementation impl) {
        this->dynamicBind = true;
        this->argDeclearation = argDeclearation;
        this->dynamicImpl = impl;
    }

    void Unbind() {
        this->dynamicBind = false;
        this->argDeclearation.clear();
        this->dynamicImpl = nullptr;
    }

    RuntimeResult *CheckArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args) override {
        auto result = new RuntimeResult;
        unsigned expectedMost = 0, expectedLeast = 0;
        for (auto argName : argNames) {
            expectedMost++;
            if (!argName.starts_with("__") || !argName.ends_with("__")) {
                expectedLeast++;
            }
        }

        if (args.size() > expectedMost) {
            return result->Failure(new TypeError(
                std::format("Too many arguments given to function '{}' (Expected {} to {} args but got {})", this->functionName, expectedLeast, expectedMost, args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        } else if (args.size() < expectedLeast) {
            return result->Failure(new TypeError(
                std::format("Too few arguments given to function '{}' (Expected {} to {} args but got {})", this->functionName, expectedLeast, expectedMost, args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        }
        return result->Success(nullptr);
    }

    Object *Copy() override {
        auto copy = new BuiltinFunction(this->functionName);
        copy->SetContext(this->ctx)->SetPos(this->startPos, this->endPos);
        copy->argDeclearation = this->argDeclearation;
        if (this->dynamicBind) {
            copy->Bind(this->argDeclearation, this->dynamicImpl);
        }
        return copy;
    }

    inline std::string ToString() override {
        if (!this->dynamicBind) {
            return std::format("<builtin-function {} at {}>", this->functionName, (void *) this);
        } else {
            return std::format("<native-function {} at {}>", this->functionName, (void *) this);
        }
    }
};

struct BuiltinMethod : public BuiltinFunction {
    Object *self;

    static BuiltinMethod *FromBuiltinFunction(BuiltinFunction *bf, Object *self) {
        auto m = new BuiltinMethod(bf->functionName);
        m->SetPos(bf->startPos, bf->endPos);
        m->SetContext(bf->ctx);
        m->self = self;
        m->Bind(bf->argDeclearation, bf->dynamicImpl);
        return m;
    }

    explicit BuiltinMethod(const std::string &name)
        : BuiltinFunction(name), Object("BuiltinMethod") {}

    RuntimeResult *CheckArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args) override {
        auto result = new RuntimeResult;
        if (args.size() + 1 != argNames.size()) {
            return result->Failure(new RuntimeError(
                std::format("Invilid arguments given to method '{}' (Expected {} but got {})", this->functionName, argNames.size() - 1, args.size()),
                this->startPos, this->endPos, this->ctx
            ));
        }
        return result->Success(nullptr);
    }
    
    RuntimeResult *PopulateArguments(const std::vector<std::string> &argNames, std::vector<Object *> &args, Context *execCtx) override {
        if (argNames.size() == 0) {
            return BuiltinFunction::PopulateArguments(argNames, args, execCtx);
        }

        execCtx->symbols->Set(argNames[0], this->self);
        std::vector<std::string> remain;
        for (int i = 1; i < argNames.size(); i++) {
            remain.push_back(argNames[i]);
        }

        return BuiltinFunction::PopulateArguments(remain, args, execCtx);
    }

    Object *Copy() override {
        auto copiedMethod = new BuiltinMethod(this->functionName);
        copiedMethod->self = this->self;
        copiedMethod->SetContext(this->ctx);
        copiedMethod->SetPos(this->startPos, this->endPos);
        copiedMethod->argDeclearation = this->argDeclearation;
        if (this->dynamicBind) {
            copiedMethod->Bind(this->argDeclearation, this->dynamicImpl);
        }
        return copiedMethod;
    }

    inline std::string ToString() override {
        std::string tpName;
        if (this->self->typeName == std::string("ClassObject")) {
            tpName = As<ClassObject>(this->self)->className;
        } else {
            tpName = this->self->typeName;
        }
        return std::format("<bound-method {} at {} of object '{}' at {} (dynamically-binded)>", this->functionName, (void *) this, tpName, (void *) self);
    }
};

Object *ClassObject::Instantiate(const std::vector<Object *> &args, Context *ctx, Position *st, Position *et) {
    auto result = new RuntimeResult;
    auto object = As<ClassObject>(this->Copy());
    object->isProto = false;
    auto ctor = this->GetAttr("__init__");
    assert(ctor.first != nullptr);
    auto unboundedCtor = As<BuiltinFunction>(ctor.first);
    auto boundedCtor = BuiltinMethod::FromBuiltinFunction(unboundedCtor, object);

    object->SetContext(ctx)->SetPos(st, et);
    result->Register(boundedCtor->Execute(args));
    if (result->ShouldReturn()) {
        std::cerr << "Internal interpreter error: " << result->error->name << ": " << result->error->details << std::endl;
        std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;
        assert(false);
    }
    return object;
}

static std::map<std::string, BuiltinFunction *> allBuiltins {
    { "print", nullptr },
    { "println", nullptr },
    { "typeof", nullptr },
    { "readLine", nullptr },
    { "len", nullptr },
    { "parseInt", nullptr },
    { "parseFloat", nullptr },
    { "str", nullptr },
    { "eval", nullptr },
    { "sin", nullptr },
    { "cos", nullptr },
    { "tan", nullptr },
    { "abs", nullptr },
    { "log", nullptr },
    { "ln", nullptr },
    { "sqrt", nullptr },
    { "input", nullptr },
    { "import", nullptr },
    { "set", nullptr },
    { "require", nullptr },
    { "readFile", nullptr },
    { "writeFile", nullptr },
    { "append", nullptr },
    { "concat", nullptr },
    { "remove", nullptr },
    { "builtins", nullptr },
    { "del", nullptr },
    { "range", nullptr },
    { "addressOf", nullptr },
    { "keys", nullptr }, 
    { "values", nullptr },
    { "global", nullptr },
    { "recover", nullptr },
    { "BigInteger", nullptr }
};

DylibType OpenDynamicLibrary(const std::string &dylib) {
    // not implemented
    #ifdef __linux__
        auto descr = dlopen(std::format("./yan-{}.so", dylib).c_str(), RTLD_LAZY);
        if (descr == nullptr) {
            descr = dlopen(std::format("{}/yan-{}.so", builtins::GetEnvVar("native-lib-path"), dylib).c_str(), RTLD_LAZY);
            if (descr == nullptr) {
                std::cerr << "Fatal: Dynamic lib '" + dylib << "' opened failed: " << dlerror() << std::endl;
                return nullptr; 
            }
        }
        return descr;
    #elif defined(_WIN32)
        auto s = std::format(".\\yan-{}.dll", dylib).c_str();
        auto dll = win32::LoadLibrary(s);
        if (dll == nullptr) {
            s = std::format("{}\\yan-{}.dll", builtins::GetEnvVar("native-lib-path"), dylib).c_str();
            dll = win32::LoadLibrary(s);
            if (dll == nullptr) {
                std::cerr << "Fatal: Dynamic lib '" + dylib << "' opened failed: [WinError " << win32::GetLastError() << "]" << std::endl;
                return nullptr;
            }
        }
        return dll;
    #else
        return nullptr;
    #endif
}


struct BigInt : public ClassObject {
    BigInteger value;

    explicit BigInt(BigInteger value) : value(value), ClassObject({}) {
        this->typeName = "BigInt";
    }

    static RuntimeResult *CheckType(Object *other) {
        auto result = new RuntimeResult;
        if (other->typeName != std::string("Number") && other->typeName != std::string("BigInt") && other->typeName != std::string("String")) {
            return result->Failure(new TypeError(
                std::format("Invilid operation with big integer with type `{}`", other->typeName),
                other->startPos, other->endPos, other->ctx
            ));
        }
        if (other->typeName == std::string("Number")) {
            if (!builtins::Math::HoldsInteger(As<Number>(other))) {
                return result->Failure(new TypeError(
                    "BigInt could not resolve floating values",
                    other->startPos, other->endPos, other->ctx
                ));
            }
            return result->Success(new Number(1));
        }
        if (other->typeName == std::string("BigInt")) {
            return result->Success(new Number(2));
        }
        return result->Success(new Number(3));
    }

    static RuntimeResult *Create(Object *val) {
        auto result = CheckType(val);
        if (result->ShouldReturn()) {
            return result;
        }

        auto v = result->value;
        if (builtins::Math::GetInt(As<Number>(v)) == 1) {
            return result->Success(new BigInt(builtins::Math::GetInt(As<Number>(val))));
        } else if (builtins::Math::GetInt(As<Number>(v)) == 2) {
            return result->Success(new BigInt(As<BigInt>(val)->value));
        } else {
            return result->Success(new BigInt(As<String>(val)->s));
        }
    }

    using ObjectWithError = std::pair<Object *, Error *>;
    ObjectWithError AddTo(Object *other) override {
        auto result = CheckType(other);
        if (result->ShouldReturn()) {
            return std::make_pair(nullptr, result->error);
        }

        auto v = result->value;
        if (builtins::Math::GetInt(As<Number>(v)) == 1) {
            return std::make_pair(new BigInt(this->value + builtins::Math::GetInt(As<Number>(other))), nullptr);
        } else {
            return std::make_pair(new BigInt(this->value + As<BigInt>(other)->value), nullptr);
        }
    }

    std::string ToString() override {
        std::stringstream ss;
        ss << this->value;
        return ss.str();
    }

    Object *Copy() override {
        auto c = new BigInt(this->value);
        return c; 
    }
};


builtins::YanObject builtins::BigInteger(builtins::YanContext ctx) {
    auto result = new RuntimeResult;
    auto arg = ctx->symbols->Get("__val__");

    Object *val = arg ? arg : Number::null;
    auto r = result->Register(BigInt::Create(val));
    if (result->ShouldReturn()) {
        return result;
    }
    return result->Success(r);
}

auto LoadNativeFunctionImplementation(const std::string &dynamicLib, const std::string &name) -> RuntimeResult *(*)(Context *) {
    #ifdef __linux__
        void *dylib = nullptr;
        void *symbol = nullptr;
        if (dylibs.find(dynamicLib) == dylibs.end()) {
            dylib = OpenDynamicLibrary(dynamicLib);
            dylibs.insert(std::make_pair(dynamicLib, dylib));
        } else {
            dylib = dylibs.at(dynamicLib);
        }
    #elif defined(_WIN32)
        DylibType dylib = 0;
        void *symbol = nullptr;
        if (dylibs.find(dynamicLib) == dylibs.end()) {
            dylib = OpenDynamicLibrary(dynamicLib);
            dylibs.insert(std::make_pair(dynamicLib, dylib));
        } else {
            dylib = dylibs.at(dynamicLib);
        }
    #endif

    assert(dylib != nullptr);
    if (dynamicLoadedSymbol.find(name) == dynamicLoadedSymbol.end()) {
        #ifdef __linux__
            symbol = dlsym(dylib, name.c_str());
            if (symbol == nullptr) {
                std::cerr << "Fatal: Error locating symbol '" + name << "' in dynamic lib '" + dynamicLib + "': " << dlerror() << std::endl;
                return nullptr;
            }
        #elif defined(_WIN32)
            symbol = reinterpret_cast<void *>(win32::GetProcAddress(dylib, name.c_str()));
             if (symbol == nullptr) {
                std::cerr << "Fatal: Error locating symbol '" + name << "' in dynamic lib '" + dynamicLib + "': " << std::format("[WinError {}]", win32::GetLastError()) << std::endl;
                return nullptr;
            }
        #endif
        dynamicLoadedSymbol.insert(std::make_pair(name, std::make_pair(dynamicLib, symbol)));
    }
    if (nativeModules.find(dynamicLib) == nativeModules.end()) {
        #ifdef __linux__
            symbol = dlsym(dylib, "YanModule_OnLoad");
            if (symbol == nullptr) {
                std::cerr << "Fatal: Error locating onLoad() function of '" + dynamicLib + "'" << std::endl;
                return nullptr;
            }
        #elif defined(_WIN32)
            symbol = reinterpret_cast<void *>(win32::GetProcAddress(dylib, "YanModule_OnLoad"));
                if (symbol == nullptr) {
                std::cerr << "Fatal: Error locating onLoad() function of '" << dynamicLib + "': " << std::format("[WinError {}]", win32::GetLastError()) << std::endl;
                return nullptr;
            }
        #endif
        auto moduleDeclearation = ((builtins::ModuleLoaderFunc) symbol)();
        nativeModules.insert(std::make_pair(dynamicLib, moduleDeclearation));
    }

    symbol = dynamicLoadedSymbol.at(name).second;
    return (RuntimeResult *(*)(Context *)) symbol;
}

builtins::YanObject builtins::_LoadNativeSymbol(const std::string &mod, const std::string &symbolName, Position *st, Position *et, builtins::YanContext ctx) {
    auto symbol = LoadNativeFunctionImplementation(mod, symbolName);
    auto func = new BuiltinFunction(symbolName);
    func->SetPos(st, et)->SetContext(ctx);
    auto modDec = nativeModules.at(mod);
    if (modDec->functionArgumentDeclearation.find(symbolName) != modDec->functionArgumentDeclearation.end()) {
        func->argDeclearation = modDec->functionArgumentDeclearation.at(symbolName);
    }
    return (new RuntimeResult)->Success(func);
}


void InitializeBuiltins() {
    for (auto builtinNames : builtinNames) {
        // if (debug) {
        //     std::cout << "[Debug Initalization] Initalizing builtin '" << builtinNames << "'" << std::endl;
        // }
        allBuiltins[builtinNames] = new BuiltinFunction(builtinNames);
    }
}

void SetBuiltins(SymbolTable *global) {
    for (auto [builtinName, o] : allBuiltins) {
        global->Set(builtinName, o);
    }
}


Interpreter::Interpreter() {
    this->callStack = new std::vector<Context *>;
}

RuntimeResult *Interpreter::Visit(NodeBase *node, Context *ctx) {
    switch (node->nodeType) {
        case NodeType::Expression:
            return this->VisitExpression(node, ctx);
        case NodeType::Number:
            return this->VisitNumber(node, ctx);
        case NodeType::SingleExpression:
            return this->VisitSingleExpression(node, ctx);
        case NodeType::VarAccess:
            return this->VisitVarAccessNode(node, ctx);
        case NodeType::VarAssign:
            return this->VisitVarAssignNode(node, ctx);
        case NodeType::IfExpression:
            return this->VisitIfExpressionNode(node, ctx);
        case NodeType::ForExpression:
            return this->VisitForExpression(node, ctx);
        case NodeType::WhileExpression:
            return this->VisitWhileExpression(node, ctx);
        case NodeType::FunctionDefinition:
            return this->VisitFunctionDefinition(node, ctx);
        case NodeType::FunctionCall:
            return this->VisitFunctionCall(node, ctx);
        case NodeType::String:
            return this->VisitString(node, ctx);
        case NodeType::List:
            return this->VisitList(node, ctx);
        case NodeType::Return:
            return this->VisitReturn(node, ctx);
        case NodeType::Continue:
            return this->VisitContinue(node, ctx);
        case NodeType::Break:
            return this->VisitBreak(node, ctx);
        case NodeType::Subscription:
            return this->VisitSubscription(node, ctx);
        case NodeType::Dictionary:
            return this->VisitDictionary(node, ctx);
        case NodeType::Attribution:
            return this->VisitAttribution(node, ctx);
        case NodeType::AdvancedVarAccess:
            return this->VisitAdvancedVarAccess(node, ctx);
        case NodeType::NewExpression:
            return this->VisitNewExpression(node, ctx);
        case NodeType::AttributionCall:
            return this->VisitAttributionCall(node, ctx);
        case NodeType::NonlocalStatement:
            return this->VisitNonlocal(node, ctx);
        case NodeType::Defer:
            return this->VisitDefer(node, ctx);
        case NodeType::Invilid:
            return this->VisitEmpty(node, ctx);
    }
    return nullptr;
}

RuntimeResult *Interpreter::VisitExpression(NodeBase *node, Context *ctx) {
    auto rtResult = new RuntimeResult;        
    auto r = rtResult->Register(this->Visit(node->left, ctx));
    if (rtResult->ShouldReturn()) {
        return rtResult;
    }
    // Number *left = dynamic_cast<Number *>(r);

    auto rn = rtResult->Register(this->Visit(node->right, ctx));
    if (rtResult->ShouldReturn()) {
        return rtResult;
    }
    // Number *right = dynamic_cast<Number *>(rn);        

    #define ERR_RET() if (error != nullptr) return rtResult->Failure(error)
    
    auto op = dynamic_cast<BinaryOperationNode *>(node);
    if (op->binaryOperator.type == TokenType::OP_Plus) {
        auto [result, error] = r->AddTo(rn);
        ERR_RET();
        return rtResult->Success(result->SetPos(op->st, op->et));
    } else if (op->binaryOperator.type == TokenType::OP_Minus) {
        auto [result, error] = r->SubstractedBy(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et));
    } else if (op->binaryOperator.type == TokenType::OP_Mul) {
        auto [result, error] = r->MultipliedBy(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et));
    } else if (op->binaryOperator.type == TokenType::OP_Div) {
        auto [result, error] = r->DividedBy(rn);
        ERR_RET();        
        return rtResult->Success(result->SetPos(op->st, op->et));
    } else if (op->binaryOperator.type == TokenType::OP_Pow) {
        auto [result, error] = r->PoweredBy(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et));
    } else if (op->binaryOperator.type == TokenType::OP_Equal) {
        auto [result, error] = r->GetCompEquals(rn);
        ERR_RET();    
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.type == TokenType::OP_Nequal) {
        auto [result, error] = r->GetCompNequals(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.type == TokenType::OP_Lt) {
        auto [result, error] = r->GetCompLt(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.type == TokenType::OP_Gt) {
        auto [result, error] = r->GetCompGt(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.type == TokenType::OP_Lte) {
        auto [result, error] = r->GetCompLte(rn);
        ERR_RET();    
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.type == TokenType::OP_Gte) {
        auto [result, error] = r->GetCompGte(rn);
        ERR_RET();   
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.Matches<std::string>(TokenType::Keyword, "and")) {
        auto [result, error] = r->And(rn);
        ERR_RET();    
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    } else if (op->binaryOperator.Matches<std::string>(TokenType::Keyword, "or")) {
        auto [result, error] = r->Or(rn);
        ERR_RET();    
        return rtResult->Success(result->SetPos(op->st, op->et)); 
    }

    return nullptr;
}

RuntimeResult *Interpreter::VisitSingleExpression(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto op = dynamic_cast<UnaryOperationNode *>(node);
    auto o = result->Register(this->Visit(dynamic_cast<UnaryOperationNode *>(node)->node, ctx));
    if (result->ShouldReturn()) {
        return result;
    }
    if (o->typeName != "Number") {
        return result->Failure(new TypeError(
            std::format("Unary operation '{}' is not supported on type '{}'", op->unaryOperator.ToString(), o->typeName),
            node->st, node->et, ctx
        ));
    }
    Number *num = dynamic_cast<Number *>(o);
    if (result->ShouldReturn()) {
        return result;
    }

    if (dynamic_cast<UnaryOperationNode *>(node)->unaryOperator.type == TokenType::OP_Minus) {
        auto r = num->MultipliedBy(new Number(-1));
        if (r.second != nullptr) {
            return result->Failure(r.second);
        } else {
            num = dynamic_cast<Number *>(r.first);
        }
    } else if (dynamic_cast<UnaryOperationNode *>(node)->unaryOperator.Matches<std::string>(TokenType::Keyword, "not")) {
        auto r = num->Not();
        if (r.second != nullptr) {
            return result->Failure(r.second);
        } else {
            num = dynamic_cast<Number *>(r.first);
        }
    }
    return result->Success(num->SetPos(op->st, op->et));
}

RuntimeResult *Interpreter::VisitVarAccessNode(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto nd = dynamic_cast<VariableAccessNode *>(node);
    auto variableName = *(std::string *) nd->variableNameToken.value;
    auto value = ctx->symbols->Get(variableName);

    if (value == nullptr) {
        return result->Failure(new RuntimeError(
            std::format("'{}' is not defined", variableName),
            nd->st, nd->et, ctx
        ));
    }

    // Object *oldValue = value;
    // mutable types & immutable types ? 
    if (value->typeName != std::string("List") && value->typeName != std::string("Dictionary") && value->typeName != std::string("ClassObject")) {
        value = value->Copy()->SetPos(nd->st, nd->et)->SetContext(ctx);
    } else {
        value = value->SetPos(nd->st, nd->et)->SetContext(ctx);
    }
    // delete oldValue;

    return result->Success(value);
}

RuntimeResult *Interpreter::VisitVarAssignNode(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto nd = dynamic_cast<VariableAssignNode *>(node);
    auto variableName = *(std::string *) nd->variableNameToken.value;
    auto variableValue = result->Register(this->Visit(nd->valueNode, ctx));
    
    if (result->ShouldReturn()) {
        return result;
    }

    ctx->symbols->Set(variableName, variableValue);
    return result->Success(variableValue);
}

RuntimeResult *Interpreter::VisitNumber(NodeBase *node, Context *ctx) {
    auto type = dynamic_cast<NumberNode *>(node)->numberToken.type;
    auto result = new RuntimeResult;
    Number *num = nullptr;
    if (type == TokenType::Int) {
        num = new Number(*((int *) (dynamic_cast<NumberNode *>(node)->numberToken.value)));
        num->SetContext(ctx);
    } else if (type == TokenType::Float) {
        num = new Number(*((double *) (dynamic_cast<NumberNode *>(node)->numberToken.value)));
        num->SetContext(ctx);            
    }
    return result->Success(num->SetPos(dynamic_cast<NumberNode *>(node)->st, dynamic_cast<NumberNode *>(node)->et));
}

RuntimeResult *Interpreter::VisitIfExpressionNode(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    assert(node->nodeType == NodeType::IfExpression);
    auto ifNode = dynamic_cast<IfExpressionNode *>(node);
    for (auto [case_, shouldReturnNull] : ifNode->cases) {
        auto conditionValue = result->Register(this->Visit(case_.first, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        if (conditionValue->AsBool()) {
            auto exprValue = result->Register(this->Visit(case_.second, ctx));
            if (result->ShouldReturn()) {
                return result;
            }
            return result->Success(shouldReturnNull ? Number::null : exprValue);
        }
    }

    if (ifNode->elseCase.first != nullptr) {
        auto elseValue = result->Register(this->Visit(ifNode->elseCase.first, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        return result->Success(ifNode->elseCase.second ? Number::null : elseValue);
    }

    return result->Success(Number::null);
}

RuntimeResult *Interpreter::VisitForExpression(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    std::vector<Object *> elements;

    assert(node->nodeType == NodeType::ForExpression);
    auto forNode = dynamic_cast<ForExpressionNode *>(node);
    if (!forNode->rangeBasedLoop) {
        Object *stepValue = nullptr;
        auto stv = result->Register(this->Visit(forNode->stvNode, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
    
        auto etv = result->Register(this->Visit(forNode->etvNode, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
    
        if (forNode->stepvNode != nullptr) {
            stepValue = result->Register(this->Visit(forNode->stepvNode, ctx));
            if (result->ShouldReturn()) {
                return result;
            }
        } else {
            stepValue = new Number(1);
        }
    
        Number *i;
        Number *stvNum = nullptr, *etvNum = nullptr, *stepNum = nullptr;
        if (stv->typeName == "Number" && stepValue->typeName == "Number" && etv->typeName == "Number") {
            stvNum = dynamic_cast<Number *>(stv);
            stepNum = dynamic_cast<Number *>(stepValue);
            etvNum = dynamic_cast<Number *>(etv);
            i = new Number(stvNum->value);
        } else {
            return result->Failure(new TypeError(
                std::format("For-loop expects 3 number but got ({}, {}, {})", stv->typeName, stepValue->typeName, etv->typeName),
                node->st, node->et, ctx
            ));
        }
    
        std::function<bool()> condition;
        if (stepNum->ntype == NumberType::Int && std::get<int>(stepNum->value) >= 0) {
            condition = [&i, &etv]() {
                return i->GetCompLt(etv).first->AsBool();
            };
        } else if (stepNum->ntype == NumberType::Int) {
            condition = [&i, &etv]() {
                return i->GetCompGt(etv).first->AsBool();
            };
        } else if (stepNum->ntype == NumberType::Float && std::get<double>(stepNum->value) >= 0) {
            condition = [&i, &etv]() {
                return i->GetCompLt(etv).first->AsBool();
            };
        } else {
            condition = [&i, &etv]() {
                return i->GetCompGt(etv).first->AsBool();
            };
        }
    
        while (condition()) {
            ctx->symbols->Set(*(std::string *) forNode->var.value, i);
            i = dynamic_cast<Number *>(i->AddTo(stepValue).first);
            auto value = result->Register(this->Visit(forNode->body, ctx));
            if (result->ShouldReturn() && !result->shouldContinue && !result->shouldBreak) {
                return result;
            }
    
            if (result->shouldContinue) {
                continue;
            }
            if (result->shouldBreak) {
                break;
            }
            elements.push_back(value);
        }
        ctx->symbols->Remove(*(std::string *) forNode->var.value);
        return result->Success(forNode->shouldReturnNull ? Number::null : (new List(elements))->SetContext(ctx)->SetPos(node->st, node->et));
    } else {
        auto iterableRaw = result->Register(this->Visit(forNode->range, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        auto iteratorRaw = iterableRaw->Iter();
        bool iterList = true;
        if (iterableRaw->typeName != std::string("List")) {
            iterList = false;
            if (iteratorRaw.second != nullptr || iteratorRaw.first == nullptr) {
                return result->Failure(new TypeError(
                    std::format("Object '{}' is not iterable", iterableRaw->typeName),
                    iterableRaw->startPos, iterableRaw->endPos, ctx
                ));
            }
        }

        if (iterList) {
            auto iterable = As<List>(iterableRaw);
            for (auto &i : iterable->elements) {
                ctx->symbols->Set(*(std::string *) forNode->var.value, i);
                auto value = result->Register(this->Visit(forNode->body, ctx));
                if (result->ShouldReturn() && !result->shouldContinue && !result->shouldBreak) {
                    return result;
                }
        
                if (result->shouldContinue) {
                    continue;
                }
                if (result->shouldBreak) {
                    break;
                }
                elements.push_back(value);
            }
            ctx->symbols->Remove(*(std::string *) forNode->var.value);
            return result->Success(forNode->shouldReturnNull ? Number::null : (new List(elements))->SetContext(ctx)->SetPos(node->st, node->et));
        } else {
            auto iterator = iteratorRaw.first;
            std::pair<Object *, Error *> iterResultTmp { nullptr, nullptr };
            iterResultTmp = iterator->Next();
            while (iterResultTmp.second == nullptr) {
                ctx->symbols->Set(*(std::string *) forNode->var.value, iterResultTmp.first);
                auto value = result->Register(this->Visit(forNode->body, ctx));
                if (result->ShouldReturn() && !result->shouldContinue && !result->shouldBreak) {
                    return result;
                }
        
                if (result->shouldContinue) {
                    continue;
                }
                if (result->shouldBreak) {
                    break;
                }
                elements.push_back(value);
                iterResultTmp = iterator->Next();                      
            }
            ctx->symbols->Remove(*(std::string *) forNode->var.value);
            delete iterator;
            if (iterResultTmp.second->name != "StopIteration") {
                return result->Failure(iterResultTmp.second);
            }          
            return result->Success(forNode->shouldReturnNull ? Number::null : (new List(elements))->SetContext(ctx)->SetPos(node->st, node->et));              
        }
    }
}

RuntimeResult *Interpreter::VisitWhileExpression(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto whileNode = dynamic_cast<WhileExpressionNode *>(node);

    while (true) {
        auto cond = result->Register(this->Visit(whileNode->conditionNode, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        if (!cond->AsBool()) {
            break;
        }
        auto value = result->Register(this->Visit(whileNode->body, ctx));

        if (result->ShouldReturn() && !result->shouldContinue && !result->shouldBreak) {
            return result;
        }
        if (result->shouldContinue) {
            continue;
        }
        if (result->shouldBreak) {
            break;
        }
    }
    return result->Success(Number::null);
}

RuntimeResult *Interpreter::VisitFunctionDefinition(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto funcDefNode = dynamic_cast<FunctionDefinitionNode *>(node);
    auto funcBody = funcDefNode->body;
    std::vector<std::string> parameters;
    Function *function;

    for (auto param : funcDefNode->parameters) {
        parameters.push_back(*(std::string *) param.value);
    }

    if (funcDefNode->fun.type != TokenType::Invilid) {
        function = dynamic_cast<Function *>((new Function(*(std::string *) funcDefNode->fun.value, funcBody, parameters, funcDefNode->shouldAutoReturn))->SetContext(ctx)->SetPos(node->st, node->et));
        ctx->symbols->Set(*(std::string *) funcDefNode->fun.value, function);
    } else {
        function = dynamic_cast<Function *>((new Function(funcBody, parameters, funcDefNode->shouldAutoReturn))->SetContext(ctx)->SetPos(node->st, node->et));    
    }

    for (auto &fv : funcDefNode->cellVars) {
        function->cellVars.push_back(*(std::string *) fv->freeVar.value);
    }

        // FixMe: Fix REPEAT Code
    if (ctx->parent != nullptr) {
        for (auto &fv : funcDefNode->freeVars) {
            auto fvName = *(std::string *) fv->freeVar.value;
            auto fvValue = ctx->symbols->Get(fvName);
            if (fvValue == nullptr) {
                return result->Failure(new RuntimeError(
                    std::format("'{}' is not a valid freevar", fvName),
                    fv->st, fv->et, ctx
                ));
            }
            function->closureVarsTable->Set(fvName, fvValue);
        }
    }
    return result->Success(function);
}

RuntimeResult *Interpreter::VisitFunctionCall(NodeBase *node, Context *ctx) {
    if (overflowCount >= INVILID_OVERFLOW_TOLERANCE) {
        std::cerr << "Fatal: Stack corrupted" << std::endl;
        std::cerr << "[Native Stack Info]" << std::endl;
        std::cerr << RuntimeError::GetNativeCallStackInfo() << std::endl;
        assert(false);
    }

    auto result = new RuntimeResult;
    auto funcCallNode = dynamic_cast<FunctionCallNode *>(node);
    std::vector<Object *> args;
    currentCallStackDepth++;
    if (currentCallStackDepth >= MAX_CALLSTACK_DEPTH) {
        currentCallStackDepth = 0;
        overflowCount++;    
        return result->Failure(new RuntimeError(
            std::format("Maximum call stack depth ({}) exceeded, recompile the source and change MAX_CALLSTACK_DEPTH to extend stack capacity", MAX_CALLSTACK_DEPTH),
            node->st, node->et, ctx
        ));
    }
    
    // if (ctx->interpreter) {
    //     ctx->interpreter->callStack->push_back(ctx);
    // }
        
    auto functionTarget = result->Register(this->Visit(funcCallNode->target, ctx));
    if (result->ShouldReturn()) {
        return result;
    }
    // functionTarget = functionTarget->Copy()->SetPos(node->st, node->et)->SetContext(ctx);
    // functionTarget = functionTarget->Copy()->SetPos(node->st, node->et);
    functionTarget = functionTarget->Copy();
    
    for (auto &arg : funcCallNode->arguments) {
        args.push_back(result->Register(this->Visit(arg, ctx)));
        if (result->ShouldReturn()) {
            return result;
        }
    }
    
    // if (functionTarget->typeName != "Function" && functionTarget->typeName != "BuiltinFunction" && functionTarget->typeName != "Method" && functionTarget->typeName != "ClassObject" && functionTarget->typeName != std::string("BuiltinMethod")) {
    //     return result->Failure(new TypeError(
    //         std::format("'{}' object is not callable", functionTarget->typeName),
    //         node->st, node->et, ctx
    //      ));
    // } else {
    Object *returnValue;
    if (functionTarget->typeName == "Function") {
        auto target = dynamic_cast<Function *>(functionTarget);
        auto returnValueTmp = result->Register(target->Execute(args));
        if (result->ShouldReturn()) {
            return result;
        }
        returnValue = returnValueTmp;
    } else if (functionTarget->typeName == "BuiltinFunction") {
        auto returnValueTmp = result->Register((dynamic_cast<BuiltinFunction *>(functionTarget))->Execute(args));
        if (result->ShouldReturn()) {
            return result;
        }
        returnValue = returnValueTmp;
    // } else if (functionTarget->typeName == "ClassObject") {
    //     auto [ctor, error] = dynamic_cast<ClassObject *>(functionTarget)->GetAttr("__init__");
    //     if (error != nullptr) {
    //         return result->Failure(new TypeError(
    //             std::format("Prototype of object '{}' has no constructor", As<ClassObject>(functionTarget)->className),
    //             node->st, node->et, ctx
    //         ));
    //     }
    //     if (ctor->typeName != "Function") {
    //         return result->Failure(new TypeError(
    //             "Constructor is not callable",
    //             node->st, node->et, ctx
    //         ));
    //     }
    //     auto returnValueTmp = functionTarget->Copy();
    //     auto boundedCtor = Method::FromFunction(As<Function>(ctor), returnValueTmp, "__init__");
    //     boundedCtor->SetPos(node->st, node->et)->SetContext(ctx);
    //     result->Register(boundedCtor->Execute(args));
    //     if (result->ShouldReturn()) {
    //         return result;
    //     }
    //     As<ClassObject>(returnValueTmp)->isProto = false;
    //     returnValue = returnValueTmp;
    } else if (functionTarget->typeName == "ClassObject") {
        return result->Success(functionTarget);
    } else {
        if (functionTarget->typeName == "Method") {
            auto returnValueTmp = result->Register((dynamic_cast<Method *>(functionTarget))->Execute(args));
            if (result->ShouldReturn()) {
                return result;
            }
            returnValue = returnValueTmp;
        } else if (functionTarget->typeName == std::string("BuiltinMethod")) {
            auto returnValueTmp = result->Register((dynamic_cast<BuiltinFunction *>(functionTarget))->Execute(args));
            if (result->ShouldReturn()) {
                return result;
            }
            returnValue = returnValueTmp;
        } else {
            auto returnValueTmp = result->Register((functionTarget->Execute(args)));
            if (result->ShouldReturn()) {
                return result;
            }
            returnValue = returnValueTmp;
        }
    }
    
    currentCallStackDepth--;
    return result->Success(returnValue->Copy()->SetPos(node->st, node->et)->SetContext(ctx));
    // }
}

RuntimeResult *Interpreter::VisitNonlocal(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto nlNode = dynamic_cast<NonlocalStatementNode *>(node);
    auto fvName = *(std::string *) nlNode->freeVar.value;
    auto fvValue = ctx->nonlocals->Get(fvName);

    if (fvValue == nullptr) {
        return result->Failure(new RuntimeError(
            std::format("'{}' is not a valid cellvar", fvName),
            node->st, node->et, ctx
        ));
    }
    ctx->symbols->Set(fvName, fvValue);
    return result->Success(Number::null);
}

RuntimeResult *Interpreter::VisitString(NodeBase *node, Context *ctx) {
    return (new RuntimeResult)->Success(
        (new String(*(std::string *) (dynamic_cast<StringNode *>(node)->stringToken.value)))->SetContext(ctx)->SetPos(node->st, node->et)
    );
}

RuntimeResult *Interpreter::VisitList(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    std::vector<Object *> elements;
    auto elementNodes = dynamic_cast<ListNode *>(node);

    for (auto elementNode : elementNodes->elements) {
        elements.push_back(result->Register(this->Visit(elementNode, ctx)));
        if (result->ShouldReturn()) {
            return result;
        }
    }
    if (elementNodes->subscripting == nullptr) {
        return result->Success((new List(elements))->SetContext(ctx)->SetPos(node->st, node->et));
    } else {
        auto idx = result->Register(this->Visit(elementNodes->subscripting, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        if (idx->typeName != "Number") {
            return result->Failure(new TypeError(
                "List subsciption takes an number", 
                idx->startPos, idx->endPos, ctx
            ));
        }
        auto indexNum = As<Number>(idx);
        if (!builtins::Math::HoldsInteger(indexNum)) {
            return result->Failure(new TypeError(
                "List subsciption takes an integer", 
                idx->startPos, idx->endPos, ctx
            ));
        }

        auto indexReal = builtins::Math::GetInt(indexNum);
        if (indexReal < 0) {
            return result->Failure(new TypeError(
                "List subsciption takes an non-negative integer", 
                idx->startPos, idx->endPos, ctx
            ));
        }
        if (indexReal > elements.size() - 1) {
            return result->Failure(new RuntimeError(
                std::format("List index out of range ({} > max: {})", indexReal, elements.size() - 1),
                node->st, node->et, ctx
            ));
        }
        if (elementNodes->newVal == nullptr) {
            if (indexReal < elements.size() && indexReal >= 0) {
                return result->Success(elements[indexReal]);
            }
            return result->Failure(new RuntimeError(
                "List index out of range",
                node->st, node->et, ctx
            ));
        } else {
            return result->Failure(new ValueError(
                "Assignment to literal",
                node->st, node->et, ctx
            ));
        }
    }
}

RuntimeResult *Interpreter::VisitDictionary(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto dictNode = dynamic_cast<DictionaryNode *>(node);
    bool isClass = false;
    bool hasCtor = false;
    std::map<Object *, Object *> dict;
    for (auto &[k, v] : dictNode->elements) {
        auto kvalue = result->Register(this->Visit(k, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        if (kvalue->typeName == "String") {
            if (As<String>(kvalue)->s == "__cls__") {
                isClass = true;
            } else if (As<String>(kvalue)->s == "__init__") {
                hasCtor = true;
            }
        }
        auto vvalue = result->Register(this->Visit(v, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
        dict.insert(std::make_pair(kvalue, vvalue));
    }

    if (isClass && !hasCtor) {
        return result->Failure(new TypeError(
            "Prototype of object '{}' should have a '__init__' as its constructor",
            node->st, node->et, ctx
        ));
    }

    if (!isClass) {
        return result->Success((new Dictionary(dict))->SetPos(node->st, node->et)->SetContext(ctx));
    } else {
        auto cls = new ClassObject(dict);
        cls->SetPos(node->st, node->et)->SetContext(ctx);
        result->Register(cls->BuildClass());
        if (result->ShouldReturn()) {
            return result;
        }
        return result->Success(cls);
    }
}

Number *Number::null = new Number(0);

RuntimeResult *Interpreter::VisitReturn(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto returnNode = dynamic_cast<ReturnStatementNode *>(node);
    // if (ctx->symbols->Get(ctx->ctxLabel) == nullptr) {
    //     return result->Failure(new RuntimeError(
    //         "'return' outside a user-defined function",
    //         node->st, node->et, ctx
    //     ));
    // }
    Object *value = nullptr;
    if (returnNode->nodeToReturn != nullptr) {
        value = result->Register(this->Visit(returnNode->nodeToReturn, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
    } else {
        value = Number::null;
    }
    return result->SuccessReturn(value);
}

RuntimeResult *Interpreter::VisitContinue(NodeBase *node, Context *ctx) {
    return (new RuntimeResult)->SuccessContinue();
}

RuntimeResult *Interpreter::VisitBreak(NodeBase *node, Context *ctx) {
    return (new RuntimeResult)->SuccessBreak();
}

RuntimeResult *Interpreter::VisitSubscription(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto subNode = dynamic_cast<SubscriptionNode *>(node);
    auto var = result->Register(this->Visit(subNode->target, ctx));
    if (result->ShouldReturn()) {
        return result;
    }
    auto indexValue = result->Register(this->Visit(subNode->index, ctx));
    if (result->error != nullptr) {
        return result;
    }
    
    if (subNode->assignment == nullptr) {
        auto v = var->Subsciption(indexValue);
        if (v.second != nullptr) {
            auto err = v.second;
            err->st = node->st;
            err->et = node->et;
            return result->Failure(err);
        }
        if (subNode->calls.find(0) == subNode->calls.end()) {   
            if (subNode->subIndexes.size() == 0) {
                return result->Success(v.first);
            }
        } else {
            std::vector<Object *> args;
            for (auto &&argNode : subNode->calls[0]) {
                auto argValue = result->Register(this->Visit(argNode, ctx));
                if (result->ShouldReturn()) {
                    return result;
                }
                args.push_back(argValue);
            }
            auto ret = result->Register(v.first->Execute(args));
            if (result->ShouldReturn()) {
                result->error->st = node->st;
                result->error->et = node->et;
                return result;
            }

            if (subNode->subIndexes.size() == 0) {
                return result->Success(ret);
            }
        }
        
        Object *tmp = v.first;
        int index = 1;
        for (auto i : subNode->subIndexes) {
            auto sv = result->Register(this->Visit(i, ctx));
            auto subsciptionLayerResult = tmp->Subsciption(sv);
            if (subsciptionLayerResult.second != nullptr) {
                auto err = subsciptionLayerResult.second;
                err->st = node->st;
                err->et = node->et;
                return result->Failure(err);
            }
            if (subNode->calls.find(index) == subNode->calls.end()) {
                tmp = subsciptionLayerResult.first->SetContext(ctx)->SetPos(node->st, node->et);;
            } else {
                std::vector<Object *> args;
                for (auto &&argNode : subNode->calls[index]) {
                    auto argValue = result->Register(this->Visit(argNode, ctx));
                    if (result->ShouldReturn()) {
                        return result;
                    }
                    args.push_back(argValue);
                }

                tmp = result->Register(subsciptionLayerResult.first->Execute(args));
                if (result->ShouldReturn()) {
                    result->error->st = node->st;
                    result->error->et = node->et;
                    return result;
                }
            }
        
            index++;
        }
        return result->Success(tmp->SetContext(ctx)->SetPos(node->st, node->et));        
    } else {        
        auto newValue = result->Register(this->Visit(subNode->assignment, ctx));
        if (result->error != nullptr) {
            return result;
        }

        if (subNode->subIndexes.size() == 0 && subNode->calls.find(0) != subNode->calls.end()) {
            return result->Failure(new RuntimeError(
                "Unable to assign a new value to a function return value (rvalue)",
                node->st, node->et, ctx
            ));
        }

        if (subNode->calls.find(subNode->subIndexes.size()) != subNode->calls.end()) {
            return result->Failure(new RuntimeError(
                "Unable to assign a new value to a function return value (rvalue)",
                node->st, node->et, ctx
            ));
        }

        if (subNode->subIndexes.size() == 0) {
            auto status = var->SubsciptionAssignment(indexValue, newValue);
            if (status.second != nullptr) {
                auto err = status.second;
                err->st = node->st;
                err->et = node->et;
                return result->Failure(err);
            }
            return result->Success(Number::null);
        } else {
            auto v = var->Subsciption(indexValue);
            if (v.second != nullptr) {
                auto err = v.second;
                err->st = node->st;
                err->et = node->et;
                return result->Failure(err);
            }
            if (subNode->subIndexes.size() == 0) {
                return result->Success(v.first);
            }
            Object *tmp = v.first;

            int index = 1;
            for (unsigned i = 0; i < subNode->subIndexes.size() - 1; i++) {
                auto sv = result->Register(this->Visit(subNode->subIndexes[i], ctx));
                auto subsciptionLayerResult = tmp->Subsciption(sv);
                if (subsciptionLayerResult.second != nullptr) {
                    auto err = subsciptionLayerResult.second;
                    err->st = node->st;
                    err->et = node->et;
                    return result->Failure(err);
                }
                if (subNode->calls.find(index) == subNode->calls.end()) {
                    tmp = subsciptionLayerResult.first;
                } else {
                    std::vector<Object *> args;
                    for (auto &&argNode : subNode->calls[index]) {
                        auto argValue = result->Register(this->Visit(argNode, ctx));
                        if (result->ShouldReturn()) {
                            return result;
                        }
                        args.push_back(argValue);
                    }

                    tmp = result->Register(subsciptionLayerResult.first->Execute(args));
                    if (result->ShouldReturn()) {
                        result->error->st = node->st;
                        result->error->et = node->et;
                        return result;
                    }
                }
                index++;
            }

            auto lastIndex = result->Register(this->Visit(subNode->subIndexes[subNode->subIndexes.size() - 1], ctx));
            if (result->error != nullptr) {
                return result;
            }
            auto status = tmp->SubsciptionAssignment(lastIndex, newValue);
            if (status.second != nullptr) {
                auto err = status.second;
                err->st = node->st;
                err->et = node->et;
                return result->Failure(err);
            }
            return result->Success(Number::null);
        }
    }
}

RuntimeResult *Interpreter::VisitAttribution(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto attrNode = dynamic_cast<AttributionNode *>(node);
    auto var = result->Register(this->Visit(attrNode->target, ctx));
    if (result->ShouldReturn()) {
        return result;
    }
    auto attr = *(std::string *) attrNode->attr.value;
    auto calls = attrNode->calls;
    
    auto TryGenerateMethod = [=, &result](Object *v, Object *self, const std::string attr) -> Object * {
        if (v->typeName == "Function") {
            if (As<Function>(v)->parameters.size() > 0) {
                if (As<Function>(v)->parameters[0] == "self" || As<Function>(v)->parameters[0] == "this") {
                    std::string pa {};
                    if (self->typeName == std::string("ClassObject")) {
                        pa = As<ClassObject>(self)->className;
                    } else if (self->typeName == std::string("Dictionary")) {
                        pa = "<anonymous>";
                    }
                    auto method = Method::FromFunction(As<Function>(v), self, pa + '.' + attr)->SetPos(node->st, node->et)->SetContext(ctx);    
                    return method; 
                }
            }       
        }
        return v;
    };

    if (attrNode->assignment == nullptr) {
        Object *v;
        if (calls.find(0) != calls.end()) {
            auto va = var->GetAttr(attr);
            if (va.first == nullptr) {
                return result->Failure(va.second);
            }
            (va.first)->SetContext(ctx)->SetPos(node->st, node->et);
            ctx->symbols->Set(std::format("__@attrCall_{}__", *(std::string *) attrNode->attr.value), 
                TryGenerateMethod(va.first, var, attr));
            v = result->Register(this->Visit(calls[0], ctx));
            if (result->ShouldReturn()) {
                return result;
            }
            v = v->SetContext(ctx)->SetPos(node->st, node->et);
            if (attrNode->subAttrs.size() == 0) {
                return result->Success(v);
            }
        } else {
            auto vp = var->GetAttr(attr);
            v = vp.first;        
            if (vp.second != nullptr) {
                auto err = vp.second;
                err->st = node->st;
                err->et = node->et;
                ((RuntimeError *) err)->SetContext(ctx);
                return result->Failure(err);
            }

            if (attrNode->subAttrs.size() == 0) {
                if (vp.first->typeName == std::string("Function")) {
                    if (As<Function>(vp.first)->parameters.size() > 0) {
                        if (As<Function>(vp.first)->parameters[0] == "self" || As<Function>(vp.first)->parameters[0] == "this") {
                            auto method = Method::FromFunction(As<Function>(vp.first), var, attr);    
                            return result->Success(method); 
                        }
                    }       
                }
                return result->Success(vp.first->SetContext(ctx)->SetPos(node->st, node->et));
            }
        }

        Object *tmp = v;
        Object *self = nullptr;
        std::string ls {};
        int callId = 1;
        for (auto i : attrNode->subAttrs) {
            auto subsciptionLayerResult = tmp->GetAttr(*(std::string *) i.value);
            if (subsciptionLayerResult.second != nullptr) {
                auto err = subsciptionLayerResult.second;
                err->st = node->st;
                err->et = node->et;
                ((RuntimeError *) err)->SetContext(ctx);
                return result->Failure(err);
            }
            if (calls.find(callId) != calls.end()) {
                (subsciptionLayerResult.first)->SetContext(ctx)->SetPos(node->st, node->et);
                ctx->symbols->Set(std::format("__@attrCall_{}__", *(std::string *) i.value), 
                    TryGenerateMethod(subsciptionLayerResult.first, tmp, *(std::string *) i.value));
                auto layerResult = result->Register(this->Visit(calls[callId], ctx));
                if (result->ShouldReturn()) {
                    return result;
                }
                tmp = layerResult->SetContext(ctx)->SetPos(node->st, node->et);
                
            } else {
                ls = *(std::string *) attrNode->subAttrs[attrNode->subAttrs.size() - 1].value;
                if (*(std::string *) i.value == ls) {
                    self = tmp;
                }
                tmp = subsciptionLayerResult.first->SetContext(ctx)->SetPos(node->st, node->et);
            }
            callId++;
        }
        // if (tmp->typeName == "Function") {
        //    if (As<Function>(tmp)->parameters[0] == "self" || As<Function>(tmp)->parameters[0] == "this") {
        //         auto method = Method::FromFunction(As<Function>(tmp), self, ls);    
        //         return result->Success(method); 
        //     }    
        // }
        return result->Success(tmp->SetContext(ctx)->SetPos(node->st, node->et));  
    } else {
        Object *v;
        if (attrNode->calls.find(0) != attrNode->calls.end() && attrNode->subAttrs.empty()) {
            return result->Failure(new RuntimeError(
                "Unable to assign a new value to a function return value (rvalue)",
                node->st, node->et, ctx
            ));
        }

        if (attrNode->calls.find(attrNode->subAttrs.size()) != attrNode->calls.end()) {
            return result->Failure(new RuntimeError(
                "Unable to assign a new value to a function return value (rvalue)",
                node->st, node->et, ctx
            ));
        }
        
        auto expr = result->Register(this->Visit(attrNode->assignment, ctx));
        if (result->ShouldReturn()) {
            return result;
        }

        if (attrNode->calls.find(0) != attrNode->calls.end()) {
            auto va = var->GetAttr(attr);
            if (va.first == nullptr) {
                return result->Failure(va.second);
            }
            (va.first)->SetContext(ctx)->SetPos(node->st, node->et);
            ctx->symbols->Set(std::format("__@attrCall_{}__", *(std::string *) attrNode->attr.value), 
                TryGenerateMethod(va.first, var, attr));
            v = result->Register(this->Visit(calls[0], ctx));
            if (result->ShouldReturn()) {
                return result;
            }
            v = v->SetContext(ctx)->SetPos(node->st, node->et);
            if (attrNode->subAttrs.size() == 0) {
                return result->Success(v);
            }
        } else {
            auto vp = var->GetAttr(attr);
            v = vp.first;        
            if (vp.second != nullptr) {
                auto err = vp.second;
                err->st = node->st;
                err->et = node->et;
                ((RuntimeError *) err)->SetContext(ctx);
                return result->Failure(err);
            }

            if (attrNode->subAttrs.size() == 0) {
                if (vp.first->typeName == std::string("Function")) {
                    if (As<Function>(vp.first)->parameters.size() > 0) {
                        if (As<Function>(vp.first)->parameters[0] == "self" || As<Function>(vp.first)->parameters[0] == "this") {
                            auto method = Method::FromFunction(As<Function>(vp.first), var, attr);    
                            return result->Success(method); 
                        }
                    }       
                }
                return result->Success(vp.first->SetContext(ctx)->SetPos(node->st, node->et));
            }
        }
        
        if (attrNode->subAttrs.size() != 0) {
            Object *tmp = v;
            Object *self = nullptr;
            std::string ls {};
            int callId = 1;
            for (auto i : attrNode->subAttrs) {
                auto subsciptionLayerResult = tmp->GetAttr(*(std::string *) i.value);
                if (subsciptionLayerResult.second != nullptr) {
                    auto err = subsciptionLayerResult.second;
                    err->st = node->st;
                    err->et = node->et;
                   ((RuntimeError *) err)->SetContext(ctx);            
                    return result->Failure(err);
                }
                if (calls.find(callId) != calls.end()) {
                    ctx->symbols->Set(std::format("__@attrCall_{}__", *(std::string *) i.value), 
                        TryGenerateMethod(subsciptionLayerResult.first, tmp, *(std::string *) i.value));
                    auto layerResult = result->Register(this->Visit(calls[callId], ctx));
                    if (result->ShouldReturn()) {
                        return result;
                    }
                    tmp = layerResult;
                } else {
                    ls = *(std::string *) attrNode->subAttrs[attrNode->subAttrs.size() - 1].value;
                    if (*(std::string *) i.value == ls) {
                        self = tmp;
                    }
                    tmp = subsciptionLayerResult.first;
                }
                callId++;
            }

            auto r = self->SetAttr(*(std::string *) attrNode->subAttrs[attrNode->subAttrs.size() - 1].value, expr);
            if (r.second != nullptr) {
                auto err = r.second;
                err->st = node->st;
                err->et = node->et;
                ((RuntimeError *) err)->SetContext(ctx);
                return result->Failure(err);
            }
            return result->Success(Number::null);
        } else {
            auto rv = var->SetAttr(attr, expr);
            if (rv.second != nullptr) {
                auto err = rv.second;
                err->st = node->st;
                err->et = node->et;
                ((RuntimeError *) err)->SetContext(ctx);
                return result->Failure(err);
            }
            return result->Success(Number::null);
        }
    }
}

RuntimeResult *Interpreter::VisitAttributionCall(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto callNode = dynamic_cast<AttributionCallNode *>(node);
    std::vector<Object *> args;

    auto callv = std::format("__@attrCall_{}__", *(std::string *) dynamic_cast<VariableAccessNode *>(callNode->call->target)->variableNameToken.value);
    auto o = ctx->symbols->Get(callv);
    if (o == nullptr) {
        std::cerr << "Internal interpreter error: Couldn't get attribution call" << std::endl;
        std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;
        assert(false);
    }

    currentCallStackDepth++;
    if (currentCallStackDepth >= MAX_CALLSTACK_DEPTH) {
        currentCallStackDepth = 0;
        return result->Failure(new RuntimeError(
            std::format("Maximum call stack depth ({}) exceeded, recompile the source and change MAX_CALLSTACK_DEPTH to extend stack capacity", MAX_CALLSTACK_DEPTH),
            node->st, node->et, ctx
        ));
    }

     for (auto &arg : callNode->call->arguments) {
        auto v1 = result->Register(this->Visit(arg, ctx));
        if (v1 != nullptr) {
            v1->SetPos(node->st, node->et)->SetContext(ctx);
        }
        args.push_back(v1);
        if (result->ShouldReturn()) {
            result->error->st = node->st;
            result->error->et = node->et;
            ((RuntimeError *) result->error)->SetContext(ctx);
            return result;
        }
    }

    auto v = result->Register(o->Execute(args));
    if (result->ShouldReturn()) {
        return result;
    }
    
    ctx->symbols->Remove(callv);
    currentCallStackDepth--;
    return result->Success(v->SetContext(ctx)->SetPos(node->st, node->et));
}

RuntimeResult *Interpreter::VisitAdvancedVarAccess(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto avNode = dynamic_cast<AdvancedVarAccessNode *>(node);
    Object *tmp = nullptr;

    for (auto &accessNode : avNode->advancedAccess) {        
        tmp = result->Register(this->Visit(accessNode, ctx));
        if (result->ShouldReturn()) {
            return result;
        }
    }
    return result->Success(tmp->SetContext(ctx)->SetPos(node->st, node->et)); 
}

RuntimeResult *Interpreter::VisitNewExpression(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    auto newNode = dynamic_cast<NewExprNode *>(node);
    std::vector<Object *> args;
    currentCallStackDepth++;
    if (currentCallStackDepth >= MAX_CALLSTACK_DEPTH) {
        currentCallStackDepth = 0;
        return result->Failure(new RuntimeError(
            std::format("Maximum call stack depth ({}) exceeded, recompile the source and change MAX_CALLSTACK_DEPTH to extend stack capacity", MAX_CALLSTACK_DEPTH),
            node->st, node->et, ctx
        ));
    }
    
    if (newNode->newExpr->nodeType == NodeType::FunctionCall) {
         for (auto &arg : dynamic_cast<FunctionCallNode *>(newNode->newExpr)->arguments) {
            args.push_back(result->Register(this->Visit(arg, ctx)));
            if (result->ShouldReturn()) {
                return result;
            }
        }
    }

    if (newNode->newExpr->nodeType == NodeType::AdvancedVarAccess) {
        auto access = dynamic_cast<AdvancedVarAccessNode *>(newNode->newExpr)->advancedAccess;
        auto call = access[access.size() - 1];
        if (call->nodeType != NodeType::Attribution) {
            // std::cerr << "Fatal: Interpreter state error [While instantiating objects: 'invilid node']" << std::endl;
            // std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;        
            // assert(false); 
            return result->Failure(new RuntimeError(
                "Invilid access for object prototypes: without attribute access",
                call->st, call->et, ctx
            ));
        }

        auto caller = dynamic_cast<AttributionNode *>(call)->calls;
        if (caller.size() > 1) {
            // std::cerr << "Fatal: Interpreter state error [While instantiating objects: 'invalid call']" << std::endl;
            // std::cerr << "Native call stack traceback:" << std::endl << RuntimeError::GetNativeCallStackInfo() << std::endl;        
            // assert(false);
            return result->Failure(new RuntimeError(
                "Invilid access for object prototypes: with dynamic function call",
                call->st, call->et, ctx
            ));
        } else if (caller.size() == 0) {
            goto instantiate;
        }

        std::vector<int> positions;
        for (const auto &[idx, call] : caller) {
            positions.emplace_back(idx);
        }

        // get the last call
        AttributionCallNode *nd = dynamic_cast<AttributionCallNode *>(caller[*std::max_element(positions.begin(), positions.end())]);
        
        for (auto &arg : nd->call->arguments) {
            args.push_back(result->Register(this->Visit(arg, ctx)));
            if (result->ShouldReturn()) {
                return result;
            }
        }

        for (auto n : dynamic_cast<AdvancedVarAccessNode *>(newNode->newExpr)->advancedAccess) {
            if (n->nodeType == NodeType::Attribution) {
                dynamic_cast<AttributionNode *>(n)->calls.clear();
            }
        }
    }

instantiate:
    auto functionTarget = result->Register(this->Visit(newNode->newExpr, ctx));

    if (result->ShouldReturn()) {
        return result;
    }
    if (functionTarget->typeName != std::string("ClassObject")) {
        return result->Failure(new TypeError(
            "Keyword 'new' requires a constructor call",
            functionTarget->startPos, functionTarget->endPos, ctx
        ));
    }

    auto [ctor, error] = dynamic_cast<ClassObject *>(functionTarget)->GetAttr("__init__");
    if (error != nullptr) {
        return result->Failure(new TypeError(
            std::format("Prototype of object '{}' has no constructor", As<ClassObject>(functionTarget)->className),
            node->st, node->et, ctx
        ));
    }
    if (ctor->typeName != std::string("Function") && ctor->typeName != std::string("BuiltinFunction")) {
        return result->Failure(new TypeError(
            "Constructor is not callable",
            node->st, node->et, ctx
        ));
    }
    auto returnValueTmp = functionTarget->Copy();
    Object *boundedCtor = nullptr;
    if (ctor->typeName == std::string("Function")) {
        boundedCtor = Method::FromFunction(As<Function>(ctor), returnValueTmp, std::format("{}.__init__", dynamic_cast<ClassObject *>(functionTarget)->className));
    } else if (ctor->typeName == std::string("BuiltinFunction")) {
        if (As<BuiltinFunction>(ctor)->dynamicImpl == nullptr) {
            return result->Failure(new RuntimeError(
                "Failed to construct object with invilid native constructor",
                node->st, node->et, ctx
            ));    
        }

        boundedCtor = BuiltinMethod::FromBuiltinFunction(As<BuiltinFunction>(ctor), returnValueTmp);
    }
    boundedCtor->SetPos(node->st, node->et)->SetContext(ctx);
    result->Register(boundedCtor->Execute(args));
    if (result->ShouldReturn()) {
        result->error->st = node->st;
        result->error->et = node->et;
        return result->Failure(result->error);
    }
    As<ClassObject>(returnValueTmp)->isProto = false;
    auto returnValue = returnValueTmp;
    return result->Success(returnValue);
}

[[noreturn]] RuntimeResult *Interpreter::VisitEmpty(NodeBase *node, Context *ctx) {
    std::cerr << "Fatal: Invilid node" << std::endl;
    assert(false);
}

RuntimeResult *Interpreter::VisitDefer(NodeBase *node, Context *ctx) {
    auto result = new RuntimeResult;
    // auto currentFrame = ctx->symbols->Get(ctx->ctxLabel);
    // if (currentFrame == nullptr) {
    //     return result->Failure(new RuntimeError(
    //         "'defer' statement outside a valid frame",
    //         node->st, node->et, ctx
    //     ));
    // }
    currentCallStackDepth++;
    // if (currentFrame->typeName != std::string("Function") && currentFrame->typeName != std::string("Method")) {
    //     return result->Failure(new RuntimeError(
    //         "frame is not callable",
    //         node->st, node->et, ctx
    //     ));
    // }

    if (currentCallStackDepth >= MAX_CALLSTACK_DEPTH) {
        currentCallStackDepth = 0;
        return result->Failure(new RuntimeError(
            std::format("Maximum call stack depth ({}) exceeded, recompile the source and change MAX_CALLSTACK_DEPTH to extend stack capacity", MAX_CALLSTACK_DEPTH),
            node->st, node->et, ctx
        ));
    }
    
    ctx->deferNodes.push_back(node);
    return result->Success(Number::null);
}

Interpreter::~Interpreter() = default;

void Initialize() {
    srand((unsigned) time(nullptr));
    // globalSymbolTable->Set("null", new Number(0));
    globalSymbolTable->Set("false", new Number(0));
    globalSymbolTable->Set("true", new Number(1));
    // globalSymbolTable->Set("pi", new Number(3.141592653589793238462));
    globalSymbolTable->Set("null", Number::null);
    InitializeBuiltins();
    SetBuiltins(globalSymbolTable);

    builtins::AllocEnvVars();
    builtins::RestoreEnvVar();
}

bool startAsShell = false;

void Interprete(const std::string &file, const std::string &text, InterpreterStartMode mode, const std::string &frameId, Context *parent, Position *parentEntry) {
    auto lexer = new Lexer(file, ToWideString(text));
    auto result = lexer->MakeTokens();
    if (result.second != nullptr) {
        std::cerr << result.second->ToString() << std::endl;
        return;
    }
    // if (debug) {
    //     std::cout << "[DEBUG] Tokens: ";
    //     PrintSequence(result.first);
    //     std::cout << std::endl;    
    // }

    auto parser = new Parser(result.first);
    auto parseResult = parser->Parse();
    if (parseResult->err != nullptr) {
        std::cerr << parseResult->err->ToString() << std::endl;
        return;       
    } 
    // if (debug) {
    //     std::cout << "[DEBUG] AST: " << parseResult->ast->ToString() << std::endl;    
    // }

    auto interpreter = new Interpreter();
    Context *context;
    if (mode != InterpreterStartMode::Evaluation) {
        context = new Context("<module>");
        context->symbols = globalSymbolTable;    
        context->interpreter = interpreter;
    } else {
        context = new Context(frameId);
        context->parent = parent;
        context->symbols = globalSymbolTable;
        context->parentEntry = parentEntry;
    }
    auto n = interpreter->Visit(parseResult->ast, context);
    if (n->error != nullptr) {
        if (n->cause == nullptr) {
            std::cerr << n->error->ToString() << std::endl;
            return; 
        } else {
            std::cerr << n->cause->ToString() << std::endl;
            if (n->extraInfo == "") {
                std::cerr << std::endl << "Above exception is the direct cause of the following exception: " << std::endl << std::endl;
            } else {
                std::cerr << std::endl << n->extraInfo << std::endl << std::endl;
            }
            std::cerr << n->error->ToString() << std::endl;
            return;
        }
    }

    if (n->value != nullptr) {
        if (n->value->typeName == "String") {
            // It may be impossible to reach here ...
            dynamic_cast<String *>(n->value)->Representation();
        } else {
            int resultCount = std::get<int>(As<Number>(n->value->Len().first)->value);
            if (resultCount == 1) {
                if (mode == InterpreterStartMode::Repl) {
                    auto o = As<List>(n->value)->elements[0];
                    if (o->typeName == std::string("String")) {
                        std::cout << "= ";
                        As<String>(o)->Representation();
                    } else {
                        std::cout << "= " << o->ToString() << std::endl;
                    }
                } else if (mode == InterpreterStartMode::Evaluation && startAsShell) {
                    auto o = As<List>(n->value)->elements[0];
                    if (o->typeName == std::string("String")) {
                        std::cout << std::format("[@{}]= ", frameId);
                        As<String>(o)->Representation(); 
                    } else {
                        std::cout << std::format("[@{}]= ", frameId) << o->ToString() << std::endl;                    
                    }
                } else if (mode == InterpreterStartMode::Evaluation && !startAsShell) {
                    auto o = As<List>(n->value)->elements[0];
                    if (o->typeName == std::string("String")) {
                        As<String>(o)->Representation();
                    } else {
                        std::cout << As<List>(n->value)->elements[0]->ToString() << std::endl;
                    }
                }
            } else {
                for (int i = 0; i < resultCount; i++) {
                    if (mode == InterpreterStartMode::Repl) {
                        auto o = As<List>(n->value)->elements[i];
                        if (o->typeName == std::string("String")) {
                            std::cout << std::format("[#{}]= ", i + 1);
                            As<String>(o)->Representation(); 
                        } else {
                            std::cout << std::format("[#{}]= ", i + 1) << o->ToString() << std::endl;                    
                        }
                        // std::cout << std::format("[#{}]= ", i + 1) << As<List>(n->value)->elements[i]->ToString() << std::endl;
                    } else if (mode == InterpreterStartMode::Evaluation && startAsShell) {
                        auto o = As<List>(n->value)->elements[i];
                        if (o->typeName == std::string("String")) {
                            std::cout << std::format("[#{}, @{}]= ", i + 1, frameId);
                            As<String>(o)->Representation(); 
                        } else {
                            std::cout << std::format("[#{}, @{}]= ", i + 1, frameId) << o->ToString() << std::endl;                    
                        }
                        // std::cout << std::format("[#{}, @{}]= ", i + 1, frameId) << As<List>(n->value)->elements[i]->ToString() << std::endl;
                    } else if (mode == InterpreterStartMode::Evaluation && !startAsShell) {
                        auto o = As<List>(n->value)->elements[i];
                        if (o->typeName == std::string("String")) {
                            As<String>(o)->Representation(); 
                        } else {
                            std::cout << o->ToString() << std::endl;                    
                        }
                        // std::cout << As<List>(n->value)->elements[i]->ToString() << std::endl;
                    }
                }
            }
        }
    }

    if (mode != InterpreterStartMode::Evaluation) {
        for (auto r : contextResultCache) {
            delete r;
        }
        contextResultCache.clear();
    }
    
    delete lexer;
    delete parser;
    delete interpreter;
}

void CopyCommandLineArgs(int argc, char **argv, InterpreterStartMode mode, SymbolTable *dest) {
    auto args = new List({});
    assert(mode != InterpreterStartMode::Evaluation);
    // if (debug) {
    //     std::cout << "[DEBUG] Applying command line args to the global symbol table" << std::endl;
    // }
    for (int i = 1; i < argc; i++) {
        args->elements.push_back(new String(std::string(argv[i])));
    } 
    dest->Set("args", args);
}

void Finalize() {
    delete globalSymbolTable;

    for (auto [builtinName, builtin] : allBuiltins) {
        delete builtin;
    }

    for (auto [_, dylib] : dylibs) {
        #ifdef __linux__
            auto moduleFinalizer = dlsym(dylib, "Yan_OnDestroy");
            if (moduleFinalizer != nullptr) {
                ((void (*)()) moduleFinalizer)();
            }
            dlclose(dylib);
        #elif defined(_WIN32)
            auto moduleFinalizer = win32::GetProcAddress(dylib, "Yan_OnDestroy");
            if (module != nullptr) {
                ((void (*)()) moduleFinalizer)();
            }
            win32::FreeLibrary(dylib);
        #endif
    }

    for (auto [_, mod] : nativeModules) {
        delete mod;
    }

    delete envVars;
}

void InterpreteFile(const std::string &filename) {
    std::ifstream ifs;
    std::ostringstream buf;
    ifs.open(filename, std::ios::in);
    if (!ifs.is_open()) {
        std::cerr << std::format("Fatal: Couldn't open script: '{}'", filename) << std::endl;
        return;
    }
    buf << ifs.rdbuf();
    auto script = buf.str();

    Interprete(filename, script, InterpreterStartMode::File);
}
