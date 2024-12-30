#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../audiostream.h"

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CAudioStream* pAudioStream;

// 0.3.7
void ScrSetGravity(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fGravity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fGravity);
	
	pGame->SetGravity(fGravity);

	return;
}
// 0.3.7
void ScrSetCameraPos(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

    CCamera::SetPosition(fX, fY, fZ, 0.0f, 0.0f, 0.0f);

	return;
}
// 0.3.7
void ScrSetCameraLookAt(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	uint8_t byteType;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(byteType);
	if (byteType < 1 || byteType > 2) {
		byteType = 2;
	}

	CCamera::LookAtPoint(fX, fY, fZ, byteType);

	return;
}
// 0.3.7
void ScrInterpolateCamera(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool mode;
	CVector vecFrom;
	CVector vecTo;
	int iTime;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(mode);
	bsData.Read(vecFrom.x);
	bsData.Read(vecFrom.y);
	bsData.Read(vecFrom.z);
	bsData.Read(vecTo.x);
	bsData.Read(vecTo.y);
	bsData.Read(vecTo.z);
	bsData.Read(iTime);
	bsData.Read(byteMode);

	if (byteMode < 1 || byteMode > 2) {
		byteMode = 2;
	}

	if (iTime > 0) {
		pNetGame->GetPlayerPool()->GetLocalPlayer()->m_bSpectateProcessed = true;
		if (mode) {
            CCamera::InterpolateCameraPos(&vecFrom, &vecTo, iTime, byteMode);
		}
		else {
            CCamera::InterpolateCameraLookAt(&vecFrom, &vecTo, iTime, byteMode);
		}
	}

	return;
}
// 0.3.7
void ScrTogglePlayerSpectating(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t dwToggle; 
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(dwToggle);

	FLog("TogglePlayerSpectating: %d", dwToggle);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->ToggleSpectating(dwToggle);

	return;
}
// 0.3.7
void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYER_SPAWN_INFO spawnInfo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read((char*)&spawnInfo, sizeof(PLAYER_SPAWN_INFO));

	pNetGame->GetPlayerPool()->GetLocalPlayer()->SetSpawnInfo(&spawnInfo);

	return;
}
// 0.3.7
void ScrAddGangZone(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wZoneID;
	float minX, minY, maxX, maxY;
	uint32_t dwColor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		bsData.Read(wZoneID);
		bsData.Read(minX);
		bsData.Read(minY);
		bsData.Read(maxX);
		bsData.Read(maxY);
		bsData.Read(dwColor);
		pGangZonePool->New(wZoneID, minX, minY, maxX, maxY, dwColor);
	}
}
// 0.3.7
void ScrGangZoneDestroy(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->Delete(wZoneID);
	}
}
// 0.3.7
void ScrGangZoneFlash(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(dwColor);
		pGangZonePool->Flash(wZoneID, dwColor);
	}
}
// 0.3.7
void ScrGangZoneStopFlash(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->StopFlash(wZoneID);
	}
}

int iTotalObjects = 0;

void ScrCreateObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	int iModel;
	CVector vecPos;
	CVector vecRot;
	float fDrawDistance;
	uint8_t byteNoCameraCol;
	OBJECTID AttachedObjectID;
	VEHICLEID AttachedVehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(iModel);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(vecRot.x);
	bsData.Read(vecRot.y);
	bsData.Read(vecRot.z);
	bsData.Read(fDrawDistance);
	bsData.Read(byteNoCameraCol);
	bsData.Read(AttachedVehicleID);
	bsData.Read(AttachedObjectID);

	CVector vecAttachOffset;
	CVector vecAttachRot;
	uint8_t bSyncRotation;

	if (AttachedObjectID != INVALID_OBJECT_ID || AttachedVehicleID != INVALID_VEHICLE_ID)
	{
		bsData.Read(vecAttachOffset.x);
		bsData.Read(vecAttachOffset.y);
		bsData.Read(vecAttachOffset.z);
		bsData.Read(vecAttachRot.x);
		bsData.Read(vecAttachRot.y);
		bsData.Read(vecAttachRot.z);
		bsData.Read(bSyncRotation);
	}

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->New(ObjectID, iModel, vecPos, vecRot, fDrawDistance);

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (AttachedObjectID != INVALID_OBJECT_ID)
	{
		if (pObject) {
			pObject->SetAttachedObject(AttachedObjectID, &vecAttachOffset, &vecAttachRot, bSyncRotation);
		}
	}
	else if (AttachedVehicleID != INVALID_VEHICLE_ID)
	{
		if (pObject) {
			pObject->SetAttachedVehicle(AttachedVehicleID, &vecAttachOffset, &vecAttachRot);
		}
	}

	uint8_t byteMaterialsCount;
	bsData.Read(byteMaterialsCount);
	if (byteMaterialsCount > 0)
	{
		char txdname[256];
		char texturename[256];
		uint8_t byteType;
		uint8_t byteMaterialIndex;
		uint16_t MaterialModel;
		uint8_t byteLength;
		uint32_t dwColor;

		// Material Text
		uint8_t byteMaterialSize;
		uint8_t byteFontNameLength;
		char szFontName[32];
		uint8_t byteFontSize;
		uint8_t byteFontBold;
		uint32_t dwFontColor;
		uint32_t dwBackgroundColor;
		uint8_t byteAlign;
		char szText[2048];

		bsData.Read(byteType);

		if (byteType == 1) // material
		{
			bsData.Read(byteMaterialIndex);
			bsData.Read(MaterialModel);
			bsData.Read(byteLength);
			bsData.Read(txdname, byteLength);
			txdname[byteLength] = '\0';
			bsData.Read(byteLength);
			bsData.Read(texturename, byteLength);
			texturename[byteLength] = '\0';
			bsData.Read(dwColor);

			if (strlen(txdname) < 32 && strlen(texturename) < 32)
			{
				if (MaterialModel == 0xFFFF || MaterialModel > 20000)
					MaterialModel = 0xFFFF;

				CObject* pObject = pObjectPool->GetAt(ObjectID);
				if (pObject)
					pObject->SetMaterial(MaterialModel, byteMaterialIndex, txdname, texturename, dwColor);
			}
		}
		else if (byteType == 2) // material text
		{
			bsData.Read(byteMaterialIndex);
			bsData.Read(byteMaterialSize);
			bsData.Read(byteFontNameLength);
			bsData.Read(szFontName, byteFontNameLength);
			szFontName[byteFontNameLength] = '\0';
			bsData.Read(byteFontSize);
			bsData.Read(byteFontBold);
			bsData.Read(dwFontColor);
			bsData.Read(dwBackgroundColor);
			bsData.Read(byteAlign);
			stringCompressor->DecodeString(szText, 2048, &bsData);

			if(strlen(szFontName) <= 32)
			{
				if(pObject)
				{
					pObject->SetMaterialText(byteMaterialIndex, szText, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign);
				}
			}
		}
	}


	iTotalObjects++;
	//LOGI("CreateObject: model %d; Total objects: %d", iModel, iTotalObjects);
	//MyLog2("CreateObject: model %d; Total objects: %d", iModel, iTotalObjects);
	//MyLog2("CreateObject: id: %d model: %d x: %f y: %f z: %f", iTotalObjects, iModel, vecPos.x, vecPos.y, vecPos.z);
}

void ScrDestroyObject(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	iTotalObjects--;
	//LOGI("DestroyObject; Total objects: %d", iTotalObjects);

	CObjectPool *pObjectPool = pNetGame->GetObjectPool();
	pObjectPool->Delete(ObjectID);
}

void ScrSetObjectMaterial(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	OBJECTID ObjectID;
	uint8_t byteMaterialType;
	uint8_t byteMaterialIndex;
	uint16_t wModelID;
	uint8_t byteLength;
	char txdname[256], texname[256], fontname[256];
	uint32_t dwColor;
	uint8_t byteMaterialSize;
	uint8_t byteFontSize;
	uint8_t byteBold;
	uint32_t dwFontColor;
	uint32_t dwBackColor;
	uint8_t byteTextAlignment;
	char text[2048];

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	CObject* pObject = pObjectPool->GetAt(ObjectID);

	bsData.Read(byteMaterialType);
	if (byteMaterialType == 1)
	{
		bsData.Read(byteMaterialIndex);
		bsData.Read(wModelID);
		bsData.Read(byteLength);
		bsData.Read(txdname, byteLength);
		txdname[byteLength] = '\0';
		bsData.Read(byteLength);
		bsData.Read(texname, byteLength);
		texname[byteLength] = '\0';
		bsData.Read(dwColor);
		if (strlen(txdname) < 32 && strlen(texname) < 32)
		{
			if (pObject)
				pObject->SetMaterial(wModelID, byteMaterialIndex, txdname, texname, dwColor);
		}
	}
	else if (byteMaterialType == 2)
	{
		bsData.Read(byteMaterialIndex);
		bsData.Read(byteMaterialSize);
		bsData.Read(byteLength);
		bsData.Read(fontname, byteLength);
		bsData.Read(byteFontSize);
		bsData.Read(byteBold);
		bsData.Read(dwFontColor);
		bsData.Read(dwBackColor);
		bsData.Read(byteTextAlignment);

		stringCompressor->DecodeString(text, 2048, &bsData);


		if (strlen(fontname) > 0 && strlen(fontname) < 32)
		{
			if (pObject) {
				pObject->SetMaterialText(
					byteMaterialIndex,
					text,
					byteMaterialSize,
					fontname,
					byteFontSize,
					byteBold,
					dwFontColor,
					dwBackColor,
					byteTextAlignment
				);
			}
		}
	}
}

