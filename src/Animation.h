#ifndef _Animation_H
#define _Animation_H

#include <SDL2/SDL.h>

class Renderer;

class Animation {
public:
    Animation(const char* filename, unsigned int frameWidth, unsigned int frameHeight, const Renderer& renderer);

    ~Animation();

    Animation(const Animation&) = delete;

    Animation& operator =(const Animation&) = delete;

    void draw(int x, int y, const Renderer& renderer) const;

    bool update(float elapsed);

    unsigned int getWidth() const;

    unsigned int getHeight() const;

private:
    void tidyup();

    bool step();

    const unsigned int frameWidth_;

    const unsigned int frameHeight_;

    unsigned int frameCount_;

    unsigned int currentFrame_;

    float currentTime_;

    float lastTime_;

    SDL_Texture* texture_;
};

#endif  // _Animation_H
