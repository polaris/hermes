#ifndef _Window_H
#define _Window_H

#include <SDL2/SDL.h>

class Window {
public:
    Window(const char *title, unsigned int xpos, unsigned int ypos, unsigned int width, unsigned int height);

    Window(const char *title, unsigned int width, unsigned int height);

    ~Window();

    Window(const Window&) = delete;

    Window& operator =(const Window&) = delete;

    unsigned int getWidth() const;

    unsigned int getHeight() const;

    SDL_Window* getSDLWindow() const {
        return window_;
    }

private:
    SDL_Window* window_;
};

#endif  // _Window_H
