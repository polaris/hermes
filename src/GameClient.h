#ifndef _GameClient_H
#define _GameClient_H

#include "Game.h"
#include "World.h"
#include "BufferedQueue.h"
#include "InputHandler.h"
#include "Transceiver.h"
#include "LatencyEmulator.h"
#include "LatencyEstimator.h"

#include <memory>
#include <unordered_map>

class Renderer;
class Clock;
class Packet;

class GameClient : public Game {
public:
    GameClient(unsigned int frameRate, unsigned int emulatedLatency, const char *address, uint16_t port, Renderer& renderer);

    GameClient(const GameClient&) = delete;

    GameClient& operator =(const GameClient&) = delete;

private:
    void update(const Clock& clock) override;
    void handleEvent(SDL_Event& event, bool& running) override;
    void finishFrame() override;

    void processIncomingPackets(const Clock& clock);
    void renderFrame();

    GameObject* createNewGameObject(uint32_t classId, uint32_t objectId);

    class State {
    public:
        explicit State(GameClient* gameClient);
        virtual ~State() = default;
        State(const GameClient::State&) = delete;
        GameClient::State& operator =(const GameClient::State&) = delete;
        virtual void handleWillUpdateWorld(const Clock&) {}
        virtual void handleIncomingPacket(Packet* packet, const Clock& clock) = 0;
        virtual void sendOutgoingPackets(const Clock& clock) = 0;

    protected:
        GameClient* gameClient_;
    };

    using StatePtr = std::shared_ptr<State>;

    void setState(StatePtr& newState);

    class Connecting : public State {
    public:
        explicit Connecting(GameClient* gameClient);
        void handleIncomingPacket(Packet* packet, const Clock& clock) override;
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
        void handleIncomingPacket(Packet* packet, const Clock& clock) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        bool sendInput();
        bool sendTick(const Clock& clock);
        void handleState(Packet* packet);
        void handleTock(Packet* packet, const Clock& clock);

        float lastInputTime_;
        float lastTickTime_;

        std::unordered_map<uint32_t, GameObjectPtr> objectIdToGameObjectMap_;
    };

    World world_;

    InputHandler inputHandler_;
    LatencyEstimator latencyEstimator_;

    StatePtr currentState;
    StatePtr nextState;

    BufferedQueue bufferedQueue_;
    LatencyEmulator latencyEmulator_;
    Transceiver transceiver_;

    boost::asio::ip::udp::endpoint serverEndpoint_;

    uint32_t playerId_;
    uint32_t objectId_;
};

#endif  // _GameClient_H
