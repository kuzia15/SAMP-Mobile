#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../gui/gui.h"
#include "../vendor/encoding/encoding.h"
#include "../settings.h"

extern UI* pUI;
extern CGame *pGame;
extern CNetGame *pNetGame;
extern CSettings *pSettings;
//extern CVoice* pVoice;

extern int iNetModeNormalOnFootSendRate;
extern int iNetModeNormalInCarSendRate;
extern int iNetModeFiringSendRate;
extern int iNetModeSendMultiplier;

int g_iLagCompensationMode = 0;

#define REJECT_REASON_BAD_VERSION   1
#define REJECT_REASON_BAD_NICKNAME  2
#define REJECT_REASON_BAD_MOD		3
#define REJECT_REASON_BAD_PLAYERID	4

void ProcessIncommingEvent(PLAYERID playerID, int iEventType,
	uint32_t dwParam1, uint32_t dwParam2, uint32_t dwParam3);

void InitGame(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] InitGame");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();

	PLAYERID MyPlayerID;
	int iLagCompensation;
	bool bStuntBonus;
	bool bLanMode;
	RakNet::BitStream bsInitGame(Data, (iBitLength / 8) + 1, false);

	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bZoneNames);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bUseCJWalk);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bAllowWeapons);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bLimitGlobalChatRadius);
	bsInitGame.Read(pNetGame->m_pNetSet->fGlobalChatRadius);
	bsInitGame.ReadCompressed(bStuntBonus);
	bsInitGame.Read(pNetGame->m_pNetSet->fNameTagDrawDistance);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bDisableInteriorEnterExits);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bNameTagLOS);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bManualVehicleEngineAndLight);
	bsInitGame.Read(pNetGame->m_pNetSet->iSpawnsAvailable);
	bsInitGame.Read(MyPlayerID);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bShowNameTags);
	bsInitGame.Read(pNetGame->m_pNetSet->iShowPlayerMarkers);
	bsInitGame.Read(pNetGame->m_pNetSet->byteWorldTime_Hour);
	bsInitGame.Read(pNetGame->m_pNetSet->byteWeather);
	bsInitGame.Read(pNetGame->m_pNetSet->fGravity);
	bsInitGame.ReadCompressed(bLanMode);
	bsInitGame.Read(pNetGame->m_pNetSet->iDeathDropMoney);
	bsInitGame.ReadCompressed(pNetGame->m_pNetSet->bInstagib);

	bsInitGame.Read(iNetModeNormalOnFootSendRate);
	bsInitGame.Read(iNetModeNormalInCarSendRate);
	bsInitGame.Read(iNetModeFiringSendRate);
	bsInitGame.Read(iNetModeSendMultiplier);
	bsInitGame.Read(iLagCompensation);

	Log::addParameter("bUseCJWalk", pNetGame->m_pNetSet->bUseCJWalk);
	Log::addParameter("bZoneNames", pNetGame->m_pNetSet->bZoneNames);
	Log::addParameter("bAllowWeapons", pNetGame->m_pNetSet->bAllowWeapons);
	Log::addParameter("bLimitGlobalChatRadius", pNetGame->m_pNetSet->bLimitGlobalChatRadius);
	Log::addParameter("fGlobalChatRadius", pNetGame->m_pNetSet->fGlobalChatRadius);
	Log::addParameter("bStuntBonus", bStuntBonus);
	Log::addParameter("fNameTagDrawDistance", pNetGame->m_pNetSet->fNameTagDrawDistance);
	Log::addParameter("bDisableInteriorEnterExits", pNetGame->m_pNetSet->bDisableInteriorEnterExits);
	Log::addParameter("bNameTagLOS", pNetGame->m_pNetSet->bNameTagLOS);
	Log::addParameter("bManualVehicleEngineAndLight", pNetGame->m_pNetSet->bManualVehicleEngineAndLight);
	Log::addParameter("iSpawnsAvailable", pNetGame->m_pNetSet->iSpawnsAvailable);
	Log::addParameter("MyPlayerID", MyPlayerID);
	Log::addParameter("bShowNameTags", pNetGame->m_pNetSet->bShowNameTags);
	Log::addParameter("iShowPlayerMarkers", pNetGame->m_pNetSet->iShowPlayerMarkers);
	Log::addParameter("byteWorldTime_Hour", pNetGame->m_pNetSet->byteWorldTime_Hour);
	Log::addParameter("byteWeather", pNetGame->m_pNetSet->byteWeather);
	Log::addParameter("fGravity", pNetGame->m_pNetSet->fGravity);
	Log::addParameter("bLanMode", bLanMode);
	Log::addParameter("iDeathDropMoney", pNetGame->m_pNetSet->iDeathDropMoney);
	Log::addParameter("bInstagib", pNetGame->m_pNetSet->bInstagib);
	Log::addParameter("iNetModeNormalOnFootSendRate", iNetModeNormalOnFootSendRate);
	Log::addParameter("iNetModeNormalInCarSendRate", iNetModeNormalInCarSendRate);
	Log::addParameter("iNetModeFiringSendRate", iNetModeFiringSendRate);
	Log::addParameter("iNetModeSendMultiplier", iNetModeSendMultiplier);
	Log::addParameter("iLagCompensation", iLagCompensation);

	switch (iLagCompensation)
	{
		case 1:
			g_iLagCompensationMode = 0;
			break;
		case 2:
			g_iLagCompensationMode = 1;
			break;
		default:
			g_iLagCompensationMode = 2;
			break;
	}

	//InstallWeaponFireHooks();
	if (pGame) {
		pGame->DisableAutoAim();
	}

	uint8_t byteStrLen;
	bsInitGame.Read(byteStrLen);
	if (byteStrLen) {
		memset(pNetGame->m_szHostName, 0, sizeof(pNetGame->m_szHostName));
		bsInitGame.Read(pNetGame->m_szHostName, byteStrLen);
	}
	pNetGame->m_szHostName[byteStrLen] = 0;

	uint8_t byteVehicleModels[212];
	bsInitGame.Read((char*)byteVehicleModels, 212);
	//pGame->PreloadVehicleModels(byteVehicleModels);

	bsInitGame.Read(pNetGame->m_pNetSet->iVehicleFriendlyFire);
	pPlayerPool->SetLocalPlayerID(MyPlayerID);
	pGame->EnableStuntBonus(bStuntBonus);

	if (bLanMode) {
		pNetGame->m_bLanMode = true;
	}

	pNetGame->InitGameLogic();
	pGame->SetGravity(pNetGame->m_pNetSet->fGravity);

	if (pNetGame->m_pNetSet->bDisableInteriorEnterExits) {
		pGame->DisableEnterExits();
	}

	if (pNetGame->m_pNetSet->iVehicleFriendlyFire) {
		//InstallHook_VehicleFriendlyFire();
	}

	pGame->SetWorldWeather(pNetGame->m_pNetSet->byteWeather);
	pGame->ToggleCJWalk(pNetGame->m_pNetSet->bUseCJWalk);

	Log::addParameter("szHostName", pNetGame->m_szHostName);
	Log::addParameter("iVehicleFriendlyFire", pNetGame->m_pNetSet->iVehicleFriendlyFire);

	if (pUI) pUI->chat()->addDebugMessage("Connected to {B9C9BF}%.64s", Encoding::cp2utf(pNetGame->m_szHostName).c_str());

	pNetGame->SetGameState(GAMESTATE_CONNECTED);

	pPlayerPool->GetLocalPlayer()->HandleClassSelection();

	
	if (pUI) {
		pUI->voicebutton()->setVisible(true);
	}

	/*if (pVoice && pVoice->recorder()->active())
	{
		if (pUI) {
			pUI->voicebutton()->setVisible(true);
		}
	}
	else
	{
		//pUI->dialog()->show(-1, DialogStyle::MSGBOX, u8"Voice", u8"Voice chat not initalized. \nPlease allow the app to access the microphone!", u8"OK", "");
		// or
		pUI->chat()->addInfoMessage(u8"{ff0000}Voice chat not inited. Please allow the app to access the microphone!");
	}*/
}
// 0.3.7
#include "java/jniutil.h"
extern CJavaWrapper* pJavaWrapper;
int Button2Show;
void DialogBox(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] DialogBox");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	int16_t wDialogID;
	uint8_t byteDialogStyle;
	uint8_t byteLength;
	char szTitle[256 + 1];
	char szButton1[256 + 1];
	char szButton2[256 + 1];
	char szMessage[4096 + 1];
	memset(szTitle, 0, sizeof(szTitle));
	memset(szButton1, 0, sizeof(szButton1));
	memset(szButton2, 0, sizeof(szButton2));
	memset(szMessage, 0, sizeof(szMessage));

	bsData.Read(wDialogID);
	bsData.Read(byteDialogStyle);
	// title
	bsData.Read(byteLength);
	bsData.Read(szTitle, byteLength);
	// button1
	bsData.Read(byteLength);
	bsData.Read(szButton1, byteLength);
	// button2
	bsData.Read(byteLength);
	bsData.Read(szButton2, byteLength);
	// message
	stringCompressor->DecodeString(szMessage, 4096, &bsData);

	Log::addParameter("wDialogID", wDialogID);
	Log::addParameter("byteDialogStyle", (int)byteDialogStyle);
	Log::addParameter("szTitle", szTitle);
	Log::addParameter("szButton1", szButton1);
	Log::addParameter("szButton2", szButton2);
	Log::addParameter("szMessage", szMessage);
	
	/*if (wDialogID == 65535 || wDialogID < 0) //Fix Blank Dialog
	{
		pNetGame->SendDialogResponse(wDialogID, 1, -1, " ");
		return;
	}*/

	DialogStyle style;
	switch (byteDialogStyle)
	{
	case 0:
		style = DialogStyle::MSGBOX;
		break;
	case 1:
		style = DialogStyle::INPUT;
		break;
	case 2:
		style = DialogStyle::LIST;
		break;
	case 3:
		style = DialogStyle::PASSWORD;
		break;
	case 4:
		style = DialogStyle::TABLIST;
		break;
	case 5:
		style = DialogStyle::TABLIST_HEADERS;
		break;
	}

	if (pUI) 
	{
		if (strcmp(szButton2, "") == 0)
		{
			Button2Show = 2;
			//MyLog("lol %s", szButton2);
		}
		else
		{
			Button2Show = 1;
			//MyLog("lol2 %s", szButton2);
		}
		//pUI->dialog()->show(wDialogID, style, Encoding::cp2utf(szTitle), Encoding::cp2utf(szMessage), Encoding::cp2utf(szButton1), Encoding::cp2utf(szButton2));
		pJavaWrapper->ShowDialog(byteDialogStyle, wDialogID, (char*)Encoding::cp2utf(szTitle).c_str(), (char*) Encoding::cp2utf(szMessage).c_str(), (char*) Encoding::cp2utf(szButton1).c_str(), (char*) Encoding::cp2utf(szButton2).c_str());
	}

	return;
}
// 0.3.7
void Chat(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Chat");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	uint8_t byteMsgLen;
	char szMessage[256];
	memset(szMessage, 0, sizeof(szMessage));

	if (pNetGame->GetGameState() != GAMESTATE_CONNECTED) return;

	bsData.Read(playerId);
	bsData.Read(byteMsgLen);
	bsData.Read(szMessage, byteMsgLen);
	szMessage[byteMsgLen] = '\0';

	Log::addParameter("playerId", playerId);
	Log::addParameter("szMessage", szMessage);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if (playerId == pPlayerPool->GetLocalPlayerID()) 
	{
		if (pUI) pUI->chat()->addChatMessage(Encoding::cp2utf(szMessage),
			pPlayerPool->GetLocalPlayerName(), UI::fixcolor(pPlayerPool->GetLocalPlayer()->GetPlayerColorAsRGBA()));
	}
	else {
		CRemotePlayer *pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->Say(szMessage);
		}
	}
}
// 0.3.7
void ClientMessage(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Client message ");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint32_t dwColor, dwStrLen;
	char szMsg[256];
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	
	memset(szMsg, 0, 256);
	bsData.Read(dwColor);
	bsData.Read(dwStrLen);

	Log::addParameter("dwColor", dwColor);
	Log::addParameter("dwStrLen", dwStrLen);

	if (dwStrLen <= 255)
	{
		bsData.Read(szMsg, dwStrLen);
		szMsg[dwStrLen] = '\0';
		Log::addParameter("szMsg", szMsg);
		if (pUI) pUI->chat()->addClientMessage(Encoding::cp2utf(szMsg), UI::fixcolor(dwColor));
	}
}
// 0.3.7
void RequestClass(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Request class");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteRequestOutcome = 0;
	PLAYER_SPAWN_INFO SpawnInfo;
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	Log::addParameter("byteRequestOutcome", byteRequestOutcome);
	Log::addParameter("SpawnInfo.byteTeam", SpawnInfo.byteTeam);
	Log::addParameter("SpawnInfo.fRotation", SpawnInfo.fRotation);
	Log::addParameter("SpawnInfo.iSkin", SpawnInfo.iSkin);
	Log::addParameter("SpawnInfo.iSpawnWeapons", SpawnInfo.iSpawnWeapons);
	Log::addParameter("SpawnInfo.iSpawnWeaponsAmmo", SpawnInfo.iSpawnWeaponsAmmo);
	Log::addParameter("SpawnInfo.unk", SpawnInfo.unk);
	Log::addParameter("SpawnInfo.vecPos.x", SpawnInfo.vecPos.x);
	Log::addParameter("SpawnInfo.vecPos.y", SpawnInfo.vecPos.y);
	Log::addParameter("SpawnInfo.vecPos.z", SpawnInfo.vecPos.z);

	if (byteRequestOutcome) {
		pLocalPlayer->SetSpawnInfo(&SpawnInfo);
		pLocalPlayer->HandleClassSelectionOutcome(true);
	}
	else {
		pLocalPlayer->HandleClassSelectionOutcome(false);
	}
}
// 0.3.7
void RequestSpawn(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Request spawn");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteRequestOutcome = false;
	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	bsData.Read(byteRequestOutcome);
	Log::addParameter("byteRequestOutcome", byteRequestOutcome);

	if (byteRequestOutcome == 2 || (byteRequestOutcome && pLocalPlayer->m_bWaitingForSpawnRequestReply)) {
		pLocalPlayer->Spawn();
	}
	else {
		pLocalPlayer->m_bWaitingForSpawnRequestReply = false;
	}
}
// 0.3.7
void ToggleClock(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Toggle clock");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteClock;

	bsData.Read(byteClock);
	Log::addParameter("byteClock", byteClock);

	if (byteClock) {
		pNetGame->m_pNetSet->byteHoldTime = false;
	}
	else {
		pNetGame->m_pNetSet->byteHoldTime = true;
		pGame->GetWorldTime((int*)&pNetGame->m_pNetSet->byteWorldTime_Hour, 
			(int*)&pNetGame->m_pNetSet->byteWorldTime_Minute);
	}
}
// 0.3.7
void SetTimeEx(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] SetTimeEx");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteHour;
	uint8_t byteMinute;

	bsData.Read(byteHour);
	bsData.Read(byteMinute);

	Log::addParameter("byteHour", byteHour);
	Log::addParameter("byteMinute", byteMinute);

	pGame->SetWorldTime(byteHour, byteMinute);
	pNetGame->m_pNetSet->byteWorldTime_Hour = byteHour;
	pNetGame->m_pNetSet->byteWorldTime_Minute = byteMinute;
}
// 0.3.7
void Weather(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Weather");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteWeather;
	
	bsData.Read(byteWeather);
	
	Log::addParameter("byteWeather", byteWeather);

	pNetGame->m_pNetSet->byteWeather = byteWeather;
	pGame->SetWorldWeather(byteWeather);
}
// 0.3.7
void WorldTime(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] World time");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint8_t byteWorldTIme;
	bsData.Read(byteWorldTIme);

	Log::addParameter("byteWorldTIme", byteWorldTIme);

	pNetGame->m_pNetSet->byteWorldTime_Hour = byteWorldTIme;
}
// 0.3.7
void ConnectionRejected(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Connection rejected");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteRejectReason;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteRejectReason);

	Log::addParameter("byteRejectReason", byteRejectReason);

	switch (byteRejectReason)
	{
	case REJECT_REASON_BAD_VERSION:
		if (pUI) pUI->chat()->addInfoMessage("CONNECTION REJECTED: Incorrect Version.");
		break;

	case REJECT_REASON_BAD_NICKNAME:
		if (pUI) pUI->chat()->addInfoMessage("CONNECTION REJECTED: Unacceptable NickName");
		if (pUI) pUI->chat()->addInfoMessage("Please choose another nick between and 3-20 characters");
		if (pUI) pUI->chat()->addInfoMessage("Please use only a-z, A-z, 0-9");
		if (pUI) pUI->chat()->addInfoMessage("Use /quit to exit or press ESC and select Quit Game");
		pNetGame->SetGameState(GAMESTATE_WAIT_CONNECT);
		break;

	case REJECT_REASON_BAD_MOD:
		if (pUI) pUI->chat()->addInfoMessage("CONNECTION REJECTED: Bad mod version.");
		break;
	case REJECT_REASON_BAD_PLAYERID:
		if (pUI) pUI->chat()->addInfoMessage("CONNECTION REJECTED: Unable to allocate a player slot.");
		break;
	}

	pNetGame->GetRakClient()->Disconnect(500);
}
// 0.3.7
void GameModeRestart(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Gamemode restart");

	pNetGame->ShutdownForGameModeRestart();
}
// 0.3.7
void ServerJoin(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Server join");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pRemotePlayer = nullptr;
	uint8_t byteNameLen;
	uint8_t byteIsNPC;
	PLAYERID PlayerID;
	uint32_t dwMarkerColor;
	char szPlayerName[256];
	memset(szPlayerName, 0, sizeof(szPlayerName));

	bsData.Read(PlayerID);
	bsData.Read(dwMarkerColor);
	bsData.Read(byteIsNPC);
	bsData.Read(byteNameLen);
	bsData.Read(szPlayerName, byteNameLen);
	szPlayerName[byteNameLen] = '\0';

	Log::addParameter("PlayerID", PlayerID);
	Log::addParameter("dwMarkerColor", dwMarkerColor);
	Log::addParameter("byteIsNPC", byteIsNPC);
	Log::addParameter("szPlayerName", szPlayerName);

	if (strlen(szPlayerName) > MAX_PLAYER_NAME) {
		return;
	}

	pPlayerPool->New(PlayerID, szPlayerName, byteIsNPC);

	pRemotePlayer = pPlayerPool->GetAt(PlayerID);

	if (dwMarkerColor && pRemotePlayer) {
		pRemotePlayer->SetPlayerColor(dwMarkerColor);
	}
}
// 0.3.7
void ServerQuit(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Server quit");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	uint8_t byteReason;
	bsData.Read(playerId);
	bsData.Read(byteReason);

	Log::addParameter("playerId", playerId);
	Log::addParameter("byteReason", byteReason);

	pNetGame->GetPlayerPool()->Delete(playerId, byteReason);
}
// 0.3.7
void WorldPlayerAdd(RPCParameters *rpcParams)
{
	FLog("[RPC-IN] World player add");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pRemotePlayer = nullptr;
	CPlayerPed *pPlayerPed = nullptr;
	PLAYERID playerId;
	uint8_t byteTeam;
	int iSkin;
	CVector vecPos;
	float fRotation;
	uint32_t dwColor;
	uint8_t byteFightingStyle;
	uint16_t wUnkData[11];

	bsData.Read(playerId);
	bsData.Read(byteTeam);
	bsData.Read(iSkin);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fRotation);
	bsData.Read(dwColor);
	bsData.Read(byteFightingStyle);
	bsData.Read((char*)wUnkData, sizeof(wUnkData));

	Log::addParameter("playerId", playerId);
	Log::addParameter("byteTeam", byteTeam);
	Log::addParameter("iSkin", iSkin);
	Log::addParameter("vecPos.x", vecPos.x);
	Log::addParameter("vecPos.y", vecPos.y);
	Log::addParameter("vecPos.z", vecPos.z);
	Log::addParameter("fRotation", fRotation);
	Log::addParameter("dwColor", dwColor);
	Log::addParameter("byteFightingStyle", byteFightingStyle);
	Log::addParameter("wUnkData[0]", wUnkData[0]);
	Log::addParameter("wUnkData[1]", wUnkData[1]);
	Log::addParameter("wUnkData[2]", wUnkData[2]);
	Log::addParameter("wUnkData[3]", wUnkData[3]);
	Log::addParameter("wUnkData[4]", wUnkData[4]);
	Log::addParameter("wUnkData[5]", wUnkData[5]);
	Log::addParameter("wUnkData[6]", wUnkData[6]);
	Log::addParameter("wUnkData[7]", wUnkData[7]);
	Log::addParameter("wUnkData[8]", wUnkData[8]);
	Log::addParameter("wUnkData[9]", wUnkData[9]);
	Log::addParameter("wUnkData[10]", wUnkData[10]);

	if (pPlayerPool)
	{
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) 
		{
			if (pRemotePlayer->Spawn(byteTeam, iSkin, &vecPos, fRotation, dwColor, byteFightingStyle))
			{
                FLog("WorldPlayerAdd");
				pPlayerPed = pRemotePlayer->GetPlayerPed();
				if (pPlayerPed)
				{
					for (int i = 0; i < 11; i++) {
						//pPlayerPed->SetSkillLevel(i, wUnkData[i]);
					}
				}

				if (pPlayerPool->IsPlayerNPC(playerId)) {
					if (pPlayerPed) {
						pPlayerPed->SetImmunities(0, 1, 1, 0, 0);
					}
				}
			}
			else {
				if (pUI) pUI->chat()->addDebugMessage("Warning: Couldn't spawn player(%u)", playerId);
			}
		}
	}
}
// 0.3.7
void WorldPlayerRemove(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] World player remove");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pRemotePlayer = nullptr;
	PLAYERID playerId;

	bsData.Read(playerId);
	Log::addParameter("playerId", playerId);

	if (pPlayerPool) {
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->Remove();
		}
	}
}
// 0.3.7
void WorldPlayerDeath(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] World player death");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CRemotePlayer *pRemotePlayer = nullptr;
	PLAYERID playerId;

	bsData.Read(playerId);
	Log::addParameter("playerId", playerId);

	if (pPlayerPool) {
		pRemotePlayer = pPlayerPool->GetAt(playerId);
		if (pRemotePlayer) {
			pRemotePlayer->HandleDeath();
		}
	}
}
// 0.3.7
void WorldVehicleAdd(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] World vehicle add");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	NEW_VEHICLE VehicleInfo;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	memset(&VehicleInfo, 0, sizeof(NEW_VEHICLE));

	bsData.Read((char*)& VehicleInfo, sizeof(NEW_VEHICLE));

	Log::addParameter("VehicleInfo.byteAddSiren", VehicleInfo.byteAddSiren);
	Log::addParameter("VehicleInfo.byteInterior", VehicleInfo.byteInterior);
	Log::addParameter("VehicleInfo.byteInteriorColor1", VehicleInfo.byteInteriorColor1);
	Log::addParameter("VehicleInfo.byteInreriorColor2", VehicleInfo.byteInreriorColor2);
	Log::addParameter("VehicleInfo.byteLightDamageStatus", VehicleInfo.byteLightDamageStatus);
	Log::addParameter("VehicleInfo.byteTireDamageStatus", VehicleInfo.byteTireDamageStatus);
	Log::addParameter("VehicleInfo.dwDoorDamageStatus", VehicleInfo.dwDoorDamageStatus);
	Log::addParameter("VehicleInfo.dwPanelDamageStatus", VehicleInfo.dwPanelDamageStatus);
	Log::addParameter("VehicleInfo.fHealth", VehicleInfo.fHealth);
	Log::addParameter("VehicleInfo.fRotation", VehicleInfo.fRotation);
	Log::addParameter("VehicleInfo.iVehicleType", VehicleInfo.iVehicleType);
	Log::addParameter("VehicleInfo.vecPos.x", VehicleInfo.vecPos.x);
	Log::addParameter("VehicleInfo.vecPos.y", VehicleInfo.vecPos.y);
	Log::addParameter("VehicleInfo.vecPos.z", VehicleInfo.vecPos.z);
	Log::addParameter("VehicleInfo.VehicleID", VehicleInfo.VehicleID);

	Log::addParameter("VehicleModinfo.byteModSlots", VehicleInfo.byteModSlots);
	Log::addParameter("VehicleModinfo.bytePaintJob", VehicleInfo.bytePaintJob);
	Log::addParameter("VehicleModinfo.iBodyColor1", VehicleInfo.iBodyColor1);
	Log::addParameter("VehicleModinfo.iBodyColor2", VehicleInfo.iBodyColor2);

	if (VehicleInfo.iVehicleType < 400 || VehicleInfo.iVehicleType > 611) return;

	pVehiclePool->New(&VehicleInfo);
	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleInfo.VehicleID);
	if (!pVehicle) return;

	bool bIsTrain = false;

	if (VehicleInfo.iVehicleType == 537) {
		VehicleInfo.iVehicleType = 569;
		bIsTrain = true;
	}

	if (VehicleInfo.iVehicleType == 538) {
		VehicleInfo.iVehicleType = 570;
		bIsTrain = true;
	}

	if (bIsTrain)
	{
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
		VehicleInfo.VehicleID++;
		pVehiclePool->New(&VehicleInfo);
	}

	/*if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_CAR)
	{
		for (int i = 0; i < 14; i++)
		{
			uint8_t  byteMod = VehicleInfo.byteModSlots[i];
			if (byteMod) {
				pVehicle->AddComponent(byteMod + 999);
			}
		}

		if (VehicleInfo.bytePaintJob) {
			pVehicle->SetPaintJob(VehicleInfo.bytePaintJob - 1);
		}
	}

	if (VehicleInfo.iBodyColor1 != -1 || VehicleInfo.iBodyColor2 != -1) {
		pVehicle->SetColor(VehicleInfo.iBodyColor1, VehicleInfo.iBodyColor2);
	}*/
}
// 0.3.7
void WorldVehicleRemove(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] World vehicle remove");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	VEHICLEID VehicleID;
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	bsData.Read(VehicleID);

	Log::addParameter("VehicleID", VehicleID);

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	CPlayerPed* pPlayerPed = pGame->FindPlayerPed();

	if (pPlayerPed->IsInVehicle())
	{
        CVehicleGTA* pGtaVehicle = pPlayerPed->GetGtaVehicle();
		PLAYERID MyVehicleID = pVehiclePool->FindIDFromGtaPtr(pGtaVehicle);

		if (MyVehicleID == VehicleID)
		{
			RwMatrix matPlayer = pPlayerPed->m_pPed->GetMatrix().ToRwMatrix();
			pPlayerPed->RemoveFromVehicleAndPutAt(
				matPlayer.pos.x,
				matPlayer.pos.y,
				matPlayer.pos.z);
		}
	}

	for (int i = 0; i < MAX_PLAYERS; i++)
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
		if (pRemotePlayer) 
		{
			CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();

			if (pRemotePlayer->GetPlayerPed() &&
				pRemotePlayer->GetState() != PLAYER_STATE_NONE &&
				pRemotePlayer->m_pCurrentVehicle == pVehicle) 
			{
				pRemotePlayer->RemoveFromVehicle();
			}
		}
	}

	for (int i = 0; i < MAX_VEHICLES; i++)
	{
		CVehicle* pVeh = pVehiclePool->GetAt(i);
		if (pVeh) {
			if (pVeh->m_pTrailer == pVehicle)
			{
				pVeh->DetachTrailer();
				pVeh->SetTrailer(0);
			}
		}
	}

	int iModel = pVehicle->m_pVehicle->GetModelId();
	pVehiclePool->Delete(VehicleID);

	if (iModel == 537 || iModel == 538)
	{
		pVehiclePool->Delete(VehicleID + 1);
		pVehiclePool->Delete(VehicleID + 2);
		pVehiclePool->Delete(VehicleID + 3);
	}
}
// 0.3.7
void TimerUpdate(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Timer update");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint32_t dwTime = 0;

	bsData.Read(dwTime);
	Log::addParameter("dwTime", dwTime);

	//pGame->UpdateGlobalTimer(dwTime);
}
// 0.3.7
void ScmEvent(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] SCM event");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	PLAYERID playerId;
	int iEvent;
	uint32_t dwParam1, dwParam2, dwParam3;

	bsData.Read(playerId);
	bsData.Read(iEvent);
	bsData.Read(dwParam1);
	bsData.Read(dwParam2);
	bsData.Read(dwParam3);

	Log::addParameter("playerId", playerId);
	Log::addParameter("iEvent", iEvent);
	Log::addParameter("dwParam1", dwParam1);
	Log::addParameter("dwParam2", dwParam2);
	Log::addParameter("dwParam3", dwParam3);
	
	ProcessIncommingEvent(playerId, iEvent, dwParam1, dwParam2, dwParam3);
}
// 0.3.7
void UpdateScoresPingsIPs(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Update scores and pings");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playerId;
	int iPlayerScore;
	int iPlayerPing;

	for (int i = 0; i < (iBitLength / 8) / 10; i++) {
		bsData.Read(playerId);
		bsData.Read(iPlayerScore);
		bsData.Read(iPlayerPing);

		FLog("RPC_PINGIP: (%d): score: %d, ping: %d", playerId, iPlayerScore, iPlayerPing);

		pPlayerPool->UpdatePlayerScore(playerId, iPlayerScore);
		pPlayerPool->UpdatePlayerPing(playerId, iPlayerPing);
	}
}
// 0.3.7
void Pickup(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Pickup");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iIndex;
	PICKUP Pickup;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iIndex);
	bsData.Read((char*)&Pickup, sizeof(PICKUP));

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) {
		pPickupPool->New(&Pickup, iIndex);
	}
}
// 0.3.7
void DestroyPickup(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Destroy pickup");

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iIndex;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(iIndex);

	CPickupPool *pPickupPool = pNetGame->GetPickupPool();
	if (pPickupPool) {
		pPickupPool->Destroy(iIndex);
	}
}
// 0.3.7
void Create3DTextLabel(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Create 3D text label");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	C3DTextLabelPool* pTextLabelPool = pNetGame->GetTextLabelPool();
	if (pTextLabelPool == nullptr) return;

	uint16_t wLabelId;
	TEXT_LABEL label;
	char szBuffer[2048 + 1];
	memset(szBuffer, 0, sizeof(szBuffer));

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wLabelId);
	bsData.Read(label.dwColor);
	bsData.Read(label.vecPos.x);
	bsData.Read(label.vecPos.y);
	bsData.Read(label.vecPos.z);
	bsData.Read(label.fDistance);
	bsData.Read(label.bTestLOS);
	bsData.Read(label.playerId);
	bsData.Read(label.vehicleId);

	stringCompressor->DecodeString(szBuffer, sizeof szBuffer, &bsData);
	label.text = szBuffer;

	if (wLabelId < MAX_TEXT_LABELS) {
		pTextLabelPool->NewLabel(wLabelId, &label);
	}
}
// 0.3.7
void Update3DTextLabel(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Update 3D text label");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	C3DTextLabelPool* pTextLabelPool = pNetGame->GetTextLabelPool();
	if (pTextLabelPool == nullptr)  return;

	uint16_t wLabelId;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(wLabelId);
	
	if (pTextLabelPool->GetSlotState(wLabelId)) {
			pTextLabelPool->ClearLabel(wLabelId);
	}
}
// 0.3.7
void SetCheckpoint(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Set checkpoint");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVector vecPos;
	float fSize;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(fSize);

	CVector vecSize;
	vecSize.x = fSize;
	vecSize.y = fSize;
	vecSize.z = fSize;

	pGame->SetCheckpointInformation(&vecPos, &vecSize);
	pGame->m_bCheckpointsEnabled = true;
}
// 0.3.7
void DisableCheckpoint(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Disable checkpoint");

	pGame->m_bCheckpointsEnabled = false;
}
// 0.3.7
void SetRaceCheckpoint(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Set race checkpoint");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint8_t byteType;
	CVector vecPos;
	CVector vecNextPos;
	float fRadius;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(byteType);
	bsData.Read(vecPos.x);
	bsData.Read(vecPos.y);
	bsData.Read(vecPos.z);
	bsData.Read(vecNextPos.x);
	bsData.Read(vecNextPos.y);
	bsData.Read(vecNextPos.z);
	bsData.Read(fRadius);

	pGame->SetRaceCheckpointInformation(byteType, &vecPos, &vecNextPos, fRadius);
	pGame->m_bRaceCheckpointsEnabled = true;
}
// 0.3.7
void DisableRaceCheckpoint(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Disable race checkpoint");

	pGame->m_bRaceCheckpointsEnabled = false;
}
// 0.3.7
void DamageVehicle(RPCParameters *rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Damage vehicle");

	RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

	VEHICLEID vehId;
	bsData.Read(vehId);
	if (pNetGame->GetVehiclePool()->GetSlotState(vehId))
	{
		uint32_t dwPanelStatus, dwDoorStatus;
		uint8_t byteLightStatus, byteTireStatus;

		bsData.Read(dwPanelStatus);
		bsData.Read(dwDoorStatus);
		bsData.Read(byteLightStatus);
		bsData.Read(byteTireStatus);

		CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(vehId);
		if (pVehicle)
		{
			pVehicle->SetWheelPoppedStatus(byteTireStatus);
			pVehicle->UpdateDamageStatus(dwPanelStatus, dwDoorStatus, byteLightStatus);
		}
	}
}

