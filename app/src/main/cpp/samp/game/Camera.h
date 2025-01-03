//
// Created by x1y2z on 26.07.2023.
//

#pragma once


#include "common.h"
#include "game/Enums/eCamMode.h"
#include "Cam.h"
#include "QueuedMode.h"
#include "Matrix.h"
#include "game/Entity/Placeable.h"
#include "game/Entity/CEntityGTA.h"

struct CPedGTA;
enum class eFadeFlag : uint16 {
    FADE_IN,
    FADE_OUT
};

enum class eSwitchType : uint16 {
    NONE,
    INTERPOLATION,
    JUMPCUT
};

enum class eGroundHeightType : int32 {
    ENTITY_BB_BOTTOM = 0,    // ground height + boundingBoxMin.z of colliding entity
    EXACT_GROUND_HEIGHT = 1, // ignores height of colliding entity at position
    ENTITY_BB_TOP = 2        // ground height + boundingBoxMax.z of colliding entity
};

struct CVehicleCamTweak
{
    int32 m_ModelId;
    float m_LenMod;
    float m_TargetZMod;
    float m_PitchMod;
};
VALIDATE_SIZE(CVehicleCamTweak, 0x10);

class CCamera : public CPlaceable {
public:
    bool            m_bAboveGroundTrainNodesLoaded{};
    bool            m_bBelowGroundTrainNodesLoaded{};
    bool            m_bCamDirectlyBehind{};
    bool            m_bCamDirectlyInFront{};

    bool            m_bCameraJustRestored{};
    bool            m_bCutsceneFinished{};
    bool            m_bCullZoneChecksOn{};
    bool            m_bIdleOn{};

    bool            m_bInATunnelAndABigVehicle{};
    bool            m_bInitialNodeFound{};
    bool            m_bInitialNoNodeStaticsSet{};
    bool            m_bIgnoreFadingStuffForMusic{};

    bool            m_bPlayerIsInGarage{};
    bool            m_bPlayerWasOnBike{};
    bool            m_bJustCameOutOfGarage{};
    bool            m_bJustInitialized{true};

    bool8           m_bJust_Switched;
    bool            m_bLookingAtPlayer{true};
    bool            m_bLookingAtVector;
    bool            m_bMoveCamToAvoidGeom;

    bool            m_bObbeCinematicPedCamOn;
    bool            m_bObbeCinematicCarCamOn;
    bool            m_bRestoreByJumpCut;
    bool            m_bUseNearClipScript;

    bool            m_bStartInterScript;
    bool8           m_bStartingSpline;
    bool            m_bTargetJustBeenOnTrain;
    bool            m_bTargetJustCameOffTrain;

    bool            m_bUseSpecialFovTrain;
    bool            m_bUseTransitionBeta;
    bool            m_bUseScriptZoomValuePed;
    bool            m_bUseScriptZoomValueCar;

    bool            m_bWaitForInterpolToFinish;
    bool            m_bItsOkToLookJustAtThePlayer;
    bool            m_bWantsToSwitchWidescreenOff;
    bool            m_WideScreenOn;

    bool            m_1rstPersonRunCloseToAWall;
    bool            m_bHeadBob;
    bool            m_bVehicleSuspenHigh;
    bool            m_bEnable1rstPersonCamCntrlsScript;

    bool            m_bAllow1rstPersonWeaponsCamera;
    bool            m_bCooperativeCamMode;
    bool            m_bAllowShootingWith2PlayersInCar{true};
    bool            m_bDisableFirstPersonInCar;

    eCamMode        m_nModeForTwoPlayersSeparateCars{ MODE_TWOPLAYER_SEPARATE_CARS };
    eCamMode        m_nModeForTwoPlayersSameCarShootingAllowed{ MODE_TWOPLAYER_IN_CAR_AND_SHOOTING };
    eCamMode        m_nModeForTwoPlayersSameCarShootingNotAllowed{ MODE_BEHINDCAR };
    eCamMode        m_nModeForTwoPlayersNotBothInCar{ MODE_TWOPLAYER };

