#include <GLES2/gl2.h>
#include "../main.h"
#include "../vendor/armhook/patch.h"
#include "game.h"
#include "../net/netgame.h"
#include "../gui/gui.h"
#include "Textures/TextureDatabase.h"
#include "Textures/TextureDatabaseEntry.h"
#include "Textures/TextureDatabaseRuntime.h"
#include "Scene.h"
#include "sprite2d.h"
#include "Entity/PlayerPedGta.h"
#include "Pools.h"
#include "java/jniutil.h"
#include "game/Models/ModelInfo.h"
#include "MatrixLink.h"
#include "MatrixLinkList.h"
#include "game/Collision/Collision.h"
#include "TxdStore.h"
#include "util/CUtil.h"
#include "Coronas.h"
#include "multitouch.h"
#include "Streaming.h"
#include "References.h"
#include "VisibilityPlugins.h"
#include "game/Animation/AnimManager.h"
#include "FileLoader.h"
#include "Renderer.h"
#include "CrossHair.h"

extern UI* pUI;
extern CGame* pGame;
extern CNetGame *pNetGame;
extern MaterialTextGenerator* pMaterialTextGenerator;

uint8_t byteInternalPlayer = 0;
CPedGTA* dwCurPlayerActor = 0;
uint8_t byteCurPlayer = 0;

extern "C" uintptr_t get_lib()
{
	return g_libGTASA;
}
// 0.3.7
PLAYERID FindPlayerIDFromGtaPtr(CEntityGTA* pEntity)
{
	if (pEntity == nullptr) return INVALID_PLAYER_ID;

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();

	PLAYERID PlayerID = pPlayerPool->FindRemotePlayerIDFromGtaPtr((CPedGTA*)pEntity);
	if (PlayerID != INVALID_PLAYER_ID) return PlayerID;

	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((CVehicleGTA*)pEntity);
	if (VehicleID != INVALID_VEHICLE_ID)
	{
		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(i);
			if (pRemotePlayer && pRemotePlayer->CurrentVehicleID() == VehicleID) {
				return i;
			}
		}
	}

	return INVALID_PLAYER_ID;
}
// 0.3.7
PLAYERID FindActorIDFromGtaPtr(CPedGTA* pPed)
{
	if (pPed) {
		return pNetGame->GetActorPool()->FindIDFromGtaPtr(pPed);
	}

	return INVALID_PLAYER_ID;
}

/* =============================================================================== */

/* =============================================================================== */

void CStream_InitImageList()
{
	FLog("Initializing ImageList..");

    char* ms_files = (char*)(g_libGTASA+(VER_x32?0x792EC8:0x972BEC));
    ms_files[0] = 0;
    *(uint32_t*)&ms_files[44] = 0;
    ms_files[48] = 0;
    *(uint32_t*)&ms_files[92] = 0;
    ms_files[96] = 0;
    *(uint32_t*)&ms_files[140] = 0;
    ms_files[144] = 0;
    *(uint32_t*)&ms_files[188] = 0;
    ms_files[192] = 0;
    *(uint32_t*)&ms_files[236] = 0;
    ms_files[240] = 0;
    *(uint32_t*)&ms_files[284] = 0;
    ms_files[288] = 0;
    *(uint32_t*)&ms_files[332] = 0;
    ms_files[336] = 0;
    *(uint32_t*)&ms_files[380] = 0;

	// CStreaming::AddImageToList x32 2CF7D0
	((uintptr_t(*)(const char*, int))(g_libGTASA + (VER_x32 ? 0x2CF7D0 + 1:0x39155C)))("TEXDB\\SAMPCOL.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + (VER_x32 ? 0x2CF7D0 + 1:0x39155C)))("TEXDB\\GTA3.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + (VER_x32 ? 0x2CF7D0 + 1:0x39155C)))("TEXDB\\GTA_INT.IMG", 1);
	((uintptr_t(*)(const char*, int))(g_libGTASA + (VER_x32 ? 0x2CF7D0 + 1:0x39155C)))("TEXDB\\SAMP.IMG", 1);
}

/* =============================================================================== */

void RenderEffects() {
//	RenderEffects();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0059DA40 + 1 : 0x6C1D6C));
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(TRUE));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005BE914 + 1 : 0x6E2FB4));
//    CRopes::Render();
//    CGlass::Render();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005A6BC8 + 1 : 0x6CA5D0));
    CVisibilityPlugins::RenderReallyDrawLastObjects();
    CCoronas::Render();

    // FIXME
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));
    auto g_fx = *(uintptr_t *) (g_libGTASA + (VER_x32 ? 0x00820520 : 0xA062A8));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x00363DF0 + 1 : 0x433F54), &g_fx, TheCamera.m_pRwCamera, false);

    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005CBBAC + 1 : 0x6F054C));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0059BF84 + 1 : 0x6C0268));
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005A1C38 + 1 : 0x6C552C));
    //   CClouds::VolumetricCloudsRender();
////    if (CHeli::NumberOfSearchLights || CTheScripts::NumberOfScriptSearchLights) {
////        CHeli::Pre_SearchLightCone();
////        CHeli::RenderAllHeliSearchLights();
////        CTheScripts::RenderAllSearchLights();
////        CHeli::Post_SearchLightCone();
////    }
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005E3390 + 1 : 0x708DF0));
////    if (CReplay::Mode != MODE_PLAYBACK && !CPad::GetPad(0)->DisablePlayerControls) {
////        FindPlayerPed()->DrawTriangleForMouseRecruitPed();
////    }
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005C0B14 + 1 : 0x6E50CC));
//    //CVehicleRecording::Render();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005B19D0 + 1 : 0x6D6068));
//    //CRenderer::RenderFirstPersonVehicle();
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x005B5F78 + 1 : 0x6DA2B8));

    //DebugModules::Render3D();
}

void MainLoop();
void Render2dStuff()
{
    if( CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x001BB7F4 + 1 : 0x24EA90)) ) // emu_IsAltRenderTarget()
        CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x001BC20C + 1 : 0x24F5B8)); // emu_FlushAltRenderTarget()

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    if (pUI) pUI->render();

    CCrossHair::Render();

    ((void (*)()) (g_libGTASA + (VER_x32 ? 0x00437B0C + 1 : 0x51CFF0)))(); // CHud::DrawRadar
    //	GPS::Draw();
    //
    ( ( void(*)(bool) )(g_libGTASA + (VER_x32 ? 0x002B0BD8 + 1 : 0x36FB00)) )(false); // CTouchInterface::DrawAll

    //CSpecialFX::Render2DFXs(void)	00000000006E5A78
    //CSpecialFX::Render2DFXs(void)	005C156C
    CHook::CallFunction<void>(g_libGTASA+(VER_x32?0x1C0750+1:0x252CE4), 1);
    CHook::CallFunction<void>(g_libGTASA+(VER_x32?0x5C156C:0x6E5A78));

    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x0054BDD4 + 1 : 0x66B678)))(1u); // CMessages::Display - gametext
    ((void (*)(bool)) (g_libGTASA + (VER_x32 ? 0x005A9120 + 1 : 0x6CCEA0)))(1u); // CFont::RenderFontBuffer
    CHook::CallFunction<void>(g_libGTASA+(VER_x32?0x1C0750+1:0x252CE4), 0);
}

void CPools_Initialise(void)
{
    LOGI("GTA pools initializing..");

    struct PoolAllocator {

        struct Pool {
            void* objects;
            uint8_t* flags;
            uint32_t count;
            uint32_t top;
            uint32_t bInitialized;
        };
        static_assert(sizeof(Pool) == (VER_x32 ? 0x14 : 0x20));

        static Pool* Allocate(size_t count, size_t size) {

            Pool *p = new Pool;

            p->objects = new char[size*count];
            p->flags = new uint8_t[count];
            p->count = count;
            p->top = 0xFFFFFFFF;
            p->bInitialized = 1;

            for (size_t i = 0; i < count; i++) {
                p->flags[i] |= 0x80;
                p->flags[i] &= 0x80;
            }

            return p;
        }
    };

    // 600000 / 75000 = 8
    static auto ms_pPtrNodeSingleLinkPool	= PoolAllocator::Allocate(100000,	(VER_x32 ? 0x8 : 0x10));		// 75000
    // 72000 / 6000 = 12
    static auto ms_pPtrNodeDoubleLinkPool	= PoolAllocator::Allocate(60000,	(VER_x32 ? 0xC : 0x18));	// 6000
    // 10000 / 500 = 20
    static auto ms_pEntryInfoNodePool		= PoolAllocator::Allocate(20000,	(VER_x32 ? 0x14 : 0x28));	// 500
    // 279440 / 140 = 1996
    static auto ms_pPedPool					= PoolAllocator::Allocate(240,		(VER_x32 ? 0x7CC : 0x9C8));	// 140
    // 286440 / 110 = 2604
    static auto ms_pVehiclePool				= PoolAllocator::Allocate(2000,		(VER_x32 ? 0xA2C : 0xC60));	// 110
    // 840000 / 14000 = 60
    static auto ms_pBuildingPool			= PoolAllocator::Allocate(20000,	(VER_x32 ? 0x3C : 0x60));	// 14000
    // 147000 / 350 = 420
    static auto ms_pObjectPool				= PoolAllocator::Allocate(3000,		(VER_x32 ? 0x1A4 : 0x220));	// 350
    // 210000 / 3500 = 60
    static auto ms_pDummyPool				= PoolAllocator::Allocate(40000,	(VER_x32 ? 0x3C : 0x60));	// 3500
    // 487200 / 10150 = 48
    static auto ms_pColModelPool			= PoolAllocator::Allocate(50000,	(VER_x32 ? 0x30 : 0x38));	// 10150
    // 64000 / 500 = 128
    static auto ms_pTaskPool				= PoolAllocator::Allocate(6000,		(VER_x32 ? 0x80 : 0x90));	// 500
    // 13600 / 200 = 68
    static auto ms_pEventPool				= PoolAllocator::Allocate(1000,		(VER_x32 ? 0x44 : 0x58));	// 200
    // 6400 / 64 = 100
    static auto ms_pPointRoutePool			= PoolAllocator::Allocate(200,		(VER_x32 ? 0x64 : 0x64));	// 64
    // 13440 / 32 = 420
    static auto ms_pPatrolRoutePool			= PoolAllocator::Allocate(200,		420);	// 32
    // 2304 / 64 = 36
    static auto ms_pNodeRoutePool			= PoolAllocator::Allocate(200,		(VER_x32 ? 0x24 : 0x24));	// 64
    // 512 / 16 = 32
    static auto ms_pTaskAllocatorPool		= PoolAllocator::Allocate(3000,		32);	// 16
    // 92960 / 140 = 664
    static auto ms_pPedIntelligencePool		= PoolAllocator::Allocate(240,		(VER_x32 ? 0x298 : 0x440));	// 140
    // 15104 / 64 = 236
    static auto ms_pPedAttractorPool		= PoolAllocator::Allocate(200,		(VER_x32 ? 0xEC : 0xEC));	// 64

    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93E0*/(VER_x32 ? 0x95AC38 : 0xBC3BA0)) = ms_pPtrNodeSingleLinkPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93DC*/(VER_x32 ? 0x95AC3C : 0xBC3BA8)) = ms_pPtrNodeDoubleLinkPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D8*/(VER_x32 ? 0x95AC40 : 0xBC3BB0)) = ms_pEntryInfoNodePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D4*/(VER_x32 ? 0x95AC44 : 0xBC3BB8)) = ms_pPedPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93D0*/(VER_x32 ? 0x95AC48 : 0xBC3BC0)) = ms_pVehiclePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93CC*/(VER_x32 ? 0x95AC4C : 0xBC3BC8)) = ms_pBuildingPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C8*/(VER_x32 ? 0x95AC50 : 0xBC3BD0)) = ms_pObjectPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C4*/(VER_x32 ? 0x95AC54 : 0xBC3BD8)) = ms_pDummyPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93C0*/(VER_x32 ? 0x95AC58 : 0xBC3BE0)) = ms_pColModelPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93BC*/(VER_x32 ? 0x95AC5C : 0xBC3BE8)) = ms_pTaskPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B8*/(VER_x32 ? 0x0095AC60 : 0xBC3BF0)) = ms_pEventPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B4*/(VER_x32 ? 0x0095AC64 : 0xBC3BF8)) = ms_pPointRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93B0*/(VER_x32 ? 0x0095AC68 : 0xBC3C00)) = ms_pPatrolRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93AC*/(VER_x32 ? 0x0095AC6C : 0xBC3C08)) = ms_pNodeRoutePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A8*/(VER_x32 ? 0x0095AC70 : 0xBC3C10)) = ms_pTaskAllocatorPool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A4*/(VER_x32 ? 0x0095AC74 : 0xBC3C18)) = ms_pPedIntelligencePool;
    *(PoolAllocator::Pool**)(g_libGTASA + /*0x8B93A0*/(VER_x32 ? 0x0095AC78 : 0xBC3C20)) = ms_pPedAttractorPool;
}

