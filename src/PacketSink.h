#ifndef _PacketSink_H
#define _PacketSink_H

class Packet;

class PacketSink {
public:
    virtual ~PacketSink() = default;

    virtual void push(Packet* packet) = 0;

    virtual Packet* pop() = 0;

    virtual void enqueue(Packet* packet) = 0;

    virtual Packet* dequeue() = 0;
};

#endif  // _PacketSink_H
