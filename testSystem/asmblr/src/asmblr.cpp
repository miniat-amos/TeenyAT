#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <sstream>

#include <teenyat.h>


//state machine states
std::string newToken(std::ifstream &assemblyFile, std::ofstream &tokenFile);
std::string newVariable(std::ifstream &assemblyFile, std::ofstream &tokenFile);
void commentStart(std::ifstream &assemblyFile);
std::string memoryAddressTokenStart(std::ifstream &assemblyFile, std::ofstream &tokenFile);
uint8_t registerStart(std::ifstream &assemblyFile, std::ofstream &tokenFile);
void letterTokenStart(std::ifstream &assemblyFile, std::ofstream &tokenFile);
std::string hexLiteralStart(std::ifstream &assemblyFile, std::ofstream &tokenFile);
std::string decimalLiteralTokenStart(char firstChar, std::ifstream &assemblyFile, std::ofstream &tokenFile);

int line = 0;
std::map<std::string,std::string> variables;
std::map<std::string,std::string> labels;
std::map<std::string,int>instructions = {
    {"and", TNY_OPCODE_AND},
    {"or", TNY_OPCODE_OR},
    {"xor", TNY_OPCODE_XOR},
    {"inv", TNY_OPCODE_XOR},//pseudo instruction according to the vm all 1's xor
    {"shf", TNY_OPCODE_SHF},
    {"rot", TNY_OPCODE_ROT},
    {"add", TNY_OPCODE_ADD},
    {"sub", TNY_OPCODE_SUB},
    {"mpy", TNY_OPCODE_MPY},
    {"div", TNY_OPCODE_DIV},
    {"mod", TNY_OPCODE_MOD},
    {"neg", TNY_OPCODE_NEG},
    {"inc", TNY_OPCODE_ADD},
    {"dec", TNY_OPCODE_SUB},
    {"set", TNY_OPCODE_SET},
    {"lod", TNY_OPCODE_LOD},
    {"str", TNY_OPCODE_STR},
    {"psh", TNY_OPCODE_PSH},
    {"pop", TNY_OPCODE_POP},
    {"bts", TNY_OPCODE_BTS},
    {"btc", TNY_OPCODE_BTC},
    {"btf", TNY_OPCODE_BTF},
    {"cal", TNY_OPCODE_CAL},
    {"ret", TNY_OPCODE_POP},//pseudo instruction pop into PC
    {"cmp", TNY_OPCODE_CMP},
    {"jmp", TNY_OPCODE_JMP},
    {"je", TNY_OPCODE_JMP},
    {"jne", TNY_OPCODE_JMP},
    {"jl", TNY_OPCODE_JMP},
    {"jle", TNY_OPCODE_JMP},
    {"jg", TNY_OPCODE_JMP},
    {"jge", TNY_OPCODE_JMP},
    {"djz", TNY_OPCODE_DJZ}
};

std::map<std::string,int>registers = {
    {"pc", TNY_REG_PC},
    {"sp", TNY_REG_SP},
    {"z", TNY_REG_ZERO},
    {"a", TNY_REG_A},
    {"b", TNY_REG_B},
    {"c", TNY_REG_C},
    {"d", TNY_REG_D},
    {"e", TNY_REG_E}
};

std::string toHexString(uint16_t value){
    std::ostringstream oss;
    oss << "0x" << std::hex << std::setw(2) << std::setfill('0') << 
        static_cast<int>(value);
    return oss.str();
}

uint16_t fromHexString(std::string hexString){
    int value;
    std::istringstream iss(hexString);
    iss >> std::hex >> value;
    return static_cast<uint16_t>(value);
}

bool dataSection = false;
uint16_t literalBuffer = 0;
std::string characterBuffer = "";

std::string newVariable(std::ifstream &assemblyFile, std::ofstream &tokenFile){
std::cout << "working on variable token" << std::endl;
    char character;
    assemblyFile.get(character);
    if(character != '\n'){
        std::string followingCharacters = newVariable(assemblyFile, tokenFile);
return "" + character + followingCharacters;
    }
    return "";
}

void commentStart(std::ifstream &assemblyFile){

    std::cout << "working on comment token" << std::endl;
    char character;
    assemblyFile.get(character);
    if(character != '\n')
        commentStart(assemblyFile);
    return;
}

