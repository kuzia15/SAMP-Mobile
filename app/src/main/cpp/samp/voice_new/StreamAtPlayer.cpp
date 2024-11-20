#include "../main.h"
#include "../game/game.h"
#include "../net/netgame.h"

#include "StreamAtPlayer.h"

#include "StreamInfo.h"

extern CNetGame *pNetGame;

StreamAtPlayer::StreamAtPlayer(const uint32_t color, std::string name,
                               const float distance, const PLAYERID playerId) noexcept
    : LocalStream(StreamType::LocalStreamAtPlayer, color, std::move(name), distance)
    , playerId(playerId)
{}

void StreamAtPlayer::Tick() noexcept
{
    this->LocalStream::Tick();

    if(!pNetGame) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool) return;

    CRemotePlayer *pPlayer = pPlayerPool->GetAt(this->playerId);
    if(!pPlayer) return;

    CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
    if(!pPlayerPed) return;

    RwMatrix pPlayerMatrix;
    pPlayerPed->GetMatrix(&pPlayerMatrix);

    for(const auto& channel : this->GetChannels())
    {
        if(channel->HasSpeaker())
        {
            BASS_ChannelSet3DPosition(channel->GetHandle(),
                reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix.pos),
                nullptr, nullptr);
        }
    }
}

void StreamAtPlayer::OnChannelCreate(const Channel& channel) noexcept
{
    static const BASS_3DVECTOR kZeroVector { 0, 0, 0 };

    this->LocalStream::OnChannelCreate(channel);

    if(!pNetGame) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool) return;

    CRemotePlayer *pPlayer = pPlayerPool->GetAt(this->playerId);
    if(!pPlayer) return;

    CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
    if(!pPlayerPed) return;

    RwMatrix pPlayerMatrix;
    pPlayerPed->GetMatrix(&pPlayerMatrix);

    BASS_ChannelSet3DPosition(channel.GetHandle(),
        reinterpret_cast<BASS_3DVECTOR*>(&pPlayerMatrix.pos),
        &kZeroVector, &kZeroVector);
}
