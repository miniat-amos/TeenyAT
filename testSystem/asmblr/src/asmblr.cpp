#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cstdint>
#include <sstream>
#include <vector>
#include <string.h>

#include <teenyat.h>

#define INSTRUCTION 0
#define LABEL 1
#define VARIABLE 2
#define DATASECTION_START_LABEL 3
#define COMMENT 4
#define GARBAGE 5
#define VALUE 6

bool isInstructionTiny(std::vector<std::string> line){
    return false;
}

int checkLineType(std::string firstWord){
    if((firstWord[0] >= 'A' && firstWord[0] <= 'Z') || (firstWord[0] >= 'a' && firstWord[0] <= 'z')){
        if(firstWord[firstWord.size() - 1] == ':'){
            return LABEL;
        }
        return INSTRUCTION;
    } else if((firstWord[0] >= '0' && firstWord[0] <= '9') || (firstWord[0] == '#')){
        return VALUE;
    } else if(firstWord[0] == "."){
        return VARIABLE;
    } else if(firstWord[0] == "@"){
        return DATASECTION_START_LABEL;
    } else if(firstWord[0] == ";"){
        return COMMENT;
    }
    return GARBAGE;
}

std::vector<std::string> readLinesFromFile(const std::string &filename){
    std::vector<std::string> lines;
    std::ifstream file;
    file.open(filename);
    if(!file.is_open()){
        std::cout << "Failed to open the file " << filename << " empty vector<string> returned" << std::endl;
        return lines;
    }

    std::string line;
    while(std::getline(file, line)){
        lines.push_back(line);
    }

    file.close();
    return lines;
}

std::vector<std::vector<std::string>> parseLines(const std::string &filename) {
    std::vector<std::string> lines = readLinesFromFile(filename);
    std::vector<std::vector<std::string>> parsedLines;

    for (const std::string &line : lines){
        std::stringstream ss(line);
        std::string word;
        std::vector<std::string> tokens;
        while(ss >> word) {
            tokens.push_back(word);
        }
        parsedLines.push_back(tokens);
    }

    return parsedLines;
}

int main(int argc, char* argv[]){
    std::unordered_map<std::string, uint16_t> variables;
    std::unordered_map<std::string, uint16_t> labels;
    uint16_t zero = 0;
    uint16_t currentAddress = 0;
    uint16_t outputFile[0x8000];
    bool datasection = false;
    
    //Check to ensure correct arguments provided
    if(argc < 2 || strcmp(argv[1], "--help") == 0){
        std::cout << "Usage: ./asmblr.out <path to .asm file to assemble>" << std::endl;
        return EXIT_FAILURE;
    }
    int lineCount = 0;
    //Load and parse the .asm file
    std::vector<std::vector<std::string>> parsed = parseLines(argv[1]);
    for (const auto &line : parsed) {
        if(!datasection){
            if(line.isEmpty || checkLineType(line[0]) == COMMENT){
                //remove this line
                lines.erase(lines.begin() + lineCount);
            }else if(checkLineType(line[0]) == LABEL){
                labels.insert_or_assign(line[0].substr(0, line[0].size() - 1), currentAddress);
            } else if(checkLineType(line[0]) == DATASECTION_START_LABEL){
                //this is the data section now things change a bit here where lists of data can be added as a specific label address
                datasection = true;
            } else if(checkLineType(line[0]) == VARIABLE){
                variables.insert_or_assing(line[0].substr(1,line[0].size()), line[1])
            } else if(checkLineType(line[0]) == INSTRUCTION){
                if(isInstructionTiny){
                    currentAddress++;
                }
                currentAddress++;
            } else {
                //something is wrong I could not tell what this line is supposed to be
                std::cout << "Something is wrong on line: " << line << std::endl;
                return EXIT_FAILURE;
            }
        } else {
            if(line.isEmpty || checkLineType(line[0]) == COMMENT){
                //remove this line
                lines.erase(lines.begin() + lineCount);
            }else if(checkLineType(line[0]) == LABEL){
                labels.insert_or_assign(line[0].substr(0, line[0].size() - 1), currentAddress);
            } else if(checkLineType(line[0] == VALUE)){
                for(std::string word : line){
                    if(checkLineType(line[0] == VALUE)){
                        currentAddress++;
                    } else if(checkLineType(word) == COMMENT){
                        break;
                    }
                }
            }else {
                //something is wrong I could not tell what this line is supposed to be
                std::cout << "Something is wrong on line: " << line << std::endl;
                return EXIT_FAILURE;
            }
        }
        line++;
    }


    //---make map that maps variable occurances to there values

    return EXIT_SUCCESS;
}