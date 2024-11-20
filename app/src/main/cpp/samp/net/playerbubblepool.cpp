#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../game/RW/RenderWare.h"
#include "../gui/gui.h"
#include "playerbubblepool.h"

extern CNetGame *pNetGame;
//extern CChatWindow *pChatWindow;
//extern CGUI *pGUI;

int WrapText(char* szText, int iWidth, int iMaxWord)
{
    int s, i, r, w, c;
    char buf[512];

    r = strlen(szText);
    w = iWidth;
    i = 0;
    c = 1;

    memset(buf, 0, sizeof(buf));

    // if (strlen(szText) <= 512)
    if(r <= (int)sizeof(buf))
    {
        while(r > iWidth)
        {
            s = iWidth;

            while(s != 0 && szText[i + s] != ' ')
                --s;

            if(w - i - s <= iMaxWord)
            {
                i += s;
                szText[i] = '\n';
                r -= s;
                w = i + iWidth;
            }
            else
            {
                strcpy(buf, szText + w);
                szText[w + 1] = 0;
                szText[w] = '\n';
                memcpy(szText + strlen(szText), buf, strlen(buf) + 1);
                i = w + 1;
                r -= iWidth;
                w += 1 + iWidth;
            }

            c++;
        }

        return c;
    }

    return -1;
}

CPlayerBubblePool::CPlayerBubblePool()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        m_pPlayerBubble[i] = 0;
        m_bSlotState[i] = false;
    }
}

CPlayerBubblePool::~CPlayerBubblePool()
{
    for(int i = 0; i < MAX_PLAYERS; i++)
    {
        if(m_pPlayerBubble[i] && m_bSlotState[i])
        {
            delete m_pPlayerBubble[i];
        }

        m_pPlayerBubble[i] = 0;
        m_bSlotState[i] = false;
    }
}

PlayerBubbleStruct *CPlayerBubblePool::New(PLAYERID playerId, const char *text, uint32_t color, float distance, uint32_t time)
{
    if(m_bSlotState[playerId] && m_pPlayerBubble[playerId])
    {
        delete m_pPlayerBubble[playerId];
    }

    PlayerBubbleStruct* playerBubbleStruct = new PlayerBubbleStruct;
    //Utils::cpToUtf8(m_pPlayerBubble[playerId]->szText, text);
    std::string GText = Encoding::cp2utf(text);
    strcpy(playerBubbleStruct->szText, GText.c_str());

    playerBubbleStruct->fDistance = distance;
    playerBubbleStruct->uiExpireTime = GetTickCount() + time;
    playerBubbleStruct->dwLineCount = WrapText(playerBubbleStruct->szText, 36, 12) - 1;
    playerBubbleStruct->uiColor = color;

    m_pPlayerBubble[playerId] = playerBubbleStruct;
    m_bSlotState[playerId] = true;

    return m_pPlayerBubble[playerId];
}

void CPlayerBubblePool::Delete(PLAYERID playerId)
{
    if(m_bSlotState[playerId] && m_pPlayerBubble[playerId])
    {
        delete m_pPlayerBubble[playerId];
        m_bSlotState[playerId] = false;
    }
}

void CPlayerBubblePool::Render(ImGuiRenderer* renderer)
{
    this->Draw(renderer);
}

void CPlayerBubblePool::Draw(ImGuiRenderer* renderer)
{
    for(PLAYERID i = 0; i < MAX_PLAYERS; i++)
    {
        if(m_bSlotState[i] && m_pPlayerBubble[i])
        {   
            if(GetTickCount() >= m_pPlayerBubble[i]->uiExpireTime)
            {
                Delete(i);
                continue;
            }

            if(pNetGame) {

                CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
                if (pPlayerPool->GetSlotState(i)) {
                    CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(i);
                    if (pRemotePlayer && pRemotePlayer->IsActive()) {
                        CPlayerPed *pPlayerPed = pRemotePlayer->GetPlayerPed();

                        if (pPlayerPed && pPlayerPed->IsAdded()) {
                            if (pPlayerPed->GetDistanceFromCamera() <=
                                m_pPlayerBubble[i]->fDistance) {
                                CVector VecPos;
                                VecPos.x = 0.0f;
                                VecPos.y = 0.0f;
                                VecPos.z = 0.0f;

                                pPlayerPed->GetBonePosition(8, &VecPos);

                                CVector TagPos;
                                TagPos.x = VecPos.x;
                                TagPos.y = VecPos.y;
                                TagPos.z = VecPos.z;

                                float fTextHeight =
                                        ((m_pPlayerBubble[i]->dwLineCount * 0.0125f) + 0.065f) +
                                        (m_pPlayerBubble[i]->dwLineCount * 0.0125f);
                                TagPos.z +=
                                        pPlayerPed->GetDistanceFromCamera() * fTextHeight + 0.2f;

                                CVector Out;

                                // CSprite::CalcScreenCoors
                                //(( void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_GTASAAdr+0x54EEC0+1))(&TagPos, &Out, 0, 0, 0, 0);
                                // CSprite::CalcScreenCoors
                                ((void (*)(CVector *, CVector *, float *, float *, bool, bool)) (
                                        g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(&TagPos, &Out, 0, 0, 0, 0);

                                if (Out.z < 1.0f) {
                                    return;
                                }

                                //ImVec2 pos = ImVec2(Out.x, Out.y - (pGUI->GetFontSize() * 1.67));
                                ImVec2 pos = ImVec2(Out.x, Out.y - (UISettings::fontSize() * 1.0));
                                //pos.x -= ImGui::CalcTextSize(m_pPlayerBubble[i]->szText).x / 2;
                                pos.x -= renderer->calculateTextSize(m_pPlayerBubble[i]->szText,
                                                                     UISettings::fontSize() / 2).x /
                                         2;
                                //UIUtils::TextWithColors(pos, __builtin_bswap32(m_pPlayerBubble[i]->uiColor | (0x000000FF)), m_pPlayerBubble[i]->szText, pGUI->GetFontSize());
                                //renderer->drawText(pos, __builtin_bswap32(m_pPlayerBubble[i]->uiColor | (0x000000FF)), m_pPlayerBubble[i]->szText, true, UISettings::fontSize() / 2);
                                renderer->drawText(pos, __builtin_bswap32(
                                                           m_pPlayerBubble[i]->uiColor | (0x000000FF)),
                                                   std::string(m_pPlayerBubble[i]->szText), true,
                                                   UISettings::fontSize() / 2);
                            }
                        }
                    }
                }
            }
        }
    }
}