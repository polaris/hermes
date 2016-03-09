#ifndef _Game_H
#define _Game_H

#include <SDL2/SDL.h>

/** Represents a game.
 */
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
     *
     *  \param title the title of the window.
     *  \param width the width of the window.
     *  \param height the height of the window.
     */
    void setUp(const char *title, unsigned int width, unsigned int height);

    /** Tear down SDL.
     */
    void tearDown();

    /** Pointer to the SDL window.
     */
    SDL_Window* window_;

    /** Pointer to the SDL renderer.
     */
    SDL_Renderer* renderer_;

    /** Flag is true while the game is running.
     */
    bool running_;
};

#endif  // _Game_H
