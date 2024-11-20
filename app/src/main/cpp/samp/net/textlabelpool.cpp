#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame* pGame;
extern CNetGame* pNetGame;

// 0.3.7
C3DTextLabelPool::C3DTextLabelPool()
{
	for (int i = 0; i < MAX_TEXT_LABELS; i++) {
		//m_TextLabels[i].pszText = nullptr;
		m_TextLabels[i] = nullptr;

		m_bSlotUsed[i] = false;
	}
}
// 0.3.7
C3DTextLabelPool::~C3DTextLabelPool()
{
	for (int i = 0; i < MAX_TEXT_LABELS; i++) {
		if (m_bSlotUsed[i]) {
			this->ClearLabel(i);
		}
	}
}
// 0.3.7
void C3DTextLabelPool::NewLabel(uint16_t wLabelId, TEXT_LABEL* pLabel) {

	if (wLabelId < MAX_TEXT_LABELS) {

		if (m_TextLabels[wLabelId])
		{
			delete m_TextLabels[wLabelId];
			m_TextLabels[wLabelId] = nullptr;
			m_bSlotUsed[wLabelId] = false;
		}

		//labelInfo.dwColor = (labelInfo.dwColor >> 8) | (labelInfo.dwColor << 24);
		//pTextLabel->pszText = new char[strlen(pLabel->pszText) + 1];
		//strcpy(pTextLabel->pszText, pLabel->pszText);
		TEXT_LABEL* pTextLabel = new TEXT_LABEL;
		pTextLabel->text = Encoding::cp2utf(pLabel->text);

		pTextLabel->dwColor = pLabel->dwColor;
		pTextLabel->vecPos.x = pLabel->vecPos.x;
		pTextLabel->vecPos.y = pLabel->vecPos.y;
		pTextLabel->vecPos.z = pLabel->vecPos.z;
		pTextLabel->fDistance = pLabel->fDistance;
		pTextLabel->bTestLOS = pLabel->bTestLOS;
		pTextLabel->playerId = pLabel->playerId;
		pTextLabel->vehicleId = pLabel->vehicleId;

		m_TextLabels[wLabelId] = pTextLabel;
		m_bSlotUsed[wLabelId] = true;
	}
}
// 0.3.7
void C3DTextLabelPool::ClearLabel(uint16_t wLabelId) {
	if (wLabelId < 0 || wLabelId >= MAX_TEXT_LABELS)
	{
		return;
	}
	m_bSlotUsed[wLabelId] = false;
	if (m_TextLabels[wLabelId])
	{
		delete m_TextLabels[wLabelId];
		m_TextLabels[wLabelId] = nullptr;
	}
}

