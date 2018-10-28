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

    explicit lexer_exception(std::istream &in);
};

enum token_type {
    LEFT_PARENTHESIS, RIGHT_PARENTHESIS, PLUS, MINUS, NUMBER, MUL, END
};

std::ostream& operator<<(std::ostream &os, token_type type);

struct token {
    token_type type;
    std::string data;

    explicit token(token_type _type, std::string _data = "")
            : type(_type), data(std::move(_data)) {}
};

bool operator==(token const& a, token const& b);
bool operator!=(token const& a, token const& b);

std::vector<token> tokenize(std::istream &in);