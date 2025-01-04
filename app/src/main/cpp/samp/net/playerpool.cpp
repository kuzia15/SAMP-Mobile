#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "playerpool.h"

CPlayerPool::CPlayerPool()
{
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		m_bPlayerSlotState[playerId] = false;
	}

	m_pLocalPlayer = new CLocalPlayer();
	m_iLocalPlayerScore = 0;
	m_iLocalPlayerPing = 0;
	m_LocalPlayerID = 0;
	m_iLastPlayerID = 0;
}
// 0.3.7
CPlayerPool::~CPlayerPool()
{
	if (m_pLocalPlayer) {
		delete m_pLocalPlayer;
		m_pLocalPlayer = nullptr;
	}

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		this->Delete(playerId, 0);
	}
}
// 0.3.7
void CPlayerPool::Process()
{
	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		if (m_bPlayerSlotState[playerId] == true) {
			m_pPlayers[playerId]->Process();
		}
	}

	if (m_pLocalPlayer) {
		m_pLocalPlayer->Process();
	}
}
// 0.3.7
bool CPlayerPool::New(PLAYERID playerId, const char* szPlayerName, bool bIsNPC)
{
	/*CPlayer *pPlayer = new CPlayer(szPlayerName, bIsNPC);

	if (playerId > MAX_PLAYERS) {
		return false;
	}

	m_pPlayers[playerId] = pPlayer;
	if (pPlayer == nullptr) {
		return false;
	}

	pPlayer->m_pRemotePlayer->SetID(playerId);

	if (bIsNPC) {
		pPlayer->m_pRemotePlayer->SetNPC();
	}

	m_bPlayerSlotState[playerId] = true;

	this->FindLastPlayerID();

	return true;*/
	m_pPlayers[playerId] = new CRemotePlayer();

	if(m_pPlayers[playerId])
	{
		strcpy(m_szPlayerNames[playerId], szPlayerName);
		m_pPlayers[playerId]->SetID(playerId);
		if(bIsNPC)
			m_pPlayers[playerId]->SetNPC();
		m_bPlayerSlotState[playerId] = true;

		return true;
	}

	return false;
}
// 0.3.7
bool CPlayerPool::Delete(PLAYERID playerId, uint8_t byteReason)
{
	if (playerId >= MAX_PLAYERS || !m_bPlayerSlotState[playerId] || !m_pPlayers[playerId]) {
		return false;
	}

	if (m_pLocalPlayer && m_pLocalPlayer->IsSpectating() && m_pLocalPlayer->m_SpectateID == playerId) {
		m_pLocalPlayer->ToggleSpectating(false);
	}
	m_bPlayerSlotState[playerId] = false;

	if (m_pPlayers[playerId]) {
		delete m_pPlayers[playerId];
	}
	m_pPlayers[playerId] = nullptr;
	this->FindLastPlayerID();

	return true;
}
// 0.3.7
void CPlayerPool::FindLastPlayerID()
{
	int iLastPlayerID = 0;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++) {
		if (m_bPlayerSlotState[playerId] == true) {
			iLastPlayerID = playerId;
		}
	}

	m_iLastPlayerID = iLastPlayerID;
}

int CPlayerPool::GetPlayerScore(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) return 0;
	return m_iPlayerScores[playerId];
}

int CPlayerPool::GetPlayerPing(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) {
		return false;
	}

	if (playerId > MAX_PLAYERS) return 0;
	return m_dwPlayerPings[playerId];
}

// 0.3.7
bool CPlayerPool::IsPlayerNPC(PLAYERID playerId)
{
	if (playerId > MAX_PLAYERS) {
		return false;
	}

	return m_pPlayers[playerId]->IsNPC();
}

int CPlayerPool::GetTotalPlayers()
{
	int iTotal = 1;

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		if (GetSlotState(i)) iTotal++;
	}

	return iTotal;
}

// 0.3.7
void CPlayerPool::UpdatePlayerScore(PLAYERID playerId, int iScore)
{
	if (playerId == m_LocalPlayerID)
	{
		m_iLocalPlayerScore = iScore;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_iPlayerScores[playerId] = iScore;
	}
}
// 0.3.7
void CPlayerPool::UpdatePlayerPing(PLAYERID playerId, int iPing)
{
	if (playerId == m_LocalPlayerID)
	{
		m_iLocalPlayerPing = iPing;
	}
	else
	{
		if (playerId > MAX_PLAYERS - 1) { return; }
		m_dwPlayerPings[playerId] = iPing;
	}
}
// 0.3.7
PLAYERID CPlayerPool::FindRemotePlayerIDFromGtaPtr(CPedGTA* pActor)
{
	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CRemotePlayer* pRemotePlayer = GetAt(i);
		if (pRemotePlayer)
		{
			CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed)
			{
				if (pPlayerPed->m_pPed)
				{
					if (pPlayerPed->m_pPed == pActor)
					{
						return pRemotePlayer->GetID();
					}
				}
			}
		}
	}

	return INVALID_PLAYER_ID;
}
// 0.3.7
void CPlayerPool::DeactivateAll()
{
	m_pLocalPlayer->m_bIsActive = false;
	m_pLocalPlayer->m_iSelectedClass = 0;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer* pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer)
		{
			pRemotePlayer->SetState(PLAYER_STATE_NONE);
			pRemotePlayer->m_bShowNameTag = true;
		}
	}
}
// 0.3.7
void CPlayerPool::ProcessAttachedObjects()
{
	if (m_pLocalPlayer)
	{
		CPlayerPed* pPlayerPed = m_pLocalPlayer->GetPlayerPed();
		if (pPlayerPed)
		{
			pPlayerPed->ProcessSpecialAction(pPlayerPed->iSpecialAction);
			pPlayerPed->ProcessAttach();
            pPlayerPed->m_pPed->GetIntelligence()->ProcessAfterPreRender();
		}
	}

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		CRemotePlayer* pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer)
		{
			CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed && !pPlayerPed->m_pPed->m_bRemoveFromWorld)
			{
				pPlayerPed->ProcessSpecialAction(pPlayerPed->iSpecialAction);
				pPlayerPed->ProcessAttach();
                pPlayerPed->m_pPed->GetIntelligence()->ProcessAfterPreRender();
			}
		}
	}
}
// 0.3.7
void CPlayerPool::ApplyCollisionChecking()
{
	CRemotePlayer* pRemotePlayer = nullptr;
	CPlayerPed* pPlayerPed = nullptr;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer)
		{
			pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed)
			{
				if (!pPlayerPed->IsInVehicle())
				{
					m_bCollisionChecking[playerId] = pPlayerPed->m_pPed->m_bCollisionProcessed;
					pPlayerPed->m_pPed->SetCollisionChecking(true);
				}
			}
		}
	}
}

void CPlayerPool::ResetCollisionChecking()
{
	CRemotePlayer* pRemotePlayer = nullptr;
	CPlayerPed* pPlayerPed = nullptr;

	for (PLAYERID playerId = 0; playerId < MAX_PLAYERS; playerId++)
	{
		pRemotePlayer = GetAt(playerId);
		if (pRemotePlayer)
		{
			pPlayerPed = pRemotePlayer->GetPlayerPed();
			if (pPlayerPed)
			{
				if (!pPlayerPed->IsInVehicle())
				{
					if(!pPlayerPed->IsInVehicle())
						pPlayerPed->m_pPed->SetCollisionChecking(m_bCollisionChecking[playerId]);
				}
			}
		}
	}
}