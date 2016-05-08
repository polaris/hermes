#include "BufferedQueue.h"
#include "Packet.h"

BufferedQueue::BufferedQueue(uint32_t size)
: pool_(size , [] () { return new Packet(1500); })
, queue_(size) {
}

void BufferedQueue::push(Packet* packet) {
    pool_.push(packet);
}

Packet* BufferedQueue::pop() {
    return pool_.pop();
}

void BufferedQueue::enqueue(Packet* packet) {
    queue_.push(packet);
}

Packet* BufferedQueue::dequeue() {
    return queue_.pop();
}

uint32_t BufferedQueue::getNumEnqueued() {
    return queue_.getCount();
}