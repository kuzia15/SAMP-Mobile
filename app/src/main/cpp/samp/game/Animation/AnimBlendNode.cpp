//
// Created by x1y2z on 06.05.2023.
//

#include "AnimBlendNode.h"
#include "AnimBlendAssociation.h"
#include "../vendor/armhook/patch.h"

void CAnimBlendNode::Init() {
    m_pAnimBlendAssociation = nullptr;
    m_pAnimSequence = nullptr;
    m_fRemainingTime = 0.0f;
    m_PreviousKeyFrameId = -1;
    m_nNextKeyFrameId = -1;
}

bool CAnimBlendNode::FindKeyFrame(float time) {
    if (m_pAnimSequence->m_nFrameCount < 1) {
        return false;
    }

    m_nNextKeyFrameId = 0;
    m_PreviousKeyFrameId = m_nNextKeyFrameId;

    if (m_pAnimSequence->m_nFrameCount == 1) {
        m_fRemainingTime = 0.0f;
        CalcDeltas();
        return true;
    }

    // advance until t is between m_PreviousKeyFrameId and m_nNextKeyFrameId
    // maybe check if GetUncompressedFrame is < numFrames?
    while (time > m_pAnimSequence->GetUncompressedFrame(++m_nNextKeyFrameId)->deltaTime) {
        time -= m_pAnimSequence->GetUncompressedFrame(m_nNextKeyFrameId)->deltaTime;
        if (m_nNextKeyFrameId + 1 >= m_pAnimSequence->m_nFrameCount) {
            // reached end of animation
            if (!m_pAnimBlendAssociation->IsRepeating()) {
                CalcDeltas();
                m_fRemainingTime = 0.0f;
                return false;
            }
            m_nNextKeyFrameId = 0;
        }
        m_PreviousKeyFrameId = m_nNextKeyFrameId;
    }

    m_fRemainingTime = m_pAnimSequence->GetUncompressedFrame(m_nNextKeyFrameId)->deltaTime - time;

    CalcDeltas();
    return true;
}

void CAnimBlendNode::CalcTheta(float angle) {
    m_fTheta0 = std::acos(std::min(angle, 1.0f));
    m_fTheta1 = m_fTheta0 == 0.0f ? 0.0f : 1.0f / std::sin(m_fTheta0);
}

// 0x4D0190
void CAnimBlendNode::CalcDeltas() {
    if (!m_pAnimSequence->m_numFramesSet) {
        return;
    }

    KeyFrame* kfA = m_pAnimSequence->GetUncompressedFrame(m_nNextKeyFrameId);
    KeyFrame* kfB = m_pAnimSequence->GetUncompressedFrame(m_PreviousKeyFrameId);

    CalcTheta(DotProduct(kfA->rotation, kfB->rotation));
}

// 0x4D0350
void CAnimBlendNode::CalcDeltasCompressed() {
    if (!m_pAnimSequence->m_numFramesSet) {
        return;
    }

    KeyFrameCompressed* kfA = m_pAnimSequence->GetCompressedFrame(m_nNextKeyFrameId);
    KeyFrameCompressed* kfB = m_pAnimSequence->GetCompressedFrame(m_PreviousKeyFrameId);
    CQuaternion rotA, rotB;
    kfA->GetRotation(&rotA);
    kfB->GetRotation(&rotB);
    float cos = DotProduct(rotA, rotB);
    if (cos < 0.0f) {
        rotB = -rotB;
        kfB->SetRotation(rotB);
    }
    CalcTheta(DotProduct(rotA, rotB));
}

// 0x4CFC50
void CAnimBlendNode::GetCurrentTranslation(CVector& trans, float weight) {
    assert(0);
}

// 0x4CFE60
void CAnimBlendNode::GetCurrentTranslationCompressed(CVector& trans, float weight) {
    assert(0);
}

// 0x4CFD90
void CAnimBlendNode::GetEndTranslation(CVector& trans, float weight) {
    assert(0);
}

// 0x4D0000
void CAnimBlendNode::GetEndTranslationCompressed(CVector& trans, float weight) {
    assert(0);
}

bool CAnimBlendNode::NextKeyFrame() {
    return CHook::CallFunction<bool> (g_libGTASA + (VER_x32 ? 0x0038AC1C + 1 : 0x463114), this);
}

// 0x4D0570
bool CAnimBlendNode::NextKeyFrameCompressed() {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x0038B3FC + 1 : 0x463954), this);
}

// 0x4CFB90
bool CAnimBlendNode::NextKeyFrameNoCalc() {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x0038A9EE + 1 : 0x462E90), this);
}

// 0x4D0650
bool CAnimBlendNode::SetupKeyFrameCompressed() {
    if (m_pAnimSequence->m_nFrameCount < 1) {
        return false;
    }

    m_nNextKeyFrameId = 1;
    m_PreviousKeyFrameId = 0;

    if (m_pAnimSequence->m_nFrameCount == 1) {
        m_nNextKeyFrameId = 0;
        m_fRemainingTime = 0.0f;
    } else
        m_fRemainingTime = m_pAnimSequence->GetCompressedFrame(m_nNextKeyFrameId)->GetDeltaTime();

    CalcDeltasCompressed();
    return true;
}

// 0x4D06C0
bool CAnimBlendNode::Update(CVector& trans, CQuaternion& rot, float weight) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x0038AA94 + 1 : 0x462F74), this, trans, rot, weight);
}

// 0x4D08D0
bool CAnimBlendNode::UpdateCompressed(CVector& trans, CQuaternion& rot, float weight) {
    return CHook::CallFunction<bool>(g_libGTASA + (VER_x32 ? 0x0038B1D8 + 1 : 0x4636D0), this, trans, rot, weight);
}

// 0x4D0160
bool CAnimBlendNode::UpdateTime() {
    if (m_pAnimBlendAssociation->IsRunning()) {
        m_fRemainingTime -= m_pAnimBlendAssociation->m_fTimeStep;
        if (m_fRemainingTime <= 0.0f) {
            return NextKeyFrameNoCalc();
        }
    }
    return false;
}
