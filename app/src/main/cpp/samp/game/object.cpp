#include "../main.h"
#include "game.h"
#include "../net/netgame.h"

#include "RW/RenderWare.h"

#include <cmath>

extern CGame* pGame;
extern CNetGame* pNetGame;
extern MaterialTextGenerator* pMaterialTextGenerator;

CObject::CObject(int iModel, CVector vecPos, CVector vecRot, float fDrawDistance, uint8_t bAttached)
{
	m_AttachedVehicleID = INVALID_VEHICLE_ID;
	m_AttachedObjectID = INVALID_OBJECT_ID;
	m_bAttachedToPed = bAttached;

	m_pEntity = 0;
	m_dwGTAId = 0;

	m_vecAttachedPos.x = 0.0f;
	m_vecAttachedPos.y = 0.0f;
	m_vecAttachedPos.z = 0.0f;
	m_vecAttachedRot.x = 0.0f;
	m_vecAttachedRot.y = 0.0f;
	m_vecAttachedRot.z = 0.0f;
	m_bSyncRotation = true;

	if (!IsValidModel(iModel)) {
		iModel = 18631;
	}

	uint32_t dwRetID;
	ScriptCommand(&create_object, iModel, vecPos.x, vecPos.y, vecPos.z, &dwRetID);

	ENTITY_TYPE* pEntity = GamePool_Object_GetAt(dwRetID);

	if (dwRetID && pEntity)
	{
		m_pEntity = pEntity;
		m_dwGTAId = dwRetID;

		m_byteMoving = 0;
		m_fMoveSpeed = 0.0f;
		m_bNeedRotate = false;

		m_iModel = iModel;

		GetMatrix(&m_Matrix);
		m_Matrix.pos.x = vecPos.x;
		m_Matrix.pos.y = vecPos.y;
		m_Matrix.pos.z = vecPos.z;
		SetMatrix(m_Matrix);
		SetRotation(&vecRot);
	}

	for (int i = 0; i < 16; i++)
	{
		m_MaterialTexture[i] = 0;
		m_MaterialTextTexture[i] = 0;
		m_dwMaterialColor[i] = 0;
		m_iMaterialType[i] = 0;

		/* material text */
		m_szMaterialText[i] = nullptr;
		m_iMaterialSize[i] = 0;
		m_iMaterialFontSize[i] = 0;
		m_dwMaterialFontColor[i] = 0;
		m_dwMaterialBackColor[i] = 0;
		m_iMaterialTextAlign[i] = 0;
	}
	m_bHasMaterial = false;
	m_bHasMaterialText = false;

	m_bAttachedToPed = bAttached;

	m_bForceRender = false;
}

CObject::~CObject()
{
    ENTITY_TYPE* pEntity = GamePool_Object_GetAt(m_dwGTAId);
	m_pEntity = pEntity;

	/*if(pGame->GetCamera())
	{
		if(pGame->GetCamera()->GetAttachedEntity() == this)
			pGame->GetCamera()->AttachToEntity(0);
	}*/

	if (m_pEntity && m_pEntity->vtable != (g_libGTASA + /*0x5C7358*/0x667D14)) /* CPlaceable */
	{
		ScriptCommand(&destroy_object, m_dwGTAId);
		if (GetModelRefCounts(m_iModel) == 0)
			pGame->RemoveModel(m_iModel, false);
	}

	for (int i = 0; i < 16; i++)
	{
		if (m_szMaterialText[i] != nullptr) {
			delete m_szMaterialText[i];
			m_szMaterialText[i] = nullptr;
		}
	}
}