// 0.3.7
void ScrRemoveBuilding(RPCParameters *rpcParams)
{
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iModel;
	float fX, fY, fZ;
	float fRadius;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iModel);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fRadius);
	RemoveBuilding(iModel, CVector(fX, fY, fZ), fRadius);
}
// 0.3.7
void ScrSetPlayerSkin(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	int iPlayerID;
	int iModel;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iPlayerID);
	bsData.Read(iModel);

	if (IsValidPedModel(iModel))
	{
		if (pPlayerPool->GetLocalPlayerID() == iPlayerID)
		{
			pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetModelIndex(iModel);
		}
		else
		{
			if (pPlayerPool->GetSlotState(iPlayerID) == true)
			{
				CRemotePlayer* pPlayer = pPlayerPool->GetAt(iPlayerID);
				if (!pPlayer || pPlayer->GetState() == PLAYER_STATE_NONE) return;
				CPlayerPed* pPlayerPed = pPlayer->GetPlayerPed();
				if (!pPlayerPed) return;
				pPlayerPed->SetModelIndex(iModel);
			}
		}
	}
	else
	{
		//if (gui) gui->chat()->addDebugMessage("Warning: SetPlayerSkin %d isn't a valid ped model.", iModel);
	}
}
// 0.3.7
void ScrSetPlayerMapIcon(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteIconID;
	CVector vecPos;
	uint8_t byteType;
	uint32_t dwColor;
	uint8_t byteStyle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteIconID);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(byteType);
	bsData.Read(dwColor);
	bsData.Read(byteStyle);

	// fix crash (invalid GTASA map icons)
	if (byteIconID == 1 or
		byteIconID == 2 or
		byteIconID == 4 or
		byteIconID == 56) byteIconID = 52;

	pNetGame->SetMapIcon(byteIconID, vecPos.x, vecPos.y, vecPos.z, byteType, dwColor, byteStyle);
}
// 0.3.7
void ScrRemovePlayerMapIcon(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteIconID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteIconID);

	pNetGame->DisableMapIcon(byteIconID);
}
// 0.3.7
void ScrShowNameTag(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteShowNameTag;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteShowNameTag);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool->GetSlotState(PlayerID))
	{
		CRemotePlayer* pPlayer = pPlayerPool->GetAt(PlayerID);
		if (pPlayer) {
			pPlayer->m_bShowNameTag = byteShowNameTag;
		}
	}
}
// 0.3.7
void ScrApplyPlayerAnimation(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szAnimLib[256];
	char szAnimName[256];
	memset(szAnimLib, 0, sizeof(szAnimLib));
	memset(szAnimName, 0, sizeof(szAnimName));

	PLAYERID PlayerID;
	uint8_t byteLength;
	float fDelta;
	bool bLoop, bLockX, bLockY, bFreeze;
	uint32_t dwTime;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	
	bsData.Read(byteLength);
	bsData.Read(szAnimLib, byteLength);
	szAnimLib[byteLength] = '\0';

	bsData.Read(byteLength);
	bsData.Read(szAnimName, byteLength);
	szAnimName[byteLength] = '\0';

	bsData.Read(fDelta);
	bsData.Read(bLoop);
	bsData.Read(bLockX);
	bsData.Read(bLockY);
	bsData.Read(bFreeze);
	bsData.Read(dwTime);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CPlayerPed* pPlayerPed = nullptr;

	if (pPlayerPool)
	{
		if (PlayerID == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(PlayerID)) {
				pPlayerPed = pPlayerPool->GetAt(PlayerID)->GetPlayerPed();
				pPlayerPool->GetAt(PlayerID)->m_bAppliedAnimation = true;
			}
		}

		if (pPlayerPed) {
			FLog("ApplyAnimation: %s:%s", szAnimLib, szAnimName);
			pPlayerPed->ApplyAnimation(szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, dwTime);
		}
	}
}
// 0.3.7
void ScrClearPlayerAnimations(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

	FLog("ClearAnimation");

	PLAYERID playerId;
	bsData.Read(playerId);
	RwMatrix mat;

	CPlayerPool * pPlayerPool = NULL;
	CPlayerPed * pPlayerPed = NULL;

	pPlayerPool = pNetGame->GetPlayerPool();

	if (pPlayerPool)
	{
		if (playerId == pPlayerPool->GetLocalPlayerID()) {
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else {
			if (pPlayerPool->GetSlotState(playerId)) {
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
				pPlayerPool->GetAt(playerId)->m_bAppliedAnimation = false;
			}
		}
		if (pPlayerPed) {
            mat = pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
			pPlayerPed->m_pPed->SetPosn(mat.pos.x, mat.pos.y, mat.pos.z);
		}
	}
}
// 0.3.7
void ScrSetPlayerHealth(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fHealth;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);
	bsData.Read(fHealth);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->SetHealth(fHealth);
}
// 0.3.7
void ScrGivePlayerWeapon(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iWeapon;
	int iAmmo;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);
	bsData.Read(iWeapon);
	bsData.Read(iAmmo);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GiveWeapon(iWeapon, iAmmo);
}
// 0.3.7
void ScrSetPlayerInterior(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteInteriorId;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteInteriorId);

	pGame->FindPlayerPed()->m_pPed->SetInterior(byteInteriorId, true);
}
// 0.3.7
extern UI *pUI;
void ScrShowTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

    FLog("ScrShowTextDraw");

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) {
        FLog("no textdraw pool");
        return;
    }

	uint16_t wTextDrawID;
	TEXT_DRAW_TRANSMIT textDrawTransmit;
	uint16_t wTextLength;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);
	bsData.Read((char*)& textDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
	bsData.Read(wTextLength);

	char szText[1024 + 1];

	bsData.Read(szText, wTextLength);
    szText[wTextLength] = 0;

    pTextDrawPool->New(wTextDrawID, &textDrawTransmit, szText);
}
// 0.3.7
void ScrHideTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) return;

	uint16_t wTextDrawID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);

	pTextDrawPool->Delete(wTextDrawID);
}
// 0.3.7
void ScrTextDrawSetString(RPCParameters * rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool == nullptr) return;

	uint16_t wTextDrawID;
	uint16_t wTextLength;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wTextDrawID);
	bsData.Read(wTextLength);

	if (wTextLength < 1024)
	{
		char szText[1024 + 1];
		bsData.Read(szText, wTextLength);
		szText[wTextLength] = '\0';

		CTextDraw* pTextDraw = pTextDrawPool->GetAt(wTextDrawID);
		if (pTextDraw) pTextDraw->SetText(szText);
	}

}

void ScrSelectTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable = false;
	uint32_t dwColor = 0;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);
	bsData.Read(dwColor);

	pNetGame->GetTextDrawPool()->SetSelectState(bEnable ? true : false, dwColor);
}

// 0.3.7
void ScrSetPlayerAmmo(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	
	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	uint8_t byteWeapon;
	uint16_t wAmmo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteWeapon);
	bsData.Read(wAmmo);

	pLocalPlayer->GetPlayerPed()->SetAmmo(byteWeapon, wAmmo);
}
// 0.3.7
void ScrSetVehicleHealth(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fHealth;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fHealth);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (pVehiclePool)
	{
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (pVehicle) {
			pVehicle->SetHealth(fHealth);
		}
	}
}
// 0.3.7
void ScrAttachTrailerToVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID TrailerID;
	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(TrailerID);
	bsData.Read(VehicleID);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CVehicle* pTrailer = pVehiclePool->GetAt(TrailerID);
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);

	if (pTrailer && pVehicle)
	{
		pVehicle->SetTrailer(pTrailer);
		pVehicle->AttachTrailer();
	}
}
// 0.3.7
void ScrDetachTrailerFromVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (pVehicle)
	{
		pVehicle->DetachTrailer();
		pVehicle->SetTrailer(nullptr);
	}
}
// 0.3.7
void ScrSetObjectPos(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	float fX;
	float fY;
	float fZ;
	float fUnused;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fUnused);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) return;

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (pObject) {
		pObject->m_pEntity->SetPosn(fX, fY, fZ);
	}
}
// 0.3.7
void ScrSetObjectRotation(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	CVector vecRot;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(vecRot.x);
	bsData.Read(vecRot.y);
	bsData.Read(vecRot.z);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) return;

	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (!pObject) return;

	pObject->InstantRotate(vecRot.x, vecRot.y, vecRot.z);
}
// 0.3.7
void ScrCreateExplosion(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fX, fY, fZ;
	uint32_t dwType;
	float fRadius;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(dwType);
	bsData.Read(fRadius);

	ScriptCommand(&create_explosion_with_radius, fX, fY, fZ, dwType, fRadius);
}
// 0.3.7
void ScrSetVehicleNumberPlate(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteLength;
	char szPlateName[32+1];
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteLength);
	if (byteLength > 32) return;

	bsData.Read(szPlateName, byteLength);
	szPlateName[byteLength] = '\0';

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->SetPlateText(szPlateName);
}

#define SPECTATE_TYPE_NORMAL	1
#define SPECTATE_TYPE_FIXED		2
#define SPECTATE_TYPE_SIDE		3

