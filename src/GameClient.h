#ifndef _GameClient_H
#define _GameClient_H

#include "Game.h"
#include "Queue.h"
#include "Packet.h"
#include "InputHandler.h"
#include "Transceiver.h"

#include <memory>
#include <unordered_map>

class Renderer;
class Clock;
class SpaceShip;

class GameClient : public Game {
public:
    GameClient(unsigned int frameRate, const char *address, uint16_t port, Renderer& renderer);

private:
    void handleWillUpdateWorld(const Clock& clock) override;
    void handleDidUpdateWorld(const Clock& clock) override;
    void handleEvent(SDL_Event& event, bool& running) override;
    void finishFrame() override;

    void processIncomingPackets(const Clock& clock);
    void renderFrame();

    class State {
    public:
        explicit State(GameClient* gameClient);
        virtual ~State() = default;
        virtual void handleWillUpdateWorld(const Clock& clock) = 0;
        virtual void handleIncomingPacket(Packet* packet) = 0;
        virtual void sendOutgoingPackets(const Clock& clock) = 0;

    protected:
        GameClient* gameClient_;
    };

    void setState(std::shared_ptr<State>& newState);

    class Connecting : public State {
    public:
        explicit Connecting(GameClient* gameClient);
        void handleWillUpdateWorld(const Clock& clock) override;
        void handleIncomingPacket(Packet* packet) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void sendHello(const Clock& clock);
        void handleWelcome(Packet* packet);

        float lastHelloTime_;
    };

    class Connected : public State {
    public:
        explicit Connected(GameClient* gameClient);
        void handleWillUpdateWorld(const Clock& clock) override;
        void handleIncomingPacket(Packet* packet) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void sendInput();
        void handleState(Packet* packet);

        float lastInputTime_;

        std::unordered_map<uint32_t, GameObjectPtr> objectIdToGameObjectMap_;
    };

    InputHandler inputHandler_;

    std::shared_ptr<SpaceShip> localSpaceShip_;

    std::shared_ptr<State> currentState;
    std::shared_ptr<State> nextState;

    Queue<Packet> packetPool_;
    Queue<Packet> incomingPackets_;
    Transceiver transceiver_;

    boost::asio::ip::udp::endpoint serverEndpoint_;

    uint32_t playerId_;
};

#endif  // _GameClient_H