/* =============================================================================== */

int (*CRadar__SetCoordBlip)(int r0, float X, float Y, float Z, int r4, int r5, char* name);
int CRadar__SetCoordBlip_hook(int r0, float X, float Y, float Z, int r4, int r5, char* name)
{
	if(pNetGame && !strncmp(name, "CODEWAY", 7))
	{
		float fFindZ = pGame->FindGroundZForCoord(X, Y, Z) + 1.5f;

		if(pNetGame->GetGameState() != GAMESTATE_CONNECTED) return 0;

		RakNet::BitStream bsSend;
		bsSend.Write(X);
		bsSend.Write(Y);
		bsSend.Write(fFindZ);
		pNetGame->GetRakClient()->RPC(&RPC_MapMarker, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
	}

	return CRadar__SetCoordBlip(r0, X, Y, Z, r4, r5, name);
}

/* =============================================================================== */

void(*CRadar_DrawRadarGangOverlay)(uint32_t unk);
void CRadar_DrawRadarGangOverlay_hook(uint32_t unk)
{
	/*if (pNetGame)
	{
		CGangZonePool *pGangZonePool = pNetGame->GetGangZonePool();
		if (pGangZonePool) {
			pGangZonePool->Draw(unk);
		}
	}*/
}

/* =============================================================================== */

typedef struct {
    CVector     vecPosObject;
    CQuaternion m_qRotation;
    int32       wModelIndex;
    union {
        struct { // CFileObjectInstanceType
            uint32 m_nAreaCode : 8;
            uint32 m_bRedundantStream : 1;
            uint32 m_bDontStream : 1; // Merely assumed, no countercheck possible.
            uint32 m_bUnderwater : 1;
            uint32 m_bTunnel : 1;
            uint32 m_bTunnelTransition : 1;
            uint32 m_nReserved : 19;
        };
        uint32 m_nInstanceType;
    };
    int32 m_nLodInstanceIndex; // -1 - without LOD model
} stLoadObjectInstance;
VALIDATE_SIZE(stLoadObjectInstance, (VER_x32 ? 0x28 : 0x28));

extern int iBuildingToRemoveCount;
extern REMOVEBUILDING_DATA BuildingToRemove[1000];

int (*CFileLoader__LoadObjectInstance)(stLoadObjectInstance *thiz);
int CFileLoader__LoadObjectInstance_hook(stLoadObjectInstance *thiz) {
	if (thiz) {
		if (iBuildingToRemoveCount >= 1) {
			for (int i = 0; i < iBuildingToRemoveCount; i++)
			{
				float fDistance = GetDistance(BuildingToRemove[i].vecPos, thiz->vecPosObject);
				if (fDistance <= BuildingToRemove[i].fRange) {
					if (BuildingToRemove[i].dwModel == -1 || thiz->wModelIndex == (uint16_t) BuildingToRemove[i].dwModel) {
						thiz->wModelIndex = 19300;
						break;
					}
				}
			}
		}
	}

	return CFileLoader__LoadObjectInstance(thiz);
}

/* =============================================================================== */

uint32_t dwParam1, dwParam2;
extern "C" void pickup_pickedup()
{
	if (pNetGame && pNetGame->GetPickupPool())
	{
		CPickupPool *pPickups = pNetGame->GetPickupPool();
		pPickups->PickedUp(((dwParam1 - (g_libGTASA + /*0x70E264*/0x7AFD70)) / 0x20));
	}
}

void (*CPlaceable_InitMatrixArray)(void);
void CPlaceable_InitMatrixArray_hook(void)
{
	// CMatrixLinkList::Init
	((void (*)(uintptr_t, size_t))(g_libGTASA + 0x407F84 + 1))(g_libGTASA + 0x95A988, 10000);
}

/* =============================================================================== */

void (*CObject_Render)(uintptr_t thiz);
void CObject_Render_hook(uintptr_t thiz)
{
	CPhysical *object = (CPhysical*)thiz;
	if(pNetGame && object != 0)
	{
		CObject *pObject = pNetGame->GetObjectPool()->FindObjectFromGtaPtr(object);
		if(pObject)
		{
			RwObject* rwObject = (RwObject*)pObject->m_pEntity->m_pRwObject;
			if(rwObject)
			{
				// SetObjectMaterial
				if(pObject->m_bHasMaterial)
				{
					((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
					//RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialCallBack, pObject);
					RpAtomic* atomic = (RpAtomic*)object->m_pRwAtomic;
					RpGeometryForAllMaterials(atomic->geometry, ObjectMaterialCallBack, (void*)pObject);
				}
				// SetObjectMaterialText
				if(pObject->m_bHasMaterialText)
					RwFrameForAllObjects((RwFrame*)rwObject->parent, (RwObject *(*)(RwObject *, void *))ObjectMaterialTextCallBack, pObject);
			}


		}
	}

    ((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
	CObject_Render(thiz);
    ((void (*)(void))(g_libGTASA + 0x5D1F5C + 1))();
}

/*((void (*)(void))(g_libGTASA + 0x5D1F48 + 1))();
				CObject_Render(thiz);
				// ActivateDirectional
				((void (*)(void))(g_libGTASA + 0x5D1F5C + 1))();*/
/* =============================================================================== */

void (*CGame_Process)();
void CGame_Process_hook()
{
	if(pGame->bIsGameExiting)return;

	CGame_Process();

	if (pNetGame)
	{
		if(pGame && pGame->FindPlayerPed() && pUI && pUI->buttonpanel() && pUI->buttonpanel()->m_bH)
		{
			if(pGame->FindPlayerPed()->IsInVehicle())
			{
				pUI->buttonpanel()->m_bH->setCaption("D/B");
			}
			else
				pUI->buttonpanel()->m_bH->setCaption("H");
		}

		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool) {
			pObjectPool->Process();
			pObjectPool->ProcessMaterialText();
		}

		CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
		if (pTextDrawPool) {
			pTextDrawPool->SnapshotProcess();
		}
	}
}

/* =============================================================================== */

bool NotifyEnterVehicle(CVehicleGTA *_pVehicle)
{
	FLog("NotifyEnterVehicle");

	if(!pNetGame) {
		return false;
	}

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(!pVehiclePool) {
		return false;
	}

	CVehicle *pVehicle = nullptr;
	VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr(_pVehicle);

	if(VehicleID <= 0 || VehicleID >= MAX_VEHICLES) {
		return false;
	}

	if(!pVehiclePool->GetSlotState(VehicleID)) {
		return false;
	}

	pVehicle = pVehiclePool->GetAt(VehicleID);
	if(!pVehicle) {
		return false;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	if(pLocalPlayer) {
		FLog("Vehicle ID: %d", VehicleID);
		pLocalPlayer->SendEnterVehicleNotification(VehicleID, false);
	}

	return true;
}

void (*CTaskComplexLeaveCar)(uintptr_t** thiz, CVehicleGTA* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut);
void CTaskComplexLeaveCar_hook(uintptr_t** thiz, CVehicleGTA* pVehicle, int iTargetDoor, int iDelayTime, bool bSensibleLeaveCar, bool bForceGetOut)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if (dwRetAddr == 0x409A42+1 || dwRetAddr == 0x40A818+1)
	{
		if (pNetGame)
		{
			if ((CVehicleGTA*)GamePool_FindPlayerPed()->pVehicle == pVehicle)
			{
				CVehiclePool* pVehiclePool = pNetGame->GetVehiclePool();
				VEHICLEID VehicleID = pVehiclePool->FindIDFromGtaPtr((CVehicleGTA*)GamePool_FindPlayerPed()->pVehicle);
				if (VehicleID != INVALID_VEHICLE_ID)
				{
					CVehicle* pVehicle = pVehiclePool->GetAt(VehicleID);
					CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
					if (pVehicle && pLocalPlayer)
					{
						if (pVehicle->IsATrainPart())
						{
							RwMatrix mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();
							pLocalPlayer->GetPlayerPed()->RemoveFromVehicleAndPutAt(mat.pos.x + 2.5f, mat.pos.y + 2.5f, mat.pos.z);
						}
						else
						{
							pLocalPlayer->SendExitVehicleNotification(VehicleID);
						}
					}
				}
			}
		}
	}

	(*CTaskComplexLeaveCar)(thiz, pVehicle, iTargetDoor, iDelayTime, bSensibleLeaveCar, bForceGetOut);
}

/* =============================================================================== */

uint32_t CRadar__GetRadarTraceColor(uint32_t color, uint8_t bright, uint8_t friendly)
{
    return TranslateColorCodeToRGBA(color);
}

#if VER_x32
uint32_t CHudColours__GetIntColour(uint32 colour_id)
{
	return TranslateColorCodeToRGBA(colour_id);
}
#else
uint32_t CHudColours__GetIntColour(uintptr* thiz, uint8 colour_id)
{
    return TranslateColorCodeToRGBA(colour_id);
}
#endif

/* =============================================================================== */

uint32_t(*Idle)(uint32_t r0, uint32_t r1);
uint32_t Idle_hook(uint32_t r0, uint32_t r1)
{
	uint32_t result = Idle(r0, r1);

	if (pUI) pUI->render();

	RwCameraEndUpdate(*(RwCamera**)(g_libGTASA + 0x9FC93C));

	return result;
}

void (*AND_TouchEvent)(int type, int num, int posX, int posY);
void AND_TouchEvent_hook(int type, int num, int posX, int posY)
{
	// imgui
	//bool bRet = pUI->OnTouchEvent(type, num, posX, posY);

	if (pGame->IsGamePaused())
		return AND_TouchEvent(type, num, posX, posY);

	if (pUI != nullptr)
	{
		switch (type)
		{
			case 2: // push
				pUI->touchEvent(ImVec2(posX, posY), TouchType::push);
				break;

			case 3: // move
				pUI->touchEvent(ImVec2(posX, posY), TouchType::move);
				break;

			case 1: // pop
				pUI->touchEvent(ImVec2(posX, posY), TouchType::pop);
				break;
		}

		if (pUI->keyboard()->visible() || pUI->dialog()->visible()) {
			AND_TouchEvent(1, 0, 0, 0);
			return;
		}
		else
		{
			if (pNetGame && pNetGame->GetTextDrawPool())
			{
				if (!pNetGame->GetTextDrawPool()->onTouchEvent(type, num, posX, posY)) {
					return AND_TouchEvent(1, 0, 0, 0);
				}
			}
		}
	}

	if (pGame->IsGameInputEnabled())
		AND_TouchEvent(type, num, posX, posY);
	else
		AND_TouchEvent(1, 0, 0, 0);
}

/* =============================================================================== */

void (*CWorld_ProcessPedsAfterPreRender)();
void CWorld_ProcessPedsAfterPreRender_Hook()
{
	CWorld_ProcessPedsAfterPreRender();

	if (pNetGame)
	{
		CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
		if (pPlayerPool)
			pPlayerPool->ProcessAttachedObjects();
	}
}

/* =============================================================================== */

void (*CWorld_ProcessAttachedEntities)();
void CWorld_ProcessAttachedEntities_Hook()
{
	if (pNetGame)
	{
		CLocalPlayer* pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		if(pLocalPlayer)
		{
			//pLocalPlayer->UpdateSurfingPosition();
		}

		for (PLAYERID i = 0; i < MAX_PLAYERS; i++)
		{
			CRemotePlayer* pRemotePlayer = pNetGame->GetPlayerPool()->GetAt(i);
			if(pRemotePlayer)
			{
				if(pRemotePlayer->GetPlayerPed() && pRemotePlayer->GetPlayerPed()->GetStateFlags())
				{
					pRemotePlayer->ProcessSurfing();
				}
			}
		}
	}

	CWorld_ProcessAttachedEntities();
}

/* =============================================================================== */

void (*CFileMgr_Initialise)();
void CFileMgr_Initialise_hook()
{
	Log::traceLastFunc("Initializing zip archive");

	// ZIP_FileCreate
	uintptr_t zipFile = ((uintptr_t (*)(const char*))(g_libGTASA + 0x26FE54 + 1))(SAMP_ARCHIVE_PATH);
	Log::addParameter("ZIP handle", zipFile);
	if (zipFile)
	{
		// ZIP_AddStorage
		bool result = ((bool (*)(uintptr_t))(g_libGTASA + 0x26FF70 + 1))(zipFile);
		Log::addParameter("ZIP addstorage result", result);
	}

	CFileMgr_Initialise();
}

/* =============================================================================== */

#include "../java/jniutil.h"
#include "World.h"

extern CJavaWrapper* pJavaWrapper;

void (*CTimer_StartUserPause)();
void CTimer_StartUserPause_hook()
{
	*(uint8_t*)(g_libGTASA + 0x96B514) = 1;
	if (pUI) pUI->setVisible(false);

	if(pJavaWrapper && pNetGame)
	{
		pJavaWrapper->SetPauseState(true);
	}
}

void (*CTimer_EndUserPause)();
void CTimer_EndUserPause_hook()
{
	*(uint8_t*)(g_libGTASA + 0x96B514) = 0;
	if (pUI) pUI->setVisible(true);
	if(pJavaWrapper && pNetGame)
	{
		pJavaWrapper->SetPauseState(false);
	}
}

/* =============================================================================== */

uint32_t (*CPed__GetWeaponSkill)(CPedGTA *thiz);
uint32_t CPed__GetWeaponSkill_hook(CPedGTA *thiz)
{
	bool bWeaponSkillStored = false;

	dwCurPlayerActor = thiz;
	byteInternalPlayer = CWorld::PlayerInFocus;
	byteCurPlayer = FindPlayerNumFromPedPtr(dwCurPlayerActor);

	if(dwCurPlayerActor && byteCurPlayer != 0 && CWorld::PlayerInFocus == 0)
	{
		GameStoreLocalPlayerSkills();
		GameSetRemotePlayerSkills(byteCurPlayer);
		bWeaponSkillStored = true;
	}

	// CPed::GetWeaponSkill
	uint32_t result = (( uint32_t (*)(CPedGTA *, uint32_t))(g_libGTASA+0x4A55E2+1))(thiz, thiz->m_aWeapons[thiz->m_nActiveWeaponSlot].dwType);

	if(bWeaponSkillStored)
	{
		GameSetLocalPlayerSkills();
		bWeaponSkillStored = false;
	}

	return result;
}

/* =============================================================================== */

extern CPlayerPed* g_pCurrentFiredPed;
extern BULLET_DATA* g_pCurrentBulletData;

extern int g_iLagCompensationMode;

void SendBulletSync(CVector* vecOrigin, CVector* a2, CVector* vecPos, CEntityGTA** ppEntity)
{
    CMatrix mat1, mat2;

    static BULLET_DATA bulletData;
    memset(&bulletData, 0, sizeof(BULLET_DATA));

    bulletData.vecOrigin.x = vecOrigin->x;
    bulletData.vecOrigin.y = vecOrigin->y;
    bulletData.vecOrigin.z = vecOrigin->z;

    bulletData.vecPos.x = vecPos->x;
    bulletData.vecPos.y = vecPos->y;
    bulletData.vecPos.z = vecPos->z;

    if (ppEntity)
    {
        CEntityGTA* pEntity = *ppEntity;
        if (pEntity && pEntity->m_matrix)
        {
            if (g_iLagCompensationMode != 0)
            {
                bulletData.vecOffset.x = vecPos->x - pEntity->m_matrix->m_pos.x;
                bulletData.vecOffset.y = vecPos->y - pEntity->m_matrix->m_pos.y;
                bulletData.vecOffset.z = vecPos->z - pEntity->m_matrix->m_pos.z;
            }
            else
            {
                memset(&mat1, 0, sizeof(CMatrix));
                memset(&mat2, 0, sizeof(CMatrix));
                // RwMatrixOrthoNormalize
                auto entMat = pEntity->GetMatrix().ToRwMatrix();
                RwMatrixOrthoNormalize(reinterpret_cast<RwMatrix *>(&mat2), &entMat);
                // RwMatrixInvert
                Invert(mat1, mat2);

                ProjectMatrix(&bulletData.vecOffset, &mat1, vecPos);
            }

            bulletData.pEntity = pEntity;
        }
    }

    pGame->FindPlayerPed()->ProcessBulletData(&bulletData);
}

extern bool g_customFire;
/* 0.3.7 */
uint32_t(*CWeapon__FireInstantHit)(CWeapon* thiz, CPedGTA* pFiringEntity, CVector* vecOrigin, CVector* muzzlePosn, CEntityGTA* targetEntity,
								  CVector* target, CVector* originForDriveBy, bool arg6, bool muzzle);
uint32_t CWeapon__FireInstantHit_hook(CWeapon* thiz, CPedGTA* pFiringEntity, CVector* vecOrigin, CVector* muzzlePosn, CEntityGTA* targetEntity,
									 CVector* target, CVector* originForDriveBy, bool arg6, bool muzzle)
{
	if (pNetGame && pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->m_pPed)		// CWeapon::Fire
    {
       	if(pFiringEntity != GamePool_FindPlayerPed())
			return muzzle;

		if(pNetGame)
		{
            pNetGame->GetPlayerPool()->ApplyCollisionChecking();
		}

		if(pGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed)
				pPlayerPed->FireInstant();
		}

		if(pNetGame)
		{
            pNetGame->GetPlayerPool()->ResetCollisionChecking();
		}

		return muzzle;
    }

    return CWeapon__FireInstantHit(thiz, pFiringEntity, vecOrigin, muzzlePosn, targetEntity,
                                  target, originForDriveBy, arg6, muzzle);
}

bool g_bForceWorldProcessLineOfSight = false;
uint32_t (*CWeapon__ProcessLineOfSight)(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGTA **ppEntity, CWeapon *pWeaponSlot, CPedGTA **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7);
uint32_t CWeapon__ProcessLineOfSight_hook(CVector *vecOrigin, CVector *vecEnd, CVector *vecPos, CPedGTA **ppEntity, CWeapon *pWeaponSlot, CPedGTA **ppEntity2, bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7)
{
    uintptr_t dwRetAddr = 0;
    GET_LR(dwRetAddr);

    if(dwRetAddr >= 0x005DC178 && dwRetAddr <= 0x005DD684)
        g_bForceWorldProcessLineOfSight = true;

    return CWeapon__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, pWeaponSlot, ppEntity2, b1, b2, b3, b4, b5, b6, b7);
}

uint32_t(*CWorld__ProcessLineOfSight)(CVector*, CVector*, CVector*, CEntityGTA**, bool, bool, bool, bool, bool, bool, bool, bool);
uint32_t CWorld__ProcessLineOfSight_hook(CVector* vecOrigin, CVector* vecEnd, CVector* vecPos, CEntityGTA** ppEntity,
										bool b1, bool b2, bool b3, bool b4, bool b5, bool b6, bool b7, bool b8)
{
    uintptr_t dwRetAddr = 0;
    GET_LR(dwRetAddr);

    if(dwRetAddr == 0x5dd0b0 + 1 || g_bForceWorldProcessLineOfSight)
    {
        g_bForceWorldProcessLineOfSight = false;
		LOGI("CWorld_ProcessLineOfSight iLagCompensationMode: %d", g_iLagCompensationMode);
        CEntityGTA* pEntity = nullptr;
		static CVector vecPosPlusOffset = { 0.0f, 0.0f, 0.0f };

		if (g_iLagCompensationMode != 2)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					pEntity = g_pCurrentBulletData->pEntity;
					if (pEntity) // CPlaceable
					{
						if (pEntity->m_matrix)
						{
							if (g_iLagCompensationMode)
							{
								vecPosPlusOffset.x = pEntity->GetPosition().x + g_pCurrentBulletData->vecOffset.x;
								vecPosPlusOffset.y = pEntity->GetPosition().y + g_pCurrentBulletData->vecOffset.y;
								vecPosPlusOffset.z = pEntity->GetPosition().z + g_pCurrentBulletData->vecOffset.z;
							}
							else
							{
								ProjectMatrix(&vecPosPlusOffset, &pEntity->GetMatrix(), &g_pCurrentBulletData->vecOffset);
							}

							vecEnd->x = vecPosPlusOffset.x - vecOrigin->x + vecPosPlusOffset.x;
							vecEnd->y = vecPosPlusOffset.y - vecOrigin->y + vecPosPlusOffset.y;
							vecEnd->z = vecPosPlusOffset.z - vecOrigin->z + vecPosPlusOffset.z;
						}
					}
				}
			}
		}

		uint32_t result = CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);

		if (g_iLagCompensationMode == 2)
		{
			if (g_pCurrentFiredPed == pGame->FindPlayerPed()) {
				SendBulletSync(vecOrigin, vecEnd, vecPos, ppEntity);
			}
			return result;
		}

		if (g_pCurrentFiredPed)
		{
			if (g_pCurrentFiredPed != pGame->FindPlayerPed())
			{
				if (g_pCurrentBulletData)
				{
					if (g_pCurrentBulletData->pEntity == nullptr)
					{
                        CPedGTA* pLocalPed = GamePool_FindPlayerPed();
						if (*ppEntity == GamePool_FindPlayerPed() ||
                                pLocalPed->IsInVehicle() && *ppEntity == pLocalPed->pVehicle)
						{
							result = 0;
							*ppEntity = nullptr;
							vecPos->x = 0.0f;
							vecPos->y = 0.0f;
							vecPos->z = 0.0f;
							return result;
						}
					}
				}
			}
			else {
				SendBulletSync(vecOrigin, vecEnd, vecPos, ppEntity);
			}
		}

		return result;
	}

	return CWorld__ProcessLineOfSight(vecOrigin, vecEnd, vecPos, ppEntity, b1, b2, b3, b4, b5, b6, b7, b8);
}
// 0.3.7
uint32_t(*CWeapon__FireSniper)(CWeapon* thiz, CPedGTA* pFiringEntity, CEntityGTA* victim, CVector* target);
uint32_t CWeapon__FireSniper_hook(CWeapon* thiz, CPedGTA* pFiringEntity, CEntityGTA* victim, CVector* target)
{
	if (pFiringEntity == GamePool_FindPlayerPed())
	{
		if (pGame)
		{
			CPlayerPed* pPlayerPed = pGame->FindPlayerPed();
			if (pPlayerPed) {
				pPlayerPed->FireInstant();
			}
		}
	}

	return true;
}
// 0.3.7
bool(*CBulletInfo_AddBullet)(CEntityGTA* creator, int weaponType, CVector pos, CVector velocity);
bool CBulletInfo_AddBullet_hook(CEntityGTA* creator, int weaponType, CVector pos, CVector velocity)
{
	velocity.x *= 50.0f;
	velocity.y *= 50.0f;
	velocity.z *= 50.0f;

	CBulletInfo_AddBullet(creator, weaponType, pos, velocity);

	// CBulletInfo::Update
	((void (*)())(g_libGTASA + 0x5D7044 + 1))();
	return true;
}