void SetVehicleTireStatus(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

	VEHICLEID vehicleId;
	uint8_t byteTire;

	bsData.Read(vehicleId);
	if(vehicleId < 0 || vehicleId >= MAX_VEHICLES)
		return;

	bsData.Read(byteTire);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
	{
		CVehicle *pVehicle = pVehiclePool->GetAt(vehicleId);
		if(pVehicle)
			pVehicle->SetWheelPoppedStatus(byteTire);
	}
}

// 0.3.7
void EnterVehicle(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Enter vehicle");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	uint8_t bytePassenger;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
    if(PlayerID < 0 || PlayerID >= MAX_PLAYERS)
        return;

	bsData.Read(VehicleID);
    if(VehicleID < 0 || VehicleID >= MAX_VEHICLES)
        return;

	bsData.Read(bytePassenger);

	if (bytePassenger) {
		bytePassenger = 1;
	}

	CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(PlayerID);
	if (pRemotePlayer) {
		pRemotePlayer->EnterVehicle(VehicleID, bytePassenger);
	}
}
// 0.3.7
void ExitVehicle(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Exit vehicle");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID PlayerID;
	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(PlayerID);
	bsData.Read(VehicleID);

	CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(PlayerID);
	if (pRemotePlayer) {
		if (pRemotePlayer->GetDistanceFromLocalPlayer() < 200.0f)
			pRemotePlayer->ExitVehicle();
	}
}
// 0.3.7
void VehicleParamsEx(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] VehicleParamsEx");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
	if (!pVehiclePool) return;

	VEHICLEID VehicleID;
	VEHICLE_PARAMS_EX vehParamsEx;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	bsData.Read((char*)& vehParamsEx, sizeof(VEHICLE_PARAMS_EX));

	CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
	if (!pVehicle) return;

	pVehicle->ApplyEngineState(vehParamsEx.byteEngine);

	pVehicle->ApplyLightState(vehParamsEx.byteLight);

	if (vehParamsEx.byteDoors)
	{
		if (vehParamsEx.byteDoors == 1) {
			pVehicle->SetDoorState(true);
		}
	}
	else {
		pVehicle->SetDoorState(false);
	}

	if (vehParamsEx.byteObjective)
	{
		if (vehParamsEx.byteObjective == 1)
			pVehicle->m_byteObjectiveVehicle = 1;
	}
	else
	{
		pVehicle->m_byteObjectiveVehicle = 0;
	}

	if (vehParamsEx.byteBoot)
	{
		if (vehParamsEx.byteBoot == 1)
			pVehicle->SetComponentOpenState(1, 17, 1.0f);
	}
	else
	{
		pVehicle->SetComponentOpenState(1, 17, 0);
	}

	if (vehParamsEx.byteBonnet)
	{
		if (vehParamsEx.byteBonnet == 1)
			pVehicle->SetComponentOpenState(0, 16, 1.0);
	}
	else
	{
		pVehicle->SetComponentOpenState(0, 16, 0.0);
	}

	if (vehParamsEx.byteAlarm)
	{
		if (vehParamsEx.byteAlarm == 1)
		{
			pVehicle->EnableSiren(true);
			pVehicle->SetAlarmState(20000);
		}
	}
	else
	{
		pVehicle->EnableSiren(false);
		pVehicle->SetAlarmState(0);
	}

	if (vehParamsEx.byteSiren)
	{
		if (vehParamsEx.byteSiren == 1)
			pVehicle->EnableSiren(true);
	}
	else
	{
		pVehicle->EnableSiren(false);
	}

	if (vehParamsEx.byteDoor1)
	{
		if (vehParamsEx.byteDoor1 == 1)
			pVehicle->SetComponentOpenState(2, 10, 1.0);
	}
	else
	{
		pVehicle->SetComponentOpenState(2, 10, 0.0);
	}

	if (vehParamsEx.byteDoor2)
	{
		if (vehParamsEx.byteDoor2 == 1)
			pVehicle->SetComponentOpenState(3, 8, 1.0);
	}
	else
	{
		pVehicle->SetComponentOpenState(3, 8, 0.0);
	}

	if (vehParamsEx.byteDoor3)
	{
		if (vehParamsEx.byteDoor3 == 1)
			pVehicle->SetComponentOpenState(4, 11, 1.0);
	}
	else
	{
		pVehicle->SetComponentOpenState(4, 11, 0.0);
	}

	if (vehParamsEx.byteDoor4)
	{
		if (vehParamsEx.byteDoor4 == 1)
			pVehicle->SetComponentOpenState(5, 9, 1.0);
	}
	else
	{
		pVehicle->SetComponentOpenState(5, 9, 0.0);
	}

	if (vehParamsEx.byteWindow1)
	{
		if (vehParamsEx.byteWindow1 == 1)
			pVehicle->CloseWindow(10);
	}
	else
	{
		pVehicle->OpenWindow(10);
	}

	if (vehParamsEx.byteWindow2)
	{
		if (vehParamsEx.byteWindow2 == 1)
			pVehicle->CloseWindow(8);
	}
	else
	{
		pVehicle->OpenWindow(8);
	}

	if (vehParamsEx.byteWindow3)
	{
		if (vehParamsEx.byteWindow3 == 1)
			pVehicle->CloseWindow(11);
	}
	else
	{
		pVehicle->OpenWindow(11);
	}

	if (vehParamsEx.byteWindow4)
	{
		if (vehParamsEx.byteWindow4 == 1)
			pVehicle->CloseWindow(9);
	}
	else
	{
		pVehicle->OpenWindow(9);
	}
}
// 0.3.7
void ShowActor(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Show actor");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	NEW_ACTOR newActor;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read((char*)& newActor, sizeof(NEW_ACTOR));

	if (IsValidPedModel(newActor.iSkin))
	{
		pActorPool->New(&newActor);
	}
}
// 0.3.7
void HideActor(RPCParameters* rpcParams)
{
	Log::traceLastFunc("[RPC-IN] Hide actor");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CActorPool* pActorPool = pNetGame->GetActorPool();
	if (!pActorPool) return;

	PLAYERID ActorID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ActorID);

	pActorPool->Delete(ActorID);
}

