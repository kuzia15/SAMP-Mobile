//
// Created by x1y2z on 21.05.2023.
//

#include "RegisteredCorona.h"
#include "game/Enums/eEntityType.h"
inline const float invLerp(float fMin, float fMax, float fVal) {
    return (fVal - fMin) / (fMax - fMin);
}
auto CRegisteredCorona::CalculateIntensity(float scrZ, float farClip) const -> float {
    return std::clamp(invLerp(farClip, farClip / 2.f, scrZ), 0.f, 1.f);
}
