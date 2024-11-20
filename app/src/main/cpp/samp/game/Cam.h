/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Enums/eCamMode.h"
#include "common.h"

/* http://code.google.com/p/mtasa-blue/source/browse/tags/1.3.4/MTA10/game_sa/CCamSA.h */

//extern bool& gbFirstPersonRunThisFrame;

// __attribute__((aligned(4)))
struct CCam
{
    bool bBelowMinDist;
    bool bBehindPlayerDesired;
    bool m_bCamLookingAtVector;
    bool m_bCollisionChecksOn;

    bool m_bFixingBeta;
    bool m_bTheHeightFixerVehicleIsATrain;
    bool LookBehindCamWasInFront;
    bool LookingBehind;

    bool LookingLeft;
    bool LookingRight;
    bool ResetStatics;
    bool ResetKeyboardStatics;

    bool Rotating;
    uint8 pad0;
    eCamMode m_nMode;

    uint32 m_uiFinishTime;
    int m_iDoCollisionChecksOnFrameNum;
    int m_iDoCollisionCheckEveryNumOfFrames;
    int m_iFrameNumWereAt;

    int DirectionWasLooking;
    float m_fSyphonModeTargetZOffSet;
    float m_fAlphaSpeedOverOneFrame;
    float m_fBetaSpeedOverOneFrame;

    float m_fCamBufferedHeight;
    float m_fCamBufferedHeightSpeed;
    float m_fCloseInPedHeightOffset;
    float m_fCloseInPedHeightOffsetSpeed;
    float m_fCloseInCarHeightOffset;
    float m_fCloseInCarHeightOffsetSpeed;
    float m_fDimensionOfHighestNearCar;
    float m_fDistanceBeforeChanges;
    float m_fFovSpeedOverOneFrame;
    float m_fMinDistAwayFromCamWhenInterPolating;
    float m_fPedBetweenCameraHeightOffset;
    float m_fPlayerInFrontSyphonAngleOffSet;
    float m_fRadiusForDead;
    float m_fRealGroundDist;
    float m_fTimeElapsedFloat;
    float m_fTilt;
    float m_fTiltSpeed;
    float m_fTransitionBeta;
    float m_fTrueBeta;
    float m_fTrueAlpha;
    float m_fInitialPlayerOrientation;
    float Alpha;
    float AlphaSpeed;
    float FOV;
    float FOVSpeed;
    float m_fHorizontalAngle;
    float BetaSpeed;
    float Distance;
    float DistanceSpeed;
    float CA_MIN_DISTANCE;
    float CA_MAX_DISTANCE;
    float SpeedVar;
    float m_fCameraHeightMultiplier;
    float m_fTargetZoomGroundOne;
    float m_fTargetZoomGroundTwo;
    float m_fTargetZoomGroundThree;
    float m_fTargetZoomOneZExtra;
    float m_fTargetZoomTwoZExtra;
    float m_fTargetZoomTwoInteriorZExtra;
    float m_fTargetZoomThreeZExtra;
    float m_fTargetZoomZCloseIn;
    float m_fMinRealGroundDist;
    float m_fTargetCloseInDist;
    float Beta_Targeting;
    float X_Targetting;
    float Y_Targetting;
    int32 CarWeAreFocussingOn;
    float CarWeAreFocussingOnI;
    float m_fCamBumpedHorz;
    float m_fCamBumpedVert;
    int32 m_nCamBumpedTime;
    CVector m_cvecSourceSpeedOverOneFrame;
    CVector m_cvecTargetSpeedOverOneFrame;
    CVector m_cvecUpOverOneFrame;
    CVector m_cvecTargetCoorsForFudgeInter;
    CVector m_cvecCamFixedModeVector;
    CVector m_cvecCamFixedModeSource;
    CVector m_cvecCamFixedModeUpOffSet;
    CVector m_vecLastAboveWaterCamPosition;
    CVector m_vecBufferedPlayerBodyOffset;
    CVector Front;
    CVector Source;
    CVector SourceBeforeLookBehind;
    CVector Up;
    CVector m_arrPreviousVectors[2];
    CVector m_aTargetHistoryPos[4];
    uint32 m_nTargetHistoryTime[4];
    int32 m_nCurrentHistoryPoints;
    uintptr *CamTargetEntity;
    float m_fCameraDistance;
    float m_fIdealAlpha;
    float m_fPlayerVelocity;
    uintptr *m_pLastCarEntered;
    uintptr *m_pLastPedLookedAt;
    bool m_bFirstPersonRunAboutActive;
    uint8 pad1[3];
};


VALIDATE_SIZE(CCam, (VER_x32 ? 0x210 : 0x228));
