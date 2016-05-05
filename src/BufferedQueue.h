#ifndef _BufferedQueue_H
#define _BufferedQueue_H

#include "PacketSink.h"
#include "Packet.h"
#include "Queue.h"

class BufferedQueue : public PacketSink {
public:
    BufferedQueue(uint32_t size);

    void push(Packet* packet) override;

    Packet* pop() override;

    void enqueue(Packet* packet) override;

    Packet* dequeue() override;

private:
    Queue<Packet> pool_;

    Queue<Packet> queue_;
};

#endif  // _BufferedQueue_H