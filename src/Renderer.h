#ifndef _Renderer_H
#define _Renderer_H

#include <SDL2/SDL.h>

class Window;

class Renderer {
public:
    explicit Renderer(const Window &window);

    ~Renderer();

    void clear(float r, float g, float b);

    void setDrawColor(float r, float g, float b, float a);

    void present();

    SDL_Renderer* getSDLRenderer() const {
        return renderer_;
    }

private:
    SDL_Renderer* renderer_;
};

#endif  // _Renderer_H
