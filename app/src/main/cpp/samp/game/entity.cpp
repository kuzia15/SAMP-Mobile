#include "../main.h"
#include "game.h"
#include "../net/netgame.h"
#include <cmath>

extern CGame* pGame;
extern CNetGame *pNetGame;

// 0.3.7
void CEntity::GetMatrix(RwMatrix* Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	Matrix->right.x = m_pEntity->mat->right.x;
	Matrix->right.y = m_pEntity->mat->right.y;
	Matrix->right.z = m_pEntity->mat->right.z;

	Matrix->up.x = m_pEntity->mat->up.x;
	Matrix->up.y = m_pEntity->mat->up.y;
	Matrix->up.z = m_pEntity->mat->up.z;

	Matrix->at.x = m_pEntity->mat->at.x;
	Matrix->at.y = m_pEntity->mat->at.y;
	Matrix->at.z = m_pEntity->mat->at.z;

	Matrix->pos.x = m_pEntity->mat->pos.x;
	Matrix->pos.y = m_pEntity->mat->pos.y;
	Matrix->pos.z = m_pEntity->mat->pos.z;
}
// 0.3.7
void CEntity::SetMatrix(RwMatrix Matrix)
{
	if (!m_pEntity || !m_pEntity->mat) return;

	m_pEntity->mat->right.x = Matrix.right.x;
	m_pEntity->mat->right.y = Matrix.right.y;
	m_pEntity->mat->right.z = Matrix.right.z;

	m_pEntity->mat->up.x = Matrix.up.x;
	m_pEntity->mat->up.y = Matrix.up.y;
	m_pEntity->mat->up.z = Matrix.up.z;

	m_pEntity->mat->at.x = Matrix.at.x;
	m_pEntity->mat->at.y = Matrix.at.y;
	m_pEntity->mat->at.z = Matrix.at.z;

	m_pEntity->mat->pos.x = Matrix.pos.x;
	m_pEntity->mat->pos.y = Matrix.pos.y;
	m_pEntity->mat->pos.z = Matrix.pos.z;
}
// 0.3.7
void CEntity::GetMoveSpeedVector(CVector* Vector)
{
	Vector->x = m_pEntity->vecMoveSpeed.x;
	Vector->y = m_pEntity->vecMoveSpeed.y;
	Vector->z = m_pEntity->vecMoveSpeed.z;
}
// 0.3.7
void CEntity::SetMoveSpeedVector(CVector Vector)
{
	m_pEntity->vecMoveSpeed.x = Vector.x;
	m_pEntity->vecMoveSpeed.y = Vector.y;
	m_pEntity->vecMoveSpeed.z = Vector.z;
}
// 0.3.7
void CEntity::GetTurnSpeedVector(CVector* Vector)
{
	Vector->x = m_pEntity->vecTurnSpeed.x;
	Vector->y = m_pEntity->vecTurnSpeed.y;
	Vector->z = m_pEntity->vecTurnSpeed.z;
}
// 0.3.7
void CEntity::SetTurnSpeedVector(CVector Vector)
{
	m_pEntity->vecTurnSpeed.x = Vector.x;
	m_pEntity->vecTurnSpeed.y = Vector.y;
	m_pEntity->vecTurnSpeed.z = Vector.z;
}
// 0.3.7
uint CEntity::GetModelIndex()
{
	return m_pEntity->nModelIndex;
}
// 0.3.7
void CEntity::SetModelIndex(uint uiModel)
{
	if (!m_pEntity) return;

	int iTryCount = 0;
	if (!pGame->IsModelLoaded(uiModel) && !GetModelRWObject(uiModel))
	{
		pGame->RequestModel(uiModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(uiModel))
		{
			sleep(1);
			if (iTryCount > 200)
			{
				//if (gui) gui->chat()->addDebugMessage("Warning: Model %u wouldn't load in time!", uiModel);
				return;
			}

			iTryCount++;
		}
	}


	// CEntity::DeleteRWObject
	((void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable + 36)))(m_pEntity);
	m_pEntity->nModelIndex = uiModel;
	// CEntity::SetModelIndex
	((void (*)(ENTITY_TYPE*, unsigned int))(*(void**)(m_pEntity->vtable + 24)))(m_pEntity, uiModel);
}
// 0.3.7
void CEntity::TeleportTo(float fX, float fY, float fZ)
{
	RwMatrix mat;

	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + (VER_x32 ? 0x667D14 : 0x830098))) /* CPlaceable */
	{
		uint16_t modelIndex = m_pEntity->nModelIndex;
		if (modelIndex != TRAIN_PASSENGER_LOCO &&
			modelIndex != TRAIN_FREIGHT_LOCO &&
			modelIndex != TRAIN_TRAM)
			//((void(*)(ENTITY_TYPE*, float, float, float, bool))(*(void**)(m_pEntity->vtable + 0x3C)))(m_pEntity, fX, fY, fZ, 0);
		{
            LOGI("TeleportTo 0x%llx", m_pEntity->vtable + 0x3C);
			((void(*)(ENTITY_TYPE*, float, float, float))(*(void**)(m_pEntity->vtable + 0x3C)))(m_pEntity, fX, fY, fZ);
		}
		else
			ScriptCommand(&put_train_at, m_dwGTAId, fX, fY, fZ);
	}
}
// 0.3.7
bool CEntity::IsAdded()
{
	if (m_pEntity)
	{
		if (m_pEntity->vtable == (g_libGTASA + /*0x5C7358*/0x667D14)) // CPlaceable
			return false;

		if (m_pEntity->dwUnkModelRel)
			return true;
	}

	return false;
}
// 0.3.7
void CEntity::Add()
{
	if (!m_pEntity || m_pEntity->vtable == (g_libGTASA + /*0x5C7358*/0x667D14)) { // CPlaceable
		return;
	}

	if (!m_pEntity->dwUnkModelRel) {

		CVector vec = { 0.0f, 0.0f, 0.0f };

		SetMoveSpeedVector(vec);
		SetTurnSpeedVector(vec);

		// CWorld::Add
		((void(*)(ENTITY_TYPE*))(g_libGTASA + /*0x3C14B0*/0x4233C8 + 1))(m_pEntity);

		RwMatrix mat;
		GetMatrix(&mat);
		TeleportTo(mat.pos.x, mat.pos.y, mat.pos.z);
	}
}
// 0.3.7
void CEntity::Remove()
{
	if (!m_pEntity || m_pEntity->vtable == (g_libGTASA + /*0x5C7358*/0x667D14)) { // CPlaceable
		return;
	}

	if (m_pEntity->dwUnkModelRel) {
		// CWorld::Remove
		((void(*)(ENTITY_TYPE*))(g_libGTASA + /*0x3C1500*/0x4232BC + 1))(m_pEntity);
	}
}