void CObject::Process(float fElapsedTime)
{
	if (m_AttachedVehicleID != INVALID_VEHICLE_ID)
	{
		if (pNetGame)
		{
			CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
			if (pVehiclePool)
			{
				CVehicle* pVehicle = pVehiclePool->GetAt(m_AttachedVehicleID);
				if (pVehicle)
				{
					if (pVehicle->IsAdded()) {
						this->AttachToVehicle(pVehicle);
					}
				}
			}
		}
		return;
	}

	if (m_AttachedObjectID != INVALID_OBJECT_ID)
	{
		if (pNetGame)
		{
			CObjectPool* pObjectPool = pNetGame->GetObjectPool();
			if (pObjectPool)
			{
				CObject* pObject = pObjectPool->GetAt(m_AttachedObjectID);
				if (pObject) {
					this->AttachToObject(pObject);
				}
			}
		}

		return;
	}

	if (m_byteMoving & 1)
	{
		CVector vecSpeed = { 0.0f, 0.0f, 0.0f };
		RwMatrix matEnt;
		GetMatrix(&matEnt);
		float distance = fElapsedTime * m_fMoveSpeed;
		float remaining = DistanceRemaining(&matEnt);
		uint32_t dwThisTick = GetTickCount();

		float posX = matEnt.pos.x;
		float posY = matEnt.pos.y;
		float posZ = matEnt.pos.z;

		float f1 = ((float)(dwThisTick - m_dwMoveTick)) * 0.001f * m_fMoveSpeed;
		float f2 = m_fDistanceToTargetPoint - remaining;

		if (distance >= remaining)
		{
			SetMoveSpeedVector(vecSpeed);
			SetTurnSpeedVector(vecSpeed);
			matEnt.pos.x = m_matTarget.pos.x;
			matEnt.pos.y = m_matTarget.pos.y;
			matEnt.pos.z = m_matTarget.pos.z;
			if (m_bNeedRotate) {
				m_quatTarget.GetMatrix(reinterpret_cast<RwMatrix *>(&matEnt));
			}
			UpdateMatrix(matEnt);
			StopMoving();
			return;
		}

		if (fElapsedTime <= 0.0f)
			return;

		float delta = 1.0f / (remaining / distance);
		matEnt.pos.x += ((m_matTarget.pos.x - matEnt.pos.x) * delta);
		matEnt.pos.y += ((m_matTarget.pos.y - matEnt.pos.y) * delta);
		matEnt.pos.z += ((m_matTarget.pos.z - matEnt.pos.z) * delta);

		distance = remaining / m_fDistanceToTargetPoint;
		float slerpDelta = 1.0f - distance;

		delta = 1.0f / fElapsedTime;
		vecSpeed.x = (matEnt.pos.x - posX) * delta * 0.02f;
		vecSpeed.y = (matEnt.pos.y - posY) * delta * 0.02f;
		vecSpeed.z = (matEnt.pos.z - posZ) * delta * 0.02f;

		if (FloatOffset(f1, f2) > 0.1f)
		{
			if (f1 > f2)
			{
				delta = (f1 - f2) * 0.1f + 1.0f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}

			if (f2 > f1)
			{
				delta = 1.0f - (f2 - f1) * 0.1f;
				vecSpeed.x *= delta;
				vecSpeed.y *= delta;
				vecSpeed.z *= delta;
			}
		}

		SetMoveSpeedVector(vecSpeed);
		ApplyMoveSpeed();

		if (m_bNeedRotate)
		{
			float fx, fy, fz;
			GetRotation(&fx, &fy, &fz);
			distance = m_vecRotationTarget.z - distance * m_vecSubRotationTarget.z;
			vecSpeed.x = 0.0f;
			vecSpeed.y = 0.0f;
			vecSpeed.z = subAngle(remaining, distance) * 0.01f;
			if (vecSpeed.z <= 0.001f)
			{
				if (vecSpeed.z < -0.001f)
					vecSpeed.z = -0.001f;
			}
			else
			{
				vecSpeed.z = 0.001f;
			}

			SetTurnSpeedVector(vecSpeed);
			GetMatrix(&matEnt);
			CQuaternion quat;
			quat.Slerp(&m_quatStart, &m_quatTarget, slerpDelta);
			quat.Normalize();
			quat.GetMatrix(reinterpret_cast<RwMatrix *>(&matEnt));
		}
		else
		{
			GetMatrix(&matEnt);
		}

		UpdateMatrix(matEnt);
	}
}

// 0.3.7
void CObject::SetRotation(CVector * vecRotation)
{
	if (m_pEntity && GamePool_Object_GetAt(m_dwGTAId))
	{
		ScriptCommand(&set_object_rotation, m_dwGTAId, vecRotation->x, vecRotation->y, vecRotation->z);
		m_vecRotation.x = vecRotation->x;
		m_vecRotation.y = vecRotation->y;
		m_vecRotation.z = vecRotation->z;
	}
}