void ChatBubble(RPCParameters* rpcParams)
{
	Log::traceLastFunc("RPC: ChatBubble");

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerBubblePool* pPlayerBubblePool = pNetGame->GetPlayerBubblePool();
	if (pPlayerBubblePool == nullptr) return;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	uint32_t color;
	float drawDistance;
	uint32_t duration;
	uint8_t byteTextLen;

	char szText[256];
	memset(szText, 0, 256);

	bsData.Read(playerId);

	if (playerId == pNetGame->GetPlayerPool()->GetLocalPlayerID()) return;
	if (playerId >= MAX_PLAYERS) return;

	bsData.Read(color);
	bsData.Read(drawDistance);
	bsData.Read(duration);
	bsData.Read(byteTextLen);
	bsData.Read((char*)szText, byteTextLen);
	szText[byteTextLen] = '\0';

	pPlayerBubblePool->New(playerId, szText, color, drawDistance, duration);
}

void SetPlayerSkillLevel(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID playerId;
	uint32_t ucSkillType;
	uint16_t uiSkillLevel;

	bsData.Read(playerId);
	bsData.Read(ucSkillType);
	bsData.Read(uiSkillLevel);

	if(ucSkillType < 0 || ucSkillType > 10)
		return;
	if(uiSkillLevel < 0 || uiSkillLevel > 1000)
		return;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool)
	{
		if(playerId == pPlayerPool->GetLocalPlayerID())
			pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetWeaponSkill(ucSkillType, uiSkillLevel);
		else if(pPlayerPool->GetSlotState(playerId))
			pPlayerPool->GetAt(playerId)->GetPlayerPed()->SetWeaponSkill(ucSkillType, uiSkillLevel);;
	}
}

