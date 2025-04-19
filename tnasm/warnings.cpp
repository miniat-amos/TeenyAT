#include <iostream>
#include <vector>
#include <string>

#include "warnings.h"

using namespace std;

vector <string> warnings;

void new_warning(int lineno, string msg) {
    string s = "WARNING, Line " +  to_string(lineno) + ": " + msg;
    warnings.push_back(s);
    
    return;
}


void print_warnings() {
    if(warnings.size() > 0) {
        cerr << endl << "------------------------------------" << endl;
        for(auto w: warnings) {
            cerr << w << endl;
        }
        cerr << endl;
    }

    return;
}
