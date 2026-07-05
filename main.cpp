#include "chip8.hpp"
#include "platform.hpp"
#include <chrono>
#include "SDL3/SDL_main.h"

int main(int argc, char* args[])
{
    if(argc != 4)
    {
        std::cerr << "Usage: " << args[0] << " <Scale> <Delay> <ROM>\n";
        std::exit(EXIT_FAILURE);
    }

    int video_scale = std::stoi(args[1]);
    int cycle_delay = std::stoi(args[2]);
    char const *rom_filename = args[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * video_scale, VIDEO_HEIGHT * video_scale, VIDEO_WIDTH, VIDEO_HEIGHT);

    CHIP8 chip8;
    chip8.LoadRom(rom_filename);

    int video_pitch = sizeof(chip8.pixels[0]) * VIDEO_WIDTH;

    auto last_cycle_time = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while(!quit)
    {
        quit = platform.ProcessInput(chip8.keys);

        auto current_time = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(current_time - last_cycle_time).count();

        if(dt > cycle_delay)
        {
            last_cycle_time = current_time;

            chip8.Cycle();
            
            platform.Update(chip8.pixels, video_pitch);
        }
    }

    return 0;
}