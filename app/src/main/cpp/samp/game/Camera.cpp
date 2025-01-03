//
// Created by x1y2z on 26.07.2023.
//

#include "Camera.h"
#include "../vendor/armhook/patch.h"
#include "scripting.h"

void CCamera::InjectHooks() {

}

CCam& CCamera::GetActiveCamera() {
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    return TheCamera.m_aCams[TheCamera.m_nActiveCam];
}

void CCamera::Init() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0046F8C0 + 1 : 0x55BA30), this);
}

void CCamera::SetRwCamera(RwCamera *pCamera) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x003E161C + 1 : 0x4BF318), this, pCamera);
}

void CCamera::TakeControl(CEntityGTA *target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x003E1714 + 1 : 0x4BF474), this, target, modeToGoTo, switchType, whoIsInControlOfTheCamera);
}

float CCamera::CalculateGroundHeight(eGroundHeightType type) {
    return CHook::CallFunction<float>(g_libGTASA + (VER_x32 ? 0x3DC5C8 + 1 : 0x4BA958), this, type);
}

void CCamera::RestoreWithJumpCut() {
    CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x3DB154 + 1 : 0x4B94B4), this);
}

void CCamera::SetBehindPlayer()
{
    ScriptCommand(&lock_camera_position, 0);
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&set_camera_behind_player);
    ScriptCommand(&restore_camera_jumpcut);
}

// 0.3.7
void CCamera::SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ)
{
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&set_camera_position, fX, fY, fZ, fRotationX, fRotationY, fRotationZ);
}


// 0.3.7
void CCamera::LookAtPoint(float fX, float fY, float fZ, int iType)
{
    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&point_camera, fX, fY, fZ, iType);
}

// 0.3.7
void CCamera::InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
    CCamera& TheCamera = *reinterpret_cast<CCamera*>(g_libGTASA + (VER_x32 ? 0x00951FA8 : 0xBBA8D0));

    ScriptCommand(&restore_camera_to_user);
    ScriptCommand(&lock_camera_position1, 1);
    ScriptCommand(&set_camera_pos_time_smooth, posFrom->x, posFrom->y, posFrom->z, posTo->x, posTo->y, posTo->z, time, mode);
}

// 0.3.7
void CCamera::InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode)
{
    ScriptCommand(&lock_camera_position, 1);
    ScriptCommand(&point_camera_transverse, posFrom->x, posFrom->y, posFrom->z, posTo->x, posTo->y, posTo->z, time, mode);
}