// 0.3.7
void ScrSpectatePlayer(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteMode);

	switch (byteMode) 
	{
	case SPECTATE_TYPE_FIXED:
		byteMode = 15;
		break;
	case SPECTATE_TYPE_SIDE:
		byteMode = 14;
		break;
	default:
		byteMode = 4;
	}

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectatePlayer(PlayerID);
}
// 0.3.7
void ScrSpectateVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteMode;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteMode);

	switch (byteMode)
	{
	case SPECTATE_TYPE_FIXED:
		byteMode = 15;
		break;
	case SPECTATE_TYPE_SIDE:
		byteMode = 14;
		break;
	default:
		byteMode = 3;
	}

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectateVehicle(VehicleID);
}
// 0.3.7
void ScrRemoveVehicleComponent(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	VEHICLEID VehicleID;
	uint16_t wComponent;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(wComponent);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->RemoveComponent(wComponent);
}
// 0.3.7
void ScrAttachObjectToPlayer(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	PLAYERID PlayerID;
	float offsetX, offsetY, offsetZ;
	float rX, rY, rZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(PlayerID);
	bsData.Read(offsetX);
	bsData.Read(offsetY);
	bsData.Read(offsetZ);
	bsData.Read(rX);
	bsData.Read(rY);
	bsData.Read(rZ);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	if (!pObjectPool) return;
	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (!pObject) return;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	if (pPlayerPool->GetLocalPlayerID() == PlayerID)
	{
		CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
		ScriptCommand(&attach_object_to_actor,
			pObject->m_dwGTAId,
			pLocalPlayer->GetPlayerPed()->m_dwGTAId,
			offsetX, offsetY, offsetZ,
			rX, rY, rZ);
	}
	else
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		ScriptCommand(&attach_object_to_actor,
			pObject->m_dwGTAId,
			pRemotePlayer->GetPlayerPed()->m_dwGTAId,
			offsetX, offsetY, offsetZ,
			rX, rY, rZ);
	}
}
// 0.3.7
void ScrSetPlayerWantedLevel(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteWantedLevel;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteWantedLevel);

	if (pGame) pGame->SetWantedLevel(byteWantedLevel);
}
// 0.3.7
void ScrSetPlayerSpecialAction(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteSpecialAction;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteSpecialAction);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	pPlayerPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
}
// 0.3.7
void ScrEnableStuntBonus(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);

	pGame->EnableStuntBonus(bEnable);
}
// 0.3.7
void ScrSetPlayerFightingStyle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteStyle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteStyle);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	CPlayerPed* pPlayerPed = nullptr;

	if (PlayerID == pPlayerPool->GetLocalPlayerID()) {
		pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
	}
	else {
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pPlayerPed = pRemotePlayer->GetPlayerPed();
		}
	}

	if (pPlayerPed) {
		pPlayerPed->SetFightingStyle(byteStyle);
	}
}

