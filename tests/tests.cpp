#include <sstream>
#include <vector>
#include <random>
#include <string>
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
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
    EXPECT_THROW(parse("5 + + 7"), parser_exception);
    EXPECT_THROW(parse("((5 + 3) * 2) + --------"), parser_exception);
    EXPECT_THROW(parse("(5 + 3) + (5 - 3) + * -7"), parser_exception);
    EXPECT_THROW(parse("------------------------------"), parser_exception);
    EXPECT_THROW(parse("(((( 5 + 66)"), parser_exception);
    EXPECT_THROW(parse("(5 + 7)) *    3"), parser_exception);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}