void CObject::InstantRotate(float x, float y, float z)
{
	if (GamePool_Object_GetAt(m_dwGTAId))
	{
        LOGI("InstantRotate: x: %f, y: %f, z: %f", x, y, z);
		ScriptCommand(&set_object_rotation, m_dwGTAId, x, y, z);
	}
}

// 0.3.7
void CObject::GetRotation(float* pfX, float* pfY, float* pfZ)
{
	if (m_pEntity) {
		RwMatrix* mat = m_pEntity->mat;
		
		if (mat) {
			// CMatrix::ConvertToEulerAngles
			((void (*)(RwMatrix*, float*, float*, float*, int))(g_libGTASA + 0x44E6AC + 1))(mat, pfX, pfY, pfZ, 21);
		}

		*pfX = *pfX * 57.295776 * -1.0;
		*pfY = *pfY * 57.295776 * -1.0;
		*pfZ = *pfZ * 57.295776 * -1.0;
	}
}
// 0.3.7
void CObject::RotateMatrix(CVector vecRot)
{
	m_vecRotation = vecRot;

	vecRot.x *= 0.017453292f; // x * pi/180
	vecRot.y *= 0.017453292f; // y * pi/180
	vecRot.z *= 0.017453292f; // z * pi/180

	float cosx = cos(vecRot.x);
	float sinx = sin(vecRot.x);

	float cosy = cos(vecRot.y);
	float siny = sin(vecRot.y);

	float cosz = cos(vecRot.z);
	float sinz = sin(vecRot.z);

	float sinzx = sinz * sinx;
	float coszx = cosz * sinx;

	m_matTarget.right.x = cosz * cosy - sinzx * siny;
	m_matTarget.right.y = coszx * siny + sinz * cosy;
	m_matTarget.right.z = -(siny * cosx);
	m_matTarget.up.x = -(sinz * cosx);
	m_matTarget.up.y = cosz * cosx;
	m_matTarget.up.z = sinx;
	m_matTarget.at.x = sinzx * cosy + cosz * siny;
	m_matTarget.at.y = sinz * siny - coszx * cosy;
	m_matTarget.at.z = cosy * cosx;
}
// 0.3.7
void CObject::ApplyMoveSpeed()
{
	if (m_pEntity)
	{
		float fTimeStep = *(float*)(g_libGTASA + 0x96B504);

		RwMatrix mat;
		GetMatrix(&mat);
		mat.pos.x += fTimeStep * m_pEntity->vecMoveSpeed.x;
		mat.pos.y += fTimeStep * m_pEntity->vecMoveSpeed.y;
		mat.pos.z += fTimeStep * m_pEntity->vecMoveSpeed.z;
		UpdateMatrix(mat);
	}
}
// 0.3.7
float CObject::DistanceRemaining(RwMatrix* matPos)
{
	float	fSX, fSY, fSZ;
	fSX = (matPos->pos.x - m_matTarget.pos.x) * (matPos->pos.x - m_matTarget.pos.x);
	fSY = (matPos->pos.y - m_matTarget.pos.y) * (matPos->pos.y - m_matTarget.pos.y);
	fSZ = (matPos->pos.z - m_matTarget.pos.z) * (matPos->pos.z - m_matTarget.pos.z);
	return (float)sqrt(fSX + fSY + fSZ);
}

void CObject::SetMaterial(int iModel, int iMaterialIndex, char* txdname, char* texturename, uint32_t dwColor)
{
	FLog("SetMaterial: model: %d, %s, %s", iModel, txdname, texturename);

	int iTryCount = 0;

	if (iMaterialIndex < 16)
	{
		if (m_MaterialTexture[iMaterialIndex]) {
			DeleteRwTexture(m_MaterialTexture[iMaterialIndex]);
			m_MaterialTexture[iMaterialIndex] = 0;
		}

		m_MaterialTexture[iMaterialIndex] = (uintptr_t)LoadTextureFromTxd(txdname, texturename);
		m_dwMaterialColor[iMaterialIndex] = dwColor;
		m_iMaterialType[iMaterialIndex] = MATERIAL_TYPE_MATERIAL;
		m_bHasMaterial = true;
		return;
	}
}

