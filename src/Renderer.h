#ifndef _Renderer_H
#define _Renderer_H

#include <SDL2/SDL.h>

class Window;
class Vector2d;

class Renderer {
public:
    explicit Renderer(const Window &window);

    ~Renderer();

    Renderer(const Renderer&) = delete;

    Renderer& operator =(const Renderer&) = delete;

    void clear(float r, float g, float b);

    void setDrawColor(float r, float g, float b, float a);

    void drawLine(const Vector2d& a, const Vector2d& b);

    void present();

    SDL_Renderer* getSDLRenderer() const {
        return renderer_;
    }

private:
    SDL_Renderer* renderer_;
};

#endif  // _Renderer_H
