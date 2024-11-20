#include "../main.h"
#include "../gui/gui.h"
#include "../game/common.h"
#include "../game/util.h"
#include "../game/game.h"
#include "../game/playerped.h"
#include "../net/netgame.h"

#include "SpeakerList.h"

#include "PluginConfig.h"
#include "vendor/imgui/imconfig.h"

#include "../audiostream.h"
extern CAudioStream* pAudioStream;

extern CNetGame *pNetGame;
extern UI* pUI;

bool SpeakerList::Init() noexcept
{
    if(SpeakerList::initStatus)
        return false;

    try
    {
        SpeakerList::tSpeakerIcon = (RwTexture*)CUtil::LoadTextureFromDB("samp", "speaker");
    }
    catch (const std::exception& exception)
    {
        LogVoice("[sv:err:speakerlist:init] : failed to create speaker icon");
        SpeakerList::tSpeakerIcon = nullptr;
        return false;
    }

    if(!PluginConfig::IsSpeakerLoaded())
    {
        PluginConfig::SetSpeakerLoaded(true);
    }

    SpeakerList::initStatus = true;

    return true;
}

void SpeakerList::Free() noexcept
{
    if(!SpeakerList::initStatus)
        return;

    SpeakerList::tSpeakerIcon = nullptr;

    SpeakerList::initStatus = false;
}

void SpeakerList::Show() noexcept
{
    SpeakerList::showStatus = true;
}

bool SpeakerList::IsShowed() noexcept
{
    return SpeakerList::showStatus;
}

void SpeakerList::Hide() noexcept
{
    SpeakerList::showStatus = false;
}

void SpeakerList::Render()
{
    if(!SpeakerList::initStatus || !SpeakerList::IsShowed())
        return;

    if(!pNetGame) return;

    CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
    if(!pPlayerPool) return;

    int curTextLine;
    char szText[64], szText2[64];
    //ImVec2 textPos = ImVec2(pUI->ScaleX(24), pUI->ScaleY(480));
    ImVec2 textPos = ImVec2(0/*24.0f*/, 530.0f);

    for(PLAYERID playerId { 0 }; playerId < MAX_PLAYERS; ++playerId)
    {
        CRemotePlayer* pPlayer = pPlayerPool->GetAt(playerId);
        if(pPlayer && pPlayer->IsActive())
        {
            CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
            if(pPlayerPed)
            {
                const auto playerName = pPlayerPool->GetPlayerName(playerId);
                if(playerName)
                {
                    if(!SpeakerList::playerStreams[playerId].empty())
                    {
                        for(const auto& playerStream : SpeakerList::playerStreams[playerId])
                        {
                            if (pAudioStream)
                            {
                                pAudioStream->Stop(true);
                            }
                            if(playerStream.second.GetType() == StreamType::LocalStreamAtPlayer)
                            {
                                CVector VecPos;

                                if(!pPlayerPed->IsAdded()) continue;
                                VecPos.x = 0.0f;
                                VecPos.y = 0.0f;
                                VecPos.z = 0.0f;
                                pPlayerPed->GetBonePosition(8, &VecPos);

                                if(pPlayerPed->GetDistanceFromLocalPlayerPed() < 60.0f)
                                    SpeakerList::Draw(&VecPos, pPlayerPed->GetDistanceFromCamera());
                            }
                        }

                        if(curTextLine < 4 && playerName && strlen(playerName))
                        {
                            if (pAudioStream)
                            {
                                pAudioStream->Stop(true);
                            }
                            ImVec2 a = ImVec2(textPos.x, textPos.y);
                            ImVec2 b = ImVec2(textPos.x + pUI->GetFontSize() / 2, textPos.y + pUI->GetFontSize() / 2);
                            ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)SpeakerList::tSpeakerIcon->raster, a, b);
                            
                            ImGui::PushFontOutline(0xFF000000, 2);
                            textPos.x = pUI->GetFontSize();
                            sprintf(szText, "%s (ID: %d) ", playerName, playerId);
                            pUI->renderer()->drawText(textPos, 0xFFFFFFFF, szText, true, pUI->GetFontSize() / 2);
                            ImGui::PopFontOutline();

                            for(const auto& streamInfo : SpeakerList::playerStreams[playerId])
                            {
                                if(streamInfo.second.GetColor() == NULL)
                                    continue;

                                ImGui::PushFontOutline(0xFF000000, 2);
                                textPos.x = ImGui::CalcTextSize(szText).x;//1.6;
                                sprintf(szText2, "[%s]", streamInfo.second.GetName().c_str());
                                pUI->renderer()->drawText(textPos, streamInfo.second.GetColor(), szText2, true, pUI->GetFontSize() / 2);
                                ImGui::PopFontOutline();
                            }

                            textPos.x -= ImGui::CalcTextSize(szText).x;// 1.7;
                            textPos.y += pUI->GetFontSize();

                            curTextLine++;
                        }
                    }
                }
            }
        }
    }
}

void SpeakerList::Draw(CVector* vec, float fDist)
{
	CVector TagPos;

	TagPos.x = vec->x;
	TagPos.y = vec->y;
	TagPos.z = vec->z;
	TagPos.z += 0.25f + (fDist * 0.0475f);

	CVector Out;
	// CSprite::CalcScreenCoors
    ((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(&TagPos, &Out, 0, 0, 0, 0);
	if(Out.z < 1.0f)
		return;

    ImVec2 pos = ImVec2(Out.x, Out.y);
	pos.x -= PluginConfig::kDefValSpeakerIconSize / 2;
    pos.y -= pUI->GetFontSize();

    ImVec2 a = ImVec2(pos.x, pos.y);
    ImVec2 b = ImVec2(pos.x + PluginConfig::kDefValSpeakerIconSize, pos.y + PluginConfig::kDefValSpeakerIconSize);
    ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)SpeakerList::tSpeakerIcon->raster, a, b);
}

void SpeakerList::OnSpeakerPlay(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStreams[speaker][(Stream*)(&stream)] = stream.GetInfo();
}

void SpeakerList::OnSpeakerStop(const Stream& stream, const uint16_t speaker) noexcept
{
    uint16_t wSpeaker = speaker;
    if(speaker < 0) wSpeaker = 0;
    else if(speaker > MAX_PLAYERS - 1) wSpeaker = MAX_PLAYERS - 1;
    if(speaker != wSpeaker) return;

    SpeakerList::playerStreams[speaker].erase((Stream*)(&stream));
}

std::array<std::unordered_map<Stream*, StreamInfo>, MAX_PLAYERS> SpeakerList::playerStreams;

bool SpeakerList::initStatus { false };
bool SpeakerList::showStatus { false };

RwTexture* SpeakerList::tSpeakerIcon { nullptr };