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
        res.children.emplace_back(TERM, cur.data);
        ++ind;

        res.children.push_back(parse_T(data, ind));

        res.children.push_back(parse_X(data, ind));
    } else if (in_list(cur.type, {MINUS})) {
        res.children.emplace_back(TERM, cur.data);
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
        res.children.emplace_back(TERM, cur.data);
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
        res.children.emplace_back(TERM, cur.data);
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
        res.children.emplace_back(TERM, cur.data);
        ++ind;
    } else if (in_list(cur.type, {LEFT_PARENTHESIS})) {
        res.children.emplace_back(TERM, cur.data);
        ++ind;

        res.children.push_back(parse_E(data, ind));

        if (data[ind].type != RIGHT_PARENTHESIS) {
            throw parser_exception(data, ind, {RIGHT_PARENTHESIS});
        }

        res.children.emplace_back(TERM, cur.data);
        ++ind;
    }
    return res;
}

node parse(const std::vector<token> &data) {
    size_t ind = 0;
    return parse_E(data, ind);
}
