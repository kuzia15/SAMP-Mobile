#pragma once

#include "../game/Core/Quaternion.h"

#define MATERIAL_TYPE_MATERIAL	1
#define MATERIAL_TYPE_TEXT		2

class CObject : public CEntity
{
public:
	CObject(int iModel, CVector vecPos, CVector vecRot, float fDrawDistance, uint8_t unk);
	virtual ~CObject();

	void Process(float fElapsedTime);

	virtual void TeleportTo(float fX, float fY, float fZ) override;

	void SetRotation(CVector *vecRotation);
	void GetRotation(float* pfX, float* pfY, float* pfZ);
	void RotateMatrix(CVector vecRot);
	void ApplyMoveSpeed();
	float DistanceRemaining(RwMatrix* matPos);

	void SetMaterial(int iModel, int iIndex, char* txdname, char* texturename, uint32_t dwColor);
	void SetMaterialText(int iIndex, char* text, int materialSize, char* fontname, int fontSize, bool bold,
		uint32_t dwFontColor, uint32_t dwBackColor, int textAlignment);

	void MoveTo(float fX, float fY, float fZ, float fSpeed, float fRotX, float fRotY, float fRotZ);
	void StopMoving();

	void SetAttachedObject(uint16_t ObjectID, CVector* vecPos, CVector* vecRot, bool bSyncRotation);
	void SetAttachedVehicle(uint16_t VehicleID, CVector* vecPos, CVector* vecRot);

	void AttachToVehicle(CVehicle* pVehicle);
	void AttachToObject(CObject* pObject);

	void ProcessMaterialText();

	bool AttachedToMovingEntity();

	/* attach */
	uint16_t	m_AttachedVehicleID;
	uint16_t	m_AttachedObjectID;
	CVector		m_vecAttachedPos;
	CVector		m_vecAttachedRot;
	bool		m_bSyncRotation;

	int	m_MaterialTextIndex;
	uintptr_t	m_MaterialTexture[16];
	uint32_t	m_dwMaterialColor[16];
	uintptr_t	m_MaterialTextTexture[16];
	int			m_iMaterialType[16];
	bool		m_bHasMaterial;
	bool		m_bHasMaterialText;
	/* materialText */
	char* m_szMaterialText[16];
	int			m_iMaterialSize[16];
	int			m_iMaterialFontSize[16];
	uint32_t	m_dwMaterialFontColor[16];
	uint32_t	m_dwMaterialBackColor[16];
	int			m_iMaterialTextAlign[16];

	bool		m_bAttachedToPed;
	bool		m_bForceRender;

    uint8_t		m_byteMoving;

	void InstantRotate(float x, float y, float z);

private:
	RwMatrix	m_Matrix;
	CVector		m_vecRotation;
	int			m_iModel;

    float		m_fMoveSpeed;
	bool		m_bNeedRotate;
	RwMatrix	m_matTarget;
	CQuaternion m_quatStart;
	CQuaternion m_quatTarget;
	CVector		m_vecRotationTarget;
	CVector		m_vecSubRotationTarget;
	float		m_fDistanceToTargetPoint;
	uint32_t	m_dwMoveTick;

};