float CEntity::GetDistanceFromCamera()
{
	RwMatrix matEnt;
	if (!m_pEntity || m_pEntity->vtable == 0x667D14) // CPlaceable
		return 100000.0f;

	GetMatrix(&matEnt);

	float tmpX = (matEnt.pos.x - *(float*)(g_libGTASA + 0x9528D4));
	float tmpY = (matEnt.pos.y - *(float*)(g_libGTASA + 0x9528D8));
	float tmpZ = (matEnt.pos.z - *(float*)(g_libGTASA + 0x9528DC));

	return sqrt(tmpX * tmpX + tmpY * tmpY + tmpZ * tmpZ);
}

// 0.3.7
float CEntity::GetDistanceFromPoint(CVector Vector)
{
	RwMatrix mat;
	GetMatrix(&mat);

	float tmpX = (mat.pos.x - Vector.x) * (mat.pos.x - Vector.x);
	float tmpY = (mat.pos.y - Vector.y) * (mat.pos.y - Vector.y);
	float tmpZ = (mat.pos.z - Vector.z) * (mat.pos.z - Vector.z);

	return (float)sqrt(tmpX + tmpY + tmpZ);
}

// 0.3.7
uintptr_t CEntity::GetRWObject()
{
	if (m_pEntity)
		return m_pEntity->pRwObject;

	return 0;
}
// 0.3.7
float CEntity::GetDistanceFromLocalPlayerPed()
{
	RwMatrix	matFromPlayer;
	RwMatrix	matThis;
	float		fSX, fSY, fSZ;

	CPlayerPed* pLocalPlayerPed = pGame->FindPlayerPed();
	CLocalPlayer* pLocalPlayer = nullptr;

	if (!pLocalPlayerPed) return 10000.0f;
	if (!m_pEntity) return 10000.0f;

	GetMatrix(&matThis);

	if (pNetGame) {
		pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if (pLocalPlayer && (pLocalPlayer->IsSpectating() || pLocalPlayer->IsInRCMode())) {
            CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
			TheCamera.GetMatrix(&matFromPlayer);
		}
		else {
			pLocalPlayerPed->GetMatrix(&matFromPlayer);
		}
	}
	else {
		pLocalPlayerPed->GetMatrix(&matFromPlayer);
	}

	fSX = (matThis.pos.x - matFromPlayer.pos.x) * (matThis.pos.x - matFromPlayer.pos.x);
	fSY = (matThis.pos.y - matFromPlayer.pos.y) * (matThis.pos.y - matFromPlayer.pos.y);
	fSZ = (matThis.pos.z - matFromPlayer.pos.z) * (matThis.pos.z - matFromPlayer.pos.z);

	return (float)sqrt(fSX + fSY + fSZ);
}

void CEntity::SetCollisionChecking(bool bCheck)
{
	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + /*0x5C7358*/0x667D14))
	{
		if (bCheck)
			m_pEntity->dwProcessingFlags |= 1;
		else
			m_pEntity->dwProcessingFlags &= 0xFFFFFFFE;
	}
}

bool CEntity::GetCollisionChecking()
{
	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + /*0x5C7358*/0x667D14))
		return m_pEntity->dwProcessingFlags & 1;

	return true;
}