#pragma pack(push, 1)
struct CPedDamageResponseCalculator
{
    CPedGTA* m_pDamager;
	float m_fDamageFactor;
	int m_pedPieceType;
	int m_weaponType;
};
#pragma pack(pop)
// 0.3.7
bool ComputeDamageResponse(CPedDamageResponseCalculator* calculator, CPedGTA* pPed)
{
    CPedGTA* pGamePed = GamePool_FindPlayerPed();
	bool isLocalPed = false;

	if (!pNetGame) return false;

    CPedGTA* pDamager = calculator->m_pDamager;
	if (pDamager != pGamePed && IsValidGamePed(pGamePed)) /* CCivilianPed */
		return true;

	if (pPed == pGamePed) {
		isLocalPed = true;
	}
	else if (pDamager != pGamePed) {
		return false;
	}

	CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
	CLocalPlayer* pLocalPlayer = pPlayerPool->GetLocalPlayer();
	PLAYERID PlayerID;

	if (isLocalPed)
	{
		PlayerID = FindPlayerIDFromGtaPtr(pDamager);
		pLocalPlayer->SendTakeDamageEvent(PlayerID,
										  calculator->m_fDamageFactor,
										  calculator->m_weaponType,
										  calculator->m_pedPieceType);
	}
	else
	{
		PlayerID = FindPlayerIDFromGtaPtr(pPed);
		if (PlayerID != INVALID_PLAYER_ID)
		{
			pLocalPlayer->SendGiveDamageEvent(PlayerID,
											  calculator->m_fDamageFactor,
											  calculator->m_weaponType,
											  calculator->m_pedPieceType);
			if (pPlayerPool->GetAt(PlayerID)->IsNPC())
				return true;
		}
		else
		{
			PLAYERID ActorID = FindActorIDFromGtaPtr(pPed);
			if (ActorID != INVALID_PLAYER_ID) {
				pLocalPlayer->SendGiveDamageEvent(ActorID,
												  calculator->m_fDamageFactor,
												  calculator->m_weaponType,
												  calculator->m_pedPieceType);
				return true;
			}
		}
	}


	// :check_friendly_fire
	if (!pNetGame->m_pNetSet->bFriendlyFire)
		return false;
	uint8_t byteTeam = pPlayerPool->GetLocalPlayer()->m_byteTeam;
	if (byteTeam == NO_TEAM ||
		PlayerID == INVALID_PLAYER_ID ||
		pPlayerPool->GetAt(PlayerID)->m_byteTeam != byteTeam) {
		return false;
	}

	return true;
}

