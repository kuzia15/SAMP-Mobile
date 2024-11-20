#include "../main.h"
#include "../game/game.h"
#include "netgame.h"

extern CGame *pGame;

// 0.3.7
CObjectPool::CObjectPool()
{
	for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		m_bObjectSlotState[ObjectID] = false;
		m_pObjects[ObjectID] = nullptr;
	}

	m_iObjectCount = 0;
}
// 0.3.7
CObjectPool::~CObjectPool()
{
	for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		Delete(ObjectID);
	}
}

bool CObjectPool::New(OBJECTID ObjectID, int iModel, CVector vecPos, CVector vecRot, float fDrawDistance)
{
	if (m_pObjects[ObjectID] != nullptr) {
		Delete(ObjectID);
	}

	m_pObjects[ObjectID] = pGame->NewObject(iModel, vecPos, vecRot, fDrawDistance);
	if (!m_pObjects[ObjectID]) return false;

	m_bObjectSlotState[ObjectID] = true;
	return true;
}

bool CObjectPool::Delete(OBJECTID ObjectID)
{
	if (ObjectID < MAX_OBJECTS && m_bObjectSlotState[ObjectID])
	{
		CObject* pObject = m_pObjects[ObjectID];
		if (pObject)
		{
			delete m_pObjects[ObjectID];
			m_pObjects[ObjectID] = nullptr;
			m_bObjectSlotState[ObjectID] = false;
		}
	}

	return true;
}

void CObjectPool::Process()
{
	static uint32_t s_dwLastTick = 0;

	if (s_dwLastTick == 0) {
		s_dwLastTick = GetTickCount();
	}

	uint32_t dwThisTick = GetTickCount();
	float fElapsedTime = (dwThisTick - s_dwLastTick) / 1000.0f;

	for (OBJECTID i = 0; i < MAX_OBJECTS; i++)
	{
		if (m_bObjectSlotState[i]) {
			m_pObjects[i]->Process(fElapsedTime);
		}
	}

	s_dwLastTick = dwThisTick;
}

CObject* CObjectPool::FindObjectFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
	for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		if (m_pObjects[ObjectID] && m_pObjects[ObjectID]->m_pEntity && m_pObjects[ObjectID]->m_pEntity == pGtaObject)
			return m_pObjects[ObjectID];
	}

	return nullptr;
}

OBJECTID CObjectPool::FindIDFromGtaPtr(ENTITY_TYPE* pGtaObject)
{
	for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		if (m_pObjects[ObjectID] && m_pObjects[ObjectID]->m_pEntity && m_pObjects[ObjectID]->m_pEntity == pGtaObject)
			return ObjectID;
	}

	return INVALID_OBJECT_ID;
}

void CObjectPool::ProcessMaterialText()
{
	for (OBJECTID ObjectID = 0; ObjectID < MAX_OBJECTS; ObjectID++)
	{
		if (m_pObjects[ObjectID] && m_bObjectSlotState[ObjectID] == true)
		{
			m_pObjects[ObjectID]->ProcessMaterialText();
		}
	}
} 