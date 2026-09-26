#include "cpu.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <string>
using std::cout;
using std::vector;
using std::string;

CPU::CPU(){
    instMem.resize(cap_of_inst_mem,0);
    dataMem.resize(cap_of_data_mem,0);
    reset();
}

void CPU::reset(){
    pc = 0;
    regs.fill(0);
    flag_E = false;
    flag_GT = false;
    instruction_limit = 0;
}

void CPU::loadHex(const vector<string> &hexLines){
    reset();
    size_t i = 0;
    for (const auto& line: hexLines){
        if (i>=cap_of_inst_mem) throw std::out_of_range("Instruction memory limit exceeded.");
        instMem[i] = std::stoul(line,nullptr,16);
        i++;
    }
    instruction_limit = i*4;
}

uint32_t CPU::fetchInst(){
    if(pc >= instruction_limit) throw std::out_of_range("PC out of bounds");
    return instMem[pc/4];
}

void CPU::run(){
    while (pc < instruction_limit) step();
    cout << "Execution stopped.\n";
}

void CPU::step(){
    if (pc >= instruction_limit) return;
    uint32_t word = fetchInst();
    Instruction inst = decode(word);
    execute(inst);
}

void CPU::execute (Instruction inst){
    int32_t A = regs[inst.rs1];
    int32_t B = inst.isImm ? ImmValue(inst.mod, inst.imm) : regs[inst.rs2];
    uint32_t current_pc = pc;
    pc += 4;

    switch(inst.op){
        // Arithmetic instructions
        case Opcode::add: regs[inst.rd] = A + B; break; // temporary placeholders until ALU finishes. Also will help to check if CPU data path is correct.
        case Opcode::sub: regs[inst.rd] = A - B; break;
        case Opcode::mul: regs[inst.rd] = A * B; break;
        case Opcode::div: 
        if (B == 0) throw std::runtime_error("DivideByZero");
        regs[inst.rd] = A / B; 
        break;
        case Opcode::mod: 
        if (B == 0) throw std::runtime_error("DivideByZero");
        regs[inst.rd] = A % B; 
        break;

        // Compare instruction
        case Opcode::cmp:
        flag_E = (A==B);
        flag_GT = (A>B);
        break;

        // Logical instructions
        case Opcode::and_op: regs[inst.rd] = A & B; break;
        case Opcode::or_op: regs[inst.rd] = A | B; break;
        case Opcode::not_op: regs[inst.rd] = ~A ; break;

        // Move instruction
        case Opcode::mov: regs[inst.rd] = B ; break;

        // Shift instructions
        case Opcode::lsl: regs[inst.rd] = A << (B & 0x1F) ; break;
        case Opcode::lsr: regs[inst.rd] = static_cast<uint32_t>(A) >> (B & 0x1F) ; break;
        case Opcode::asr: regs[inst.rd] = A >> (B & 0x1F) ; break;

        // Nop instruction
        case Opcode::nop: break;

        // Load and store instructions. Assuming Little endian
        case Opcode::ld:{
            uint32_t addr = A + B;
            if (addr + 3 >= cap_of_data_mem) throw std::out_of_range("Data loading failed. Out of bounds");
            regs[inst.rd] = (dataMem[addr])|(dataMem[addr+1]<<8)|(dataMem[addr+2]<<16)|(dataMem[addr+3]<<24);
            break;
        }
        case Opcode::st:{
            uint32_t addr = A + B;
            if (addr + 3 >= cap_of_data_mem) throw std::out_of_range("Data storing failed. Out of bounds");
            dataMem[addr] = regs[inst.rd] & 0xFF;
            dataMem[addr+1] = (regs[inst.rd]>>8) & 0xFF;
            dataMem[addr+2] = (regs[inst.rd]>>16) & 0xFF;
            dataMem[addr+3] = (regs[inst.rd]>>24) & 0xFF;
            break;
        }

        // Branch instructions
        case Opcode::b:
            pc = current_pc + (inst.imm * 4);
            break;
        case Opcode::beq:
            if(flag_E) pc = current_pc + (inst.imm * 4);
            break;
        case Opcode::bgt:
            if(flag_GT) pc = current_pc + (inst.imm * 4);
            break;
        
        case Opcode::call:
            regs[15] = pc;
            pc = current_pc + (inst.imm * 4);
            break;
        case Opcode::ret:
            pc = regs[15];
            break;
        default:
            throw std::runtime_error("Not a valid instruction."); // to handle opcodes from 21 to 31
    }
}

void CPU::dumpRegisters() const{
    cout << "--- CPU STATE ---\n";

    cout << "PC : 0x" << std::setfill('0') << std::setw(8) << std::hex << pc << "\n";
    cout << "CMP : E=" << flag_E << " GT=" << flag_GT << "\n\n";

    for(int i=0; i<16; ++i){
        cout << "r" << std::dec << std::setw(2) << std::setfill(' ') << i << ": 0x" << std::setfill('0') << std::hex << std::setw(8) << regs[i] << "\t";
        if ((i+1)%4==0){
            cout << "\n";
        }
    }
    cout << std::dec << "-----------------\n";
}