#include "chip8.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *texture;

void platform_init(const char *title, int window_width, int window_height, int texture_width, int texture_height)
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(title, 0, 0, window_width, window_height, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);
}

void platform_update(void *buffer, int pitch)
{
    SDL_UpdateTexture(texture, NULL, buffer, pitch);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int process_input(uint8_t *keys)
{
    int quit = 0;

    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
        {
            quit = 1;
        }
        break;

        case SDL_KEYDOWN:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
            {
                quit = 1;
            }
            break;

            case SDLK_x:
            {
                keys[0] = 1;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 1;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 1;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 1;
            }
            break;

            case SDLK_q:
            {
                keys[4] = 1;
            }
            break;

            case SDLK_w:
            {
                keys[5] = 1;
            }
            break;

            case SDLK_e:
            {
                keys[6] = 1;
            }
            break;

            case SDLK_a:
            {
                keys[7] = 1;
            }
            break;

            case SDLK_s:
            {
                keys[8] = 1;
            }
            break;

            case SDLK_d:
            {
                keys[9] = 1;
            }
            break;

            case SDLK_z:
            {
                keys[0xA] = 1;
            }
            break;

            case SDLK_c:
            {
                keys[0xB] = 1;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 1;
            }
            break;

            case SDLK_r:
            {
                keys[0xD] = 1;
            }
            break;

            case SDLK_f:
            {
                keys[0xE] = 1;
            }
            break;

            case SDLK_v:
            {
                keys[0xF] = 1;
            }
            break;
            }
        }
        break;

        case SDL_KEYUP:
        {
            switch (event.key.keysym.sym)
            {
            case SDLK_x:
            {
                keys[0] = 0;
            }
            break;

            case SDLK_1:
            {
                keys[1] = 0;
            }
            break;

            case SDLK_2:
            {
                keys[2] = 0;
            }
            break;

            case SDLK_3:
            {
                keys[3] = 0;
            }
            break;

            case SDLK_q:
            {
                keys[4] = 0;
            }
            break;

            case SDLK_w:
            {
                keys[5] = 0;
            }
            break;

            case SDLK_e:
            {
                keys[6] = 0;
            }
            break;

            case SDLK_a:
            {
                keys[7] = 0;
            }
            break;

            case SDLK_s:
            {
                keys[8] = 0;
            }
            break;

            case SDLK_d:
            {
                keys[9] = 0;
            }
            break;

            case SDLK_z:
            {
                keys[0xA] = 0;
            }
            break;

            case SDLK_c:
            {
                keys[0xB] = 0;
            }
            break;

            case SDLK_4:
            {
                keys[0xC] = 0;
            }
            break;

            case SDLK_r:
            {
                keys[0xD] = 0;
            }
            break;

            case SDLK_f:
            {
                keys[0xE] = 0;
            }
            break;

            case SDLK_v:
            {
                keys[0xF] = 0;
            }
            break;
            }
        }
        break;
        }
    }

    return quit;
}