void CObject::SetMaterialText(int index, char* text, int materialSize, char* fontname, int fontSize, bool bold,
	uint32_t dwFontColor, uint32_t dwBackColor, int textAlignment)
{
	if (index > 16) return;

	if (m_MaterialTextTexture[index]) {
		DeleteRwTexture(m_MaterialTextTexture[index]);
		m_MaterialTextTexture[index] = 0;
	}

    m_MaterialTextIndex = index;

	m_dwMaterialColor[index] = 0;
	m_iMaterialType[index] = MATERIAL_TYPE_TEXT;

	if (m_szMaterialText[index] != nullptr) {
		delete[] m_szMaterialText[index];
		m_szMaterialText[index] = nullptr;
	}

	m_szMaterialText[index] = new char[2048];
	strcpy(m_szMaterialText[index], text);

	m_iMaterialSize[index] = materialSize;
	m_iMaterialFontSize[index] = fontSize;
	m_dwMaterialFontColor[index] = dwFontColor;
	m_dwMaterialBackColor[index] = dwBackColor;
	m_iMaterialTextAlign[index] = textAlignment;
}

void CObject::ProcessMaterialText()
{
	for (int i = 0; i < 16; i++)
	{
		if (m_iMaterialType[i] == MATERIAL_TYPE_TEXT && m_MaterialTextTexture[i] == 0)
		{
			m_iMaterialFontSize[i]*=0.75f;
			m_MaterialTextTexture[i] = pMaterialTextGenerator->Generate(m_szMaterialText[i], m_iMaterialSize[i], m_iMaterialFontSize[i],
				false, m_dwMaterialFontColor[i], m_dwMaterialBackColor[i], m_iMaterialTextAlign[i]);
			m_bHasMaterialText = true;
		}
	}
}

// 0.3.7
void CObject::MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ)
{
	RwMatrix mat;
	this->GetMatrix(&mat);

	if (m_byteMoving & 1) {
		this->StopMoving();
		mat.pos.x = m_matTarget.pos.x;
		mat.pos.y = m_matTarget.pos.y;
		mat.pos.z = m_matTarget.pos.z;

		if (m_bNeedRotate) {
			m_quatTarget.GetMatrix(reinterpret_cast<RwMatrix *>(&mat));
		}

		this->UpdateMatrix(mat);
	}

	m_dwMoveTick = GetTickCount();
	m_fMoveSpeed = fSpeed;
	m_matTarget.pos.x = fX;
	m_matTarget.pos.y = fY;
	m_matTarget.pos.z = fZ;
	m_byteMoving |= 1;

	if (fRotX <= -999.0f || fRotY <= -999.0f || fRotZ <= -999.0f) {
		m_bNeedRotate = false;
	}
	else
	{
		m_bNeedRotate = true;

		CVector vecRot;
		RwMatrix matrix;
		this->GetRotation(&vecRot.x, &vecRot.y, &vecRot.z);
		m_vecRotationTarget.x = fixAngle(fRotX);
		m_vecRotationTarget.y = fixAngle(fRotY);
		m_vecRotationTarget.z = fixAngle(fRotZ);

		m_vecSubRotationTarget.x = subAngle(vecRot.x, fRotX);
		m_vecSubRotationTarget.y = subAngle(vecRot.y, fRotY);
		m_vecSubRotationTarget.z = subAngle(vecRot.z, fRotZ);

		this->RotateMatrix(CVector{ fRotX, fRotY, fRotZ });
		this->GetMatrix(&matrix);
		m_quatStart.SetFromMatrix(&matrix);
		m_quatTarget.SetFromMatrix(&m_matTarget);
		m_quatStart.Normalize();
		m_quatTarget.Normalize();
	}

	m_fDistanceToTargetPoint = this->GetDistanceFromPoint(m_matTarget.pos);

	if (pNetGame) {
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool) {
			//pPlayerPool->GetLocalPlayer()->UpdateSurfing();
		}
	}

	// sub_1009F070
	m_pEntity->flags &= 0xFFFFFFF7;
}
// 0.3.7
void CObject::StopMoving()
{
	CVector vec = { 0.0f, 0.0f, 0.0f };
	this->SetMoveSpeedVector(vec);
	this->SetTurnSpeedVector(vec);

	m_byteMoving &= ~1;
}

