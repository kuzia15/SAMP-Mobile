#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

// 0.3.7
CActorPool::CActorPool()
{
	for (PLAYERID ActorID = 0; ActorID < MAX_ACTORS; ActorID++)
	{
		m_pActors[ActorID] = nullptr;
		m_bActorSlotState[ActorID] = false;
		m_pGtaPed[ActorID] = nullptr;
	}
}
// 0.3.7
CActorPool::~CActorPool()
{
	for (PLAYERID ActorID = 0; ActorID < MAX_ACTORS; ActorID++)
	{
		Delete(ActorID);
	}
}
// 0.3.7
void CActorPool::New(NEW_ACTOR* pNewActor)
{
	if (m_pActors[pNewActor->ActorID])
	{
		//if (gui) gui->chat()->addDebugMessage("Warning: actor %u was not deleted", pNewActor->ActorID);
		Delete(pNewActor->ActorID);
	}

	CActor* pActor = new CActor(pNewActor->iSkin,
		pNewActor->vecPos.x,
		pNewActor->vecPos.y,
		pNewActor->vecPos.z,
		pNewActor->fAngle);

	m_pActors[pNewActor->ActorID] = pActor;

	if (!pActor) return;

	m_pGtaPed[pNewActor->ActorID] = pActor->m_pPed;
	m_bActorSlotState[pNewActor->ActorID] = true;
	pActor->SetHealth(pNewActor->fHealth);

	if (pNewActor->bInvulnerable) {
		pActor->SetInvulnerable(true);
	}
	else {
		pActor->SetInvulnerable(false);
	}
}
// 0.3.7
void CActorPool::Delete(PLAYERID ActorID)
{
	CActor* pActor = GetAt(ActorID);
	if (pActor) {
		m_bActorSlotState[ActorID] = false;
		delete m_pActors[ActorID];
		m_pActors[ActorID] = nullptr;
		m_pGtaPed[ActorID] = nullptr;
	}
}