void C3DTextLabelPool::Render(ImGuiRenderer* renderer)
{
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
	if(!pPlayerPed) return;

	for (int i = 0; i < MAX_TEXT_LABELS; i++)
	{
		if (m_bSlotUsed[i]) {
            CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
            if (!pPlayerPool) break;

            TEXT_LABEL *pTextLabel = m_TextLabels[i];

            CVector vecTextPos = pTextLabel->vecPos;

            if (pTextLabel->playerId != INVALID_PLAYER_ID) {
                if (pTextLabel->playerId == pPlayerPool->GetLocalPlayerID()) continue;

                if (pPlayerPool && pPlayerPool->GetSlotState(pTextLabel->playerId)) {
                    CRemotePlayer *pPlayer = pPlayerPool->GetAt(pTextLabel->playerId);
                    if (pPlayer && pPlayer->GetDistanceFromLocalPlayer() < pTextLabel->fDistance) {
                        CPlayerPed *pPlayerPed = pPlayer->GetPlayerPed();
                        if (pPlayerPed && pPlayerPed->IsAdded()) {
                            CVector matBone;
                            pPlayerPed->GetBonePosition(8, &matBone);

                            vecTextPos.x = matBone.x + pTextLabel->vecPos.x;
                            vecTextPos.y = matBone.y + pTextLabel->vecPos.y;
                            vecTextPos.z = matBone.z + 0.23 + pTextLabel->vecPos.z;

                            this->Draw(renderer, pTextLabel, vecTextPos, pTextLabel->text,
                                       pTextLabel->dwColor);

                        }
                    }
                }
            }
			if (pTextLabel->vehicleId != INVALID_VEHICLE_ID) {
				CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
				if (pVehiclePool && pVehiclePool->GetSlotState(pTextLabel->vehicleId)) {
					CVehicle *pVehicle = pVehiclePool->GetAt(pTextLabel->vehicleId);
					if (pVehicle && pVehicle->IsAdded() &&
						pVehicle->GetDistanceFromLocalPlayerPed() < pTextLabel->fDistance) {
						RwMatrix matVehicle;
						pVehicle->GetMatrix(&matVehicle);

						vecTextPos.x = matVehicle.pos.x + pTextLabel->vecPos.x;
						vecTextPos.y = matVehicle.pos.y + pTextLabel->vecPos.y;
						vecTextPos.z = matVehicle.pos.z + pTextLabel->vecPos.z;

						this->Draw(renderer, pTextLabel, vecTextPos, pTextLabel->text,
								   pTextLabel->dwColor);
					}
				}
			}

			if (pPlayerPed->GetDistanceFromPoint(pTextLabel->vecPos) <= pTextLabel->fDistance)
				this->Draw(renderer, pTextLabel, vecTextPos, pTextLabel->text,
						   pTextLabel->dwColor);
        }
	}
}

void C3DTextLabelPool::Draw(ImGuiRenderer* renderer, TEXT_LABEL* label, CVector vecPos, const std::string& text, uint32_t dwColor)
{
	CVector vPos;
	vPos.x = vecPos.x;
	vPos.y = vecPos.y;
	vPos.z = vecPos.z;

	int hitEntity = 0;
    if (label->bTestLOS) {
		CAMERA_AIM *pCam = GameGetInternalAim();
		if (!pCam)
		{
			return;
		}

        RwMatrix matPlayer;
		pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetMatrix(&matPlayer);

		CVector vec;
		vec.x = pCam->pos1x;
		vec.y = pCam->pos1y;
		vec.z = pCam->pos1z;

		//bool isLineOfSightClear = ((bool (*)(CVector*, CVector*, int, int, int, int, int, int, int))(g_libGTASA + 0x423418 + 1))(&vec, &matPlayer.pos, 1, 0, 0, 1, 0, 0, 0);

		hitEntity = ScriptCommand(&get_line_of_sight,
								  vecPos.x, vecPos.y, vecPos.z,
								  pCam->pos1x, pCam->pos1y, pCam->pos1z,
								  1, 0, 0, 1, 0);
		/*if(!isLineOfSightClear)
		{
			LOGI("labelpool draw ok no render fuck you bitch");
			return;
		}*/
    }

	if (!label->bTestLOS || hitEntity) {
		if (pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetDistanceFromPoint(vecPos) <= label->fDistance) {
			CVector vecOut;
			// CSprite::CalcScreenCoors
			((void (*)(CVector *, CVector *, float *, float *, bool, bool)) (g_libGTASA + (VER_x32 ? 0x005C57E8 + 1 : 0x6E9DF8)))(
					&vPos, &vecOut, 0, 0, 0, 0);
			if (vecOut.z < 1.0f) return;

			std::stringstream ss_data(text);
			std::string s_row;
			while (std::getline(ss_data, s_row, '\n')) {
				ImVec2 sz = renderer->calculateTextSize(s_row, UISettings::fontSize() / 2);
				renderer->drawText(ImVec2(vecOut.x - (sz.x / 2), vecOut.y),
								   __builtin_bswap32(dwColor | (0x000000FF)), s_row, true,
								   UISettings::fontSize() / 2);
				vecOut.y += UISettings::fontSize() / 2;
			}
		}
	}
}