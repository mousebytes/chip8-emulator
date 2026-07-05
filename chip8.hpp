#ifndef CHIP8_H
#define CHIP8_H

#include<cstdint>
#include <random>
#include<string>
#include<fstream>
#include<exception>
#include <iostream>
#include <stdexcept>
#include <algorithm>
#include <random>

class CHIP8
{
    private:
    // 16 8 bit regs
    std::uint8_t registers[16];
    // 4kb mem
    std::uint8_t memory[4096];
    // holds mem address for use in operations
    std::uint16_t index_register;

    std::uint16_t program_counter;

    std::uint16_t stack[16];
    std::uint8_t stack_pointer;
    
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

    std::uint8_t opcode;

    std::mt19937 rand_gen;
    std::uniform_int_distribution<std::uint8_t> rand_byte;

    void LoadFontSet();
    void MapOPs();

    // opcodes
    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    
    typedef void (CHIP8::*CHIP8Func)();
    void OP_NULL() {}

    CHIP8Func table[0xF + 1];
    CHIP8Func table0[0xE + 1];
    CHIP8Func table8[0xE + 1];
    CHIP8Func tableE[0xE + 1];
    CHIP8Func tableF[0x65 + 1];

    void Table0();
    void Table8();
    void TableE();
    void TableF();

    

    public:

    // bitmask to ref each key in order:
    // 1 2 3 4
    // Q W E R
    // A S D F
    // Z X C V
    std::uint16_t keys[16];
    std::uint32_t pixels[64 * 32];

    CHIP8();
    void LoadRom(std::string filename);

    void Cycle();
};

#endif