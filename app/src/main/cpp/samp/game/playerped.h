#pragma once

#include "vehicle.h"
#include "object.h"
#include "game/Entity/CPedGTA.h"
#include "aimstuff.h"

enum eStuffType {
	STUFF_TYPE_NONE,
	STUFF_TYPE_BEER,
	STUFF_TYPE_DYN_BEER,
	STUFF_TYPE_PINT_GLASS,
	STUFF_TYPE_CIGGI
};

#define PLAYER_PED_SLOTS	120

#pragma pack(push, 1)
typedef struct _BULLET_DATA
{
	CVector vecOrigin;
	CVector vecPos;
	CVector vecOffset;
	CEntityGTA* pEntity;
} BULLET_DATA;
#pragma pack(pop)

struct CAttachedPlayerObject
{
    uint32_t 	    dwModelId;
    uint32_t 	    dwBone;
    CVector 	    vecOffset;
    CVector 	    vecRotation;
    CVector 	    vecScale;
    CRGBA 	        dwColor[2];

    class CObject*  pObject;
    uint32_t 	    dwSampBone;
};

#pragma pack(push, 1)
struct ATTACHED_OBJECT_INFO
{
    uint32_t 	dwModelId;
    uint32_t 	dwBoneId_MP;
    CVector 	vecOffset;
    CVector 	vecRotation;
    CVector 	vecScale;
    uint32_t 	dwColor[2];
};
#pragma pack(pop)
VALIDATE_SIZE(ATTACHED_OBJECT_INFO, 52);

class CPlayerPed
{
public:
	CPlayerPed();
	CPlayerPed(int iNum, int iSkin, float fX, float fY, float fZ, float fRotation);
	virtual ~CPlayerPed();

	bool IsInVehicle();
	bool IsAPassenger();
	void RemoveFromVehicleAndPutAt(float fX, float fY, float fZ);
	void RestartIfWastedAt(CVector *vecRestart, float fRotation);

	void ExtinguishFire();
	void TogglePlayerControllable(bool bControllable);
	void DrunkProcess();
	void RemoveWeaponWhenEnteringVehicle();

	uint8_t GetActionTrigger();
	void SetActionTrigger(uint8_t action);
    CVehicleGTA* GetGtaVehicle();
	void SetDead();
	bool IsDead();
	void SetHealth(float fHealth);
	float GetHealth();
	void SetArmour(float fArmour);
	float GetArmour();
	void SatisfyHunger();
	uint32_t GetCurrentAnimationIndex();
	bool IsPlayingAnimation(int iIndex);
	void PlayAnimationFromIndex(int iIndex, float fDelta);
	uint8_t GetCurrentWeapon();
	void SetInitialState();
	void SetModelIndex(uint uiModel);
	void ClearWeapons();
	void ResetDamageEntity();
	void GiveWeapon(int iWeaponId, int iAmmo);
	void SetArmedWeapon(uint8_t weapon, bool unk);
	void SetTargetRotation(float fRotation);
	void SetImmunities(int BP, int FP, int EP, int CP, int MP);
	void ShowMarker(int nIndex);
	void SetFightingStyle(int iStyle);
	void SetRotation(float fRotation);
	void DestroyFollowPedTask();
	void GetBonePosition(int iBoneID, CVector* vecOut);
	void GetTransformedBonePosition(int iBoneID, CVector* vecOut);
	void ApplyAnimation(const char* szAnimName, const char* szAnimLib, float fT, int opt1, int opt2, int opt3, int opt4, int iTime);
	void SetInterior(uint8_t byteInteriorId, bool bRefresh);
	void PutDirectlyInVehicle(uint32_t dwVehicleGTAId, uint8_t byteSeatID);
	void EnterVehicle(uint32_t dwVehicleGtaId, bool bPassenger);
	void ExitCurrentVehicle();
	int GetCurrentVehicleID();
	void SetSkillLevel(int iSkillID, int iLevel);
	void SetAmmo(uint8_t byteWeapon, uint16_t wAmmo);
    CWeapon* FindWeaponSlot(uint8_t byteWeapon);
	int GetVehicleSeatID();

	uint32_t GetStateFlags();
	void SetStateFlags(uint32_t dwState);
	bool IsOnGround();

	// 0.3.7
	bool IsCuffed() { return m_iCuffedState; };
	// 0.3.7
	bool IsCarry() { return m_iCarryState; }

	void SetCuffedOrCarry(int cuff, int carry)
	{
		m_iCuffedState = cuff;
		m_iCarryState = carry;
	}

	CAMERA_AIM* GetCurrentAim();	
	void SetCurrentAim(CAMERA_AIM* pAim);

	uint8_t GetCameraMode();
	void SetCameraMode(uint8_t byteCameraMode);