#include "../java/editobject.h"
void EditAttachedObject(RPCParameters *rpcParams) {
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	uint32_t index;

	bsData.Read(index);

	CObjectEditor::startEditPlayerAttach(index);

	FLog("RPC: EditAttachedObject %d", index);
}

void EditObject(RPCParameters *rpcParams) {
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);

	bool bPlayerObj;
	uint16_t objectId;

	bsData.Read(bPlayerObj);
	bsData.Read(objectId);

	CObjectEditor::startEditObject(objectId);

	FLog("RPC: EditObject %d", objectId);
}

void RegisterRPCs(RakClientInterface *pRakClient)
{
	Log::traceLastFunc("Registering RPC's..");
    pRakClient->RegisterAsRemoteProcedureCall(&RPC_InitGame, InitGame);

	// RPC_SetPlayerSkillLevel
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetPlayerSkillLevel, SetPlayerSkillLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel, Create3DTextLabel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel, Update3DTextLabel);
	//pRakClient->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ChatBubble);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DialogBox, DialogBox);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetCheckpoint, SetCheckpoint);					// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableCheckpoint, DisableCheckpoint);			// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetRaceCheckpoint, SetRaceCheckpoint);			// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DisableRaceCheckpoint, DisableRaceCheckpoint);	// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs, UpdateScoresPingsIPs);
	// RPC_SvrStats - useless
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_GameModeRestart, GameModeRestart);				// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ConnectionRejected, ConnectionRejected);			// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClientMessage, ClientMessage);					// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldTime, WorldTime);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Pickup, Pickup);									// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DestroyPickup, DestroyPickup);					// DONE
	// RPC_DestroyWeaponPickup - unused in 037
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScmEvent, ScmEvent);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Weather, Weather);								// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetTimeEx, SetTimeEx);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ToggleClock, ToggleClock);						// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_TimerUpdate, TimerUpdate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd, WorldPlayerAdd);					// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath, WorldPlayerDeath);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove, WorldPlayerRemove);			// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd, WorldVehicleAdd);				// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove, WorldVehicleRemove);			// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_DamageVehicle, DamageVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleTireStatus, SetVehicleTireStatus);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_VehicleParamsEx, VehicleParamsEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EnterVehicle, EnterVehicle);						// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ExitVehicle, ExitVehicle);						// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerJoin, ServerJoin);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ServerQuit, ServerQuit);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_Chat, Chat);										// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestClass, RequestClass);						// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_RequestSpawn, RequestSpawn);						// DONE
	// RPC_OnPlayerEditAttachedObject
	// RPC_EditObject
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditAttachedObject, EditAttachedObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_EditObject, EditObject);
	// RPC_EnterEditObject
	// RPC_CancelEdit
	// RPC_SetTargeting
	// RPC_ClientCheckResponse
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ShowActor, ShowActor);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_HideActor, HideActor);							// DONE
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ChatBubble, ChatBubble);
}

void UnregisterRPCs(RakClientInterface *pRakClient)
{
	Log::traceLastFunc("Unregistering RPC's..");
    pRakClient->UnregisterAsRemoteProcedureCall(&RPC_InitGame);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetPlayerSkillLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreate3DTextLabel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrUpdate3DTextLabel);
	//pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ChatBubble);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DialogBox);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DisableCheckpoint);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_UpdateScoresPingsIPs);
	// RPC_SvrStats - useless
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_GameModeRestart);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ConnectionRejected);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClientMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldTime);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Pickup);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DestroyPickup);
	// RPC_DestroyWeaponPickup - unused in 037
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScmEvent);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Weather);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetTimeEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ToggleClock);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_TimerUpdate);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerDeath);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldPlayerRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleAdd);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_WorldVehicleRemove);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_DamageVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleTireStatus);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_VehicleParamsEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_EnterVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ExitVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerJoin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ServerQuit);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_Chat);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestClass);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_RequestSpawn);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ShowActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_HideActor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ChatBubble);
}