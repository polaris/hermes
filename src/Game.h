#ifndef _Game_H
#define _Game_H

#include <SDL2/SDL.h>

class Renderer;
class Clock;

class Game {
public:
    Game(unsigned int frameRate, Renderer& renderer);

    virtual ~Game() = default;

    void run();

protected:    
    virtual void update(const Clock& clock) = 0;

    virtual void handleEvent(SDL_Event &event, bool& running) = 0;

    Renderer& renderer_;

protected:
    const float frameDuration_;

private:
    void processEvent(SDL_Event &event, bool& running);
};

#endif  // _Game_H
