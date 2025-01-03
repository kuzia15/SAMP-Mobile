//
// Created by x1y2z on 09.04.2023.
//

#pragma once

#include "game/Core/Vector.h"
#include "cTransmission.h"
#include "Enums/eVehicleHandlingModelFlags.h"
#include "Enums/eVehicleHandlingFlags.h"


enum eVehicleLightsSize : uint8_t {
    LIGHTS_LONG,
    LIGHTS_SMALL,
    LIGHTS_BIG,
    LIGHTS_TALL
};

struct tHandlingData
{
    int32_t       m_nVehicleId;
    float         m_fMass; // 1.0 to 50000.0
    float         m_fMassRecpr; // = 1.f / m_fMass
    float         m_fTurnMass;
    float         m_fDragMult;
    CVector       m_vecCentreOfMass;   // x, y, z - 1.0 to 50000.0
    uint8_t       m_nPercentSubmerged; // 10 to 120 (> 100% vehicle sinks)
    uint8_t       pad0[3];
    float         m_fBuoyancyConstant;
    float         m_fTractionMultiplier; // 0.5 to 2.0
    cTransmission m_transmissionData;
    float         m_fBrakeDeceleration; // 0.1 to 10.0
    float         m_fBrakeBias;         // 0.0 > x > 1.0
    bool          m_bABS;               // 0/1
    char          field_9D;
    char          field_9E;
    char          field_9F;
    float         m_fSteeringLock; // 10.0 to 40.0
    float         m_fTractionLoss;
    float         m_fTractionBias;
    float         m_fSuspensionForceLevel;     // not [L/M/H]
    float         m_fSuspensionDampingLevel;   // not [L/M/H]
    float         m_fSuspensionHighSpdComDamp; // often zero - 200.0 or more for bouncy vehicles
    float         m_fSuspensionUpperLimit;
    float         m_fSuspensionLowerLimit;
    float         m_fSuspensionBiasBetweenFrontAndRear;
    float         m_fSuspensionAntiDiveMultiplier;
    float         m_fCollisionDamageMultiplier; // 0.2 to 5.0
    union {
        eVehicleHandlingModelFlags m_nModelFlags;
        struct {
            uint32_t m_bIsVan : 1;
            uint32_t m_bIsBus : 1;
            uint32_t m_bIsLow : 1;
            uint32_t m_bIsBig : 1;
            uint32_t m_bReverseBonnet : 1;
            uint32_t m_bHangingBoot : 1;
            uint32_t m_bTailgateBoot : 1;
            uint32_t m_bNoswingBoot : 1;

            uint32_t m_bNoDoors : 1;
            uint32_t m_bTandemSeats : 1;
            uint32_t m_bSitInBoat : 1;
            uint32_t m_bConvertible : 1;
            uint32_t m_bNoExhaust : 1;
            uint32_t m_bDoubleExhaust : 1;
            uint32_t m_bNo1fpsLookBehind : 1;
            uint32_t m_bForceDoorCheck : 1;

            uint32_t m_bAxleFNotlit : 1;
            uint32_t m_bAxleFSolid : 1;
            uint32_t m_bAxleFMcpherson : 1;
            uint32_t m_bAxleFReverse : 1;
            uint32_t m_bAxleRNotlit : 1;
            uint32_t m_bAxleRSolid : 1;
            uint32_t m_bAxleRMcpherson : 1;
            uint32_t m_bAxleRReverse : 1;

            uint32_t m_bIsBike : 1;
            uint32_t m_bIsHeli : 1;
            uint32_t m_bIsPlane : 1;
            uint32_t m_bIsBoat : 1;
            uint32_t m_bBouncePanels : 1;
            uint32_t m_bDoubleRwheels : 1;
            uint32_t m_bForceGroundClearance : 1;
            uint32_t m_bIsHatchback : 1;
        };
    };
    union {
        eVehicleHandlingFlags m_nHandlingFlags;
        struct {
            uint32_t m_b1gBoost : 1;
            uint32_t m_b2gBoost : 1;
            uint32_t m_bNpcAntiRoll : 1;
            uint32_t m_bNpcNeutralHandl : 1;
            uint32_t m_bNoHandbrake : 1;
            uint32_t m_bSteerRearwheels : 1;
            uint32_t m_bHbRearwheelSteer : 1;
            uint32_t m_bAltSteerOpt : 1;

            uint32_t m_bWheelFNarrow2 : 1;
            uint32_t m_bWheelFNarrow : 1;
            uint32_t m_bWheelFWide : 1;
            uint32_t m_bWheelFWide2 : 1;
            uint32_t m_bWheelRNarrow2 : 1;
            uint32_t m_bWheelRNarrow : 1;
            uint32_t m_bWheelRWide : 1;
            uint32_t m_bWheelRWide2 : 1;

            uint32_t m_bHydraulicGeom : 1;
            uint32_t m_bHydraulicInst : 1;
            uint32_t m_bHydraulicNone : 1;
            uint32_t m_bNosInst : 1;
            uint32_t m_bOffroadAbility : 1;
            uint32_t m_bOffroadAbility2 : 1;
            uint32_t m_bHalogenLights : 1;
            uint32_t m_bProcRearwheelFirst : 1;
            uint32_t m_bUseMaxspLimit : 1;

            uint32_t m_bLowRider : 1;
            uint32_t m_bStreetRacer : 1;
            uint32_t m_bSwingingChassis : 1;
        };
    };
    float              m_fSeatOffsetDistance; // // ped seat position offset towards centre of car
    uint32_t           m_nMonetaryValue;      // 1 to 100000
    eVehicleLightsSize m_nFrontLights;
    eVehicleLightsSize m_nRearLights;
    uint8_t            m_nAnimGroup;
    uint8_t            pad2;

    cTransmission& GetTransmission() { return m_transmissionData; }

    int InitFromData(int id, const char *line);
};
static_assert(sizeof(tHandlingData) == 0xE0, "Invalid size tHandlingData");

