#include <iostream>
#include <sstream>
#include <fstream>
#include "lexer.h"

using std::string;
using std::istringstream;
using std::cout;
using std::endl;

int main() {
    char buff[228];
    std::string s = "1+(2+3*ю-7)";
    istringstream is(s);
    std::ifstream in("input.txt");

    auto res = tokenize(is);
    for (auto& x : res) {
        cout << x.type << " " << x.data << endl;
    }

    return 0;
}