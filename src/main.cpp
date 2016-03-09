#include <SDL2/SDL.h>

int main() {
    SDL_Window* window = 0;
    SDL_Renderer* renderer = 0;
   
    if (SDL_Init(SDL_INIT_EVERYTHING) >= 0) {
        window = SDL_CreateWindow("",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
            SDL_WINDOW_SHOWN);
        if (window != 0) {
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        }
    } else {
        return 1;
    }

    SDL_SetRenderDrawColor(renderer, 128, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(5000);
    SDL_Quit();
    
    return 0;
}