// 0.3.7
void CObject::SetAttachedObject(uint16_t ObjectID, CVector* vecPos, CVector* vecRot, bool bSyncRotation)
{
	if (ObjectID == INVALID_OBJECT_ID)
	{
		m_AttachedObjectID = INVALID_OBJECT_ID;
		m_vecAttachedPos.x = 0.0f;
		m_vecAttachedPos.y = 0.0f;
		m_vecAttachedPos.z = 0.0f;
		m_vecAttachedRot.x = 0.0f;
		m_vecAttachedRot.y = 0.0f;
		m_vecAttachedRot.z = 0.0f;
		m_bSyncRotation = false;
	}
	else
	{
		m_AttachedObjectID = ObjectID;
		m_vecAttachedPos.x = vecPos->x;
		m_vecAttachedPos.y = vecPos->y;
		m_vecAttachedPos.z = vecPos->z;
		m_vecAttachedRot.x = vecRot->x;
		m_vecAttachedRot.y = vecRot->y;
		m_vecAttachedRot.z = vecRot->z;
		m_bSyncRotation = bSyncRotation;
	}
}
// 0.3.7
void CObject::SetAttachedVehicle(uint16_t VehicleID, CVector* vecPos, CVector* vecRot)
{
	if (VehicleID == INVALID_VEHICLE_ID)
	{
		m_AttachedVehicleID = INVALID_VEHICLE_ID;
		m_vecAttachedPos.x = 0.0f;
		m_vecAttachedPos.y = 0.0f;
		m_vecAttachedPos.z = 0.0f;
		m_vecAttachedRot.x = 0.0f;
		m_vecAttachedRot.y = 0.0f;
		m_vecAttachedRot.z = 0.0f;
	}
	else
	{
		m_AttachedVehicleID = VehicleID;
		m_vecAttachedPos.x = vecPos->x;
		m_vecAttachedPos.y = vecPos->y;
		m_vecAttachedPos.z = vecPos->z;
		m_vecAttachedRot.x = vecRot->x;
		m_vecAttachedRot.y = vecRot->y;
		m_vecAttachedRot.z = vecRot->z;
	}
} 
// 0.3.7
void CObject::AttachToVehicle(CVehicle* pVehicle)
{
	if (!ScriptCommand(&is_object_attached, m_dwGTAId)) {
		ScriptCommand(&attach_object_to_car,
			m_dwGTAId,
			pVehicle->m_dwGTAId,
			m_vecAttachedPos.x,
			m_vecAttachedPos.y,
			m_vecAttachedPos.z,
			m_vecAttachedRot.x,
			m_vecAttachedRot.y,
			m_vecAttachedRot.z);
	}
}
// 0.3.7
void CObject::AttachToObject(CObject* pObject)
{
	if (!ScriptCommand(&is_object_attached, m_dwGTAId)) {
		ScriptCommand(&attach_object_to_object,
			m_dwGTAId,
			pObject->m_dwGTAId,
			m_vecAttachedPos.x,
			m_vecAttachedPos.y,
			m_vecAttachedPos.z,
			m_vecAttachedRot.x,
			m_vecAttachedRot.y,
			m_vecAttachedRot.z);
	}
} 

bool CObject::AttachedToMovingEntity()
{
	if(m_AttachedObjectID == INVALID_OBJECT_ID)
	{
		if(m_AttachedVehicleID != INVALID_VEHICLE_ID)
			return true;

		return (m_byteMoving & 1);
	}
	else
	{
		if(m_AttachedObjectID >= 0 && m_AttachedObjectID < MAX_OBJECTS)
		{
			if(pNetGame)
			{
				CObjectPool *pObjectPool = pNetGame->GetObjectPool();
				if(pObjectPool)
				{
					CObject *pObject = pObjectPool->GetAt(m_AttachedObjectID);
					if(pObject) return (pObject->m_byteMoving & 1);
				}
			}
		}
	}

	return false;
}

void CObject::TeleportTo(float fX, float fY, float fZ) {

	/*if (fX > 3000.0f || fX < -3000.0f ||
		fY > 3000.0f || fY < -3000.0f) {
		m_bForceRender = true;
	}
	else {
		m_bForceRender = false;
	}*/

	int v4 = 0;
	int v5 = 0;
	if ( fX < -3000.0 )
		v4 = 1;
	if ( fX > 3000.0 )
		v5 = 1;

	m_bForceRender = (fY > 3000.0) | v4 | v5 | (fY < -3000.0);

	CEntity::TeleportTo(fX, fY, fZ);
}