#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
#include "Streaming.h"
#include "Scene.h"
#include "VisibilityPlugins.h"
#include "game/Models/ModelInfo.h"
#include "game/Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include <GLES2/gl2.h>

extern CGame* pGame;

CSnapShotHelper::CSnapShotHelper()
{
	m_camera = 0;
	m_light = 0;
	m_frame = 0;
	m_zBuffer = 0;
	m_raster = 0;

	SetUpScene();
}

void CSnapShotHelper::SetUpScene()
{
	// RpLightCreate
	m_light = RpLightCreate(2);
	if (m_light == 0) return;
	float rwColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// RpLightSetColor
    RpLightSetColor(m_light, reinterpret_cast<const RwRGBAReal *>(rwColor));

	m_zBuffer = RwRasterCreate(256, 256, 0, rwRASTERTYPEZBUFFER);
	m_camera = RwCameraCreate();
	m_frame = RwFrameCreate();
	CVector v = { 0.0f, 0.0f, 50.0f };
	RwFrameTranslate(m_frame, &v, rwCOMBINEPRECONCAT);
    CVector v1 = { 1.0f, 0.0f, 0.0f };
	RwFrameRotate(m_frame, &v1, 90.0f, rwCOMBINEPRECONCAT);

	if (!m_camera) return;
	if (!m_frame) return;

	m_camera->zBuffer = m_zBuffer;

    _rwObjectHasFrameSetFrame(m_camera, m_frame);
    RwCameraSetFarClipPlane(m_camera, 300.0f);
    RwCameraSetNearClipPlane(m_camera, 0.01f);
	RwV2d view = { 0.5f, 0.5f };
    RwCameraSetViewWindow(m_camera, &view);
    RwCameraSetProjection(m_camera, rwPERSPECTIVE);
    RpWorldAddCamera(Scene.m_pRpWorld, m_camera);
}

