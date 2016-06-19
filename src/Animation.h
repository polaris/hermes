#ifndef _Animation_H
#define _Animation_H

#include <SDL2/SDL.h>

class Renderer;

/** Manages an animated sprite.
 */
class Animation {
public:
    /** Constructor

        \param filename the name of the image that contains the frames of the animation.
        \param frameWidth the width of a single frame.
        \param frameHeight the height of a single frame.
        \param renderer a reference to the renderer.
     */
    Animation(const char* filename, unsigned int frameWidth, unsigned int frameHeight, const Renderer& renderer);

    /** Destructor
     */
    ~Animation();

    Animation(const Animation&) = delete;

    Animation& operator =(const Animation&) = delete;

    /** Draws the current frame of the animation.

        \param x the x coordinate of the screen position.
        \param y the y coordinate of the screen position.
        \param renderer a reference to the reference.
     */
    void draw(int x, int y, const Renderer& renderer) const;

    /** Updates the animation.

        \param elapsed the elapsed time in fractions of seconds.
        \return true if the animation has looped through all frames, otherwise false.
     */
    bool update(float elapsed);

    /** Returns the width of a frame.
     */
    unsigned int getWidth() const;

    /** Returns the height of a frame.
     */
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
