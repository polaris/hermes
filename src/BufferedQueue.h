#ifndef _BufferedQueue_H
#define _BufferedQueue_H

#include "PacketSink.h"
#include "Packet.h"
#include "Queue.h"

class BufferedQueue : public PacketSink {
public:
    explicit BufferedQueue(uint32_t size);

    void push(Packet* packet) override;

    Packet* pop() override;

    void enqueue(Packet* packet) override;

    Packet* dequeue() override;

    uint32_t getNumEnqueued();

private:
    Queue<Packet> pool_;

    Queue<Packet> queue_;
};

#endif  // _BufferedQueue_H