// 0.3.7
void (*CPedDamageResponseCalculator__ComputeDamageResponse)(CPedDamageResponseCalculator* thiz, CPedGTA* pPed, uint32_t a3, uint32_t a4);
void CPedDamageResponseCalculator__ComputeDamageResponse_hook(CPedDamageResponseCalculator* thiz, CPedGTA* pPed, uint32_t a3, uint32_t a4)
{
	if (thiz != nullptr && pPed != nullptr)
	{
		if (ComputeDamageResponse(thiz, pPed))
			return;
	}

	return CPedDamageResponseCalculator__ComputeDamageResponse(thiz, pPed, a3, a4);
}

void (*CRenderer_RenderEverythingBarRoads)();
void CRenderer_RenderEverythingBarRoads_hook() {

	CRenderer_RenderEverythingBarRoads();

	if (pNetGame) {
		CObjectPool* pObjectPool = pNetGame->GetObjectPool();
		if (pObjectPool) {
			for (OBJECTID i = 0; i < MAX_OBJECTS; i++) {
				CObject* pObject = pObjectPool->GetAt(i);
				if (pObject && pObject->m_bForceRender) {
					//pObject->Render();
				}
			}
		}
	}
}

#include "CFPSFix.h"
#include "ES2VertexBuffer.h"
#include "RQ_Commands.h"
#include "Pickups.h"

CFPSFix g_fps;

void (*ANDRunThread)(void* a1);
void ANDRunThread_hook(void* a1)
{
	g_fps.PushThread(gettid());

	ANDRunThread(a1);
}

static constexpr float ar43 = 4.0f/3.0f;
float *ms_fAspectRatio;
void (*DrawCrosshair)(uintptr_t* thiz);
void DrawCrosshair_hook(uintptr_t* thiz)
{
	float save1 = CCamera::m_f3rdPersonCHairMultX;
	CCamera::m_f3rdPersonCHairMultX = 0.530f - (*ms_fAspectRatio - ar43) * 0.01125f;

	float save2 = CCamera::m_f3rdPersonCHairMultY;
	CCamera::m_f3rdPersonCHairMultY = 0.400f + (*ms_fAspectRatio - ar43) * 0.03600f;

	DrawCrosshair(thiz);

	CCamera::m_f3rdPersonCHairMultX = save1;
	CCamera::m_f3rdPersonCHairMultY = save2;
}

CVector& (*FindPlayerSpeed)(int a1);
CVector& FindPlayerSpeed_hook(int a1)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr":"=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(dwRetAddr == 0x43E1F6 + 1)
	{
		if(pNetGame)
		{
			CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
			if(pPlayerPed &&
			   pPlayerPed->IsInVehicle() &&
			   pPlayerPed->IsAPassenger())
			{
				CVector vec = CVector(-1.0f);
				return vec;
			}
		}
	}

	return FindPlayerSpeed(a1);
}

int (*RwFrameAddChild)(int a1, int a2);
int RwFrameAddChild_hook(int a1, int a2)
{
	if(a1 == 0 || a2 == 0) return 0;
	return RwFrameAddChild(a1, a2);
}