// 0.3.7
RwTexture* CSnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom)
{
	FLog("Object snapshot: %d", iModel);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	RwTexture* bufferTexture = RwTextureCreate(raster);

    FLog("Object snapshot 1");

	if (!raster || !bufferTexture) return nullptr;

	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
		iModel = 18631;

    if (!CStreaming::TryLoadModel(iModel))
        iModel = 18631;

    if(!CModelInfo::GetModelInfo(iModel))
        return nullptr;

    FLog("Object snapshot 2");
	RwObject* atomic = ModelInfoCreateInstance(iModel);
	if (!atomic) return nullptr;

	CVector vec;
	vec.x = 0.0f;
	vec.y = 0.0f;
	vec.z = 0.0f;

    float fRadius = CModelInfo::GetModelInfo(iModel)->m_pColModel->GetBoundRadius();
    CVector vecCenter = CModelInfo::GetModelInfo(iModel)->m_pColModel->GetBoundCenter();
    FLog("Object snapshot 3");
	RwFrame* parent = static_cast<RwFrame *>(atomic->parent);
    if(!parent) return nullptr;
    fZoom = (-0.1f - fRadius * 2.25f) * fZoom;
	if (parent)
	{
        RwV3d v = {
                -vecCenter.x + vecRot->x,
                fZoom + vecRot->y,
                50.0f - vecCenter.z - vecRot->z
        };
        RwFrameTranslate(parent, &v, rwCOMBINEPRECONCAT);
		if (iModel == 18631) {
			RwFrameRotate(parent, &vec, 180.0f,rwCOMBINEPRECONCAT);
		}
		else
		{
			if (vecRot->x != 0.0f) {
				RwFrameRotate(parent, &vec, vecRot->x,rwCOMBINEPRECONCAT);
			}
			if (vecRot->y != 0.0f) {
				RwFrameRotate(parent, &vec, vecRot->y,rwCOMBINEPRECONCAT);
			}
			if (vecRot->z != 0.0f) {
				RwFrameRotate(parent, &vec, vecRot->z,rwCOMBINEPRECONCAT);
			}
		}
	}
    FLog("Object snapshot 4");
    m_camera->frameBuffer = raster;
    FLog("Object snapshot 5");
    CVisibilityPlugins::SetRenderWareCamera(m_camera);
    RwCameraClear(m_camera, reinterpret_cast<RwRGBA *>(&dwColor), 3);
	RwCameraBeginUpdate((RwCamera*)m_camera);
	RpWorldAddLight(Scene.m_pRpWorld, m_light);
    FLog("Object snapshot 6");
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);
    FLog("Object snapshot 7");
	// DefinedState
	DefinedState();
    FLog("Object snapshot 8");
	RenderClumpOrAtomic((uintptr_t)atomic);
    FLog("Object snapshot 9");
	RwCameraEndUpdate(m_camera);
    FLog("Object snapshot 10");
	RpWorldRemoveLight(Scene.m_pRpWorld, m_light);
    FLog("Object snapshot 11");
	DestroyAtomicOrClump(reinterpret_cast<uintptr_t>(atomic));
    FLog("Object snapshot 12");
	return bufferTexture;
}
// 0.3.7
RwTexture* CSnapShotHelper::CreatePedSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom)
{
	FLog("Ped snapshot: %d", iModel);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	RwTexture* bufferTexture = RwTextureCreate(raster);

	CPlayerPed* pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

	if (!raster || !bufferTexture || !pPed) return 0;

	float posZ = iModel == 162 ? 50.15f : 50.05f;
	float posY = fZoom * -2.25f;
	pPed->m_pPed->SetPosn(0.0f, posY, posZ);
	pPed->SetModelIndex(iModel);
	//pPed->m_pPed->SetGravityProcessing(false);
	pPed->m_pPed->SetCollisionChecking(false);

	RwMatrix mat = pPed->m_pPed->GetMatrix().ToRwMatrix();

    CVector axis { 1.0f, 0.0f, 0.0f };
    if (vecRot->x != 0.0f)
    {
        RwMatrixRotate(&mat, &axis, vecRot->x);
    }
    axis.Set( 0.0f, 1.0f, 0.0f );
    if (vecRot->y != 0.0f)
    {
        RwMatrixRotate(&mat, &axis, vecRot->y);
    }
    axis.Set( 0.0f, 0.0f, 1.0f );
    if (vecRot->z != 0.0f)
    {
        RwMatrixRotate(&mat, &axis, vecRot->z);
    }

	pPed->m_pPed->SetMatrix((CMatrix&)mat);

	// set camera frame buffer //
    m_camera->frameBuffer = raster;
	// CVisibilityPlugins::SetRenderWareCamera
    CVisibilityPlugins::SetRenderWareCamera(m_camera);

    RwCameraClear(m_camera, reinterpret_cast<RwRGBA *>(&dwColor), 3);
	RwCameraBeginUpdate((RwCamera*)m_camera);
	RpWorldAddLight(Scene.m_pRpWorld, m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

    DefinedState();

	pPed->m_pPed->Add();

	RpAnimBlendClumpUpdateAnimations(pPed->m_pPed->m_pRwClump, 100.0f, 1);
    RenderEntity(pPed->m_pPed);

	RwCameraEndUpdate((RwCamera*)m_camera);

	RpWorldRemoveLight(Scene.m_pRpWorld, m_light);

	pPed->m_pPed->Remove();

	delete pPed;

    CStreaming::RemoveModelIfNoRefs(iModel);

	return bufferTexture;
}

RwTexture* CSnapShotHelper::CreateVehicleSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom, uint32_t dwColor1, uint32_t dwColor2)
{
	FLog("Vehicle snapshot: %d", iModel);

	RwRaster* raster = RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	RwTexture* bufferTexture = RwTextureCreate(raster);

	if (iModel == 570) {
		iModel = 538;
	}
	else if (iModel == 569) {
		iModel = 537;
	}

    FLog("Vehicle snapshot1");
	CVehicle* pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f, false, false);

	if (!raster || !bufferTexture || !pVehicle || !pVehicle->m_pVehicle) {
        FLog("somethign went wrong in snapshot");
        return 0;
    }

	//pVehicle->m_pVehicle->SetGravityProcessing(false);
	pVehicle->m_pVehicle->SetCollisionChecking(false);
    FLog("Vehicle snapshot2");
	float radius = CModelInfo::GetModelInfo(iModel)->m_pColModel->GetBoundRadius();
	float posY = (-1.0f - (radius + radius)) * fZoom;
	if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT) {
		posY = -5.5f - radius * 2.5f;
	}
    FLog("Vehicle snapshot3");
	pVehicle->m_pVehicle->SetPosn(0.0f, posY, 50.0f);
	if (dwColor1 != 0xFFFFFFFF && dwColor2 != 0xFFFFFFFF) {
		pVehicle->SetColor(dwColor1, dwColor2);
	}
    FLog("Vehicle snapshot4");
	RwMatrix mat = pVehicle->m_pVehicle->GetMatrix().ToRwMatrix();

    FLog("Vehicle snapshot5");
    CVector axis { 1.0f, 0.0f, 0.0f };
    if (vecRot->x != 0.0f)
    {
        FLog("Vehicle snapshot6");
        RwMatrixRotate(&mat, &axis, vecRot->x);
    }
    axis.Set( 0.0f, 1.0f, 0.0f );
    if (vecRot->y != 0.0f)
    {
        FLog("Vehicle snapshot7");
        RwMatrixRotate(&mat, &axis, vecRot->y);
    }
    axis.Set( 0.0f, 0.0f, 1.0f );
    if (vecRot->z != 0.0f)
    {
        FLog("Vehicle snapshot8");
        RwMatrixRotate(&mat, &axis, vecRot->z);
    }

    FLog("Vehicle snapshot9");
	pVehicle->m_pVehicle->SetMatrix((CMatrix&)mat);
    pVehicle->m_pVehicle->UpdateRW();
    FLog("Vehicle snapshot10");
    m_camera->frameBuffer = raster;
    FLog("Vehicle snapshot10.5");
    CVisibilityPlugins::SetRenderWareCamera(m_camera);
    FLog("Vehicle snapshot10.9");
    RwCameraClear(m_camera, reinterpret_cast<RwRGBA *>(&dwColor), 3);
    FLog("Vehicle snapshot11");
	RwCameraBeginUpdate((RwCamera*)m_camera);
	RpWorldAddLight(Scene.m_pRpWorld, m_light);
    FLog("Vehicle snapshot12");
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	DefinedState();
    FLog("Vehicle snapshot13");
	pVehicle->m_pVehicle->Add();

    RenderEntity(pVehicle->m_pVehicle);
	RwCameraEndUpdate((RwCamera*)m_camera);
	RpWorldRemoveLight(Scene.m_pRpWorld, m_light);
    FLog("Vehicle snapshot14");
	pVehicle->m_pVehicle->Remove();
	delete pVehicle;

	return bufferTexture;
}