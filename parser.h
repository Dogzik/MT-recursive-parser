#pragma once

#include <vector>
#include <sstream>
#include <memory>
#include "lexer.h"


class parser_exception : public std::exception {
    std::string reason;
public:
    const char* what() const noexcept override {
        return reason.c_str();
    }

    parser_exception(std::vector<token> const& data, size_t pos, std::initializer_list<token_type> expected) {
        std::ostringstream os;
        os << "Unexpected token " << data[pos].type << "at position pos:\n";
        size_t cnt = 0;
        for (size_t i = 0; i < pos; ++i) {
            os << data[i].data;
            cnt += data[i].data.size();
        }
        os << data[pos].data;
        for (size_t i = 0; i < cnt; ++i) {
            os.put(' ');
        }
        os.put('^');
        for (size_t i = 1; i < data[pos].data.size(); ++i) {
            os.put('~');
        }
        os << "\nExpected: ";
        for (auto x : expected) {
            os << x << " ";
        }
    }
};

enum node_type {
    E, X, T, Y, F, P, TERM, EPS
};


struct node {
    node_type type;
    std::string data;
    std::vector<node> children;

    explicit node(node_type _type, std::string _data = "") : type(_type), data(std::move(_data)), children() {}
};

node parse(const std::vector<token> &data);
