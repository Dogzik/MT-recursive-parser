#pragma once

#include <vector>
#include <sstream>
#include <memory>
#include <optional>
#include <fifo_map.h>
#include <json.h>
#include "lexer.h"


class parser_exception : public std::exception {
    std::string reason;
public:
    const char* what() const noexcept override {
        return reason.c_str();
    }

    parser_exception(std::vector<token> const& data, size_t pos, std::initializer_list<token_type> expected);
};

enum node_type {
    E, X, T, Y, F, TERM, EPS, I, A, H, B, K
};

std::string to_string(node_type x);

struct node {
    node_type type;
    std::optional<token> data;
    std::vector<node> children;


    node(node_type _type) : type(_type) {}
    node(node_type _type, std::vector<node> _children) : type(_type), children(std::move(_children)) {}
    node(node_type _type, token const& _data) : type(_type), data(_data), children() {}

    std::string to_json() const ;

private:
    template<class K, class V, class dummy_compare, class A>
    using my_workaround_fifo_map = nlohmann::fifo_map<K, V, nlohmann::fifo_map_compare<K>, A>;
    using json = nlohmann::basic_json<my_workaround_fifo_map>;

    json to_json_inner() const;
};

bool operator==(node const& a, node const& b);
bool operator!=(node const& a, node const& b);


node parse(const std::vector<token> &data);
node parse(std::istream& in);
node parse(std::string const& s);