void ScrSetPlayerVelocity(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVector vecVelocity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecVelocity.x);
	bsData.Read(vecVelocity.y);
	bsData.Read(vecVelocity.z);

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (!pPlayerPed) return;

	if (pPlayerPed->IsOnGround()) {
		//uint32_t dwState = pPlayerPed->GetStateFlags();
		//pPlayerPed->SetStateFlags(dwState ^ 3);
	}

	pPlayerPed->m_pPed->SetVelocity(vecVelocity);
}
// 0.3.7
void ScrSetVehicleVelocity(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteType;
	CVector vecVelocity;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteType);
	bsData.Read(vecVelocity.x);
	bsData.Read(vecVelocity.y);
	bsData.Read(vecVelocity.z);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
	if (!pPlayerPed) return;

	if (pPlayerPed->IsInVehicle())
	{
        CVehicleGTA* pGtaVehicle = pPlayerPed->GetGtaVehicle();
		VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);
		CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
		if (!pVehicle) return;

		if (byteType)
		{
			if (byteType == 1)
			{
				pVehicle->m_pVehicle->SetTurnSpeed(vecVelocity);
			}
		}
		else
		{
			pVehicle->m_pVehicle->SetVelocity(vecVelocity);
		}
	}

}
// 0.3.7
void ScrToggleWidescreen(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteToggle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteToggle);
	//if (gui) gui->chat()->addDebugMessage("Widescreen = %d", byteToggle);
	ScriptCommand(&toggle_widescreen, byteToggle);
}
// 0.3.7
void ScrSetVehicleTireDamageStatus(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteTireDamageStatus;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteTireDamageStatus);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->SetTireDamageStatus(byteTireDamageStatus);
}
// 0.3.7
void ScrSetPlayerTeam(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteTeam;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteTeam);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) return;

	if (PlayerID == pPlayerPool->GetLocalPlayerID())
	{
		pLocalPlayer->SetTeam(byteTeam);
	}
	else
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pRemotePlayer->SetTeam(byteTeam);
		}
	}
}
// 0.3.7
void ScrSetPlayerName(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint8_t byteLen;
	char szName[28];
	uint8_t byteSuccess;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(byteLen);
	if (byteLen > MAX_PLAYER_NAME) return;

	bsData.Read(szName, byteLen);
	szName[byteLen] = '\0';
	bsData.Read(byteSuccess);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	if (byteSuccess == 1) {
		pPlayerPool->SetPlayerName(PlayerID, szName);
	}

	if (pPlayerPool->GetLocalPlayerID() == PlayerID) {
		pPlayerPool->SetLocalPlayerName(szName);
	}
}
// 0.3.7
void ScrSetPlayerPos(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) return;

    pLocalPlayer->DisableSurf();

    if(pLocalPlayer->GetPlayerPed()->m_pPed->IsInVehicle())
        pLocalPlayer->GetPlayerPed()->RemoveFromVehicleAndPutAt(vecPos.x, vecPos.y, vecPos.z);
    else
        pLocalPlayer->GetPlayerPed()->m_pPed->SetPosn(vecPos.x, vecPos.y, vecPos.z);
}
// 0.3.7
void ScrSetPlayerPosFindZ(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) return;

	vecPos.z = pGame->FindGroundZForCoord(vecPos.x, vecPos.y, vecPos.z) + 1.5f;
    pLocalPlayer->DisableSurf();
	pLocalPlayer->GetPlayerPed()->m_pPed->SetPosn(vecPos.x, vecPos.y, vecPos.z);
}
// 0.3.7
void ScrPutPlayerInVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteSeatID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteSeatID);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

    CPlayerPed *pPed = pGame->FindPlayerPed();
    if(!pPed)return;

    //if(vehicleid == pPed->GetCurrentSampVehicleID()) return;

    if(pPed->m_pPed->IsInVehicle()) {
        pPed->m_pPed->RemoveFromVehicle();
    }
    CVehicle *pVehicle = pVehiclePool->GetAt(VehicleID);
    if(!pVehicle)return;
    //   DLOG("seatid = %d", vehicleid);
    if(byteSeatID == 0) {
        ScriptCommand(&put_actor_in_car, pPed->m_dwGTAId, pVehicle->m_dwGTAId);
        //	CCarEnterExit::SetPedInCarDirect(pPed->m_pPed, pVehicle->m_pVehicle, 0, true);
    } else {
        byteSeatID --;
        ScriptCommand(&put_actor_in_car2, pPed->m_dwGTAId, pVehicle->m_dwGTAId, byteSeatID);
//		seatid = CCarEnterExit::ComputeTargetDoorToEnterAsPassenger(pVehicle->m_pVehicle, seatid);
//		CCarEnterExit::SetPedInCarDirect(pPed->m_pPed, pVehicle->m_pVehicle, seatid);
    }
}
// 0.3.7
void ScrRemovePlayerFromVehicle(RPCParameters* rpcParams)
{
	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	if (!pLocalPlayer) return;

	pLocalPlayer->GetPlayerPed()->ExitCurrentVehicle();
}
// 0.3.7
void ScrSetPlayerColor(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	uint32_t dwColor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(dwColor);

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	if (pPlayerPool->GetLocalPlayerID() == PlayerID)
	{
		CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
		if (pLocalPlayer) {
			pLocalPlayer->SetPlayerColor(dwColor);
		}
	}
	else
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
		if (pRemotePlayer) {
			pRemotePlayer->SetPlayerColor(dwColor);
		}
	}
}
// 0.3.7
void ScrShowGameText(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	char szMsg[256];
	memset(szMsg, 0, sizeof(szMsg));

	int iSize;
	int iTime;
	int iLen;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iSize);
	bsData.Read(iTime);
	bsData.Read(iLen);

	if (iLen < 0 || iLen > 200) return;

	bsData.Read(szMsg, iLen);
	szMsg[iLen] = '\0';
	pGame->DisplayGameText(szMsg, iTime, iSize);
}
// 0.3.7
void ScrSetVehiclePos(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fX;
	float fY;
	float fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->m_pVehicle->SetPosn(fX, fY, fZ);
}
// 0.3.7
void ScrSetVehicleZAngle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	float fAngle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(fAngle);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->SetZAngle(fAngle);
}
// 0.3.7
void ScrSetVehicleParams(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteObjective;
	uint8_t byteDoorsLocked;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteObjective);
	bsData.Read(byteDoorsLocked);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	pVehiclePool->AssignSpecialParamsToVehicle(VehicleID, byteObjective, byteDoorsLocked);
}
// 0.3.7
void ScrSetPlayerCameraBehindPlayer(RPCParameters* rpcParams)
{
    CCamera::SetBehindPlayer();
}
// 0.3.7
void ScrTogglePlayerControllable(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteControllable;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteControllable);

	pGame->FindPlayerPed()->TogglePlayerControllable(byteControllable);
}

