#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"

#include "StreamAtVehicle.h"

#include "StreamInfo.h"

extern CNetGame *pNetGame;

StreamAtVehicle::StreamAtVehicle(const uint16_t color, std::string name,
                                 const float distance, const VEHICLEID vehicleId) noexcept
    : LocalStream(StreamType::LocalStreamAtVehicle, color, std::move(name), distance)
    , vehicleId(vehicleId)
{}

void StreamAtVehicle::Tick() noexcept
{
    this->LocalStream::Tick();

    if(!pNetGame) return;

    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    if(!pVehiclePool) return;

    CVehicle *pVehicle = pVehiclePool->GetAt(this->vehicleId);
    if(!pVehicle) return;

    RwMatrix pVehicleMatrix;
    pVehicle->GetMatrix(&pVehicleMatrix);

    for(const auto& channel : this->GetChannels())
    {
        if(channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix.pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtVehicle::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    if(!pNetGame) return;

    CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
    if(!pVehiclePool) return;

    CVehicle *pVehicle = pVehiclePool->GetAt(this->vehicleId);
    if(!pVehicle) return;

    RwMatrix pVehicleMatrix;
    pVehicle->GetMatrix(&pVehicleMatrix);

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pVehicleMatrix.pos),
        &kZeroVector, &kZeroVector);
}
