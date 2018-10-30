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


static node parse_E(const std::vector<token> &data, size_t &ind);

static node parse_X(const std::vector<token> &data, size_t &ind);

static node parse_T(const std::vector<token> &data, size_t &ind);

static node parse_Y(const std::vector<token> &data, size_t &ind);

static node parse_F(const std::vector<token> &data, size_t &ind);


static node parse_I(const std::vector<token> &data, size_t &ind);

static node parse_A(const std::vector<token> &data, size_t &ind);

static node parse_H(const std::vector<token> &data, size_t &ind);

static node parse_B(const std::vector<token> &data, size_t &ind);

static node parse_K(const std::vector<token> &data, size_t &ind);


static node parse_E(const std::vector<token> &data, size_t &ind) {
    node res(E);
    auto const &cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_T(data, ind));

        res.children.push_back(parse_X(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_X(const std::vector<token> &data, size_t &ind) {
    node res(X);
    auto const &cur = data[ind];
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
    } else if (in_list(cur.type, {END})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {PLUS, MINUS, END});
    }
    return res;
}

static node parse_T(const std::vector<token> &data, size_t &ind) {
    node res(T);
    auto const &cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_F(data, ind));

        res.children.push_back(parse_Y(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_Y(const std::vector<token> &data, size_t &ind) {
    node res(Y);
    auto const &cur = data[ind];
    if (in_list(cur.type, {MUL})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_F(data, ind));

        res.children.push_back(parse_Y(data, ind));
    } else if (in_list(cur.type, {END, MINUS, PLUS})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {MUL, END, MINUS, PLUS});
    }
    return res;
}

static node parse_F(const std::vector<token> &data, size_t &ind) {
    node res(F);
    auto const &cur = data[ind];
    if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_F(data, ind));
    } else if (in_list(cur.type, {NUMBER})) {
        res.children.emplace_back(TERM, cur);
        ++ind;
    } else if (in_list(cur.type, {LEFT_PARENTHESIS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_I(data, ind));

        if (data[ind].type != RIGHT_PARENTHESIS) {
            throw parser_exception(data, ind, {RIGHT_PARENTHESIS});
        }

        res.children.emplace_back(TERM, data[ind]);
        ++ind;
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, NUMBER, MINUS});
    }
    return res;
}


static node parse_I(const std::vector<token> &data, size_t &ind) {
    node res(I);
    auto const &cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_H(data, ind));

        res.children.push_back(parse_A(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_A(const std::vector<token> &data, size_t &ind) {
    node res(A);
    auto const &cur = data[ind];
    if (in_list(cur.type, {PLUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_H(data, ind));

        res.children.push_back(parse_A(data, ind));
    } else if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_H(data, ind));

        res.children.push_back(parse_A(data, ind));
    } else if (in_list(cur.type, {RIGHT_PARENTHESIS})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {PLUS, MINUS, RIGHT_PARENTHESIS});
    }
    return res;
}

static node parse_H(const std::vector<token> &data, size_t &ind) {
    node res(H);
    auto const &cur = data[ind];
    if (in_list(cur.type, {LEFT_PARENTHESIS, MINUS, NUMBER})) {
        res.children.push_back(parse_K(data, ind));

        res.children.push_back(parse_B(data, ind));
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, MINUS, NUMBER});
    }
    return res;
}

static node parse_B(const std::vector<token> &data, size_t &ind) {
    node res(B);
    auto const &cur = data[ind];
    if (in_list(cur.type, {MUL})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_K(data, ind));

        res.children.push_back(parse_B(data, ind));
    } else if (in_list(cur.type, {RIGHT_PARENTHESIS, MINUS, PLUS})) {
        res.children.emplace_back(EPS);
    } else {
        throw parser_exception(data, ind, {MUL, RIGHT_PARENTHESIS, MINUS, PLUS});
    }
    return res;
}

static node parse_K(const std::vector<token> &data, size_t &ind) {
    node res(K);
    auto const &cur = data[ind];
    if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_K(data, ind));
    } else if (in_list(cur.type, {NUMBER})) {
        res.children.emplace_back(TERM, cur);
        ++ind;
    } else if (in_list(cur.type, {LEFT_PARENTHESIS})) {
        res.children.emplace_back(TERM, cur);
        ++ind;

        res.children.push_back(parse_I(data, ind));

        if (data[ind].type != RIGHT_PARENTHESIS) {
            throw parser_exception(data, ind, {RIGHT_PARENTHESIS});
        }

        res.children.emplace_back(TERM, data[ind]);
        ++ind;
    } else {
        throw parser_exception(data, ind, {LEFT_PARENTHESIS, NUMBER, MINUS});
    }
    return res;
}

node parse(const std::vector<token> &data) {
    size_t ind = 0;
    return parse_E(data, ind);
}

node parse(std::istream &in) {
    return parse(tokenize(in));
}

node parse(std::string const &s) {
    return parse(tokenize(s));
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
        case EPS: {
            return "EPS";
        }
        case TERM: {
            return "TERM";
        }
        case I: {
            return "I";
        }
        case A: {
            return "A";
        }
        case H: {
            return "H";
        }
        case B: {
            return "B";
        }
        case K: {
            return "K";
        }
    }
}

bool operator==(node const &a, node const &b) {
    if (a.type != b.type) {
        return false;
    }
    if (a.type == TERM) {
        return (a.data == b.data);
    }
    if (a.children.size() != b.children.size()) {
        return false;
    }
    for (size_t i = 0; i < a.children.size(); ++i) {
        if (!(a.children[i] == b.children[i])) {
            return false;
        }
    }
    return true;
}

bool operator!=(node const &a, node const &b) {
    return !(a == b);
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
    return to_json_inner().dump(2);
}

node::json node::to_json_inner() const {
    switch (type) {
        case EPS: {
            return ::to_string(EPS);
        }
        case TERM: {
            return data->data;
        }
        default: {
            json tmp;
            for (auto &&item : children) {
                tmp.push_back(item.to_json_inner());
            }
            return { {::to_string(type), tmp} };
        }
    }
}

std::string node::to_string() const {
    switch (type) {
        case EPS: {
            return "";
        }
        case TERM: {
            return data->data;
        }
        default: {
            std::string ans;
            for (auto && item : children) {
                ans.append(item.to_string());
            }
            return ans;
        }
    }
}
