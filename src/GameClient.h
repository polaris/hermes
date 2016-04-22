#ifndef _GameClient_H
#define _GameClient_H

#include "Game.h"
#include "Queue.h"
#include "Packet.h"
#include "Transceiver.h"

#include <memory>

class Renderer;
class Clock;

class GameClient : public Game {
public:
    GameClient(unsigned int frameRate, const char *address, unsigned short port, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
    void finishFrame() override;

    class State {
    public:
        explicit State(GameClient* gameClient);
        virtual ~State() = default;
        virtual void handleWillUpdateWorld(const Clock& clock) = 0;
        virtual void handleDidUpdateWorld(const Clock& clock) = 0;

    protected:
        GameClient* gameClient_;
    };

    class Connecting : public State {
    public:
        explicit Connecting(GameClient* gameClient);
        void handleWillUpdateWorld(const Clock& clock) override;
        void handleDidUpdateWorld(const Clock& clock) override;

    private:
        void sendHello(const Clock& clock);
        void processIncomingPackets();
        void handlePacket(Packet* packet);
        void handleWelcome(Packet* packet);

        float lastHelloTime_;
    };

    class Connected : public State {
    public:
        explicit Connected(GameClient* gameClient);
        void handleWillUpdateWorld(const Clock& clock) override;
        void handleDidUpdateWorld(const Clock& clock) override;
    };

    void setState(std::shared_ptr<State>& newState);

    std::shared_ptr<State> currentState;
    std::shared_ptr<State> nextState;

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;

    boost::asio::ip::udp::endpoint serverEndpoint_;
};

#endif  // _GameClient_H