    bool            m_bGarageFixedCamPositionSet;
    bool            m_vecDoingSpecialInterPolation;
    bool            m_bScriptParametersSetForInterPol;
    bool            m_bFading;

    bool            m_bMusicFading;
    bool            m_bMusicFadedOut;
    bool            m_bFailedCullZoneTestPreviously;
    bool            m_FadeTargetIsSplashScreen;

    bool            WorldViewerBeingUsed;
    uint8           m_uiTransitionJUSTStarted;
    uint8           m_uiTransitionState;
    uint8           m_nActiveCam; // SA - ActiveCam;

    uint32          m_uiCamShakeStart;
    uint32          m_uiFirstPersonCamLastInputTime;
    uint32          m_uiLongestTimeInMill{ 5000 };
    uint32          m_uiNumberOfTrainCamNodes;

    uint32          m_uiTimeLastChange;
    uint32          m_uiTimeWeLeftIdle_StillNoInput;
    uint32          m_uiTimeWeEnteredIdle;
    uint32          m_uiTimeTransitionStart;

    uint32          m_uiTransitionDuration;
    uint32          m_uiTransitionDurationTargetCoors;
    int32           m_BlurBlue;
    int32           m_BlurGreen;

    int32           m_BlurRed;
    int32           m_BlurType;
    int32           m_iWorkOutSpeedThisNumFrames{4};
    int32           m_iNumFramesSoFar;

    int32           m_iCurrentTrainCamNode;
    int32           m_motionBlur;
    int32           m_imotionBlurAddAlpha;
    int32           m_iCheckCullZoneThisNumFrames{6};

    int32           m_iZoneCullFrameNumWereAt;
    int32           WhoIsInControlOfTheCamera;
    int32           m_nCarZoom{2};
    float           m_fCarZoomBase;

    float           m_fCarZoomTotal;
    float           m_fCarZoomSmoothed;
    float           m_fCarZoomValueScript;
    int32           m_nPedZoom{2};

    float           m_fPedZoomBase;
    float           m_fPedZoomTotal;
    float           m_fPedZoomSmoothed;
    float           m_fPedZoomValueScript;

    float           m_fCamFrontXNorm;
    float           m_fCamFrontYNorm;
    float           DistanceToWater;
    float           HeightOfNearestWater;

    float           FOVDuringInter;
    float           m_fLODDistMultiplier{1.f}; // SA - LODDistMultiplier;
    float           GenerationDistMultiplier;
    float           m_fAlphaSpeedAtStartInter;

    float           m_fAlphaWhenInterPol;
    float           m_fAlphaDuringInterPol;
    float           m_fBetaDuringInterPol;
    float           m_fBetaSpeedAtStartInter;

    float           m_fBetaWhenInterPol;
    float           m_fFOVWhenInterPol;
    float           m_fFOVSpeedAtStartInter;
    float           m_fStartingBetaForInterPol;

    float           m_fStartingAlphaForInterPol;
    float           m_PedOrientForBehindOrInFront;
    float           m_CameraAverageSpeed;
    float           m_CameraSpeedSoFar;

    float           m_fCamShakeForce;
    float           m_fFovForTrain{70.f};
    float           m_fFOV_Wide_Screen;
    float           m_fNearClipScript{ 0.9f };

    float           m_fOldBetaDiff;
    float           m_fPositionAlongSpline;
    float           m_ScreenReductionPercentage;
    float           m_ScreenReductionSpeed;

    float           m_AlphaForPlayerAnim1rstPerson;
    float           m_fOrientation;
    float           m_fPlayerExhaustion{1.f}; // SA - PlayerExhaustion;
    float           SoundDistUp;

    float           SoundDistUpAsRead;
    float           SoundDistUpAsReadOld;
    float           m_fAvoidTheGeometryProbsTimer;
    int16           m_nAvoidTheGeometryProbsDirn;
    uint8           pad0[2];

    float           m_fWideScreenReductionAmount;
    float           m_fStartingFOVForInterPol;

    CCam            m_aCams[3]{};
    uintptr         *pToGarageWeAreIn;
    uintptr         *pToGarageWeAreInForHackAvoidFirstPerson;

