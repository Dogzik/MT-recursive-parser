#include <sstream>
#include <vector>
#include <random>
#include <string>
#include <unordered_map>
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
#include <queue>
#include "../lexer.h"
#include "../parser.h"

using std::istringstream;
using std::vector;
using std::string;
using vn = vector<node>;
using n = node;

void reset_stream(istringstream& is, std::string const& data) {
    is.clear();
    is.str(data);
}

vector<token> gen_random_tokens(size_t cnt) {
    auto generator = std::ranlux24();
    vector<token> res;
    static constexpr token_type types[] = {LEFT_PARENTHESIS, RIGHT_PARENTHESIS, PLUS, MINUS, NUMBER, MUL};
    constexpr int types_cnt = 6;
    for (size_t i = 0; i < cnt; ++i) {
        auto type = types[generator() % types_cnt];
        switch (type) {
            case LEFT_PARENTHESIS: {
                res.emplace_back(LEFT_PARENTHESIS, "(");
                break;
            }
            case RIGHT_PARENTHESIS: {
                res.emplace_back(RIGHT_PARENTHESIS, ")");
                break;
            }
            case PLUS: {
                res.emplace_back(PLUS, "+");
                break;
            }
            case MINUS: {
                res.emplace_back(MINUS, "-");
                break;
            }
            case MUL: {
                res.emplace_back(MUL, "*");
                break;
            }
            case NUMBER: {
                auto number = std::to_string(generator());
                res.emplace_back(NUMBER, number);
                break;
            }
            default: {
                assert(false);
            }
        }
    }
    res.emplace_back(END);
    return res;
}

string to_string(vector<token> const& tokens) {
    string res;
    for (auto&& item : tokens) {
        res.append(item.data);
        res.push_back(' ');
    }
    return res;
}

node gen_random_tree(int max_depth) {
    auto generator = std::ranlux24();
    using std::unordered_map;
    unordered_map<node_type, vector<vector<std::pair<node_type, std::optional<token>>>>> rules{};
    
    auto t = [&generator](node_type nt, token_type tt = END) -> std::pair<node_type, std::optional<token>> {
        std::optional<token> data;
        switch (nt) {
            case EPS: {
                return {nt, data};
            }
            case TERM: {
                switch (tt) {
                    case PLUS: {
                        data = token(tt, "+");
                        break;
                    }
                    case MINUS: {
                        data = token(tt, "-");
                        break;
                    }
                    case MUL: {
                        data = token(tt, "*");
                        break;
                    }
                    case NUMBER: {
                        data = token(tt, std::to_string(generator()));
                        break;
                    }
                    case LEFT_PARENTHESIS: {
                        data = token(tt, "(");
                        break;
                    }
                    case RIGHT_PARENTHESIS: {
                        data = token(tt, ")");
                        break;
                    }
                    default: {
                        assert(false);
                    }
                }
                return {nt, data};
            }
            default: {
                return {nt, data};
            }
        }
    };
    
    
    rules[E].push_back({t(T), t(X)});

    rules[X].push_back({t(EPS)});
    rules[X].push_back({t(TERM, PLUS), t(T), t(X)});
    rules[X].push_back({t(TERM, MINUS), t(T), t(X)});

    rules[T].push_back({t(F), t(Y)});


    rules[Y].push_back({t(EPS)});
    rules[Y].push_back({t(TERM, MUL), t(F), t(Y)});

    rules[F].push_back({t(TERM, NUMBER)});
    rules[F].push_back({t(TERM, MINUS), t(F)});
    rules[F].push_back({t(TERM, PLUS), t(F)});
    rules[F].push_back({t(TERM, LEFT_PARENTHESIS), t(E), t(TERM, RIGHT_PARENTHESIS)});

    node res(E);

    std::queue<std::pair<int, node*>> que;
    que.push({0, &res});

    while (!que.empty()) {
        auto tmp = que.front();
        auto cur = tmp.second;
        auto depth = tmp.first;
        que.pop();
        auto ind = generator() % rules[cur->type].size();
        if (depth > max_depth) {
            ind = 0;
        }
        cur->children.reserve(rules[cur->type][ind].size());
        for (auto&& item : rules[cur->type][ind]) {
            switch (item.first) {
                case EPS: {
                    cur->children.emplace_back(EPS);
                    break;
                }
                case TERM: {
                    cur->children.emplace_back(TERM, *item.second);
                    break;
                }
                default: {
                    cur->children.emplace_back(item.first);
                    que.push({depth + 1, &(cur->children.back())});
                    break;
                }
            }
        }
    }
    return res;
}

