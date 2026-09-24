#pragma once
#include "isa.h"
#include <iostream>
#include <vector>
#include <array>
#include <string>
const int no_of_registers = 16;
const int cap_of_inst_mem = 1024;
const int cap_of_data_mem = 4096;
const int sp_reg = 14;
const int ra_reg = 15;

class CPU
{
private:
    uint32_t pc;
    std::array<int32_t, no_of_registers> regs;
    bool flag_E;
    bool flag_GT;

    std::vector<uint32_t> instMem;
    std::vector<uint8_t> dataMem;

    uint32_t instruction_limit;

    uint32_t fetchInst();

public:
    CPU();
    void reset();
    void loadHex(const std::vector<std::string> &hexLines);

    void run();
    void step();
    void execute(Instruction inst);

    void dumpRegisters() const;
};