	float GetCameraExtendedZoom() { return GameGetLocalPlayerCameraExtZoom(); }
	void SetCameraZoomAndAspect(float fExtZoom, float fAspectRatio);

	float GetAimZ();
	void SetAimZ(float fAimZ);

    CWeapon* GetCurrentWeaponSlot();

	void SetKeys(uint16_t lrAnalog, uint16_t udAnalog, uint16_t wKeys);
	uint16_t GetKeys(uint16_t *lrAnalog, uint16_t *udAnalog, bool clear = true);

	void CheckVehicleParachute();
	void ProcessVehicleHorn();

	void GetBoneMatrix(RwMatrix* matOut, int iBoneID);

	void ClumpUpdateAnimations(float step, int flag);

	void FireInstant();
	void GetWeaponInfoForFire(bool bLeftWrist, CVector* vecBonePos, CVector* vecOut);
	CVector* GetCurrentWeaponFireOffset();
	void ProcessBulletData(BULLET_DATA* btData);

    CEntityGTA* GetEntityUnderPlayer();

	CVehicle* GetCurrentVehicle();

	bool IsCrouching();
	void ApplyCrouch();
	void ResetCrouch();

	int IsCellphoneEnabled() { return m_iCellPhoneEnabled; };
	void ToggleCellphone(int iOn);

	bool IsInJetpackMode();
	void StartJetpack();
	void StopJetpack();

	int HasHandsUp();
	void HandsUp();

	int GetDanceStyle() { return m_iDanceStyle; };
	void StartDancing(int danceId);
	void StopDancing();
	void ProcessDancing();
	char *GetDanceAnimForMove(int iMove);

	bool IsPissing() { return m_bPissingState; };
	void StartPissing();
	void StopPissing();

	eStuffType GetStuff();
	bool ApplyStuff();
	void GiveStuff(eStuffType type);
	void DropStuff();
	
	uint32_t GetDrunkLevel() { return m_stuffData.dwDrunkLevel; };
	void SetDrunkLevel(uint32_t dwLevel);
	void ProcessDrunk();

	uint8_t FindDeathReasonAndResponsiblePlayer(uint16_t *nPlayer);

	int GetPedStat();

	bool IsPerformingCustomAnim();

	uint8_t IsEnteringVehicle();
	bool IsExitingVehicle();
	bool IsJumpTask();
	bool IsTakeDamageFallTask();
	bool IsSitTask();

    CVehicleGTA* GetGtaContactVehicle();
    CEntityGTA* GetGtaContactEntity();

	void ClearAllTasks();

	void SetPlayerSpecialAction(int iAction);
	void ProcessSpecialAction(int iAction);

	bool IsAnimationPlaying(char* szAnimName);

	void ProcessCuffAndCarry();

	void SetCurrentWeapon(uint8_t weaponType);

    void AttachObject(ATTACHED_OBJECT_INFO* pInfo, int iSlot);
    void SetAttachOffset(int iSlot, CVector pos, CVector rot);
    void DeattachObject(int iSlot);
    bool IsHasAttach();
    void FlushAttach();
    void ProcessAttach();

    void ProcessHeadMatrix();
    bool IsValidAttach(int iSlot);

    CAttachedPlayerObject* GetAttachedObject(uint32 id)
    {
        auto it = m_aAttachedObject.find(id);
        return it != m_aAttachedObject.end() ? &it->second : nullptr;
    }

    RwMatrix m_HeadBoneMatrix;
    std::unordered_map<int, CAttachedPlayerObject> m_aAttachedObject;

	int m_iCuffedState;
	int m_iCarryState;
    int iSpecialAction;
private:

	bool m_bHaveBulletData;
	BULLET_DATA m_bulletData;

	struct {
		eStuffType type;
		uint32_t dwObject;
		uint32_t dwDrunkLevel;
		uint32_t dwLastUpdateTick;
	} m_stuffData;

	struct
	{
		int boneId;
		RtQuat* quat;
	} sCarryData[21];
	

public:
	CPedGTA*	m_pPed;
	uint8_t		m_bytePlayerNumber;
	uint32_t	m_dwArrow;
	uint32_t	m_dwParachuteObject;

	int			m_iCellPhoneEnabled;
	int			m_iDanceStyle;
	int			m_iLastDanceMove;
	int			m_iDancingState;
	int			m_iDancingAnim;

	int			m_bPissingState;
	uint32_t	m_dwPissParticlesHandle;

	float GetDistanceFromVehicle(CVehicle *pVehicle);

    uint8_t GetAdditionalKeys();

	void StopPassengerDriveByMode();

	bool StartPassengerDriveByMode();

	bool IsInPassengerDriveByMode();

    void SetWeaponSkill(uint32_t iWeaponType, uint16_t byteSkill);

    uintptr		m_dwGTAId;
};