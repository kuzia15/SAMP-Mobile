#include "../main.h"
#include "game.h"
#include "RW/RenderWare.h"
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
	m_light = ((uintptr_t(*)(int))(g_libGTASA + 0x216E30 + 1))(2);
	if (m_light == 0) return;

	float rwColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	// RpLightSetColor
	((void (*)(uintptr_t, float*))(g_libGTASA + 0x2167C6 + 1))(m_light, rwColor);

	m_zBuffer = (uintptr_t)RwRasterCreate(256, 256, 0, rwRASTERTYPEZBUFFER);

	// RwCameraCreate
	m_camera = ((uintptr_t(*)())(g_libGTASA + 0x1D5F60 + 1))();

	// RwFrameCreate
	m_frame = ((uintptr_t(*)())(g_libGTASA + 0x1D822C + 1))();

	// RwFrameTranslate
	CVector v = { 0.0f, 0.0f, 50.0f };
	RwFrameTranslate(m_frame, &v, 1);

	// RwFrameRotate
	//v[0] = 1.0f; v[1] = 0.0f; v[2] = 0.0f;
	//((void(*)(uintptr_t, float*, float, int))(g_libGTASA + 0x1D87A8 + 1))(m_frame, v, 90.0f, 1);
	RwFrameRotate(m_frame, 0, 90.0f);

	if (!m_camera) return;
	if (!m_frame) return;

	*(uintptr_t*)(m_camera + 0x64) = m_zBuffer;

	// RwObjectHasFrameSetFrame
	((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x1DCFE4 + 1))(m_camera, m_frame);

	// RwCameraSetFarClipPlane
	((void(*)(uintptr_t, float))(g_libGTASA + 0x1D5B4C + 1))(m_camera, 300.0f);

	// RwCameraSetNearClipPlane
	((void(*)(uintptr_t, float))(g_libGTASA + 0x1D5AB8 + 1))(m_camera, 0.01f);

	// RwCameraSetViewWindow
	float view[2] = { 0.5f, 0.5f };
	((void(*)(uintptr_t, float*))(g_libGTASA + 0x1D5E84 + 1))(m_camera, view);

	// RwCameraSetProjection
	((void(*)(uintptr_t, int))(g_libGTASA + 0x1D5DA8 + 1))(m_camera, 1);

	// RpWorldAddCamera
	uintptr_t pRwWorld = *(uintptr_t*)(g_libGTASA + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x21E004 + 1))(pRwWorld, m_camera);
	}
}

