#include "chip8.hpp"

#define MEM_SIZE 4096
#define START_ADDRESS 0x200

CHIP8::CHIP8() 
    : program_counter(START_ADDRESS),
    rand_gen(std::random_device{}()),
    rand_byte(0,255U)
{
    LoadFontSet();
}

void CHIP8::LoadRom(std::string filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if(file.is_open())
    {
        std::streampos size = file.tellg();
        const std::streampos max_size = MEM_SIZE - START_ADDRESS;
        if(size > max_size)
        {
            std::cerr << "ROM too large, try one of less than size: " << max_size << " bytes";
            return;
        }

        char* buffer = new char[size];

        file.seekg(0, std::ios::beg);
        file.read(buffer, size);
        file.close();

        for(unsigned i = 0; i < size; i++)
        {
            memory[START_ADDRESS + i] = buffer[i];
        }

        delete[] buffer;
    }
}

void CHIP8::LoadFontSet()
{
    const unsigned int fontset_start_address = 0x50;
    const unsigned int fontset_size = 80;

    std::uint8_t fontset[fontset_size] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	    0x20, 0x60, 0x20, 0x20, 0x70, // 1
	    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for(unsigned int i = 0; i < fontset_size; i++)
    {
        memory[fontset_start_address + i] = fontset[i];
    }
}

void CHIP8::OP_00E0()
{
    std::fill_n(pixels, sizeof(pixels), 0);
}

void CHIP8::OP_00EE()
{
    --stack_pointer;
    program_counter = stack[stack_pointer];
}

void CHIP8::OP_1nnn()
{
    program_counter = opcode & 0x0FFFU;
}

void CHIP8::OP_2nnn()
{
    stack[stack_pointer] = program_counter;
    ++stack_pointer;
    program_counter = opcode & 0x0FFF;
}

void CHIP8::OP_3xkk()
{
    if(registers[(opcode & 0x0F00U) >> 8U] == (opcode & 0x00FFU))
    {
        program_counter+=2;
    }
}

void CHIP8::OP_4xkk()
{
    if(registers[(opcode & 0x0F00U) >> 8U] != (opcode & 0x00FFU))
    {
        program_counter+=2;
    }
}

void CHIP8::OP_5xy0()
{
    if(registers[(opcode & 0x0F00U) >> 8U] == registers[(opcode & 0x00F0U) >> 4U])
    {
        program_counter += 2;
    }
}

void CHIP8::OP_6xkk()
{
    registers[(opcode & 0x0F00U) >> 8U] = opcode & 0x00FFU;
}

void CHIP8::OP_7xkk()
{
    registers[(opcode & 0x0F00U) >> 8U] += opcode & 0x00FFU;
}

void CHIP8::OP_8xy0()
{
    registers[(opcode & 0x0F00U) >> 8U] = registers[(opcode & 0x00F0U) >> 4U];
}

void CHIP8::OP_8xy1()
{
    registers[(opcode & 0x0F00U) >> 8U] = registers[(opcode & 0x0F00U) >> 8U] | registers[(opcode & 0x00F0U) >> 4U];
}

void CHIP8::OP_8xy2()
{
    registers[(opcode & 0x0F00U) >> 8U] = registers[(opcode & 0x0F00U) >> 8U] & registers[(opcode & 0x00F0U) >> 4U];
}

void CHIP8::OP_8xy3()
{
    registers[(opcode & 0x0F00U) >> 8U] = registers[(opcode & 0x0F00U) >> 8U] ^ registers[(opcode & 0x00F0U) >> 4U];
}

void CHIP8::OP_8xy4()
{
    std::uint8_t Vx = registers[(opcode & 0x0F00U) >> 8U];
    std::uint8_t Vy = registers[(opcode & 0x00F0U) >> 4U];
    
    std::uint16_t sum = Vx + Vy;

    if(sum > 255U)
    {
        registers[0xFU] = 1U;
    }
    else
    {
        registers[0xFU] = 0;
    }

    registers[Vx] = sum & 0xFFU;
}