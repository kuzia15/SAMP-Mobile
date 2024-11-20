#pragma once

#include <cstdint>

class CEntity
{
public:
	CEntity() {};
	virtual ~CEntity() {};
	virtual void Add();
	virtual void Remove();

	bool IsAdded();

	void GetMatrix(RwMatrix* Matrix);
	void SetMatrix(RwMatrix Matrix);
	
	void GetMoveSpeedVector(CVector* Vector);
	void SetMoveSpeedVector(CVector Vector);
	
	void GetTurnSpeedVector(CVector* Vector);
	void SetTurnSpeedVector(CVector Vector);
	
	unsigned int GetModelIndex();
	void SetModelIndex(unsigned int uiModel);

	virtual void TeleportTo(float x, float y, float z);
	float GetDistanceFromLocalPlayerPed();
	float GetDistanceFromCamera();
	float GetDistanceFromPoint(CVector Vector);

	uintptr_t GetRWObject();
	
	void SetCollisionChecking(bool bCheck);
	bool GetCollisionChecking();

	void SetGravityProcessing(bool state);

	void UpdateMatrix(RwMatrix mat);
	void UpdateRwMatrixAndFrame();

	void Render();

	RpHAnimHierarchy* GetAnimHierarchyFromSkinClump();

	ENTITY_TYPE*	m_pEntity;
	uint32_t		m_dwGTAId;

    bool IsStationary();

	bool EnforceWorldBoundries(float fPX, float fZX, float fPY, float fNY);
	bool HasExceededWorldBoundries(float fPX, float fZX, float fPY, float fNY);

    void UpdateRpHAnim();
};