#pragma once

#define INVALID_OBJECT_ID	0xFFFF

class CObjectPool
{
public:
	CObjectPool();
	~CObjectPool();

	bool New(OBJECTID ObjectID, int iModel, CVector vecPos, CVector vecRot, float fDrawDistance);
	bool Delete(OBJECTID ObjectID);

	void Process();

	// 0.3.7
	CObject* GetAt(OBJECTID ObjectID) {
		if (ObjectID <= MAX_OBJECTS && m_bObjectSlotState[ObjectID])
			return m_pObjects[ObjectID];
		
		return nullptr;
	}

	OBJECTID FindIDFromGtaPtr(ENTITY_TYPE* pGtaObject);

	CObject* FindObjectFromGtaPtr(ENTITY_TYPE* pGtaObject);

	void ProcessMaterialText();

private:
	int			m_iObjectCount;
	bool		m_bObjectSlotState[MAX_OBJECTS];
	CObject*	m_pObjects[MAX_OBJECTS];
};