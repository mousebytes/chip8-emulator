#ifndef PLATFORM_H
#define PLATFORM_H
#include "SDL3/SDL.h"
#include<cstdint>

class Platform
{
    private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};

    public:

    Platform(char const* title, int window_width, int window_height, int texture_width, int texture_height);
    ~Platform();

    void Update(void const* buffer, int pitch);
    bool ProcessInput(std::uint8_t* keys);
};

#endif