// widget fix
uintptr_t (*CWidget)(uintptr_t thiz, const char* name, int a3, int a4, int a5, int a6);
uintptr_t CWidget_hook(uintptr_t thiz, const char* name, int a3, int a4, int a5, int a6)
{
	// Log(OBFUSCATE("[debug:hooks:CWidget]: New Widget: \"%s\" 0x%X"), name, thiz-g_libGTASA);

	SetWidgetFromName(name, thiz);
	return CWidget(thiz, name, a3, a4, a5, a6);
}

void (*CWidget__Update)(uintptr_t pWidget);
void CWidget__Update_hook(uintptr_t pWidget)
{
	if(pNetGame)
	{
		switch(ProcessFixedWidget(pWidget))
		{
			case STATE_NONE: break;
			case STATE_FIXED: return;
		}
	}

	CWidget__Update(pWidget);
}

void (*CWidget__SetEnabled)(uintptr_t pWidget, bool bEnabled);
void CWidget__SetEnabled_hook(uintptr_t pWidget, bool bEnabled)
{
	if(pNetGame)
	{
		switch(ProcessFixedWidget(pWidget))
		{
			case STATE_NONE: break;
			case STATE_FIXED:
				bEnabled = false;
				break;
		}
	}

	CWidget__SetEnabled(pWidget, bEnabled);
}

int iLastTouchedWidgetId = -1;

int (*CTouchInterface__IsTouched)(int iWidgetId, int iUnk, int iEnableWidget);
int CTouchInterface__IsTouched_hook(int iWidgetId, int iUnk, int iEnableWidget)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	if(pNetGame)
	{
		switch(ProcessFixedWidgetFromId(iWidgetId))
		{
			case STATE_NONE: break;
			case STATE_FIXED:
				iEnableWidget = 0;
				break;
		}
	}

	int iTouched = CTouchInterface__IsTouched(iWidgetId, iUnk, iEnableWidget);
	if(iTouched && iEnableWidget)
	{
		iLastTouchedWidgetId = iWidgetId;
	}

	return iTouched;
}

int iLastReleasedWidgetId = -1;

int (*CTouchInterface__IsReleased)(int iWidgetId, int iUnk, int iEnableWidget);
int CTouchInterface__IsReleased_hook(int iWidgetId, int iUnk, int iEnableWidget)
{
	uintptr_t dwRetAddr = 0;
	__asm__ volatile ("mov %0, lr" : "=r" (dwRetAddr));
	dwRetAddr -= g_libGTASA;

	int iReleased = CTouchInterface__IsReleased(iWidgetId, iUnk, iEnableWidget);
	if(iReleased && iEnableWidget)
	{
		iLastReleasedWidgetId = iWidgetId;
	}

	return iReleased;
}

int (*CTextureDatabaseRuntime__GetEntry)(uintptr_t thiz, const char* a2, bool* a3);
int CTextureDatabaseRuntime__GetEntry_hook(uintptr_t thiz, const char* a2, bool* a3)
{
	if (!thiz)
	{
		return -1;
	}
	return CTextureDatabaseRuntime__GetEntry(thiz, a2, a3);
}

uintptr_t (*CTxdStore__TxdStoreFindCB)(const char *a1);
uintptr_t CTxdStore__TxdStoreFindCB_hook(const char *a1)
{
	static char* texdbs[] = { "samp", "gta_int", "gta3" };
	for(auto &texdb : texdbs)
	{
		// TextureDatabaseRuntime::GetDatabase
		uintptr_t db_handle = ((uintptr_t (*)(const char *))(g_libGTASA+0x1EAC8C+1))(texdb);

		// TextureDatabaseRuntime::registered
		uint32_t unk_61B8D4 = *(uint32_t*)(g_libGTASA+0x6BD174+4);
		if(unk_61B8D4)
		{
			// TextureDatabaseRuntime::registered
			uintptr_t dword_61B8D8 = *(uintptr_t*)(g_libGTASA+0x6BD174+8);

			int index = 0;
			while(*(uint32_t*)(dword_61B8D8 + 4 * index) != db_handle)
			{
				if(++index >= unk_61B8D4)
					goto GetTheTexture;
			}

			continue;
		}

		GetTheTexture:
		// TextureDatabaseRuntime::Register
		((void (*)(int))(g_libGTASA+0x1E9BC8+1))(db_handle);

		// TextureDatabaseRuntime::GetTexture
		uintptr_t tex = ((uintptr_t (*)(const char *))(g_libGTASA+0x1E9C64+1))(a1);

		// TextureDatabaseRuntime::Unregister
		((void (*)(int))(g_libGTASA+0x1E9C80+1))(db_handle);

		if(tex) return tex;
	}

	// RwTexDictionaryGetCurrent
	int current = ((int (*)(void))(g_libGTASA+0x1DBA64+1))();
	if(current)
	{
		while(true)
		{
			// RwTexDictionaryFindNamedTexture
			uintptr_t tex = ((int (*)(int, const char *))(g_libGTASA+0x1DB9B0+1))(current, a1);
			if(tex) return tex;

			// CTxdStore::GetTxdParent
			current = ((int (*)(int))(g_libGTASA+0x5D428C+1))(current);
			if(!current) return 0;
		}
	}

	return 0;
}

int (*CCustomRoadsignMgr_RenderRoadsignAtomic)(int a1, int a2);
int CCustomRoadsignMgr_RenderRoadsignAtomic_hook(int a1, int a2)
{
	if ( a1 )
		return CCustomRoadsignMgr_RenderRoadsignAtomic(a1, a2);
}

int (*_RwTextureDestroy)(int a1);
int _RwTextureDestroy_hook(int a1)
{
	int result; // r0

	if ( (unsigned int)(a1 + 1) >= 2 )
		result = _RwTextureDestroy(a1);
	else
		result = 0;
	return result;
}

int (*CPed_UpdatePosition)(CPedGTA* a1);
int CPed_UpdatePosition_hook(CPedGTA* a1)
{
	int result; // r0

	if ( GamePool_FindPlayerPed() == a1 )
		result = CPed_UpdatePosition(a1);
	return result;
}

void (*CCamera__Process)(uintptr_t thiz);
void CCamera__Process_hook(uintptr_t thiz)
{
	//if(pGame->GetCamera())
		//pGame->GetCamera()->Update();

	CCamera__Process(thiz);
}

extern CJavaWrapper* pJavaWrapper;
void (*MainMenuScreen__OnExit)();
void MainMenuScreen__OnExit_hook()
{
	pGame->bIsGameExiting = true;

	pNetGame->GetRakClient()->Disconnect(0);

	pJavaWrapper->exitGame();
}

int (*mpg123_param)(void* mh, int key, long val, int ZERO, double fval);
int mpg123_param_hook(void* mh, int key, long val, int ZERO, double fval) {
	// 0x2000 = MPG123_SKIP_ID3V2
	// 0x200  = MPG123_FUZZY
	// 0x100  = MPG123_SEEKBUFFER
	// 0x40   = MPG123_GAPLESS
	return mpg123_param(mh, key, val | (0x2000 | 0x200 | 0x100 | 0x40), ZERO, fval);
}

void (*rqVertexBufferSelect)(unsigned int **result);
void rqVertexBufferSelect_hook(unsigned int **result)
{
	uint32_t buffer = *(uint32_t *)*result;
	*result += 4;
	if ( buffer )
	{
		glBindBuffer(34962, *(uint32_t *)(buffer + 8));
		*(uint32_t*)(g_libGTASA + 0x6B8AF0) = 0;
	}
	else
	{
		glBindBuffer(34962, 0);
	}
}

uintptr_t* (*rpMaterialListDeinitialize)(RpMaterialList* matList);
uintptr_t* rpMaterialListDeinitialize_hook(RpMaterialList* matList)
{
	if(!matList || !matList->materials)
		return nullptr;

	return rpMaterialListDeinitialize(matList);
}

void (*rqVertexBufferDelete)(unsigned int **result);
void rqVertexBufferDelete_hook(unsigned int **result)
{
	uint32_t* buffer = *(uint32_t **)*result;
	*result += 4;
	glDeleteBuffers(1, reinterpret_cast<const GLuint *>(buffer + 2));
	buffer[2] = 0;
	if ( buffer )
		(*(void (**)(uint32_t *))(*buffer + 4))(buffer);
}

void rotate_ped_if_local(unsigned int *a1, unsigned int *a2)
{
	if ( GamePool_FindPlayerPed() == (CPedGTA*)a2 )
		*(uint32_t *)(a2 + 0x560) = *a1;
}

void (*player_control_zelda)(unsigned int *a2, unsigned int *a3);
void player_control_zelda_hook(unsigned int *a2, unsigned int *a3)
{
	rotate_ped_if_local(a2, a3);
}

#pragma pack(push, 1)
typedef struct _RES_ENTRY_OBJ
{
	PADDING(_pad0, 48); // 0-48
	uintptr_t validate; // 48-52
	PADDING(_pad1, 4); // 52-56
} RES_ENTRY_OBJ;
#pragma pack(pop)

// 006778B0
int (*rxOpenGLDefaultAllInOneRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int rxOpenGLDefaultAllInOneRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if(!resEntry || !flags)
		return 0;

	uint16_t size = *(uint16_t *)(resEntry+26);
	if(size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ*)(resEntry+28);
		if(arr)
		{
			uint32_t validFlag = flags & 0x84;
			if(validFlag)
			{
				for(int i = 0; i < size; i++)
				{
					if(!arr[i].validate) break;

					uintptr_t *v4 = *(uintptr_t**)(arr[i].validate);
					if(v4)
					{
						if(!*v4 || v4 > (uintptr_t*)0xFFFFFF00)
							return 0;
					}
				}
			}
		}
	}

	return rxOpenGLDefaultAllInOneRenderCB(resEntry, object, type, flags);
}

// 00677CB4
int (*CCustomBuildingDNPipeline__CustomPipeRenderCB)(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags);
int CCustomBuildingDNPipeline__CustomPipeRenderCB_hook(uintptr_t resEntry, uintptr_t object, uint8_t type, uint32_t flags)
{
	if(!resEntry || !flags)
		return 0;

	uint16_t size = *(uint16_t *)(resEntry+26);
	if(size)
	{
		RES_ENTRY_OBJ *arr = (RES_ENTRY_OBJ*)(resEntry+28);
		if(arr)
		{
			uint32_t validFlag = flags & 0x84;
			if(validFlag)
			{
				for(int i = 0; i < size; i++)
				{
					if(!arr[i].validate) break;

					uintptr_t *v4 = *(uintptr_t**)(arr[i].validate);
					if(v4)
					{
						if(!*v4 || v4 > (uintptr_t*)0xFFFFFF00)
							return 0;
					}
				}
			}
		}
	}

	return CCustomBuildingDNPipeline__CustomPipeRenderCB(resEntry, object, type, flags);
}

