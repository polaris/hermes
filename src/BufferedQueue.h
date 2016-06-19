#ifndef _BufferedQueue_H
#define _BufferedQueue_H

#include "PacketSink.h"
#include "Packet.h"
#include "Queue.h"

/** A packet queue with its own pool.
 */
class BufferedQueue : public PacketSink {
public:
    /** Constructor

        \param size the maximum size of the queue.
     */
    explicit BufferedQueue(uint32_t size);

    /** Push a packet to the pool.

        \param packet a pointer to the packet that should be pushed on the pool.
     */
    void push(Packet* packet) override;

    /** Pop a packet from the pool.

        \return a pointer to a packet.
     */
    Packet* pop() override;

    /** Add a packet to the end of the queue.

        \param packet a pointer to the packet that should be added to the queue.
     */
    void enqueue(Packet* packet) override;

    /** Return and remove the first packet from the queue.

        \return the first packet from the queue.
     */
    Packet* dequeue() override;

    /** Return the number of enqueued packets.
     */
    uint32_t getNumEnqueued();

private:
    Queue<Packet> pool_;

    Queue<Packet> queue_;
};

#endif  // _BufferedQueue_H
