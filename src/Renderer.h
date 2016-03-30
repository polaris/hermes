#ifndef _Renderer_H
#define _Renderer_H

#include <SDL2/SDL.h>

class Window;

class Renderer {
public:
    explicit Renderer(const Window &window);

    ~Renderer();

    void clear();

    void setDrawColor();

    void present();

    SDL_Renderer* getSDLRenderer() const {
        return renderer_;
    }

private:
    SDL_Renderer* renderer_;
};

#endif  // _Renderer_H
