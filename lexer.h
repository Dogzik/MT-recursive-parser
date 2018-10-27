#include <utility>

#pragma once

#include <exception>
#include <string>
#include <vector>
#include <iostream>

class lexer_exception : public std::exception {
    std::string reason;
public:
    const char *what() const noexcept override {
        return reason.c_str();
    }

    explicit lexer_exception(std::istream &in) : reason("Unexpected symbol at position ") {
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
};

enum token_type {
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS, PLUS, MINUS, NUMBER, MUL, END
};

std::ostream& operator<<(std::ostream &os, token_type type);

struct token {
    token_type type;
    std::string data;

    token(token_type _type, std::string _data)
            : type(_type), data(std::move(_data)) {}
};

std::vector<token> tokenize(std::istream &in);