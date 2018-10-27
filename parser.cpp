#include <type_traits>
#include "parser.h"


static bool in_list(token_type x, std::initializer_list<token_type> list) {
    for (auto item : list) {
        if (x == item) {
            return true;
        }
    }
    return false;
}



static node parse_E(const std::vector<token>& data, size_t& ind);
static node parse_X(const std::vector<token>& data, size_t& ind);
static node parse_T(const std::vector<token>& data, size_t& ind);
static node parse_Y(const std::vector<token>& data, size_t& ind);
static node parse_F(const std::vector<token>& data, size_t& ind);
static node parse_P(const std::vector<token>& data, size_t& ind);

static node parse_E(const std::vector<token>& data, size_t& ind) {
    node res(E);
    auto const& cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_T(data, ind));

        res.children.push_back(parse_X(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_X(const std::vector<token>& data, size_t& ind) {
    node res(X);
    auto const& cur = data[ind];
    if (in_list(cur.type, {PLUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_T(data, ind));

        res.children.push_back(parse_X(data, ind));
    } else if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_T(data, ind));

        res.children.push_back(parse_X(data, ind));
    } else if (in_list(cur.type, {RIGHT_PARENTHESIS, END})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {PLUS, MINUS, RIGHT_PARENTHESIS, END});
    }
    return res;
}

static node parse_T(const std::vector<token>& data, size_t& ind) {
    node res(T);
    auto const& cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_F(data, ind));

        res.children.push_back(parse_Y(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_Y(const std::vector<token>& data, size_t& ind) {
    node res(Y);
    auto const& cur = data[ind];
    if (in_list(cur.type, {MUL})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_F(data, ind));

        res.children.push_back(parse_Y(data, ind));
    } else if (in_list(cur.type, {END, RIGHT_PARENTHESIS, MINUS, PLUS})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {MUL, END, RIGHT_PARENTHESIS, MINUS, PLUS});
    }
    return res;
}

static node parse_F(const std::vector<token>& data, size_t& ind) {
    node res(F);
    auto const& cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, NUMBER})) {
        res.children.push_back(parse_P(data, ind));
    } else if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_P(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, NUMBER, MINUS});
    }
    return res;
}

static node parse_P(const std::vector<token>& data, size_t& ind) {
    node res(P);
    auto const& cur = data[ind];
    if (in_list(cur.type, {NUMBER})) {
        res.children.emplace_back(TERM, cur);
        ++ind;
    } else if (in_list(cur.type, {LEFT_PARENTHESIS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_E(data, ind));

        if (data[ind].type != RIGHT_PARENTHESIS) {
            throw parser_exception(data, ind, {RIGHT_PARENTHESIS});
        }

        res.children.emplace_back(TERM, cur);
        ++ind;
    }
    return res;
}

node parse(const std::vector<token> &data) {
    size_t ind = 0;
    return parse_E(data, ind);
}

std::string to_string(node_type x) {
    switch (x) {
        case E: {
            return "E";
        }
        case X: {
            return "X";;
        }
        case T: {
            return "T";
        }
        case Y: {
            return "Y";
        }
        case F: {
            return "F";
        }
        case P: {
            return "P";
        }
        case EPS: {
            return "EPS";
        }
        case TERM: {
            return "TERM";
        }
    }
}


parser_exception::parser_exception(std::vector<token> const &data, size_t pos,
                                   std::initializer_list<token_type> expected) {
    std::ostringstream os;
    os << "Unexpected token " << data[pos].type << " at position " << pos << ":\n";
    size_t cnt = 0;
    for (size_t i = 0; i < pos; ++i) {
        os << data[i].data << ' ';
        cnt += data[i].data.size() + 1;
    }
    os << data[pos].data << '\n';
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
    reason = os.str();
}

std::string node::to_json() const {
    json res;
    res[to_string(type)] = to_json_inner();
    return res.dump(2);
}

node::json node::to_json_inner() const {
    switch (type) {
        case EPS: {
            return "";
        }
        case TERM: {
            switch (data->type) {
                case NUMBER: {
                    return std::stoll(data->data);
                }
                default: {
                    return data->data;
                }
            }
        }
        default: {
            json res;
            for (auto&& item : children) {
                res[to_string(item.type)] = item.to_json_inner();
            }
            return res;
        }
    }
}
