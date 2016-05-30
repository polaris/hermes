#ifndef _GamePeer_H
#define _GamePeer_H

#include "Game.h"
#include "PeerToPeerWorld.h"
#include "InputHandler.h"
#include "LatencyEstimator.h"
#include "BufferedQueue.h"
#include "LatencyEmulator.h"
#include "Transceiver.h"
#include "PeerRegistry.h"

#include <memory>

class Renderer;
class Clock;
class Packet;

class GamePeer : public Game {
public:
    GamePeer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned int emulatedLatency, unsigned int stdDevMeanLatency, Renderer& renderer, unsigned short port);

    GamePeer(unsigned int width, unsigned int height, unsigned int frameRate, unsigned int updateRate, unsigned int emulatedLatency, unsigned int stdDevMeanLatency, Renderer& renderer, const char* masterAddress, unsigned short port);

    GamePeer(const GamePeer&) = delete;

    GamePeer& operator =(const GamePeer&) = delete;

private:
    void update(const Clock& clock) override;
    void handleEvent(SDL_Event &event, bool& running) override;

    void processIncomingPackets(const Clock& clock);
    void renderFrame();

    class State {
    public:
        explicit State(GamePeer* gamePeer);
        virtual ~State() = default;
        State(const GamePeer::State&) = delete;
        GamePeer::State& operator =(const GamePeer::State&) = delete;
        virtual void handleWillUpdateWorld(const Clock&) {}
        virtual void handleIncomingPacket(Packet* packet, const Clock& clock) = 0;
        virtual void sendOutgoingPackets(const Clock& clock) = 0;

    protected:
        GamePeer* gamePeer_;
    };

    using StatePtr = std::shared_ptr<State>;

    void setState(StatePtr& newState);

    class Peering : public State {
    public:
        explicit Peering(GamePeer* gamePeer);
        Peering(const GamePeer::Peering&) = delete;
        GamePeer::Peering& operator =(const GamePeer::Peering&) = delete;
        void handleIncomingPacket(Packet* packet, const Clock& clock) override;

        virtual void handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock& clock) = 0;
        virtual void sendOutgoingPackets(const Clock& clock) override;

    private:

        void handleTick(Packet* packet, const Clock& clock);
        void handleTock(Packet* packet, const Clock& clock);
        void sendTick(const Clock& clock);

        float lastTickTime_;
    };

    class Accepting : public Peering {
    public:
        explicit Accepting(GamePeer* gamePeer);
        void handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock& clock) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void handleHello(Packet* packet);

        uint32_t nextPlayerId_;
    };

    class Connecting : public State {
    public:
        explicit Connecting(GamePeer* gamePeer);
        void handleIncomingPacket(Packet* packet, const Clock& clock) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void sendHello(const Clock& clock);
        void handleInvite(Packet* packet);

        float lastHelloTime_;
    };

    class Waiting : public Peering {
    public:
        explicit Waiting(GamePeer* gamePeer);
        void handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock& clock) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void handleIntro(Packet* packet);
        void handleStart(Packet* packet);
    };

    class Playing : public Peering {
    public:
        explicit Playing(GamePeer* gamePeer);
        void handleWillUpdateWorld(const Clock&) override;
        void handleIncomingPacketType(unsigned char packetType, Packet* packet, const Clock& clock) override;
        void sendOutgoingPackets(const Clock& clock) override;

    private:
        void handleState(Packet* packet);

        uint32_t nextObjectId_;

        float lastStateUpdate_;
    };

    PeerToPeerWorld world_;

    float updateInterval_;

    InputHandler inputHandler_;
    LatencyEstimator latencyEstimator_;

    StatePtr currentState;
    StatePtr nextState;

    PeerRegistry peerRegistry_;

    uint32_t playerId_;
    uint32_t objectId_;

    BufferedQueue bufferedQueue_;
    LatencyEmulator latencyEmulator_;

    Transceiver transceiver_;
    boost::asio::ip::udp::endpoint masterPeerEndpoint_;
};

#endif	// _GamePeer_H
