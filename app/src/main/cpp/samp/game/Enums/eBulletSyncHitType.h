//
// Created by x1y2z on 09.05.2023.
//

#pragma once
#include <stdint.h>
enum eBulletSyncHitType : uint8_t
{
    BULLET_HIT_TYPE_NONE,
    BULLET_HIT_TYPE_PLAYER,
    BULLET_HIT_TYPE_VEHICLE,
    BULLET_HIT_TYPE_OBJECT,
    BULLET_HIT_TYPE_PLAYER_OBJECT
};