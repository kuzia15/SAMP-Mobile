//
// Created by admin on 17.08.2023.
//

#include "editobject.h"
#include <jni.h>
#include "main.h"

#include "../game/game.h"
#include "net/netgame.h"
#include "gui/gui.h"
#include "jniutil.h"

extern CJavaWrapper *pJavaWrapper;
extern UI *pUI;
extern CNetGame *pNetGame;

void CObjectEditor::startEditPlayerAttach(int slot)
{
    if(!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetAttachedObject(slot)){
        pUI->chat()->addDebugMessage("Invalid attach slot %d", slot);
        return;
    }
    CObjectEditor::iEditedId = slot;
    CObjectEditor::editType = TYPE_PLAYER_ATTACH;
    CObjectEditor::time = GetTickCount();

    CObjectEditor::showGui();
}

void CObjectEditor::startEditObject(uint16_t objectId)
{
    auto pObject = pNetGame->GetObjectPool()->GetAt(objectId);
    if(!pObject) {
        pUI->chat()->addDebugMessage("Invalid object id %d", objectId);
        return;
    }
    CObjectEditor::iEditedId = objectId;

    CObjectEditor::editType = TYPE_OBJECT;

    CObjectEditor::time = GetTickCount();

    CObjectEditor::showGui();
}

void CObjectEditor::showGui() {
    pJavaWrapper->ShowEditObject();

    bIsToggle = true;
}


extern "C"
JNIEXPORT void JNICALL
Java_com_samp_mobile_game_ui_AttachEdit_Exit(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CObjectEditor::iEditedId;

    CObjectEditor::iEditedId = INVALID_EDITED_SLOT;
    if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
        auto attach = pPlayer->GetAttachedObject(slot);
        CObjectEditor::SendOnEditAttach(
                0,
                slot,
                attach->dwModelId,
                attach->dwSampBone,
                attach->vecOffset,
                attach->vecRotation,
                attach->vecScale
        );
    }

    if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT)
    {
        auto pObject = pNetGame->GetObjectPool()->GetAt(CObjectEditor::iEditedId);
        if(!pObject)
        {
            CObjectEditor::bIsToggle = false;
            pJavaWrapper->HideEditObject();
        }

        RwMatrix matrix4X4 = pObject->m_pEntity->GetMatrix().ToRwMatrix();
        auto pos = matrix4X4.pos;
        CVector rot;
        pObject->GetRotation(&rot.x, &rot.y, &rot.z);

        RakNet::BitStream bsSend;

        bsSend.Write((bool)     CObjectEditor::bPlayerObj);
        bsSend.Write((uint16_t) CObjectEditor::iEditedId);
        bsSend.Write((uint32_t) 0);
        bsSend.Write((float)    pos.x);
        bsSend.Write((float)    pos.y);
        bsSend.Write((float)    pos.z);
        bsSend.Write((float)    rot.x);
        bsSend.Write((float)    rot.y);
        bsSend.Write((float)    rot.z);

        pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }

    CObjectEditor::bIsToggle = false;
    pJavaWrapper->HideEditObject();
}

