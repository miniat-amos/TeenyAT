#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>
#include <algorithm>
#include <map>
#include <cctype>
#include <cstdio>

#include <teenyat.h>
#include "grammarFunctions.hpp"

typedef void (*GrammarFunctionPtr)(const std::vector<std::string>&, const std::string&);

GrammarFunctionPtr grammars[64];

void whitespaceTrim(std::string &str)
{
    std::string whiteSpace = " \t";
    int lastCharacter = str.length() - 1;
    int firstCharacter = 0;
    while(whiteSpace.find(str[lastCharacter]) != std::string::npos && lastCharacter != 0)
        lastCharacter = lastCharacter - 1;
    while(whiteSpace.find(str[firstCharacter]) != std::string::npos && firstCharacter != str.length() - 1)
        firstCharacter++;

    str = str.substr(firstCharacter, lastCharacter - firstCharacter + 1);
    return;
}

std::vector<std::string> linesStartingWithNumber(const std::string& filepath) {
    std::vector<std::string> result;
    std::ifstream file(filepath);

    if (!file.is_open()) {
        std::cerr << "Failed to open " << filepath << std::endl;
        return result;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t nonWhitespacePos = line.find_first_not_of(" \t"); // skipping spaces and tabs

        if (nonWhitespacePos != std::string::npos && std::isdigit(line[nonWhitespacePos])) {
            result.push_back(line);
        }
    }

    file.close();
    return result;
}

std::vector<std::string> splitOnWhitespace(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string word;

    while (iss >> word) {
        result.push_back(word);
    }

    return result;
}

int main(int argc, char* argv[]){

    if(argc < 3){
        std::cout << "please provide an assembly file to assemble and output file name" << std::endl;
        exit(EXIT_FAILURE);
    }


    grammars[0] = grammar0;
    grammars[1] = grammar1;
    grammars[2] = grammar2;
    grammars[3] = grammar3;
    grammars[4] = grammar4;
    grammars[5] = grammar5;
    grammars[6] = grammar6;
    grammars[7] = grammar7;
    grammars[8] = grammar8;
    grammars[9] = grammar9;
    grammars[10] = grammar10;
    grammars[11] = grammar11;
    grammars[12] = grammar12;
    grammars[13] = grammar13;
    grammars[14] = grammar14;

    grammars[32] = grammar32;
    grammars[33] = grammar33;
    grammars[34] = grammar34;

    std::vector<std::string> file = linesStartingWithNumber(argv[1]);

    for(std::string line : file){
        std::vector<std::string> sline = splitOnWhitespace(line);
        for(std::string word : sline){
            whitespaceTrim(word);
        }
        int grammarIndex = std::stoi(sline[0]);
        std::cout << grammarIndex << std::endl;
        grammars[grammarIndex](sline, "output.txt");
    }

    std::remove("output.txt");
    std::cout << "removed output.txt" << std::endl;

    for(std::string line : file){
        std::vector<std::string> sline = splitOnWhitespace(line);
        for(std::string word : sline){
            whitespaceTrim(word);
        }
        int grammarIndex = std::stoi(sline[0]);
        std::cout << grammarIndex << std::endl;
        grammars[grammarIndex](sline, argv[2]);
    }

    return EXIT_SUCCESS;
}