    CQueuedMode     m_PlayerMode;
    CQueuedMode     PlayerWeaponMode;

    CVector         m_PreviousCameraPosition;
    CVector         m_RealPreviousCameraPosition;
    CVector         m_vecAimingTargetCoors;
    CVector         m_vecFixedModeVector;

    CVector         m_vecFixedModeSource;
    CVector         m_vecFixedModeUpOffSet;
    CVector         m_vecCutSceneOffset;
    CVector         m_cvecStartingSourceForInterPol;

    CVector         m_cvecStartingTargetForInterPol;
    CVector         m_cvecStartingUpForInterPol;
    CVector         m_cvecSourceSpeedAtStartInter;
    CVector         m_cvecTargetSpeedAtStartInter;

    CVector         m_cvecUpSpeedAtStartInter;
    CVector         m_vecSourceWhenInterPol;
    CVector         m_vecTargetWhenInterPol;
    CVector         m_vecUpWhenInterPol;

    CVector         m_vecClearGeometryVec;
    CVector         m_vecGameCamPos;
    CVector         SourceDuringInter;
    CVector         TargetDuringInter;

    CVector         UpDuringInter;
    CVector         m_vecAttachedCamOffset;
    CVector         m_vecAttachedCamLookAt;
    float           m_fAttachedCamAngle;

    RwCamera        *m_pRwCamera;
    uintptr_t         *pTargetEntity;
    uintptr_t         *pAttachedEntity;
#if VER_x32
    uint8           m_arrPathArray[0x10];
#else
    uint8           m_arrPathArray[0x20];
#endif

    bool            m_bMirrorActive;
    bool            m_bResetOldMatrix;
    uint8           pad2[2];

    float           m_sphereMapRadius;
    CMatrix         m_mCameraMatrix{ CMatrix::Identity() }; // SA - m_cameraMatrix;
    CMatrix         m_cameraMatrixOld;
    CMatrix         m_viewMatrix;

    CMatrix         m_matInverse;
    CMatrix         m_matMirrorInverse;
    CMatrix         m_matMirror;
    CVector         m_vecFrustumNormals[4];

    CVector         m_vecFrustumWorldNormals[4];
    CVector         m_vecFrustumWorldNormals_Mirror[4];
    float           m_fFrustumPlaneOffsets[4];
    float           m_fFrustumPlaneOffsets_Mirror[4];

    CVector         m_vecOldSourceForInter;
    CVector         m_vecOldFrontForInter;
    CVector         m_vecOldUpForInter;
    float           m_vecOldFOVForInter;

    float           m_fFloatingFade;
    float           m_fFloatingFadeMusic;
    float           m_fTimeToFadeOut;
    float           m_fTimeToFadeMusic;

    float           m_fTimeToWaitToFadeMusic;
    float           m_fFractionInterToStopMoving{0.25f}; // SA - m_fFractionInterToStopMoving;
    float           m_fFractionInterToStopCatchUp{0.75f}; // SA - m_fFractionInterToStopCatchUp;
    float           m_fFractionInterToStopMovingTarget;

    float           m_fFractionInterToStopCatchUpTarget;
    float           m_fGaitSwayBuffer{0.85f}; // SA - m_fGaitSwayBuffer;
    float           m_fScriptPercentageInterToStopMoving;
    float           m_fScriptPercentageInterToCatchUp;

    uint32          m_fScriptTimeForInterPolation;
    int16           m_iFadingDirection;
    uint8           pad3[2];
    int             m_nModeObbeCamIsInForCar{30}; // SA - m_iModeObbeCamIsInForCar;
    eCamMode        m_nModeToGoTo{ MODE_FOLLOWPED }; // SA - m_iModeToGoTo;
    eFadeFlag       m_iMusicFadingDirection;

    eSwitchType     m_nTypeOfSwitch{ eSwitchType::INTERPOLATION };;
    int16           pad4;
    uint32          m_uiFadeTimeStarted;
    uint32          m_uiFadeTimeStartedMusic;
    int32           m_numExtrasEntitysToIgnore;

