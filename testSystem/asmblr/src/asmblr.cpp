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
#include <grammarFunctions.hpp>

typedef void (*GrammarFunctionPtr)(const std::vector<std::string>&, const std::string&);

GrammarFunctionPtr grammars[64];

std::string toLowerCaseString(std::string &string){
    std::string result = "";
    for(int i = 0; i < string.length(); i++){
        result += tolower(string[i]);
    }
    return result;
}

void whitespaceTrim(std::string &str)
{
    std::string whiteSpace = " \t";
    int lastCharacter = str.length() - 1;
    size_t firstCharacter = 0;
    while(whiteSpace.find(str[lastCharacter]) != std::string::npos && lastCharacter != 0)
        lastCharacter = lastCharacter - 1;
    while(whiteSpace.find(str[firstCharacter]) != std::string::npos && firstCharacter != str.length() - 1)
        firstCharacter++;

    str = str.substr(firstCharacter, lastCharacter - firstCharacter + 1);
    return;
}

bool isValidLineStart(char start){
    return (start >= 'a' && start <= 'z') || (start >= 'A' && start <= 'Z') || (start == '!') || (start == '@');
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

        if (nonWhitespacePos != std::string::npos && isValidLineStart(line[nonWhitespacePos])) {
            result.push_back(line);
        }
    }

    file.close();
    return result;
}

int countRegisters(std::vector<std::string> &line){
    int result = 0;
    std::string specialCharacters = "$!";
    for(int i = 0; i < line.size(); i++){
        result += (line[i][0] == '$');
        if(specialCharacters.find(line[i][0]) != std::string::npos)
            line[i] = line[i].substr(1);
    }
    return result;
}

std::string handleInstructionUpdate(std::vector<std::string> *line){
    std::string grammar = "70";
    std::string inst = toLowerCaseString(line->at(0));
    int regCnt = countRegisters(*line);
    int operCnt = line->size() - 1;
        
    if(inst.compare("and") == 0 || inst.compare("or") == 0 || inst.compare("xor") == 0 || inst.compare("shf") == 0
    || inst.compare("rot") == 0 || inst.compare("add") == 0 || inst.compare("sub") == 0 || inst.compare("set") == 0
    || inst.compare("mpy") == 0 || inst.compare("div") == 0 || inst.compare("mod") == 0 || inst.compare("lod") == 0
    || inst.compare("bts") == 0 || inst.compare("btc") == 0 || inst.compare("btf") == 0 || inst.compare("cmp") == 0
    || inst.compare("djz") == 0){
        if(operCnt == 3) grammar = "2";
        else if(regCnt == 2) grammar = "1";
        else grammar = "3";
    } else if(inst.compare("neg") == 0 || inst.compare("pop") == 0){
        grammar = "4";
    } else if(inst.compare("inc") == 0 || inst.compare("dec") == 0){
        grammar = "5";
    } else if(inst.compare("str") == 0){
        if(operCnt == 3) grammar = "6";
        else if(regCnt == 2) grammar = "1";
        else grammar = "7";
    } else if(inst.compare("psh") == 0 || inst.compare("cal") == 0){
        if(operCnt == 2) grammar = "9";
        else if(regCnt == 1) grammar = "8";
        else grammar = "10"; 
    } else if(inst.compare("RET") == 0){
        grammar = "11";
    } else if(inst.compare("jmp") == 0 || inst.compare("je") == 0 || inst.compare("jne") == 0 || inst.compare("jl") == 0
    || inst.compare("jle") == 0 || inst.compare("jg") == 0 || inst.compare("jge") == 0){
        if(operCnt == 2) grammar = "13";
        else if(regCnt == 1) grammar = "12";
        else grammar = "14"; 
    }
    return grammar;
}

void editLine(std::vector<std::string>* line){

    switch(line->at(0)[0]){
        case '!':{//label line
            if(line->size() == 2){
                line->insert(line->begin(), "34");
                line->at(1).assign(line->at(1).substr(1));
            } else {
                line->insert(line->begin(), "32");
                line->at(1) = line->at(1).substr(1);
            }
        break;}
        case '@':{//data line
            line->insert(line->begin(), "33");
            line->at(1) = line->at(1).substr(1);
        break;}
        default:{//instruction
            std::string grammar = handleInstructionUpdate(line);
            line->insert(line->begin(), grammar);
        break;}
    }
    return;
}

std::vector<std::string> splitOnWhitespace(const std::string& line) {
    std::vector<std::string> result;
    std::istringstream iss(line);
    std::string word;

    while (iss >> word) {
        if(word[0] == ';'){
            break;
        }
        result.push_back(word);
    }

    editLine(&result);

    return result;
}

int main(int argc, char* argv[]){

    std::string outFile;

    if(argc < 2){
        std::cout << "please provide an assembly file to assemble and output file name" << std::endl;
        exit(EXIT_FAILURE);
    } else if(argc < 3){
        // Removes old output.trash & makes new one
        if( remove( "output.trash" ) != 0 )
            perror( "Error deleting file" );
        else
            std::cout<< "File successfully deleted" << std::endl;

        outFile = "output.trash";
    } else {
        outFile = argv[2];
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
            std::cout << word << " ";
        }
        std::cout << std::endl;
    }

    for(std::string line : file){
        std::vector<std::string> sline = splitOnWhitespace(line);
        for(std::string word : sline){
            whitespaceTrim(word);
        }
        int grammarIndex = std::stoi(sline[0]);
        grammars[grammarIndex](sline, "output.txt");
    }

    std::remove("output.txt");
    std::cout << "removed output.txt" << std::endl;
    resetWordCnt();

    for(std::string line : file){
        std::vector<std::string> sline = splitOnWhitespace(line);
        for(std::string word : sline){
            whitespaceTrim(word);
        }
        int grammarIndex = std::stoi(sline[0]);
        grammars[grammarIndex](sline, outFile);
    }

    return EXIT_SUCCESS;
}