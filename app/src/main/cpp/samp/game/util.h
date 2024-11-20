#pragma once

#include "../game/RW/RenderWare.h"

class CObject;
#include "../game/Core/Rect.h"

enum eWidgetType {
    TYPE_NONE = 0,
	TYPE_PUNCH,
	TYPE_SPRINT,
	TYPE_ENTERCAR,
	TYPE_ACCELERATE,
    TYPE_BRAKE,
    TYPE_SIZE
};

enum eWidgetState {
    STATE_NONE,
    STATE_FIXED
};

extern uintptr_t g_pWidgets[TYPE_SIZE];

PED_TYPE* GamePool_FindPlayerPed();
PED_TYPE* GamePool_Ped_GetAt(int iID);
int GamePool_Ped_GetIndex(PED_TYPE* pActor);
ENTITY_TYPE *GamePool_Object_GetAt(int iID);
uintptr_t GamePool_Vehicle_GetIndex(VEHICLE_TYPE* pGtaVehicle);
VEHICLE_TYPE* GamePool_Vehicle_GetAt(int iID);

int GetVehicleSubtype(VEHICLE_TYPE* pGtaVehicle);

uintptr_t GetModelInfoByID(int iModelID);
bool IsExistInfoForModel(int iModelID);
bool IsValidModel(int iModelID);
int GetModelRefCounts(int iModel);
bool IsValidPedModel(uint modelID);
uintptr_t GetModelRWObject(uint modelID);

//uintptr_t LoadTexture(const char* texname);

RwTexture* LoadTextureFromTxd(const char* txdname, const char* texturename);
int FindTxdSlot(const char* txdname);
void PushCurrentTxd();
void SetCurrentTxd(int slot);
void PopCurrentTxd();
uintptr_t SetTexture(const char* texturename);

float FloatOffset(float f1, float f2);
float GetDistance(RwV3d vec1, RwV3d vec2);
float DegToRad(float fDegress);
float fixAngle(float angle);
float subAngle(float a1, float a2);

void SetPlayerPedPtrRecord(uint8_t bytePlayer, PED_TYPE* pPed);
uint8_t FindPlayerNumFromPedPtr(PED_TYPE* dwPedPtr);

int GameGetWeaponModelIDFromWeaponID(int iWeaponID);

int GetFreeTextDrawTextureSlot();
void ResetTextDrawTextures();
void DestroyTextDrawTexture(int index);

void DrawTexture(uintptr_t texture, CRect* rect, uint32_t dwColor);
void DrawTextureUV(uintptr_t texture, CRect* rect, uint32_t dwColor, float* uv);
void DeleteRwTexture(uintptr_t texture);

bool IsPointInRect(float x, float y, CRect* rect);

uintptr_t ModelInfoCreateInstance(int iModel);

void RenderClumpOrAtomic(uintptr_t rwObject);

void RwFrameTranslate(uintptr_t frame, RwV3d* vec, int flag);
void RwFrameRotate(uintptr_t frame, int axis, float angle);

float GetModelColSphereRadius(int iModel);

void GetModelColSphereVecCenter(int iModel, RwV3d* vec);

void DestroyAtomicOrClump(uintptr_t rwObject);

void RpWorldAddLight(uintptr_t light);
void RpWorldRemoveLight(uintptr_t light);

void GamePrepareTrain(VEHICLE_TYPE* pGtaVehicle);

void DrawRaster(RwRaster* raster, CRect const& rect);

void GameResetStats();

void ProjectMatrix(RwV3d* vecOut, RwMatrix* mat, RwV3d* vecPos);
void RwMatrixRotate(RwMatrix* mat, int axis, float angle);
void RwMatrixScale(RwMatrix* mat, RwV3d* vecScale);

const char* getGameDataFolderDirectory();

int LineOfSight(RwV3d* start, RwV3d* end, void* colpoint, uintptr_t ent, char buildings, char vehicles, char peds, char objects, char dummies, bool seeThrough, bool camera, bool unk);
void RwMatrixInvert(RwMatrix *matOut, RwMatrix *matIn);

int GetTaskTypeFromTask(uint32_t *task);
int Game_PedStatPrim(int model_id);

eWidgetType GetWidgetTypeFromWidget(uintptr_t pWidget);
void SetWidgetFromName(const char* name, uintptr_t pWidget);
void SetWidgetFromId(int idWidget, uintptr_t pWidget);
eWidgetState ProcessFixedWidget(uintptr_t pWidget); 
eWidgetState ProcessFixedWidgetFromId(int iWidgetId);

uint32_t GetUpperCaseKey(const char* szUpper);

RwMatrix* mat_invert(RwMatrix *dst, const RwMatrix *src);

const char* GetAnimByIdx(int idx);
int GetAnimIdxByName(const char* szName);

bool IsGameEntityArePlaceable(ENTITY_TYPE *pEntity);

void RemoveBuilding(uint32_t dwModel, RwV3d vecPos, float fRange);
void RemoveObjectInRange(int iModel, RwV3d vecPos, float fRange);
void RemoveOccludersInRadius(RwV3d vecPos, float fRadius);

RwTexture* LoadTextureFromTxd(const char* txdname, const char* texturename);
uintptr_t LoadTexture(const char* texname);

RpMaterial* ObjectMaterialCallBack(RpMaterial* material, void* data);
RpAtomic* ObjectMaterialTextCallBack(RpAtomic* rpAtomic, CObject* pObject);

bool GetAnimationIndexFromName(const char* szName);

void SetScissorRect(void* pRect);