void ScrPlayerPlaySound(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iSound;
	float fX, fY, fZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iSound);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	// sub_100A1B90(pGame->field_0, a2, a3, a4, a5);
}
// 0.3.7
void ScrSetWorldBounds(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float bounds[4];
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bounds[0]);
	bsData.Read(bounds[1]);
	bsData.Read(bounds[2]);
	bsData.Read(bounds[3]);

	pNetGame->m_pNetSet->fWorldBounds[0] = bounds[0];
	pNetGame->m_pNetSet->fWorldBounds[1] = bounds[1];
	pNetGame->m_pNetSet->fWorldBounds[2] = bounds[2];
	pNetGame->m_pNetSet->fWorldBounds[3] = bounds[3];
}
// 0.3.7
void ScrGivePlayerMoney(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iMoney;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iMoney);

	pGame->AddToLocalMoney(iMoney);
}
// 0.3.7
void ScrSetPlayerFacingAngle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fRotation;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fRotation);

	pGame->FindPlayerPed()->SetTargetRotation(fRotation);
}
// 0.3.7
void ScrResetPlayerMoney(RPCParameters* rpcParams)
{
	pGame->ResetLocalMoney();
}
// 0.3.7
void ScrResetPlayerWeapons(RPCParameters* rpcParams)
{
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->ClearWeapons();
}
// 0.3.7
void ScrLinkVehicleToInterior(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	VEHICLEID VehicleID;
	uint8_t byteInterior;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read(byteInterior);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	pVehiclePool->LinkToInterior(VehicleID, byteInterior);
}
// 0.3.7
void ScrSetPlayerArmour(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	float fArmour;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(fArmour);
	pLocalPlayer->GetPlayerPed()->SetArmour(fArmour);
}
// 0.3.7
void ScrSetArmedWeapon(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t dwWeapon;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(dwWeapon);
	if (dwWeapon >= 0 && dwWeapon <= 46) {
		CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed) {
			pPlayerPed->SetArmedWeapon(dwWeapon, false);
		}
	}
}

void ScrSetPlayerAttachedObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	NEW_ATTACHED_OBJECT NewAttachedObject;
	memset(&NewAttachedObject, 0, sizeof(NEW_ATTACHED_OBJECT));

	PLAYERID PlayerID;
	int index;
	bool bCreate;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(index);
	bsData.Read(bCreate);

	if (bCreate)
		bsData.Read((char*)& NewAttachedObject, sizeof(NEW_ATTACHED_OBJECT));

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool)
	{
		CPlayerPed* pPlayerPed = nullptr;

		if (pPlayerPool->GetLocalPlayerID() == PlayerID)
		{
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else
		{
			CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(PlayerID);
			if (pRemotePlayer)
				pPlayerPed = pRemotePlayer->GetPlayerPed();
		}

		if (pPlayerPed)
		{
			if (bCreate)
				pPlayerPed->SetAttachedObject(index, &NewAttachedObject);
			else
				pPlayerPed->RemoveAttachedObject(index);
		}
	}
}
// 0.3.7
void ScrApplyActorAnimation(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	char szAnimLib[256];
	char szAnimName[256];
	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	PLAYERID ActorID;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	float fDelta;
	bool bLoop;
	bool bLockX;
	bool bLockY;
	bool bFreeze;
	int iTime;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fDelta);
	bsData.Read(bLoop);
	bsData.Read(bLockX);
	bsData.Read(bLockY);
	bsData.Read(bFreeze);
	bsData.Read(iTime);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->ApplyAnimation(szAnimName, szAnimLib, fDelta, bLoop, bLockX, bLockY, bFreeze, iTime);
	}
}
// 0.3.7
void ScrClearActorAnimation(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	PLAYERID ActorID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->ClearAnimation();
	}
}
// 0.3.7
void ScrSetActorFacingAngle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	PLAYERID ActorID;
	float fAngle;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(fAngle);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->SetFacingAngle(fAngle);
	}
}
// 0.3.7
void ScrSetActorPos(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	PLAYERID ActorID;
	CVector vecPos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->m_pPed->SetPosn(vecPos.x, vecPos.y, vecPos.z);
	}
}
// 0.3.7
void ScrSetActorHealth(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	PLAYERID ActorID;
	float fHealth;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);
	bsData.Read(fHealth);

	CActor* pActor = pActorPool->GetAt(ActorID);
	if (pActor) {
		pActor->SetHealth(fHealth);
	}
}

void ScrPlayAudioStream(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteUrlLen;
	char szUrl[256];
	float fX, fY, fZ;
	float fRadius;
	uint8_t byteUsePos;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	memset(szUrl, 0, sizeof(szUrl));

	bsData.Read(byteUrlLen);
	bsData.Read(szUrl, byteUrlLen);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	bsData.Read(fRadius);
	bsData.Read(byteUsePos);

	if (pAudioStream)
		pAudioStream->Play(szUrl, fX, fY, fZ, fRadius, byteUsePos);
}

void ScrStopAudioStream(RPCParameters* rpcParams)
{
	if (pAudioStream)
		pAudioStream->Stop(false);
		//pAudioStream->Stop(true);
}

void ScrMoveObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	float fPad0, fPad1, fPad2;
	float fPosX, fPosY, fPosZ;
	float fSpeed;
	float fRotX, fRotY, fRotZ;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);
	bsData.Read(fPad0);
	bsData.Read(fPad1);
	bsData.Read(fPad2);
	bsData.Read(fPosX);
	bsData.Read(fPosY);
	bsData.Read(fPosZ);
	bsData.Read(fSpeed);
	bsData.Read(fRotX);
	bsData.Read(fRotY);
	bsData.Read(fRotZ);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	CObject* pObject = pObjectPool->GetAt(ObjectID);
	if (pObject) {
		pObject->MoveTo(fPosX, fPosY, fPosZ, fSpeed, fRotX, fRotY, fRotZ);
	}
}
// 0.3.7
void ScrStopObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	OBJECTID ObjectID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	CObject* pObject = pObjectPool->GetAt(ObjectID);

	if (pObject) {
		pObject->StopMoving();
	}
}

void AttachCameraToObject(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

	CObjectPool *pObjectPool = pNetGame->GetObjectPool();
	if(pObjectPool)
	{
		OBJECTID objectId;

		bsData.Read(objectId);
		if(objectId < 0 || objectId >= MAX_OBJECTS) 
			return;

		CObject *pObject = pObjectPool->GetAt(objectId);
		if(pObject)
		{
			//if(pGameCamera)
				//pGameCamera->AttachToEntity(pObject);
		}
	}
}

void RegisterScriptRPCs(RakClientInterface *pRakClient)
{
	FLog("Registering script RPC's..");

	// RPC_ScrDisableVehicleCollision
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ScrSetPlayerMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ScrRemovePlayerMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetWeaponAmmo, ScrSetPlayerAmmo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetGravity, ScrSetGravity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ScrAttachTrailerToVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ScrDetachTrailerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ScrCreateObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos, ScrSetObjectPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ScrSetObjectRotation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ScrDestroyObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ScrCreateExplosion);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowNameTag, ScrShowNameTag);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ScrMoveObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ScrStopObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrNumberPlate, ScrSetVehicleNumberPlate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
	// RPC_null - unused
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ScrSpectatePlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ScrSpectateVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent, ScrRemoveVehicleComponent);
	// RPC_ScrForceClassSelection - useless
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer, ScrAttachObjectToPlayer);
	// RPC_ScrInitMenu
	// RPC_ScrShowMenu
	// RPC_ScrHideMenu
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ScrSetPlayerWantedLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ScrShowTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ScrHideTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTextDrawSetString, ScrTextDrawSetString);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone, ScrAddGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone, ScrGangZoneDestroy);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone, ScrGangZoneFlash);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone, ScrGangZoneStopFlash);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ScrApplyPlayerAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ScrClearPlayerAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction, ScrSetPlayerSpecialAction);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrEnableStuntBonus, ScrEnableStuntBonus);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle, ScrSetPlayerFightingStyle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerVelocity, ScrSetPlayerVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ScrSetVehicleVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrToggleWidescreen, ScrToggleWidescreen);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleTireStatus, ScrSetVehicleTireDamageStatus);
	// RPC_150 ???
	// RPC_92 ???
	// RPC_ScrPlayCrimeReport
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerTeam, ScrSetPlayerTeam);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ScrSetPlayerName);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ScrSetPlayerPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ScrSetPlayerPosFindZ);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ScrSetPlayerColor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ScrShowGameText);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ScrSetPlayerInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ScrSetCameraPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ScrSetCameraLookAt);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ScrSetVehiclePos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ScrSetVehicleZAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ScrSetVehicleParams);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ScrSetPlayerCameraBehindPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlaySound, ScrPlayerPlaySound);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetWorldBounds, ScrSetWorldBounds);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ScrGivePlayerMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ScrResetPlayerMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);
	// RPC_64 - unused
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ScrLinkVehicleToInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
	// RPC_ScrSendDeathMessage
	// RPC_ScrSetShopName
	// RPC_ScrSetPlayerDrunkLevel
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetArmedWeapon, ScrSetArmedWeapon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ScrSetPlayerAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayAudioStream, ScrPlayAudioStream);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopAudioStream, ScrStopAudioStream);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding, ScrRemoveBuilding);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachCameraToObject, AttachCameraToObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ScrInterpolateCamera);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClickTextDraw, ScrSelectTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectMaterial, ScrSetObjectMaterial);
	// RPC_ScrObjectNoCameraCol
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyActorAnimation, ScrApplyActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearActorAnimations, ScrClearActorAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorFacingAngle, ScrSetActorFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorPos, ScrSetActorPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetActorHealth, ScrSetActorHealth);
	// RPC_SetPlayerVirtualWorld - 03DL only
}

void UnregisterScriptRPCs(RakClientInterface *pRakClient)
{
	LOGI("Unregistering script RPC's..");

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetGravity);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateObject);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAddGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveBuilding);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera);
}