int (*EmuShader_Select)(uintptr_t *result);
int EmuShader_Select_hook(uintptr_t *result)
{
	int result1;
	if ( *result >= 0x1000 )
		return EmuShader_Select(result);
	return 0;
}

float float_4DD9E8;
float ms_fTimeStep;
float fMagic = 50.0f / 30.0f;
void (*CTaskSimpleUseGun__SetMoveAnim)(uintptr_t *thiz, uintptr_t *a2);
void CTaskSimpleUseGun__SetMoveAnim_hook(uintptr_t *thiz, uintptr_t *a2)
{
	ms_fTimeStep = *(float*)(g_libGTASA + 0x96B500);
	float_4DD9E8 = *(float*)(g_libGTASA + 0x4DD9E8);
	float_4DD9E8 = (fMagic) * (0.1f / ms_fTimeStep);
	CTaskSimpleUseGun__SetMoveAnim(thiz, a2);
}

int (*CAnimManager_UncompressAnimation)(int result);
int CAnimManager_UncompressAnimation_hook(int result)
{
	if ( result )
		return CAnimManager_UncompressAnimation(result);
	return 0;
}

// CStreaming::ms_memoryUsed	00792B74
// CStreaming::ms_memoryAvailable	00685FA0
// CStreaming::RemoveLeastUsedModel(uint)	002D549C
// CStreaming::MakeSpaceFor(int)	002D3974
void (*CStreaming__MakeSpaceFor)(uintptr_t *thiz, size_t memoryToCleanInBytes);
void CStreaming__MakeSpaceFor_hook(uintptr_t *thiz, size_t memoryToCleanInBytes)
{
    size_t ms_memoryUsed = *(size_t*)(g_libGTASA+0x00792B74);
    size_t ms_memoryAvailable = *(size_t*)(g_libGTASA+0x00685FA0);
    auto lastmemused = ms_memoryUsed;
    while (ms_memoryUsed >= ms_memoryAvailable - memoryToCleanInBytes) {
        lastmemused = ms_memoryUsed;
        if (!((int (*)(uintptr_t*, unsigned int))(g_libGTASA + 0x2D549C+1))(thiz, 0) || lastmemused == ms_memoryUsed) {
            //  DeleteRwObjectsBehindCamera(ms_memoryAvailable - memoryToCleanInBytes);
            return;
        }
    }
}

void (*CStreaming_Init2)();
void CStreaming_Init2_hook()
{
	CStreaming_Init2();
	CHook::UnFuck(g_libGTASA + 0x685FA0);
	*(uint32_t *)(g_libGTASA + 0x685FA0) *= 3;
}

void readVehiclesAudioSettings();

void (*CVehicleModelInfo__SetupCommonData)();

void CVehicleModelInfo__SetupCommonData_hook() {
	CVehicleModelInfo__SetupCommonData();
	readVehiclesAudioSettings();
}

extern VehicleAudioPropertiesStruct VehicleAudioProperties[20000];
static uintptr_t addr_veh_audio = (uintptr_t) &VehicleAudioProperties[0];

void (*CAEVehicleAudioEntity__GetVehicleAudioSettings)(uintptr_t thiz, int16_t a2, int a3);

void CAEVehicleAudioEntity__GetVehicleAudioSettings_hook(uintptr_t dest, int16_t a2, int ID) {
	memcpy((void *) dest, &VehicleAudioProperties[(ID - 400)], sizeof(VehicleAudioPropertiesStruct));
}

void (*CRadar_ClearBlip)(uint32_t a2);
void CRadar_ClearBlip_hook(uint32_t a2)
{
	uintptr_t dwRetAddr = 0;
	GET_LR(dwRetAddr);

	//LOGI("[CRadar::ClearBlip]: %d called from 0x%X", (uint16_t)a2, dwRetAddr);

	if ( (uint16_t)a2 > 249 )
	{
		LOGI("[CRadar::ClearBlip]: Invalid blip ID (%d) called from 0x%X", (uint16_t)a2, dwRetAddr);
	}
	else
	{
		CRadar_ClearBlip(a2);
	}
}

/* =============================================================================== */

void InstallHuaweiCrashFixHooks()
{
	CHook::InstallPLT(g_libGTASA + 0x677498, (uintptr_t)rqVertexBufferSelect_hook, (uintptr_t*)&rqVertexBufferSelect);
	CHook::InstallPLT(g_libGTASA + 0x679B14, (uintptr_t)rqVertexBufferDelete_hook, (uintptr_t*)&rqVertexBufferDelete);
	//CHook::InstallPLT(g_libGTASA + 0x677B6C, (uintptr_t)rqSetAlphaTest_hook, (uintptr_t*)&rqSetAlphaTest);
}

void InstallCrashFixHooks()
{
	// some crashfixes
	CHook::InstallPLT(g_libGTASA + 0x66F5AC, (uintptr_t)CCustomRoadsignMgr_RenderRoadsignAtomic_hook, (uintptr_t*)&CCustomRoadsignMgr_RenderRoadsignAtomic);
	CHook::InstallPLT(g_libGTASA + 0x67332C, (uintptr_t)_RwTextureDestroy_hook, (uintptr_t*)&_RwTextureDestroy);
	CHook::InstallPLT(g_libGTASA + 0x671458, (uintptr_t)CPed_UpdatePosition_hook, (uintptr_t*)&CPed_UpdatePosition);
	CHook::InstallPLT(g_libGTASA + 0x675490, (uintptr_t)RwFrameAddChild_hook, (uintptr_t*)&RwFrameAddChild);
	CHook::InstallPLT(g_libGTASA + 0x672D14, (uintptr_t)CTextureDatabaseRuntime__GetEntry_hook, (uintptr_t*)&CTextureDatabaseRuntime__GetEntry);
	//CHook::InstallPLT(g_libGTASA + 0x66FBD0, (uintptr_t)RpClumpForAllAtomics_hook, (uintptr_t*)&RpClumpForAllAtomics);
	CHook::InstallPLT(g_libGTASA + 0x6730F0, (uintptr_t)rpMaterialListDeinitialize_hook, (uintptr_t*)&rpMaterialListDeinitialize);
	//CHook::InstallPLT(g_libGTASA + 0x6778B0, (uintptr_t)rxOpenGLDefaultAllInOneRenderCB_hook, (uintptr_t*)&rxOpenGLDefaultAllInOneRenderCB);
	//CHook::InstallPLT(g_libGTASA + 0x677CB4, (uintptr_t)CCustomBuildingDNPipeline__CustomPipeRenderCB_hook, (uintptr_t*)&CCustomBuildingDNPipeline__CustomPipeRenderCB);
	//CHook::InstallPLT(g_libGTASA + 0x66F9E8, (uintptr_t)EmuShader_Select_hook, (uintptr_t*)&EmuShader_Select);
	CHook::InstallPLT(g_libGTASA + 0x6750D4, (uintptr_t)CAnimManager_UncompressAnimation_hook, (uintptr_t*)&CAnimManager_UncompressAnimation);
	//CHook::InstallPLT(g_libGTASA + 0x670E1C, (uintptr_t)CStreaming__MakeSpaceFor_hook, (uintptr_t*)&CStreaming__MakeSpaceFor);
	CHook::InstallPLT(g_libGTASA + 0x6700D0, (uintptr_t)CStreaming_Init2_hook, (uintptr_t*)&CStreaming_Init2);
}

void InstallWeaponFireHooks()
{
	//CHook::InstallPLT(g_libGTASA + 0x6716D0, (uintptr_t)CWeapon_FireInstantHit_hook, (uintptr_t*)&CWeapon_FireInstantHit);
	//CHook::InstallPLT(g_libGTASA + 0x671F10, (uintptr_t)CWorld_ProcessLineOfSight_hook, (uintptr_t*)&CWorld_ProcessLineOfSight);
	//CHook::InstallPLT(g_libGTASA + 0x670A10, (uintptr_t)CWeapon_FireSniper_hook, (uintptr_t*)&CWeapon_FireSniper);
	//CHook::InstallPLT(g_libGTASA + 0x66EAC4, (uintptr_t)CBulletInfo_AddBullet_hook, (uintptr_t*)&CBulletInfo_AddBullet);
}

