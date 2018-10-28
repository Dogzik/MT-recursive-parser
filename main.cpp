#include <iostream>
#include <sstream>
#include <fstream>
#include <cstring>
#include "lexer.h"
#include "parser.h"

using std::string;
using std::istringstream;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;

void print_usage() {
    cerr << "Invalid options. Usage:\n";
    cerr << "-s <string_to_parse>\n";
    cerr << "-f <file_to_parse>\n";
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage();
        return 0;
    }
    if ((std::strcmp(argv[1], "-s") != 0) && (std::strcmp(argv[1], "-f") != 0)) {
        print_usage();
        return 0;
    }
    try {
        if (!std::strcmp(argv[1], "-s")) {
            istringstream is(argv[2]);
            cout << parse(tokenize(is)).to_json();
        } else {
            ifstream in(argv[2]);
            if (!in.is_open()) {
                cerr << "Can't open file: " << argv[2] << endl;
                return 0;
            }
            cout << parse(tokenize(in)).to_json();
        }
    } catch (std::exception const& e) {
        cerr << e.what();
    }
    return 0;
}