    uintptr_t         *m_pExtrasEntitysToIgnore[2];
    float           m_duckZMod;
    float           m_duckZMod_Aim;

    float           m_vectorTrackStartTime;
    float           m_vectorTrackEndTime;
    CVector         m_vectorTrackFrom;
    CVector         m_vectorTrackTo;

    bool            m_bVectorTrackSmoothEnds;
    uint8           pad5[3];

    CVector         m_VectorTrackScript;
    bool            m_bVectorTrackScript;
    uint8           pad6[3];
    float           m_DegreeHandShake;
    float           m_shakeStartTime;

    float           m_shakeEndTime;
    bool            m_bShakeScript;
    uint8           pad7[3];
    int32           m_CurShakeCam{1};
    float           m_FOVLerpStartTime;

    float           m_FOVLerpEndTime;
    float           m_FOVLerpStart;
    float           m_FOVLerpEnd;

    bool            m_bFOVLerpSmoothEnds;
    bool            m_bFOVScript;
    uint8           pad8[2];

    float           m_MyFOV;
    float           m_vectorMoveStartTime;
    float           m_vectorMoveEndTime;

    CVector         m_vectorMoveFrom;
    CVector         m_vectorMoveTo;

    bool            m_bVectorMoveSmoothEnds;
    uint8           pad9[3];

    CVector         m_VectorMoveScript;

    bool            m_bVectorMoveScript;
    bool            m_bPersistFOV;
    bool            m_bPersistCamPos;
    bool            m_bPersistCamLookAt;

    bool            m_bForceCinemaCam;
    uint8           pad10[3];

    CVehicleCamTweak m_VehicleTweaks[5];

    bool m_bInitedVehicleCamTweaks;
    uint8           pad11[3];

    float m_VehicleTweakLenMod;
    float m_VehicleTweakTargetZMod;
    float m_VehicleTweakPitchMod;
    int32 m_VehicleTweakLastModelId;

    float m_TimeStartFOVLO;
    float m_TimeEndFOVLO;
    float m_FOVStartFOVLO;
    CVector m_StartPositionFOVLO;
    float m_FOVTargetFOVLO;

    bool m_bSmoothLerpFOVLO;
    bool m_bInitLockOnCam;
    uint8 pad12[2];

    //uint8 pad[0xD0];

public:
    static inline float m_f3rdPersonCHairMultX{0.53f};
    static inline float m_f3rdPersonCHairMultY{0.4f};

public:
    static void InjectHooks();

    void Init();

    void Restore();
    void RestoreCameraAfterMirror();
    void RestoreWithJumpCut();
    void RenderMotionBlur();
    void ResetDuckingSystem(CPedGTA *ped);

    void CalculateDerivedValues(bool bForMirror, bool bOriented);
    void CalculateFrustumPlanes(bool bForMirror);
    float CalculateGroundHeight(eGroundHeightType type);
    void CalculateMirroredMatrix(CVector posn, float mirrorV, CMatrix* camMatrix, CMatrix* mirrorMatrix);
    void CamControl();

    //! Get the camera's front normal (Whatever that is)
    auto GetFrontNormal2D() const { return CVector2D{ m_fCamFrontXNorm, m_fCamFrontYNorm }; }

public:
    static CCam& GetActiveCamera(); // TODO: Replace this with `TheCamera.GetActiveCam()`
    void SetRwCamera(RwCamera *pCamera);
    void TakeControl(CEntityGTA *target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera);
    CMatrix& GetViewMatrix() { return m_viewMatrix; }

public:
    // FIXME
    static void SetBehindPlayer();
    static void SetPosition(float fX, float fY, float fZ, float fRotationX, float fRotationY, float fRotationZ);
    static void LookAtPoint(float fX, float fY, float fZ, int iType);
    static void InterpolateCameraPos(CVector *posFrom, CVector *posTo, int time, uint8_t mode);
    static void InterpolateCameraLookAt(CVector *posFrom, CVector *posTo, int time, uint8_t mode);
};
VALIDATE_SIZE(CCamera, (VER_x32 ? 0xD00 : 0xDB0));
