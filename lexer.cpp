#include <cctype>
#include <sstream>
#include "lexer.h"

using std::vector;
using std::string;
using std::istream;

static bool my_isspace(char ch) {
    return std::isspace(static_cast<unsigned char>(ch));
}

vector<token> tokenize(istream &in) {
    string buff;
    vector<token> res;
    auto check = [&buff, &res]() {
        if (!buff.empty()) {
            res.emplace_back(NUMBER, buff);
            buff.clear();
        }
    };
    int x;
    while ((x = in.get()) != istream::traits_type::eof()) {
        char c = static_cast<char>(x);
        if (my_isspace(c)) {
            check();
            continue;
        }
        if ('0' <= c && c <= '9') {
            buff.push_back(c);
            continue;
        }
        switch (c) {
            case '+': {
                check();
                res.emplace_back(PLUS, "+");
                break;
            }
            case '-': {
                check();
                res.emplace_back(MINUS, "-");
                break;
            }
            case '*': {
                check();
                res.emplace_back(MUL, "*");
                break;
            }
            case '(': {
                check();
                res.emplace_back(LEFT_PARENTHESIS, "(");
                break;
            }
            case ')': {
                check();
                res.emplace_back(RIGHT_PARENTHESIS, ")");
                break;
            }
            default: {
                throw lexer_exception(in);
            }
        }
    }
    check();
    res.emplace_back(END, "");
    return res;
}



std::ostream &operator<<(std::ostream &os, token_type type) {
    switch (type) {
        case LEFT_PARENTHESIS: {
            return os << "LEFT_PARENTHESIS";
        }
        case RIGHT_PARENTHESIS: {
            return os << "RIGHT_PARENTHESIS";
        }
        case PLUS: {
            return os << "PLUS";
        }
        case MINUS: {
            return os << "MINUS";
        }
        case MUL: {
            return os << "MUL";
        }
        case NUMBER: {
            return os << "NUMBER";
        }
        case END: {
            return os << "END";
        }
    }
}

std::vector<token> tokenize(std::string const &s) {
    std::istringstream in(s);
    return tokenize(in);
}

bool operator==(token const &a, token const &b) {
    return (a.type == b.type) && (a.data == b.data);
}

bool operator!=(token const &a, token const &b) {
    return !(a == b);
}


lexer_exception::lexer_exception(std::istream &in) : reason("Unexpected symbol at position ") {
    auto pos = in.tellg();
    reason.append(std::to_string(pos));
    reason.append(":\n");
    auto sz = reason.size();
    reason.resize(sz + pos);
    in.seekg(0);
    in.read(reason.data() + sz, pos);
    reason.push_back('\n');
    pos -= 1;
    while (pos > 0) {
        reason.push_back(' ');
        pos -= 1;
    }
    reason.append("^");
}
