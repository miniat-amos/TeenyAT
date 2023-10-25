#ifndef GRAMMARFUNCTIONS__H
#define GRAMMARFUNCTIONS__H

#include <vector>
#include <sstream>
#include <map>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <bitset>
#include <algorithm>

#include "teenyat.h"

extern void resetWordCnt();

void append32ToFile(const std::string& filename, uint32_t value);

void append16ToFile(const std::string& filename, uint16_t value);

uint16_t read16FromInstruction(std::string immField);

extern void grammar0(const std::vector<std::string>& line, const std::string& filename);
extern void grammar1(const std::vector<std::string>& line, const std::string& filename);
extern void grammar2(const std::vector<std::string>& line, const std::string& filename);
extern void grammar3(const std::vector<std::string>& line, const std::string& filename);
extern void grammar4(const std::vector<std::string>& line, const std::string& filename);
extern void grammar5(const std::vector<std::string>& line, const std::string& filename);
extern void grammar6(const std::vector<std::string>& line, const std::string& filename);
extern void grammar7(const std::vector<std::string>& line, const std::string& filename);
extern void grammar8(const std::vector<std::string>& line, const std::string& filename);
extern void grammar9(const std::vector<std::string>& line, const std::string& filename);
extern void grammar10(const std::vector<std::string>& line, const std::string& filename);
extern void grammar11(const std::vector<std::string>& line, const std::string& filename);
extern void grammar12(const std::vector<std::string>& line, const std::string& filename);
extern void grammar13(const std::vector<std::string>& line, const std::string& filename);
extern void grammar14(const std::vector<std::string>& line, const std::string& filename);
extern void grammar15(const std::vector<std::string>& line, const std::string& filename);

extern void grammar32(const std::vector<std::string>& line, const std::string& filename);
extern void grammar33(const std::vector<std::string>& line, const std::string& filename);
extern void grammar34(const std::vector<std::string>& line, const std::string& filename);

#endif//GRAMMARFUNCTIONS__H