TEST(Lexing, BasicTest) {
    istringstream is;
    vector<token> expected;

    reset_stream(is, "1");
    expected = {token(NUMBER, "1"), token(END)};
    EXPECT_EQ(tokenize(is), expected);
    EXPECT_EQ(tokenize(is.str()), expected);

    reset_stream(is, " \t\n 1 \n  \t  ");
    EXPECT_EQ(tokenize(is), expected);
    EXPECT_EQ(tokenize(is.str()), expected);

    reset_stream(is, "(2+3)*4");
    expected = {token(LEFT_PARENTHESIS, "("), token(NUMBER, "2"), token(PLUS, "+"), token(NUMBER, "3"),
                token(RIGHT_PARENTHESIS, ")"), token(MUL, "*"), token(NUMBER, "4"), token(END)};
    EXPECT_EQ(tokenize(is), expected);
    EXPECT_EQ(tokenize(is.str()), expected);

    reset_stream(is, "  (  2  +  \n 3  )  *  \t\t\t\t\t\t 4  \n");
    EXPECT_EQ(tokenize(is), expected);
    EXPECT_EQ(tokenize(is.str()), expected);

    reset_stream(is, "");
    expected = {token(END)};
    EXPECT_EQ(tokenize(is), expected);
    EXPECT_EQ(tokenize(is.str()), expected);
}

TEST(Lexing, Failures) {
    istringstream is;

    reset_stream(is, "\n1\n2  + (((((( ** ((()()))) + 2 ---7");
    EXPECT_NO_THROW(tokenize(is));
    EXPECT_NO_THROW(tokenize(is.str()));

    reset_stream(is, "3451093456103456014356 + --------2");
    EXPECT_NO_THROW(tokenize(is));
    EXPECT_NO_THROW(tokenize(is.str()));

    reset_stream(is, "\t\t\t\t\n\n\n 1 + 3 / 4");
    EXPECT_THROW(tokenize(is), lexer_exception);
    EXPECT_THROW(tokenize(is.str()), lexer_exception);

    reset_stream(is, "2424 + ------- 2 * 3 x ++ 8");
    EXPECT_THROW(tokenize(is), lexer_exception);
    EXPECT_THROW(tokenize(is.str()), lexer_exception);

    reset_stream(is, "234234234 + 7 9 3 ((((((((((((((****** 4 5)% 3");
    EXPECT_THROW(tokenize(is), lexer_exception);
    EXPECT_THROW(tokenize(is.str()), lexer_exception);
}

TEST(Lexing, RandomTokens) {
    istringstream is;
    vector<token> expected;

    for (size_t len = 0; len < 200; ++len) {
        expected = gen_random_tokens(len);
        reset_stream(is, to_string(expected));
        EXPECT_EQ(tokenize(is), expected);
        EXPECT_EQ(tokenize(is.str()), expected);
    }
}


TEST(Parsing, BasicTest) {
    istringstream is;
    node expected = n(E, vn{n(T, vn{n(F, vn{n(TERM, token(NUMBER, "1"))}), n(Y, vn{n(EPS)})}),  n(X, vn{n(EPS)})});

    auto tmp = vector<token>{token(NUMBER, "1"), token(END)};
    EXPECT_EQ(parse(tmp), expected);

    reset_stream(is, "1");
    EXPECT_EQ(parse(is), expected);
    EXPECT_EQ(parse(is.str()), expected);

    reset_stream(is, "    \n\n\n\n\n\t  1 \t\t\t\t");
    EXPECT_EQ(parse(is), expected);
    EXPECT_EQ(parse(is.str()), expected);

    expected = n(E, vn{n(T, vn{n(F, vn{n(TERM, token(MINUS, "-")), n(F, vn{n(TERM, token(NUMBER, "1"))})}), n(Y, vn{n(EPS)})}),  n(X, vn{n(EPS)})});
    tmp = {token(MINUS, "-"), token(NUMBER, "1"), token(END)};
    EXPECT_EQ(parse(tmp), expected);

    reset_stream(is, "-1");
    EXPECT_EQ(parse(is), expected);
    EXPECT_EQ(parse(is.str()), expected);

    reset_stream(is, "    \n\n\n\n\n\n -\t\t\t\t\t\t\t 1     \n");
    EXPECT_EQ(parse(is), expected);
    EXPECT_EQ(parse(is.str()), expected);
}

TEST(Parsing, Failures) {
    EXPECT_NO_THROW(parse("1 + 1 + 2 * 4"));
    EXPECT_THROW(parse("1 + 2 / 1"), lexer_exception);
    EXPECT_THROW(parse("1 + 1 + 124 *"), parser_exception);
    EXPECT_THROW(parse("()"), parser_exception);
    EXPECT_THROW(parse("5 + 0x14"), lexer_exception);
    EXPECT_NO_THROW(parse("5 + + 7"));
    EXPECT_THROW(parse("((5 + 3) * 2) + --------"), parser_exception);
    EXPECT_THROW(parse("(5 + 3) + (5 - 3) + * -7"), parser_exception);
    EXPECT_THROW(parse("------------------------------"), parser_exception);
    EXPECT_THROW(parse("(((( 5 + 66)"), parser_exception);
    EXPECT_THROW(parse("(5 + 7)) *    3"), parser_exception);
}

TEST(Parsing, RandomExpressions) {
    for (int depth = 1; depth < 60; ++depth) {
        auto expected = gen_random_tree(depth);
        EXPECT_EQ(parse(expected.to_string()), expected);
    }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}