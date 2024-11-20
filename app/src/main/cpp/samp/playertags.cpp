#include "main.h"
#include "game/game.h"
#include "game/RW/RenderWare.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "playertags.h"
#include "util/CUtil.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

CPlayerTags::CPlayerTags()
{
	FLog("Loading AFK icon..");
	m_pAFKIconTexture = CUtil::LoadTextureFromDB("samp", "afk_icon");
    FLog("Loading AFK icon1..");
	m_pMicroIconTexture = CUtil::LoadTextureFromDB("samp", "icon_micro");
}

CPlayerTags::~CPlayerTags() {}

void CPlayerTags::Render(ImGuiRenderer* renderer)
{
	CVector vecPos;
	RwMatrix matLocal, matPlayer;
	int dwHitEntity;
	char szNickBuf[64];

	if (pNetGame && pNetGame->m_pNetSet->bShowNameTags)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		pGame->FindPlayerPed()->GetMatrix(&matLocal);

		for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
		{
			if (pPlayerPool->GetSlotState(playerId) == true)
			{
				CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(playerId);

				if (pRemotePlayer && pRemotePlayer->IsActive() && pRemotePlayer->m_bShowNameTag && !pRemotePlayer->IsNPC())
				{
					CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();

					if (pPlayerPed && pPlayerPed->GetDistanceFromCamera() <= pNetGame->m_pNetSet->fNameTagDrawDistance)
					{
						/*if (pRemotePlayer->GetState() == PLAYER_STATE_DRIVER &&
							pRemotePlayer->m_pCurrentVehicle &&
							pRemotePlayer->m_pCurrentVehicle->IsRCVehicle())
						{
							pRemotePlayer->m_pCurrentVehicle->GetMatrix(&matPlayer);
							vecPos.x = matPlayer.pos.x;
							vecPos.y = matPlayer.pos.y;
							vecPos.z = matPlayer.pos.z;
						}
						else
						{*/
							if (!pPlayerPed->IsAdded()) continue;
							vecPos.x = 0.0f;
							vecPos.y = 0.0f;
							vecPos.z = 0.0f;
							pPlayerPed->GetBonePosition(8, &vecPos);
						//}

						CAMERA_AIM* pCam = GameGetInternalAim();
						dwHitEntity = 0;

						if (pNetGame->m_pNetSet->bNameTagLOS)
						{
							dwHitEntity = ScriptCommand(&get_line_of_sight, vecPos.x, vecPos.y, vecPos.z,
								pCam->pos1x, pCam->pos1y, pCam->pos1z, 1, 0, 0, 1, 0);
						}

						if (!pNetGame->m_pNetSet->bNameTagLOS || dwHitEntity && !pRemotePlayer->IsNPC())
						{
							sprintf(szNickBuf, "%s (%d)", pPlayerPool->GetPlayerName(playerId), playerId);
							this->Draw(renderer,&vecPos, szNickBuf,
								pRemotePlayer->GetPlayerColor(),
								pPlayerPed->GetDistanceFromCamera(),
								pRemotePlayer->m_fReportedHealth,
								pRemotePlayer->m_fReportedArmour,
								pRemotePlayer->m_bIsAFK,
								false);
						}
					}
				}
			}
		}
	}
}

