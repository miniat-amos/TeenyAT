#include <grammarFunctions.hpp>

std::map<std::string,uint16_t> labels;

int wordCnt = 0;

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

std::map<std::string,uint8_t>registers = {
    {"pc", 0},
    {"sp", 1},
    {"z", 2},
    {"a", 3},
    {"b", 4},
    {"c", 5},
    {"d", 6},
    {"e", 7},
    {"0", 0},
    {"1", 1},
    {"2", 2},
    {"3", 3},
    {"4", 4},
    {"5", 5},
    {"6", 6},
    {"7", 7}
};

std::map<std::string,uint16_t> instructionToFlag = {
    {"je", 4},
    {"jmp", 0},
    {"jne", 3},
    {"jge", 5},
    {"jg", 1},
    {"jle", 6},
    {"jl", 2}
};

uint16_t read16FromInstruction(std::string immField){
    uint16_t imm16;
    std::string imm = immField.substr(1);
    try
    {
        imm16 = std::stoi(immField);
    }
    catch(const std::invalid_argument& e)
    {
        if(immField[0] != '#'){
            imm16 = labels[immField];
        } else{
            imm16 = std::stoi(imm, 0, 16);
        }
    }

    return imm16;
}

std::string toLowerCase(const std::string& str)
{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c){return std::tolower(c);});
    return result;
}

extern void resetWordCnt(){
    wordCnt = 0;
}

extern void grammar0(const std::vector<std::string>& line, const std::string& filename)
{
    std::cout << "empty grammar 0" << std::endl;
}

extern void grammar1(const std::vector<std::string>& line, const std::string& filename)
{
    uint16_t encoding;
    uint8_t dreg;
    uint8_t sreg;
    dreg = registers[line[2]];
    sreg = registers[line[3]];
    
    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding |= 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    append16ToFile(filename,encoding);
}

extern void grammar2(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers[line[3]];
    uint16_t imm16 = read16FromInstruction(line[4]);

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);

}

extern void grammar3(const std::vector<std::string>& line, const std::string& filename)
{
    uint32_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers["z"];
    uint16_t imm16 = read16FromInstruction(line[3]);

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar4(const std::vector<std::string>& line, const std::string& filename){
    uint16_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers["z"];
    
    encoding = instructions[toLowerCase(line[1])];
    encoding = (encoding << 1) | 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4);

    append16ToFile(filename,encoding);
}

extern void grammar5(const std::vector<std::string>& line, const std::string& filename){
    uint16_t encoding;
    uint8_t dreg;
    uint8_t sreg;
    dreg = registers[line[2]];
    sreg = registers["z"];
    
    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding |= 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4) | 1;

    append16ToFile(filename,encoding);
}

extern void grammar6(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers[line[4]];
    uint16_t imm16 = read16FromInstruction(line[3]);
    
    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar7(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers["z"];
    uint8_t sreg = registers[line[3]];
    uint16_t imm16 = read16FromInstruction(line[2]);

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar8(const std::vector<std::string>& line, const std::string& filename){
    uint16_t encoding;
    uint8_t dreg = registers["z"];
    uint8_t sreg = registers[line[2]];
    
    encoding = instructions[toLowerCase(line[1])];
    encoding = (encoding << 1) | 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4);

    append16ToFile(filename,encoding);
}

extern void grammar9(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers["z"];
    uint8_t sreg = registers[line[2]];
    uint16_t imm16 = read16FromInstruction(line[3]);

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar10(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers["z"];
    uint8_t sreg = registers["z"];
    uint16_t imm16 = read16FromInstruction(line[2]);

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = encoding << 4;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar11(const std::vector<std::string>& line, const std::string& filename){
    uint16_t encoding;
    uint8_t dreg = registers["pc"];
    uint8_t sreg = registers["z"];
    
    encoding = instructions[toLowerCase(line[1])];
    encoding = (encoding << 1) | 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4);

    append16ToFile(filename,encoding);
}

extern void grammar12(const std::vector<std::string>& line, const std::string& filename){
    uint16_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers["z"];
    uint8_t flags = instructionToFlag[toLowerCase(line[1])];
    
    encoding = instructions[toLowerCase(line[1])];
    encoding = (encoding << 1) | 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4) | flags;

    append16ToFile(filename,encoding);
}

extern void grammar13(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers[line[2]];
    uint8_t sreg = registers["z"];
    uint16_t imm16 = read16FromInstruction(line[3]);
    uint8_t flags = instructionToFlag[toLowerCase(line[1])];

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4) | flags;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar14(const std::vector<std::string>& line, const std::string& filename){
    uint32_t encoding;
    uint8_t dreg = registers["z"];
    uint8_t sreg = registers["z"];
    uint16_t imm16;
    uint8_t flags = instructionToFlag[toLowerCase(line[1])];
    try
    {
        imm16 = std::stoi(line[2]);
    }
    catch(const std::invalid_argument& e)
    {
        imm16 = labels[line[2]];
    }

    encoding = instructions[toLowerCase(line[1])] << 1;
    encoding = (encoding << 3) | dreg;
    encoding = (encoding << 3) | sreg;
    encoding = (encoding << 4) | flags;
    encoding = (encoding << 16) | imm16;

    append32ToFile(filename, encoding);
}

extern void grammar15(const std::vector<std::string>& line, const std::string& filename){
    std::cout << "empty grammer #15" << std::endl;
}

extern void grammar32(const std::vector<std::string>& line, const std::string& filename){
    labels[line[1]] = wordCnt;
}

extern void grammar33(const std::vector<std::string>& line, const std::string& filename){
    for(size_t i = 1; i < line.size(); i++){
        uint16_t value;
        value = read16FromInstruction(line[i]);
        append16ToFile(filename, value);
    }

    return;
}

extern void grammar34(const std::vector<std::string>& line, const std::string& filename){
    labels[line[1]] = read16FromInstruction(line[2]);
}

void append32ToFile(const std::string& filename, uint32_t value){
    std::ofstream outfile(filename, std::ios::app | std::ios::binary);
    if(!outfile){
        std::cerr << "Error opening the file " << filename << std::endl;
        return;
    }

    uint16_t upper = (value >> 16);
    append16ToFile(filename, upper);
    append16ToFile(filename, value);
    outfile.close();
}

void append16ToFile(const std::string& filename, uint16_t value){
    std::ofstream outfile(filename, std::ios::app | std::ios::binary);
    if(!outfile){
        std::cerr << "Error opening the file " << filename << std::endl;
        return;
    }

    outfile.write(reinterpret_cast<char*>(&value), sizeof(value));
    outfile.close();
    wordCnt++;
}