void CObjectEditor::SendOnEditAttach(int response, int index, int modelid, int bone, CVector offset, CVector rot, CVector scale){

    RakNet::BitStream bsSend;

    bsSend.Write((uint32_t)response);
    bsSend.Write((uint32_t)index);
    bsSend.Write((uint32_t)modelid);
    bsSend.Write((uint32_t)bone);
    bsSend.Write(offset);
    bsSend.Write(rot);
    bsSend.Write(scale);
    bsSend.Write((uint32_t)0);
    bsSend.Write((uint32_t)0);

    pNetGame->GetRakClient()->RPC(&RPC_EditAttachedObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_samp_mobile_game_ui_AttachEdit_AttachClick(JNIEnv *env, jobject thiz, jint button_type,
                                                  jboolean button_id) {

    auto pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CObjectEditor::iEditedId;
    auto attach = pPlayer->GetAttachedObject(slot);
    if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {

        if (!pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->GetAttachedObject(
                slot)) {
            CObjectEditor::time = 0;
            pJavaWrapper->HideEditObject();
            return;
        }
    }

    CObject *pObject = nullptr;
    if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
        pObject = pNetGame->GetObjectPool()->GetAt(CObjectEditor::iEditedId);
        if (!pObject) {
            CObjectEditor::time = 0;
            pJavaWrapper->HideEditObject();
            return;
        }
    }

    if(button_type == 0) { // право/лево
        float value = (button_id)?(0.006f):(-0.006f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecOffset.z += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            RwMatrix matrix = pObject->m_pEntity->GetMatrix().ToRwMatrix();
            auto pos = matrix.pos;
            pos.x += value;

            pObject->m_pEntity->SetPosn(pos.x, pos.y, pos.z);
            pObject->m_pEntity->UpdateRW();
            pObject->m_pEntity->UpdateRwFrame();
        }
    }

    if(button_type == 1) { // вверх/низ
        float value = (button_id)?(0.006f):(-0.006f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecOffset.x += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            RwMatrix matrix = pObject->m_pEntity->GetMatrix().ToRwMatrix();
            auto pos = matrix.pos;
            pos.z += value;

            pObject->m_pEntity->SetPosn(pos.x, pos.y, pos.z);
            pObject->m_pEntity->UpdateRW();
            pObject->m_pEntity->UpdateRwFrame();
        }
    }
    if(button_type == 2) { // y
        float value = (button_id)?(0.006f):(-0.006f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecOffset.y += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            RwMatrix matrix = pObject->m_pEntity->GetMatrix().ToRwMatrix();
            auto pos = matrix.pos;
            pos.y += value;

            pObject->m_pEntity->SetPosn(pos.x, pos.y, pos.z);
            pObject->m_pEntity->UpdateRW();
            pObject->m_pEntity->UpdateRwFrame();
        }
    }
    if(button_type == 3) { // scale
        float value = (button_id)?(0.006f):(-0.006f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecScale.x += value;
            attach->vecScale.y += value;
            attach->vecScale.z += value;
        }
    }
    if(button_type == 4) { // rot x
        float value = (button_id)?(0.1f):(-0.1f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecRotation.x += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            float x, y, z;
            pObject->GetRotation(&x, &y, &z);
            x += value;

            if(pObject)
                pObject->InstantRotate(x, y, z);
        }
    }
    if(button_type == 5) { // rot y
        float value = (button_id)?(0.1f):(-0.1f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecRotation.y += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            float x, y, z;
            pObject->GetRotation(&x, &y, &z);
            y += value;

            if(pObject)
                pObject->InstantRotate(x, y, z);
        }
    }
    if(button_type == 6) { // rot z
        float value = (button_id)?(1.0f):(-1.0f);

        if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            attach->vecRotation.z += value;
        }
        if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {

            float x, y, z;
            pObject->GetRotation(&x, &y, &z);
            z += value;

            if(pObject)
                pObject->InstantRotate(x, y, z);
        }
    }

    if(GetTickCount() - CObjectEditor::time > 250) {
        CObjectEditor::time = GetTickCount();

        if (CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
            CObjectEditor::SendOnEditAttach(
                    2,
                    slot,
                    attach->dwModelId,
                    attach->dwSampBone,
                    attach->vecOffset,
                    attach->vecRotation,
                    attach->vecScale
            );
        }
        if (CObjectEditor::editType == CObjectEditor::TYPE_OBJECT) {
            auto pObject = pNetGame->GetObjectPool()->GetAt(CObjectEditor::iEditedId);

            RwMatrix matrix4X4 = pObject->m_pEntity->GetMatrix().ToRwMatrix();
            auto pos = matrix4X4.pos;
            CVector rot;
            pObject->GetRotation(&rot.x, &rot.y, &rot.z);

            RakNet::BitStream bsSend;

            bsSend.Write((bool) CObjectEditor::bPlayerObj);
            bsSend.Write((uint16_t) CObjectEditor::iEditedId);
            bsSend.Write((uint32_t) 2);
            bsSend.Write((float) pos.x);
            bsSend.Write((float) pos.y);
            bsSend.Write((float) pos.z);
            bsSend.Write((float) rot.x);
            bsSend.Write((float) rot.y);
            bsSend.Write((float) rot.z);

            pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0,
                                          false, UNASSIGNED_NETWORK_ID, nullptr);
        }
    }
}
extern "C"
JNIEXPORT void JNICALL
Java_com_samp_mobile_game_ui_AttachEdit_Save(JNIEnv *env, jobject thiz) {
    CPlayerPed* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
    int slot = CObjectEditor::iEditedId;
    auto attach = pPlayer->GetAttachedObject(slot);

    if(CObjectEditor::editType == CObjectEditor::TYPE_PLAYER_ATTACH) {
        CObjectEditor::SendOnEditAttach(
                1,
                slot,
                attach->dwModelId,
                attach->dwSampBone,
                attach->vecOffset,
                attach->vecRotation,
                attach->vecScale
        );
    }
    if(CObjectEditor::editType == CObjectEditor::TYPE_OBJECT)
    {
        auto pObject = pNetGame->GetObjectPool()->GetAt(CObjectEditor::iEditedId);
        if(!pObject)
            goto exit;

        RwMatrix matrix4X4 = pObject->m_pEntity->GetMatrix().ToRwMatrix();
        auto pos = matrix4X4.pos;
        CVector rot;
        pObject->GetRotation(&rot.x, &rot.y, &rot.z);

        RakNet::BitStream bsSend;

        bsSend.Write((bool)     CObjectEditor::bPlayerObj);
        bsSend.Write((uint16_t) CObjectEditor::iEditedId);
        bsSend.Write((uint32_t) CObjectEditor::eResponse::EDIT_RESPONSE_FINAL);
        bsSend.Write((float)    pos.x);
        bsSend.Write((float)    pos.y);
        bsSend.Write((float)    pos.z);
        bsSend.Write((float)    rot.x);
        bsSend.Write((float)    rot.y);
        bsSend.Write((float)    rot.z);

        pNetGame->GetRakClient()->RPC(&RPC_EditObject, &bsSend, HIGH_PRIORITY, RELIABLE, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
    }

    exit:
    CObjectEditor::bIsToggle = false;
    pJavaWrapper->HideEditObject();
    CObjectEditor::time = 0;
}