void CEntity::SetGravityProcessing(bool state)
{
	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + /*0x5C7358*/0x667D14))
	{
		if (state)
			m_pEntity->dwProcessingFlags &= 0x7FFFFFFD;
		else
			m_pEntity->dwProcessingFlags |= 0x80000002;
	}
}
// 0.3.7
void CEntity::UpdateMatrix(RwMatrix matrix)
{
	if (m_pEntity && m_pEntity->mat)
	{
		// CPhysical::Remove
		((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x10)))(m_pEntity);

		this->SetMatrix(matrix);
		this->UpdateRwMatrixAndFrame();

		// CPhysical::Add
		((void (*)(ENTITY_TYPE*))(*(uintptr_t*)(m_pEntity->vtable + 0x8)))(m_pEntity);
	}
}

// 0.3.7
void CEntity::UpdateRwMatrixAndFrame()
{
	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + (VER_x32 ? 0x667D14 : 0x830098))) // CPlaceable
	{
		if (m_pEntity->pRwObject)
		{
			if (m_pEntity->mat)
			{
				uintptr_t pRwMatrix = *(uintptr_t*)(m_pEntity->pRwObject + 4) + 0x10;

				// CMatrix::UpdateRwMatrix
				((void (*)(RwMatrix*, uintptr_t))(g_libGTASA + 0x44EDEE + 1))(m_pEntity->mat, pRwMatrix);

				// CEntity::UpdateRwFrame
				((void (*)(ENTITY_TYPE*))(g_libGTASA + 0x3EBFE8 + 1))(m_pEntity);
			}
		}
	}
}
// 0.3.7
void CEntity::Render()
{
	if(!m_pEntity && IsGameEntityArePlaceable(m_pEntity))
		return;

	uintptr_t pRwObject = GetRWObject();

	int iModel = GetModelIndex();
	if (iModel >= 400 && iModel <= 611 && pRwObject) {
		// CVisibilityPlugins::SetupVehicleVariables
		((void (*)(uintptr_t))(g_libGTASA + 0x5D4B40 + 1))(pRwObject);
	}

	// CEntity::PreRender
	((void (*)(ENTITY_TYPE*))(*(void**)(m_pEntity->vtable + 0x48)))(m_pEntity);

	if (pRwObject) {
		// CRenderer::RenderOneNonRoad
		((void (*)(ENTITY_TYPE*))(g_libGTASA + 0x4102BC + 1))(m_pEntity);
	}
}

RpHAnimHierarchy* CEntity::GetAnimHierarchyFromSkinClump() 
{
	return ((RpHAnimHierarchy* (*)(uint32_t))(g_libGTASA+0x5D1021))(*(uint32_t *)(*((uint32_t *)this+1) + 24));
}

void CEntity::UpdateRpHAnim()
{
	if(!m_pEntity) return;

	((void (*)(uint32_t))(g_libGTASA + 0x3EBFF6 + 1))(*((uint32_t *)this+1));
}

bool CEntity::IsStationary()
{
	if(!IsAdded()) return false; // movespeed vectors are invalid if its not added

	if(m_pEntity->vecMoveSpeed.x == 0.0f &&
	   m_pEntity->vecMoveSpeed.y == 0.0f &&
	   m_pEntity->vecMoveSpeed.z == 0.0f)
	{
		return true;
	}
	return false;
}

bool CEntity::EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	if(!m_pEntity) return false;

	RwMatrix matWorld;
	CVector vecMoveSpeed;
	GetMatrix(&matWorld);
	GetMoveSpeedVector(&vecMoveSpeed);

	if(matWorld.pos.x > fPX)
	{
		if(vecMoveSpeed.x != 0.0f)
		{
			vecMoveSpeed.x = -0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		SetMatrix(matWorld);
		return true;
	}
	else if(matWorld.pos.x < fZX)
	{
		if(vecMoveSpeed.x != 0.0f)
		{
			vecMoveSpeed.x = 0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		SetMatrix(matWorld);
		return true;
	}
	else if(matWorld.pos.y > fPY)
	{
		if(vecMoveSpeed.y != 0.0f)
		{
			vecMoveSpeed.y = -0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		SetMatrix(matWorld);
		return true;
	}
	else if(matWorld.pos.y < fNY)
	{
		if(vecMoveSpeed.y != 0.0f)
		{
			vecMoveSpeed.y = 0.2f;
			vecMoveSpeed.z = 0.1f;
		}

		SetMoveSpeedVector(vecMoveSpeed);
		matWorld.pos.z += 0.04f;
		SetMatrix(matWorld);
		return true;
	}

	return false;
}

bool CEntity::HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY)
{
	if(m_pEntity)
	{
		RwMatrix matWorld;
		GetMatrix(&matWorld);

		if(matWorld.pos.x > fPX || matWorld.pos.x < fZX || matWorld.pos.y > fPY || matWorld.pos.y < fNY)
			return true;
	}

	return false;
}