void CPlayerTags::Draw(ImGuiRenderer* renderer, CVector* vec, const char* szNick, uint32_t dwColor, float fDist, float fHealth, float fArmour, bool bAfk, bool bMicro)
{
	CVector vecTagPos;

	vecTagPos.x = vec->x;
	vecTagPos.y = vec->y;
	vecTagPos.z = vec->z;
	vecTagPos.z += 0.25f + (fDist * 0.0475f);

	CVector vecOut;
	// CSprite::CalcScreenCoors
	((void (*)(CVector*, CVector*, float*, float*, bool, bool))(g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(&vecTagPos, &vecOut, 0, 0, 0, 0);

	if (vecOut.z < 1.0f) return;

	// name (id)
	ImVec2 pos = ImVec2(vecOut.x, vecOut.y);
	//pos.x -= ImGui::CalcTextSize(szNick).x / 2;
	//ImGuiEx::AddOutlinedText(ImGui::GetBackgroundDrawList(), pos, __builtin_bswap32(dwColor | (0x000000FF)), true, szNick);
	pos.x -= renderer->calculateTextSize(szNick, UISettings::fontSize() / 2).x / 2;
	renderer->drawText(pos, __builtin_bswap32(dwColor | (0x000000FF)), std::string(szNick), true, UISettings::fontSize() / 2);


	// Health bar
	if (fHealth < 0.0f) return;
	vecOut.x = (float)((int)vecOut.x);
	vecOut.y = (float)((int)vecOut.y);

	ImColor HealthBarBDRColor = ImColor(0x00, 0x64, 0x95, 0xED);
	ImColor HealthBarColor = ImColor(0xB9, 0x22, 0x28, 0xFF);
	ImColor HealthBarBGColor = ImColor(0x4B, 0x0B, 0x14, 0xFF);

	//float fWidth = (UISettings::fontSize() / 2) * 3;
	//float fHeight = (UISettings::fontSize() / 2) * 0.f;
	//float fOutline = 2.0f;

	float fWidth = UISettings::nametagBarSize().x;
	float fHeight = UISettings::nametagBarSize().y;
	float fOutline = UISettings::outlineSize();


	ImVec2 HealthBarBDR1;
	ImVec2 HealthBarBDR2;
	ImVec2 HealthBarBG1;
	ImVec2 HealthBarBG2;
	ImVec2 HealthBar1;
	ImVec2 HealthBar2;

	// top left
	HealthBarBDR1.x = vecOut.x - ((fWidth / 2) + fOutline);
	HealthBarBDR1.y = vecOut.y + ((UISettings::fontSize() / 2) * 1.2f);
	// bottom right
	HealthBarBDR2.x = vecOut.x + ((fWidth / 2) + fOutline);
	HealthBarBDR2.y = vecOut.y + ((UISettings::fontSize() / 2) * 1.2f) + fHeight;

	// top left
	HealthBarBG1.x = HealthBarBDR1.x + fOutline;
	HealthBarBG1.y = HealthBarBDR1.y + fOutline;
	// bottom right
	HealthBarBG2.x = HealthBarBDR2.x - fOutline;
	HealthBarBG2.y = HealthBarBDR2.y - fOutline;
	// top left
	HealthBar1.x = HealthBarBG1.x;
	HealthBar1.y = HealthBarBG1.y;
	// bottom right
	HealthBar2.y = HealthBarBG2.y;

	if (fHealth > 100.0f)
		fHealth = 100.0f;

	fHealth *= fWidth / 100.0f;
	fHealth -= (fWidth / 2);
	HealthBar2.x = vecOut.x + fHealth;

	float offsetY = 13.0f;//fHeight / 3;

	if (fArmour > 0.0f)
	{
		HealthBarBDR1.y += offsetY;//13.0f;
		HealthBarBDR2.y += offsetY;//13.0f;
		HealthBarBG1.y += offsetY;//13.0f;
		HealthBarBG2.y += offsetY;//13.0f;
		HealthBar1.y += offsetY;//13.0f;
		HealthBar2.y += offsetY;//13.0f;
	}

	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
	ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);

	// Armour Bar
	if (fArmour > 0.0f)
	{
		HealthBarBDR1.y -= offsetY;//13.0f;
		HealthBarBDR2.y -= offsetY;//13.0f;
		HealthBarBG1.y -= offsetY;//13.0f;
		HealthBarBG2.y -= offsetY;//13.0f;
		HealthBar1.y -= offsetY;//13.0f;
		HealthBar2.y -= offsetY;//13.0f;

		HealthBarColor = ImColor(200, 200, 200, 255);
		HealthBarBGColor = ImColor(40, 40, 40, 255);

		if (fArmour > 100.0f)
			fArmour = 100.0f;

		fArmour *= fWidth / 100.0f;
		fArmour -= (fWidth / 2);
		HealthBar2.x = vecOut.x + fArmour;

		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBDR1, HealthBarBDR2, HealthBarBDRColor);
		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBarBG1, HealthBarBG2, HealthBarBGColor);
		ImGui::GetBackgroundDrawList()->AddRectFilled(HealthBar1, HealthBar2, HealthBarColor);
	}

	ImVec2 a = ImVec2(HealthBarBDR1.x - ((UISettings::fontSize() / 2) * 1.4f), HealthBarBDR1.y);
	ImVec2 b = ImVec2(a.x + ((UISettings::fontSize() / 2) * 1.3f), a.y + ((UISettings::fontSize() / 2) * 1.3f));

	// micro icon
	if (bMicro)
	{
		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pMicroIconTexture->raster, a, b);
	}

	// AFK icon
	if (bAfk)
	{
		ImVec2 a = ImVec2(HealthBarBDR1.x - ((UISettings::fontSize() / 2) * 1.4f), HealthBarBDR1.y);
		ImVec2 b = ImVec2(a.x + ((UISettings::fontSize() / 2) * 1.3f), a.y + ((UISettings::fontSize() / 2) * 1.3f));
		ImGui::GetBackgroundDrawList()->AddImage((ImTextureID)m_pAFKIconTexture->raster, a, b);
	}
}