// 0.3.7
uintptr_t CSnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom)
{
	FLog("Object snapshot: %d", iModel);

	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_libGTASA + 0x1DB83C + 1))(raster);

	if (!raster || !bufferTexture) return bufferTexture;

	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
		iModel = 18631;

	bool bNeedRemoveModel = false;
	if (!pGame->IsModelLoaded(iModel))
	{
		bNeedRemoveModel = true;
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModel)) usleep(1000);
	}

	uintptr_t atomic = ModelInfoCreateInstance(iModel);
	if (!atomic) return bufferTexture;

	float fRadius = GetModelColSphereRadius(iModel);

	CVector vec;
	vec.x = 0.0f;
	vec.y = 0.0f;
	vec.z = 0.0f;

	fZoom = (-0.1f - fRadius * 2.25f) * fZoom;
	GetModelColSphereVecCenter(iModel, &vec);

	uintptr_t parent = *(uintptr_t*)(atomic + 4);
	if (parent)
	{
		vec.x = -vec.x;
		vec.y = fZoom;
		vec.z = 50.0f - vec.z;
		RwFrameTranslate(parent, &vec, 1);
		if (iModel == 18631) {
			RwFrameRotate(parent, 2, 180.0f);
		}
		else
		{
			if (vecRot->x != 0.0f) {
				RwFrameRotate(parent, 0, vecRot->x);
			}
			if (vecRot->y != 0.0f) {
				RwFrameRotate(parent, 1, vecRot->y);
			}
			if (vecRot->z != 0.0f) {
				RwFrameRotate(parent, 2, vecRot->z);
			}
		}
	}

	*(uintptr_t*)(m_camera + 0x60) = raster;

	// CVisibilityPlugins::SetRenderWareCamera
	((void(*)(uintptr_t))(g_libGTASA + 0x5D61F8 + 1))(m_camera);
	// RwCameraClear
	((void(*)(uintptr_t, uint32_t*, int))(g_libGTASA + 0x1D5D70 + 1))(m_camera, &dwColor, 3);
	RwCameraBeginUpdate((RwCamera*)m_camera);
	RpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*) (void))(g_libGTASA + 0x5D0BC0 + 1))();

	RenderClumpOrAtomic(atomic);
	RwCameraEndUpdate((RwCamera*)m_camera);
	RpWorldRemoveLight(m_light);
	DestroyAtomicOrClump(atomic);
	
	if (bNeedRemoveModel)
		pGame->RemoveModel(iModel, false);

	return bufferTexture;
}
// 0.3.7
uintptr_t CSnapShotHelper::CreatePedSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom)
{
	FLog("Ped snapshot: %d", iModel);

	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_libGTASA + 0x1DB83C + 1))(raster);

	CPlayerPed* pPed = new CPlayerPed(208, 0, 0.0f, 0.0f, 0.0f, 0.0f);

	if (!raster || !bufferTexture || !pPed) return 0;

	float posZ = iModel == 162 ? 50.15f : 50.05f;
	float posY = fZoom * -2.25f;
	pPed->TeleportTo(0.0f, posY, posZ);
	pPed->SetModelIndex(iModel);
	pPed->SetGravityProcessing(false);
	pPed->SetCollisionChecking(false);

	RwMatrix mat;
	pPed->GetMatrix(&mat);

	if (vecRot->x != 0.0f)
		RwMatrixRotate(&mat, 0, vecRot->x);
	if (vecRot->y != 0.0f)
		RwMatrixRotate(&mat, 1, vecRot->y);
	if (vecRot->z != 0.0f)
		RwMatrixRotate(&mat, 2, vecRot->z);

	pPed->UpdateMatrix(mat);

	// set camera frame buffer //
	*(uintptr_t*)(m_camera + 0x60) = raster;
	// CVisibilityPlugins::SetRenderWareCamera
	((void(*)(uintptr_t))(g_libGTASA + 0x5D61F8 + 1))(m_camera);

	// RwCameraClear
	((void(*)(uintptr_t, uint32_t*, int))(g_libGTASA + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);

	RpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*) (void))(g_libGTASA + 0x5D0BC0 + 1))();

	pPed->Add();

	pPed->ClumpUpdateAnimations(100.0f, 1);
	pPed->Render();

	RwCameraEndUpdate((RwCamera*)m_camera);

	RpWorldRemoveLight(m_light);

	pPed->Remove();

	delete pPed;

	if (!GetModelRefCounts(iModel))
		pGame->RemoveModel(iModel, false);

	return bufferTexture;
}

uintptr_t CSnapShotHelper::CreateVehicleSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom, uint32_t dwColor1, uint32_t dwColor2)
{
	FLog("Vehicle snapshot: %d", iModel);

	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_libGTASA + 0x1DB83C + 1))(raster);

	if (iModel == 570) {
		iModel = 538;
	}
	else if (iModel == 569) {
		iModel = 537;
	}

	CVehicle* pVehicle = new CVehicle(iModel, 0.0f, 0.0f, 50.0f, 0.0f, false, false);

	if (!raster || !bufferTexture || !pVehicle) return 0;

	pVehicle->SetGravityProcessing(false);
	pVehicle->SetCollisionChecking(false);
	float radius = GetModelColSphereRadius(iModel);
	float posY = (-1.0f - (radius + radius)) * fZoom;
	if (pVehicle->GetVehicleSubtype() == VEHICLE_SUBTYPE_BOAT) {
		posY = -5.5f - radius * 2.5f;
	}

	pVehicle->TeleportTo(0.0f, posY, 50.0f);
	if (dwColor1 != 0xFFFFFFFF && dwColor2 != 0xFFFFFFFF) {
		pVehicle->SetColor(dwColor1, dwColor2);
	}

	RwMatrix mat;
	pVehicle->GetMatrix(&mat);

	if (vecRot->x != 0.0f) {
		RwMatrixRotate(&mat, 0, vecRot->x);
	}
	if (vecRot->y != 0.0f) {
		RwMatrixRotate(&mat, 1, vecRot->y);
	}
	if (vecRot->z != 0.0f) {
		RwMatrixRotate(&mat, 2, vecRot->z);
	}

	pVehicle->UpdateMatrix(mat);

	*(uintptr_t*)(m_camera + 0x60) = raster;
	// CVisibilityPlugins::SetRenderWareCamera
	((void(*)(uintptr_t))(g_libGTASA + 0x5D61F8 + 1))(m_camera);

	// RwCameraClear
	((void(*)(uintptr_t, uint32_t*, int))(g_libGTASA + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);
	RpWorldAddLight(m_light);

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEGOURAUD);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*) (void))(g_libGTASA + 0x5D0BC0 + 1))();

	pVehicle->Add();

	pVehicle->Render();
	RwCameraEndUpdate((RwCamera*)m_camera);
	RpWorldRemoveLight(m_light);

	pVehicle->Remove();
	delete pVehicle;

	return bufferTexture;
}