void InstallSAMPHooks()
{
	//CHook::InstallPLT(g_libGTASA + 0x677EA0, (uintptr_t)MainMenuScreen__OnExit_hook, (uintptr_t*)&MainMenuScreen__OnExit);
	// samp main loop
	//CHook::InstallPLT(g_libGTASA + 0x67589C, (uintptr_t)Render2dStuff_hook, (uintptr_t*)&Render2dStuff);
	// imgui
	//CHook::InstallPLT(g_libGTASA + 0x6710C4, (uintptr_t)Idle_hook, (uintptr_t*)&Idle);
	//CHook::InstallPLT(g_libGTASA + 0x675DE4, (uintptr_t)AND_TouchEvent_hook, (uintptr_t*)&AND_TouchEvent);
	// splashscreen
	//ARMHook::installHook(g_libGTASA + 0x43AF28, (uintptr_t)DisplayScreen_hook, (uintptr_t*)&DisplayScreen);
	// gangzones
	//CHook::InstallPLT(g_libGTASA + 0x67196C, (uintptr_t)CRadar_DrawRadarGangOverlay_hook, (uintptr_t*)&CRadar_DrawRadarGangOverlay);
	// radar
	//CHook::InstallPLT(g_libGTASA+0x675914, (uintptr_t)CRadar__SetCoordBlip_hook, (uintptr_t*)&CRadar__SetCoordBlip);
	// removebuilding
	//CHook::InstallPLT(g_libGTASA + 0x675E6C, (uintptr_t)CFileLoader__LoadObjectInstance_hook, (uintptr_t*)&CFileLoader__LoadObjectInstance);
	// obj material
	//ARMHook::installHook(g_libGTASA + 0x454EF0, (uintptr_t)CObject_Render_hook, (uintptr_t*)& CObject_Render);
	// textdraw models
	//CHook::InstallPLT(g_libGTASA + 0x66FE58, (uintptr_t)CGame_Process_hook, (uintptr_t*)& CGame_Process);
	// enter vehicle as driver
	//ARMHook::codeInject(g_libGTASA + 0x40AC28, (uintptr_t)TaskEnterVehicle_hook, 0);
    //CHook::InstallPLT(g_libGTASA+0x6733F0, (uintptr_t)TaskEnterVehicle_hook, (uintptr_t*)&TaskEnterVehicle);
	// radar color
	//CHook::InstallPLT(g_libGTASA + 0x673950, (uintptr_t)CHudColours__GetIntColour_hook, (uintptr_t*)& CHudColours__GetIntColour);
	// exit vehicle
	CHook::InstallPLT(g_libGTASA + 0x671984, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)& CTaskComplexLeaveCar);
    CHook::InstallPLT(g_libGTASA + 0x675320, (uintptr_t)CTaskComplexLeaveCar_hook, (uintptr_t*)& CTaskComplexLeaveCar);
    // attach obj to ped
	//CHook::InstallPLT(g_libGTASA + 0x675C68, (uintptr_t)CWorld_ProcessPedsAfterPreRender_Hook, (uintptr_t*)&CWorld_ProcessPedsAfterPreRender);
	// game pause
	//CHook::InstallPLT(g_libGTASA + 0x672644, (uintptr_t)CTimer_StartUserPause_hook, (uintptr_t*)&CTimer_StartUserPause);
	//CHook::InstallPLT(g_libGTASA + 0x67056C, (uintptr_t)CTimer_EndUserPause_hook, (uintptr_t*)&CTimer_EndUserPause);
	// aim
	// Crosshair Fix
	ms_fAspectRatio = (float*)(g_libGTASA+0xA26A90);
	CHook::InstallPLT(g_libGTASA + 0x672880, (uintptr_t)DrawCrosshair_hook, (uintptr_t*)&DrawCrosshair);

	// fix radar in passenger
	CHook::InstallPLT(g_libGTASA+0x671BBC, (uintptr_t)FindPlayerSpeed_hook, (uintptr_t*)&FindPlayerSpeed);

	// fix texture loading
	CHook::InstallPLT(g_libGTASA + 0x676034, (uintptr_t)CTxdStore__TxdStoreFindCB_hook, (uintptr_t*)&CTxdStore__TxdStoreFindCB);

	// interpolate camera fix
	CHook::InstallPLT(g_libGTASA + 0x6717BC, (uintptr_t)CCamera__Process_hook, (uintptr_t*)&CCamera__Process);

	// for surfing
	//CHook::InstallPLT(g_libGTASA + 0x66EAE8, (uintptr_t)CWorld_ProcessAttachedEntities_Hook, (uintptr_t*)&CWorld_ProcessAttachedEntities);

	//CHook::InstallPLT(g_libGTASA + 0x67193C, (uintptr_t)player_control_zelda_hook, (uintptr_t*)&player_control_zelda);

	//ARMHook::installHook(g_libGTASA + 0x4DD5E8, (uintptr_t)CTaskSimpleUseGun__SetMoveAnim_hook, (uintptr_t*)&CTaskSimpleUseGun__SetMoveAnim);

    // hueta ne rabotaet no pust budet (tipo ne kak v 1.08)
	CHook::InstallPLT(g_libGTASA + 0x674280, (uintptr_t) CVehicleModelInfo__SetupCommonData_hook, (uintptr_t*)&CVehicleModelInfo__SetupCommonData);
	CHook::InstallPLT(g_libGTASA + 0x06D008, (uintptr_t) CAEVehicleAudioEntity__GetVehicleAudioSettings_hook, (uintptr_t*)&CAEVehicleAudioEntity__GetVehicleAudioSettings);

	CHook::InstallPLT(g_libGTASA + 0x66FF0C, (uintptr_t)CRadar_ClearBlip_hook, (uintptr_t*)&CRadar_ClearBlip);

	// skills
	CHook::InstallPLT(g_libGTASA + 0x6749D0, (uintptr_t)CPed__GetWeaponSkill_hook, (uintptr_t*)&CPed__GetWeaponSkill);

    //InstallHuaweiCrashFixHooks();
	InstallCrashFixHooks();
	InstallWeaponFireHooks();
	HookCPad();
}

void InstallWidgetHooks()
{
	CHook::InstallPLT(g_libGTASA+0x66F660, (uintptr_t)CWidget_hook, (uintptr_t*)&CWidget);
	CHook::InstallPLT(g_libGTASA+0x66FBCC, (uintptr_t)CWidget__Update_hook, (uintptr_t*)&CWidget__Update);
	CHook::InstallPLT(g_libGTASA+0x672A0C, (uintptr_t)CWidget__SetEnabled_hook, (uintptr_t*)&CWidget__SetEnabled);
	CHook::InstallPLT(g_libGTASA+0x674388, (uintptr_t)CTouchInterface__IsTouched_hook, (uintptr_t*)&CTouchInterface__IsTouched);
	CHook::InstallPLT(g_libGTASA+0x66F6E0, (uintptr_t)CTouchInterface__IsReleased_hook, (uintptr_t*)&CTouchInterface__IsReleased);
}

void InstallGlobalHooks()
{
	//ARMHook::installHook(g_libGTASA + 0x22F2E8, (uintptr_t) mpg123_param_hook, (uintptr_t*)&mpg123_param);
	//CHook::InstallPLT(g_libGTASA + 0x671B20, (uintptr_t)LIB_PointerGetButton_hook, (uintptr_t*)& LIB_PointerGetButton);
	//CHook::InstallPLT(g_libGTASA + 0x670268, (uintptr_t)CWorld__FindPlayerSlotWithPedPointer_hook, (uintptr_t*)&CWorld__FindPlayerSlotWithPedPointer);
	//
	//ARMHook::installHook(g_libGTASA + 0x26BF20, (uintptr_t)ANDRunThread_hook, (uintptr_t*)& ANDRunThread);
	// filesystem
	//CHook::InstallPLT(g_libGTASA + 0x6753A4, (uintptr_t)OS_FileOpen_hook, (uintptr_t*)&OS_FileOpen);
	// custom ZIP archive
	//CHook::InstallPLT(g_libGTASA + 0x66FF40, (uintptr_t)CFileMgr_Initialise_hook, (uintptr_t*)&CFileMgr_Initialise);
	// SAMP IMG archive
	//CHook::InstallPLT(g_libGTASA + 0x674C68, (uintptr_t)CStream_InitImageList_hook, (uintptr_t*)&CStream_InitImageList);
	// SAMP textures
	//CHook::InstallPLT(g_libGTASA + 0x66F2D0, (uintptr_t)CGame__InitialiseRenderWare_hook, (uintptr_t*)&CGame__InitialiseRenderWare);
	// increase Atomic models pool
	//CHook::InstallPLT(g_libGTASA + 0x67579C, (uintptr_t)CModelInfo_AddAtomicModel_hook, (uintptr_t*)&CModelInfo_AddAtomicModel);
	// ped models pool
	//CHook::InstallPLT(g_libGTASA + 0x675D98, (uintptr_t)CModelInfo_AddPedModel_hook, (uintptr_t*)&CModelInfo_AddPedModel);
	// game pools
	//CHook::InstallPLT(g_libGTASA + 0x672468, (uintptr_t)CPools_Initialise_hook, (uintptr_t*)&CPools_Initialise);
	// placeable matrix alloc
	//CHook::InstallPLT(g_libGTASA + 0x675554, (uintptr_t)CPlaceable_InitMatrixArray_hook, (uintptr_t*)& CPlaceable_InitMatrixArray);
	// render objects 3000+- pos
	CHook::InstallPLT(g_libGTASA + 0x673794, (uintptr_t)CRenderer_RenderEverythingBarRoads_hook, (uintptr_t*)&CRenderer_RenderEverythingBarRoads);

	InstallSAMPHooks();
	InstallWidgetHooks();
}

void ReadSettingFile();
void (*NvUtilInit)();
void NvUtilInit_hook()
{
    FLog("NvUtilInit");

    NvUtilInit();

    g_pszStorage = (char*)(g_libGTASA + (VER_x32 ? 0x6D687C : 0x8B46A8)); // StorageRootBuffer

    ReadSettingFile();
}

struct stFile
{
    int isFileExist;
    FILE *f;
};

char lastFile[123];

stFile* NvFOpen(const char* r0, const char* r1, int r2, int r3)
{
    strcpy(lastFile, r1);

    static char path[255]{};
    memset(path, 0, sizeof(path));

    sprintf(path, "%s%s", g_pszStorage, r1);

    // ----------------------------
    if(!strncmp(r1+12, "mainV1.scm", 10))
    {
        sprintf(path, "%sSAMP/main.scm", g_pszStorage);
        FLog("Loading %s", path);
    }
    // ----------------------------
    if(!strncmp(r1+12, "SCRIPTV1.IMG", 12))
    {
        sprintf(path, "%sSAMP/script.img", g_pszStorage);
        FLog("Loading script.img..");
    }
    // ----------------------------
    if(!strncmp(r1, "DATA/PEDS.IDE", 13))
    {
        sprintf(path, "%sSAMP/peds.ide", g_pszStorage);
        FLog("Loading peds.ide..");
    }
    // ----------------------------
    if(!strncmp(r1, "DATA/VEHICLES.IDE", 17))
    {
        sprintf(path, "%sSAMP/vehicles.ide", g_pszStorage);
        FLog("Loading vehicles.ide..");
    }

    if (!strncmp(r1, "DATA/GTA.DAT", 12))
    {
        sprintf(path, "%sSAMP/gta.dat", g_pszStorage);
        FLog("Loading gta.dat..");
    }

    if (!strncmp(r1, "DATA/HANDLING.CFG", 17))
    {
        sprintf(path, "%sSAMP/handling.cfg", g_pszStorage);
        FLog("Loading handling.cfg..");
    }

    if (!strncmp(r1, "DATA/WEAPON.DAT", 15))
    {
        sprintf(path, "%sSAMP/weapon.dat", g_pszStorage);
        FLog("Loading weapon.dat..");
    }

#if VER_x32
    auto *st = (stFile*)malloc(8);
#else
    auto *st = (stFile*)malloc(0x10);
#endif
    st->isFileExist = false;

    FLog("%s", path);
    FILE *f  = fopen(path, "rb");

    if(f)
    {
        st->isFileExist = true;
        st->f = f;
        return st;
    }
    else
    {
        FLog("NVFOpen hook | Error: file not found (%s)", path);
        free(st);
        return nullptr;
    }
}

bool g_bPlaySAMP = false;

