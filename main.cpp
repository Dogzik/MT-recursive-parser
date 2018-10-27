#include <iostream>
#include <sstream>
#include <fstream>
#include "lexer.h"
#include "parser.h"

using std::string;
using std::istringstream;
using std::cout;
using std::endl;

int main() {
    std::string s = "--3";
    istringstream is(s);

    auto tmp = tokenize(is);
    auto res = parse(tmp);

    cout << res.to_json();
    return 0;
}