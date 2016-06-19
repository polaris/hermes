#ifndef _Explosion_H
#define _Explosion_H

#include "GameObject.h"
#include "Animation.h"
#include "Vector2d.h"

class Renderer;
class Packet;

/** An explosion game.
 */
class Explosion : public GameObject {
public:
    /** Constructor

        \param renderer a reference to the renderer.
        \param position the position of the explosion on the screen.
     */
    Explosion(const Renderer& renderer, const Vector2d& position);

    /** Update the explosion.

        \param elapsed the elapsed time since the last update.
     */
    void update(float elapsed);

    /** Draws the explosion.

        \param renderer a reference to the renderer.
     */
    void draw(Renderer& renderer);

    /** Writes the explosion information to the packet.

        \param packet a pointer to a packet.
     */
    void write(Packet* packet);

    /** Reads explosion information from a packet.

        \param packet a pointer to a packet.
     */
    void read(Packet* packet);

    /** Returns the position of the explosion.
     */
    const Vector2d& getPosition() const;

    /** Returns the width of the explosion.
     */
    unsigned int getWidth() const;

    /** Returns the height of the explosion.
     */
    unsigned int getHeight() const;

    /** Returns true if this object is collidable, otherwise false.
     */
    bool doesCollide() const;

    /** Returns the class ID.
     */
    uint32_t getClassId() const;

    enum { ClassId = 3 };

protected:
    Animation animation_;

    Vector2d position_;
};

#endif  // _Explosion_H