std::string memoryAddressTokenStart(std::ifstream &assemblyFile, std::ofstream &tokenFile){
    
    std::cout << "working on memory token" << std::endl;
    //returns something like mem:reg:0x2 or mem4:reg:0x2:imm4:0x7
    char character;
    assemblyFile.get(character);
    switch(character){
        case '$':{
            //can be a register
            std::string leftSide = "reg:" + toHexString(registerStart(assemblyFile, tokenFile));//had to enforce order here
            return leftSide + memoryAddressTokenStart(assemblyFile, tokenFile);
            break;
        }
        case '#':{
            //can be a hex immediate 4 or 16 bits
            uint16_t value = fromHexString(hexLiteralStart(assemblyFile, tokenFile));
            if(value > 0xf){
                return "imm16:" + toHexString(value);
            }
            return "imm4:" + toHexString(value) + memoryAddressTokenStart(assemblyFile, tokenFile);
            break;
        }
        case ' ':
            return "" + memoryAddressTokenStart(assemblyFile, tokenFile);
            break;
        case ']':
            //end of memory address
            return "";
            break;
        case '+':
            return ":" + memoryAddressTokenStart(assemblyFile, tokenFile);
            break;
        default:
            //can be a decimal literal
            if(character >= '0' && character <= '9'){
                std::string hexValue = decimalLiteralTokenStart(character, assemblyFile, tokenFile);
                uint16_t value = fromHexString(hexValue);
                if(value > 0xf){
                    return "imm16:" + toHexString(value) + memoryAddressTokenStart(assemblyFile, tokenFile);
                }
                return "imm4:" + toHexString(value)+ memoryAddressTokenStart(assemblyFile, tokenFile);
            } else if(character >= 'A' && character <= 'Z'){
                return "imm16:0xdd";
            }
            break;
    }
    return "" + memoryAddressTokenStart(assemblyFile, tokenFile);
}

uint8_t registerStart(std::ifstream &assemblyFile, std::ofstream &tokenFile){
    
    std::cout << "working on register token" << std::endl;
    char character;
    assemblyFile.get(character);
    uint8_t value;
    std::string whiteSpace = "\n\t ";
    switch(character){
        case 'P':{
            //program counter
            value = 0;
            break;
        }
        case 'S':
            //stack pointer
            return value = 1;
            break;
        case 'Z':
            value = 2;
            break;
        default:
            //can be a decimal literal
            if(character >= '0' && character <= '9'){
                value = character - '0';
            }
            break;
    }
    //waist characters till whitespace
    while(whiteSpace.find(character) != std::string::npos){
        std::cout << "waisting space" << std::endl;
        assemblyFile.get(character);
    }
    return value;
}

std::string hexLiteralStart(std::ifstream &assemblyFile, std::ofstream &tokenFile){
    return "";
}

void letterTokenStart(std::ifstream &assemblyFile, std::ofstream &tokenFile){
    return;
}

std::string decimalLiteralTokenStart(char firstChar, std::ifstream &assemblyFile, std::ofstream &tokenFile){
    
    std::cout << "working on decimal token" << std::endl;
    return "";
}

std::string newToken(std::ifstream &assemblyFile, std::ofstream &tokenFile){
    std::string tokenString = "";
    char character;
std::string whiteSpace = "\t ";
    assemblyFile.get(character);
    switch(character){
        case '.': {//this token is a variable definition finished
            std::string newVariableName = newVariable(assemblyFile, tokenFile);
            variables.insert(std::make_pair(newVariableName , newToken(assemblyFile, tokenFile)));
            break;
}
        case ';'://this token is a comment finished
            commentStart(assemblyFile);
            break;
        case '@'://this token indicates the start of the data section finished
            commentStart(assemblyFile);
            dataSection = true;
            break;
        case '[':{//this will move us into a subsystem that works out memory address tokens
            std::string memoryAddressToken = memoryAddressTokenStart(assemblyFile, tokenFile);
if(memoryAddressToken.find("imm16") != std::string::npos){
                memoryAddressToken = "mem16:" + memoryAddressToken;//if this we know it is not a teeny instruction for a lot of memory instructions
            } else if(memoryAddressToken.find("imm16") != std::string::npos){
                memoryAddressToken = "mem4:" + memoryAddressToken;
            } else {
                memoryAddressToken = "mem:" + memoryAddressToken;
            }
            tokenString = memoryAddressToken;
            tokenFile << memoryAddressToken << std::endl;
            break;
        }
        case '$':{//this is the start for a register token
            tokenString = "reg:" + toHexString(registerStart(assemblyFile, tokenFile));
            tokenFile << tokenString << std::endl;
            break;
        }
        case '#':{//this is the start of a hex literal token
            hexLiteralStart(assemblyFile, tokenFile);
            break;
        }
        default:{
        if(character >= 'A' && character <= 'Z'){
            //this is the start of either a literal or a label token need a buffer for this one
            letterTokenStart(assemblyFile, tokenFile);
        } else if(character >= '0' && character <='9'){
            //this is the start of a decimal literal
            decimalLiteralTokenStart(character, assemblyFile, tokenFile);
        } else if(whiteSpace.find(character) != std::string::npos){
                //whitespace ignore it here
        }
        break;
    }
}
return tokenString;
}

int main(int argc, char* argv[]){

    uint16_t currentAddress = 0;
    std::ifstream assemblyFile(argv[1], std::ios::in);
    std::ofstream tokenFile(argv[2], std::ios::out);

    //first pass through the file creating a tokenized version of the file
while(assemblyFile.peek() != EOF){
        std::cout << newToken(assemblyFile, tokenFile) << std::endl;
    }

assemblyFile.close();
    tokenFile.close();


    return EXIT_SUCCESS;
}