#ifndef _Window_H
#define _Window_H

#include <SDL2/SDL.h>

class Window {
public:
    Window(const char *title, unsigned int width, unsigned int height);

    ~Window();

    SDL_Window* getSDLWindow() const {
        return window_;
    }

private:
    SDL_Window* window_;
};

#endif  // _Window_H
