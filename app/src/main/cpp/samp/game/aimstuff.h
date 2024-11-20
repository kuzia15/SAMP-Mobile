#pragma once

extern uint8_t* pbyteCameraMode;
extern float* pfCameraExtZoom;
extern uint8_t* pbyteCurrentPlayer;
extern uint16_t* wCameraMode2;
extern float *pPlayerStats;

#pragma pack(push, 1)
typedef struct _CAMERA_AIM
{
	float f1x, f1y, f1z;
	float pos1x, pos1y, pos1z;
	float pos2x, pos2y, pos2z;
	float f2x, f2y, f2z;
} CAMERA_AIM;
#pragma pack(pop)

void GameAimSyncInit();
CAMERA_AIM* GameGetInternalAim();
float GameGetAspectRatio();

/* CameraMode */
uint8_t GameGetLocalPlayerCameraMode();
void GameSetPlayerCameraMode(uint8_t byteMode, uint8_t bytePlayerNumber);
uint8_t GameGetPlayerCameraMode(uint8_t bytePlayerNumber);

/* Aim */
CAMERA_AIM* GameGetRemotePlayerAim(uint8_t bytePlayerNumber);
void GameSetLocalPlayerAim();
void GameStoreLocalPlayerAim();
void GameSetRemotePlayerAim(uint8_t bytePlayerNumber);
void GameStoreRemotePlayerAim(uint8_t bytePlayerNumber, CAMERA_AIM* pAim);

/*ExtZoom & AspectRatio*/
float GameGetLocalPlayerCameraExtZoom();
void GameSetLocalPlayerCameraExtZoomAndAspect();
void GameSetRemotePlayerCameraExtZoomAndAspect(uint8_t bytePlayerNumber);
void GameStoreLocalPlayerCameraExtZoomAndAspect();
void GameSetPlayerCameraExtZoomAndAspect(uint8_t bytePlayerNumber, float fExtZoom, float fAspectRatio);

void calculateAimVector(CVector* vec1, CVector* vec2);

void GameResetLocalPlayerSkills();
void GameUpdateLocalPlayerSkill(int iSkillType, uint16_t wSkillLevel);
void GameSetLocalPlayerSkills();
void GameStoreLocalPlayerSkills();
void GameSetRemotePlayerSkills(uint8_t bytePlayerNumber);
void GameStoreRemotePlayerSkills(uint8_t bytePlayerNumber, int iSkillType, uint16_t wSkillLevel);