#include "chip8.hpp"

#define MEM_SIZE 4096
#define START_ADDRESS 0x200
#define VIDEO_WIDTH 64
#define VIDEO_HEIGHT 32
#define FONT_START 0x50U

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
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t Vy = (opcode & 0x00F0U) >> 4U;
    
    std::uint16_t sum = registers[Vx] + registers[Vy];

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

void CHIP8::OP_8xy5()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t Vy = (opcode & 0x00F0U) >> 4U;
    
    std::uint16_t difference = registers[Vx] - registers[Vy];

    if(registers[Vx] >= registers[Vy])
    {
        registers[0xFU] = 1U;
    }
    else
    {
        registers[0xFU] = 0;
    }

    registers[Vx] = difference & 0xFFU;
}

void CHIP8::OP_8xy6()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t LSB = registers[Vx] & 1;

    registers[0xFU] = LSB;

    registers[Vx] >>= 1;
}

void CHIP8::OP_8xy7()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t Vy = (opcode & 0x00F0U) >> 4U;
    std::uint16_t difference = registers[Vy] - registers[Vx];

    if(registers[Vy] >= registers[Vx])
    {
        registers[0xFU] = 1;
    }
    else
    {
        registers[0xFU] = 0;
    }

    registers[Vx] = difference & 0xFFU;
}

void CHIP8::OP_8xyE()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t msb = registers[Vx] & 0x80U;
    msb >>= 7;

    registers[0xFU] = msb;

    Vx <<= 1;
}

void CHIP8::OP_9xy0()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t Vy = (opcode & 0x00F0U) >> 4U;

    if(registers[Vx] != registers[Vy])
    {
        program_counter += 2;
    }
}

void CHIP8::OP_Annn()
{
    std::uint16_t nnn = (opcode & 0x0FFFU);
    index_register = nnn;
}

void CHIP8::OP_Bnnn()
{
    std::uint16_t nnn = (opcode & 0x0FFFU);
    program_counter = registers[0] + nnn;
}

void CHIP8::OP_Cxkk()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t random_byte = rand_byte(rand_gen);
    std::uint8_t kk = (opcode & 0x00FF);

    registers[Vx] = random_byte & kk;
}

void CHIP8::OP_Dxyn()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t Vy = (opcode & 0x00F0U) >> 4U;
    std::uint8_t height = (opcode & 0x000FU);

    std::uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    std::uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xFU] = 0;

    for(int x = 0; x < 8; x++)
    {
        std::uint8_t sprite_byte = registers[index_register + x];
        for(int y = 0; y < height; y++)
        {
            std::uint8_t sprite_pixel = sprite_byte & (0x80U >> y);
            uint32_t* screen_pixel = &pixels[(yPos+x) * VIDEO_WIDTH + (xPos + y)]; 

            // sprite pixel is on
            if(screen_pixel)
            {
                // screen pixel is also on - collision
                if(*screen_pixel == 0xFFFFFFFF)
                {
                    registers[0xFU] = 1;
                }

                *screen_pixel ^= 0xFFFFFFFF;
            }
        }
    }
}

void CHIP8::OP_Ex9E()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t key = registers[Vx];
    if(keys[key])
    {
        program_counter += 2;
    }
}

void CHIP8::OP_ExA1()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t key = registers[Vx];

    if(!keys[key])
    {
        program_counter += 2;
    }
}

void CHIP8::OP_Fx07()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    registers[Vx] = delay_timer;
}

void CHIP8::OP_Fx0A()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t key_found = 0;

    for(std::uint8_t i = 0; i < 16; i++)
    {
        if(keys[i])
        {
            key_found = 1;
            registers[Vx] = i;
        }
    }

    if(!key_found)
    {
        program_counter -= 2;
    }
}

void CHIP8::OP_Fx15()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;

    delay_timer = registers[Vx];
}

void CHIP8::OP_Fx18()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;

    sound_timer = registers[Vx];
}

void CHIP8::OP_Fx1E()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    index_register += registers[Vx];
}

void CHIP8::OP_Fx29()
{
    /*
        bytes of font
        start at FONT_START
        
        grab first byte
        this first one would be
        FONT_START, next would be FONT_START + 5
        
        [1] 2 3 4 5
        1   2 3 4 5
        1   2 3 4 5
    */
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t digit = registers[Vx];

    // get the first byte of any char requested
    index_register = FONT_START + (digit*5);
}

void CHIP8::OP_Fx33()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;
    std::uint8_t value = registers[Vx];

    // ones
    memory[index_register + 2] = value % 10;
    value /= 10;

    // tens
    memory[index_register + 1] = value % 10;
    value /= 10;

    // hundreds
    memory[index_register] = value % 10;
}

void CHIP8::OP_Fx55()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;

    for(std::uint8_t i = 0; i <= Vx; i++)
    {
        memory[index_register + i] = registers[i];
    }
}

void CHIP8::OP_Fx65()
{
    std::uint8_t Vx = (opcode & 0x0F00U) >> 8U;

    for(std::uint8_t i = 0; i <= Vx; i++)
    {
        registers[i] = memory[index_register + i];
    }
}