void MainMenu_OnStartSAMP()
{
    if(g_bPlaySAMP) return;

    //InitInMenu();
    pGame->StartGame();

    // StartGameScreen::OnNewGameCheck()
    (( void (*)())(g_libGTASA + (VER_x32 ? 0x002A7270 + 1 : 0x365EA0)))();

    g_bPlaySAMP = true;
}

unsigned int (*MainMenuScreen__Update)(uintptr_t thiz, float a2);
unsigned int MainMenuScreen__Update_hook(uintptr_t thiz, float a2)
{
    unsigned int ret = MainMenuScreen__Update(thiz, a2);
    MainMenu_OnStartSAMP();
    return ret;
}

void (*StartGameScreen__OnNewGameCheck)();
void StartGameScreen__OnNewGameCheck_hook()
{
    // отключить кнопку начать игру
    if(g_bPlaySAMP)
        return;

    StartGameScreen__OnNewGameCheck();
}

void (*CTaskSimpleUseGun__RemoveStanceAnims)(uintptr* thiz, void* ped, float a3);
void CTaskSimpleUseGun__RemoveStanceAnims_hook(uintptr* thiz, void* ped, float a3)
{
    if(!thiz)
        return;

    uintptr* m_pAnim = (uintptr*)(thiz + 0x2c);
    if(m_pAnim) {
        if (!((uintptr *)(m_pAnim + 0x14)))
            return;
    }
    CTaskSimpleUseGun__RemoveStanceAnims(thiz, ped, a3);
}

int (*CCollision__ProcessVerticalLine)(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8);
int CCollision__ProcessVerticalLine_hook(float *a1, float *a2, int a3, int a4, int *a5, int a6, int a7, int a8)
{
    int result; // r0

    if (a3)
        result = CCollision__ProcessVerticalLine(a1, a2, a3, a4, a5, a6, a7, a8);
    else
        result = 0;
    return result;
}

int(*CUpsideDownCarCheck__IsCarUpsideDown)(int, int);
int CUpsideDownCarCheck__IsCarUpsideDown_hook(int a1, int a2)
{
    /* Passengers leave the vehicle out of fear if it overturns */

//	if (*(uintptr_t*)(a2 + 20))
//	{
//		return CUpsideDownCarCheck__IsCarUpsideDown(a1, a2);
//	}
    return 0;
}

int (*CTaskSimpleGetUp__ProcessPed)(uintptr_t* thiz, CPedGTA* ped);
int CTaskSimpleGetUp__ProcessPed_hook(uintptr_t* thiz, CPedGTA* ped)
{
    //return false;
    if(!ped)return 0;
    int res = 0;
    try {
        res = CTaskSimpleGetUp__ProcessPed(thiz, ped);
    }
    catch(...) {
        return 0;
    }

    return res;
}

void InjectHooks()
{
    FLog("InjectHooks");
    CHook::Write(g_libGTASA + (VER_x32 ? 0x678954 : 0x84F2D0), &Scene);

#if !VER_x32 // mb all.. wtf crash x64?
    CHook::RET("_ZN11CPlayerInfo14LoadPlayerSkinEv");
    CHook::RET("_ZN11CPopulation10InitialiseEv");
#endif
    CCamera::InjectHooks(); //
    CReferences::InjectHooks(); //
    CModelInfo::injectHooks(); //
    CTimer::InjectHooks(); //
    //cTransmission::InjectHooks(); //
    CAnimBlendAssociation::InjectHooks(); //
    //cHandlingDataMgr::InjectHooks(); //
    CPools::InjectHooks(); //
    CVehicleGTA::InjectHooks(); //
    CMatrixLink::InjectHooks(); //
    CMatrixLinkList::InjectHooks(); //
    CStreaming::InjectHooks();
    CPlaceable::InjectHooks(); //
    CMatrix::InjectHooks(); //
    CCollision::InjectHooks(); //
    //CIdleCam::InjectHooks(); //
    //CTouchInterface::InjectHooks(); //
    //CWidgetGta::InjectHooks();
    CEntityGTA::InjectHooks(); //
    CPhysical::InjectHooks(); //
    CAnimManager::InjectHooks(); //
    //CCarEnterExit::InjectHooks();
    CPlayerPedGta::InjectHooks(); //
    CTaskManager::InjectHooks(); //
    //CPedIntelligence::InjectHooks(); //
    CWorld::InjectHooks(); //
    CGame::InjectHooks();
    ES2VertexBuffer::InjectHooks();
    CRQ_Commands::InjectHooks();
    CTxdStore::InjectHooks();
    CVisibilityPlugins::InjectHooks();
    //CAdjustableHUD::InjectHooks();

    // new
    //CClouds::InjectHooks();
    //CWeather::InjectHooks();
    //RenderBuffer::InjectHooks();
    //CTimeCycle::InjectHooks();
    CCoronas::InjectHooks();
    //CDraw::InjectHooks();
    //CClock::InjectHooks();
    //CBirds::Init();
    CVehicleModelInfo::InjectHooks();
    //CPathFind::InjectHooks();
    CSprite2d::InjectHooks();
    //CFileLoader::InjectHooks();
    //CShadows::InjectHooks();
    CPickups::InjectHooks();
    CRenderer::InjectHooks();
    CStreamingInfo::InjectHooks();
    TextureDatabase::InjectHooks();
    TextureDatabaseEntry::InjectHooks();
    TextureDatabaseRuntime::InjectHooks();
    //CCustomBuildingDNPipeline::InjectHooks();
    //CWidgetRadar::InjectHooks();

    //CRealTimeShadowManager::InjectHooks();
}

void InstallSpecialHooks()
{
    InjectHooks();
    CHook::Redirect("_ZN5CGame20InitialiseRenderWareEv", &CGame::InitialiseRenderWare);
    CHook::InstallPLT(g_libGTASA + (VER_x32 ? 0x6785FC : 0x84EC20), &StartGameScreen__OnNewGameCheck_hook, &StartGameScreen__OnNewGameCheck);

    CHook::InlineHook("_Z10NvUtilInitv", &NvUtilInit_hook, &NvUtilInit);

    //CHook::RET("_ZN12CCutsceneMgr16LoadCutsceneDataEPKc"); // LoadCutsceneData
    //CHook::RET("_ZN12CCutsceneMgr10InitialiseEv");			// CCutsceneMgr::Initialise

    CHook::Redirect("_Z7NvFOpenPKcS0_bb", &NvFOpen);

    CHook::InlineHook("_ZN14MainMenuScreen6UpdateEf", &MainMenuScreen__Update_hook, &MainMenuScreen__Update);

    CHook::RET("_ZN4CPed31RemoveWeaponWhenEnteringVehicleEi"); // CPed::RemoveWeaponWhenEnteringVehicle

    MultiTouch::initialize();
}

void InstallHooks()
{
    CHook::Redirect("_Z13Render2dStuffv", &Render2dStuff);
    CHook::Redirect("_Z13RenderEffectsv", &RenderEffects);
    CHook::InlineHook("_Z14AND_TouchEventiiii", &AND_TouchEvent_hook, &AND_TouchEvent);

    CHook::Redirect("_ZN11CHudColours12GetIntColourEh", &CHudColours__GetIntColour); // dangerous
    CHook::Redirect("_ZN6CRadar19GetRadarTraceColourEjhh", &CRadar__GetRadarTraceColor); // dangerous
    CHook::InlineHook("_ZN6CRadar12SetCoordBlipE9eBlipType7CVectorj12eBlipDisplayPc", &CRadar__SetCoordBlip_hook, &CRadar__SetCoordBlip);
    CHook::InlineHook("_ZN6CRadar20DrawRadarGangOverlayEb", &CRadar_DrawRadarGangOverlay_hook, &CRadar_DrawRadarGangOverlay);

    CHook::Redirect("_Z10GetTexturePKc", &CUtil::GetTexture);

    CHook::InlineHook("_ZN14MainMenuScreen6OnExitEv", &MainMenuScreen__OnExit_hook, &MainMenuScreen__OnExit);

    CHook::InlineHook("_ZN17CTaskSimpleUseGun17RemoveStanceAnimsEP4CPedf", &CTaskSimpleUseGun__RemoveStanceAnims_hook, &CTaskSimpleUseGun__RemoveStanceAnims);

    // Bullet sync
    CHook::InlineHook("_ZN7CWeapon14FireInstantHitEP7CEntityP7CVectorS3_S1_S3_S3_bb", &CWeapon__FireInstantHit_hook, &CWeapon__FireInstantHit);
    CHook::InlineHook("_ZN7CWeapon10FireSniperEP4CPedP7CEntityP7CVector", &CWeapon__FireSniper_hook, &CWeapon__FireSniper);
    CHook::InlineHook("_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb", &CWorld__ProcessLineOfSight_hook, &CWorld__ProcessLineOfSight);
    CHook::InlineHook("_ZN28CPedDamageResponseCalculator21ComputeDamageResponseEP4CPedR18CPedDamageResponseb", &CPedDamageResponseCalculator__ComputeDamageResponse_hook, &CPedDamageResponseCalculator__ComputeDamageResponse);
    CHook::InlineHook("_ZN7CWeapon18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntity11eWeaponTypeS6_bbbbbbb", &CWeapon__ProcessLineOfSight_hook, &CWeapon__ProcessLineOfSight);

    CHook::InlineHook("_ZN11CFileLoader18LoadObjectInstanceEPKc", &CFileLoader__LoadObjectInstance_hook, &CFileLoader__LoadObjectInstance);

    CHook::InlineHook("_ZN6CRadar9ClearBlipEi", &CRadar_ClearBlip_hook, &CRadar_ClearBlip);

    CHook::InlineHook("_ZN10CCollision19ProcessVerticalLineERK8CColLineRK7CMatrixR9CColModelR9CColPointRfbbP15CStoredCollPoly", &CCollision__ProcessVerticalLine_hook, &CCollision__ProcessVerticalLine);

    CHook::InlineHook("_ZN19CUpsideDownCarCheck15IsCarUpsideDownEPK8CVehicle", &CUpsideDownCarCheck__IsCarUpsideDown_hook, &CUpsideDownCarCheck__IsCarUpsideDown);

    CHook::InlineHook("_ZN16CTaskSimpleGetUp10ProcessPedEP4CPed", &CTaskSimpleGetUp__ProcessPed_hook, &CTaskSimpleGetUp__ProcessPed); // CTaskSimpleGetUp::ProcessPed
    HookCPad();
}