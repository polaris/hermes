#ifndef _Game_H
#define _Game_H

#include <SDL2/SDL.h>

class Game {
public:
    /** Constructor
     *
     *  \param title the title of the window.
     *  \param width the width of the window.
     *  \param height the height of the window.
     */
    Game(const char *title, unsigned int width, unsigned int height);

    /** Destructor
     */
    virtual ~Game();

    /** Runs the game.
     */
    void run();

private:
    /** Setup SDL.
     */
    void setUp(const char *title, unsigned int width, unsigned int height);

    /** Tear down SDL.
     */
    void tearDown();

    SDL_Window* window_;
    SDL_Renderer* renderer_;
    bool running_;
};

#endif  // _Game_H