/*
uintptr_t CSnapShotHelper::CreateObjectSnapShot(int iModel, uint32_t dwColor, CVector* vecRot, float fZoom)
{
	if (iModel == 1373 || iModel == 3118 || iModel == 3552 || iModel == 3553)
		iModel = 18631;

	bool bNeedRemoveModel = false;
	if (!pGame->IsModelLoaded(iModel))
	{
		pGame->RequestModel(iModel);
		pGame->LoadRequestedModels();
		while (!pGame->IsModelLoaded(iModel)) usleep(1000);
		bNeedRemoveModel = true;
	}

	uintptr_t pRwObject = ModelInfoCreateInstance(iModel);

	float fRadius = GetModelColSphereRadius(iModel);

	CVector vecCenter = { 0.0f, 0.0f, 0.0f };
	GetModelColSphereVecCenter(iModel, &vecCenter);

	uintptr_t parent = *(uintptr_t*)(pRwObject + 4);

	if (parent == 0) return 0;
	// RwFrameTranslate
	float v[3] = {
		-vecCenter.x,
		(-0.1f - fRadius * 2.25f) * fZoom,
		50.0f - vecCenter.z };

	// RwFrameTranslate
	((void(*)(uintptr_t, float*, int))(g_libGTASA + 0x1D8694 + 1))(parent, v, 1);

	if (iModel == 18631)
		{
			// RwFrameRotate x
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 1.0f;
			((void(*)(uintptr_t, float*, float, int))(g_libGTASA + 0x1D87A8 + 1))(parent, v, 180.0f, 1);
		}
	else
	{
		if (vecRot->x != 0.0f)
		{
			// RwFrameRotate x
			v[0] = 1.0f;
			v[1] = 0.0f;
			v[2] = 0.0f;
			((void(*)(uintptr_t, float*, float, int))(g_libGTASA + 0x1D87A8 + 1))(parent, v, vecRot->x, 1);
		}

		if (vecRot->y != 0.0f)
		{
			// RwFrameRotate y
			v[0] = 0.0f;
			v[1] = 1.0f;
			v[2] = 0.0f;
			((void(*)(uintptr_t, float*, float, int))(g_libGTASA + 0x1D87A8 + 1))(parent, v, vecRot->y, 1);
		}

		if (vecRot->z != 0.0f)
		{
			// RwFrameRotate z
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 1.0f;
			((void(*)(uintptr_t, float*, float, int))(g_libGTASA + 0x1D87A8 + 1))(parent, v, vecRot->z, 1);
		}
	}

	// RENDER DEFAULT //

	// set camera frame buffer //

	uintptr_t raster = (uintptr_t)RwRasterCreate(256, 256, 32, rwRASTERFORMAT8888 | rwRASTERTYPECAMERATEXTURE);
	// RwTextureCreate
	uintptr_t bufferTexture = ((uintptr_t(*)(uintptr_t))(g_libGTASA + 0x1DB83C + 1))(raster);
	*(uintptr_t*)(m_camera + 0x60) = raster;

	// CVisibilityPlugins::SetRenderWareCamera
	((void(*)(uintptr_t))(g_libGTASA + 0x5D61F8 + 1))(m_camera);

	ProcessCamera(pRwObject, dwColor);

	DestroyAtomicOrClump(pRwObject);

	if (bNeedRemoveModel) {
		pGame->RemoveModel(iModel, false);
	}

	return (uintptr_t)bufferTexture;
}
*/
/*
void CSnapShotHelper::ProcessCamera(uintptr_t pRwObject, uint32_t dwColor)
{
	// RwCameraClear
	((void(*)(uintptr_t, uint32_t*, int))(g_libGTASA + 0x1D5D70 + 1))(m_camera, &dwColor, 3);

	RwCameraBeginUpdate((RwCamera*)m_camera);

	// RpWorldAddLight
	uintptr_t pRwWorld = *(uintptr_t*)(g_libGTASA + 0x9FC938);
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x21E830 + 1))(pRwWorld, m_light);
	}

	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
	RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODENASHADEMODE);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODENACULLMODE);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);

	// DefinedState
	((void(*) (void))(g_libGTASA + 0x5D0BC0 + 1))();

	RenderClumpOrAtomic(pRwObject);

	RwCameraEndUpdate((RwCamera*)m_camera);

	// RpWorldRemoveLight
	if (pRwWorld) {
		((void(*)(uintptr_t, uintptr_t))(g_libGTASA + 0x21E874 + 1))(pRwWorld, m_light);
	}
}
*/