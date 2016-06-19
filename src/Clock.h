#ifndef _Clock_H
#define _Clock_H

#include <chrono>

/** A clock to get different timings from.
 */
class Clock {
public:
    /** Constructor
     */
    Clock();

    /** Update the clock. This method should be called once at the beginning of each frame.
     */
    void update();

    /** Returns the current time relative to the time when the clock was created.
     */
    float getTime() const;

    /** Returns the elapsed time between the last two updates.
     */
    float getElapsed() const;

    /** Returns the duration between the time when the clock was created and the last update.
     */
    float getGameTime() const;

    /** Returns the time since the last update.
     */
    float getFrameTime() const;

private:
    std::chrono::high_resolution_clock::time_point startTime;

    std::chrono::high_resolution_clock::time_point lastTime;

    std::chrono::high_resolution_clock::time_point currentTime;

    float elapsed;
};

#endif  // _Clock_H
