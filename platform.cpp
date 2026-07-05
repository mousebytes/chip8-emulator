#include "platform.hpp"
#include <iostream>


Platform::Platform(char const* title, int window_width, int window_height, int texture_width, int texture_height)
{
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "SDL INIT FAILED IN PLATFORM CONSTRUCTOR";
        return;
    }

    window = SDL_CreateWindow(title, window_width, window_height, 0);
    if (!window)
    {
        std::cerr << "Window creation failed: " << SDL_GetError() << '\n';
        return;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer)
    {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << '\n';
        return;
    }
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);
    if (!texture)
    {
        std::cerr << "Texture creation failed: " << SDL_GetError() << '\n';
        return;
    }

    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

Platform::~Platform()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_Quit();
}

void Platform::Update(void const* buffer, int pitch)
{
    SDL_UpdateTexture(texture, nullptr, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

bool Platform::ProcessInput(std::uint8_t* keys)
{
    bool quit = false;

    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_EVENT_QUIT:
            {
                quit = true;
            }
            break;
            case SDL_EVENT_KEY_DOWN:
            {
                switch(event.key.key)
                {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_1: keys[0x1] = 1; break;
                    case SDLK_2: keys[0x2] = 1; break;
                    case SDLK_3: keys[0x3] = 1; break;
                    case SDLK_4: keys[0xC] = 1; break;
                    case SDLK_Q: keys[0x4] = 1; break;
                    case SDLK_W: keys[0x5] = 1; break;
                    case SDLK_E: keys[0x6] = 1; break;
                    case SDLK_R: keys[0xD] = 1; break;
                    case SDLK_A: keys[0x7] = 1; break;
                    case SDLK_S: keys[0x8] = 1; break;
                    case SDLK_D: keys[0x9] = 1; break;
                    case SDLK_F: keys[0xE] = 1; break;
                    case SDLK_Z: keys[0xA] = 1; break;
                    case SDLK_X: keys[0x0] = 1; break;
                    case SDLK_C: keys[0xB] = 1; break;
                    case SDLK_V: keys[0xF] = 1; break;
                }
                break;
            }
            case SDL_EVENT_KEY_UP:
            {
                switch(event.key.key)
                {
                    case SDLK_ESCAPE: quit = true; break;
                    case SDLK_1: keys[0x1] = 0; break;
                    case SDLK_2: keys[0x2] = 0; break;
                    case SDLK_3: keys[0x3] = 0; break;
                    case SDLK_4: keys[0xC] = 0; break;
                    case SDLK_Q: keys[0x4] = 0; break;
                    case SDLK_W: keys[0x5] = 0; break;
                    case SDLK_E: keys[0x6] = 0; break;
                    case SDLK_R: keys[0xD] = 0; break;
                    case SDLK_A: keys[0x7] = 0; break;
                    case SDLK_S: keys[0x8] = 0; break;
                    case SDLK_D: keys[0x9] = 0; break;
                    case SDLK_F: keys[0xE] = 0; break;
                    case SDLK_Z: keys[0xA] = 0; break;
                    case SDLK_X: keys[0x0] = 0; break;
                    case SDLK_C: keys[0xB] = 0; break;
                    case SDLK_V: keys[0xF] = 0; break;
                }
                break;
            }
        }
    }
    
    return quit;
}