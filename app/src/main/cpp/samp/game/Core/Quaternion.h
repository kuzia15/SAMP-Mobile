#pragma once

#include "game/RW/RenderWare.h"
#include "Vector.h"

#pragma pack(push, 1)
class CQuaternion
{
public:
    constexpr CQuaternion() {};
    constexpr CQuaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

    void Set(float _x, float _y, float _z, float _w)
    {
        w = _w;
        x = _x;
        y = _y;
        z = _z;
    }

    void Set(CQuaternion q)
    {
        w = q.w;
        x = q.x;
        y = q.y;
        z = q.z;
    }

    void SetFromMatrix(RwMatrix* mat);
    void GetMatrix(RwMatrix *mat);
    void Normalize();
    void Slerp(CQuaternion *pQ1, CQuaternion *pQ2, float t);

    // Add right to the quat 0x4D12F0
    void operator+=(const CQuaternion& right) {
        x += right.x;
        y += right.y;
        z += right.z;
        w += right.w;
    }

    // Substract right from the quat 0x4D1320
    void operator-=(const CQuaternion& right) {
        x -= right.x;
        y -= right.y;
        z -= right.z;
        w -= right.w;
    }

    // Assigns value from other quat 0x4D00C0
    CQuaternion& operator=(const CQuaternion& right) {
        x = right.x;
        y = right.y;
        z = right.z;
        w = right.w;
        return *this;
    }

    // Multiplies quat by a floating point value 0x4CF9B0
    void operator*=(float multiplier) {
        x *= multiplier;
        y *= multiplier;
        z *= multiplier;
        w *= multiplier;
    }

    CQuaternion operator-() const {
        return { -x, -y, -z, -w };
    }

    // NOTSA
    RtQuat* AsRtQuat() { return (RtQuat*)this; }

public:

    struct {
        float w;
        float x;
        float y;
        float z;
    };
};
#pragma pack(pop)
static_assert(sizeof(CQuaternion) == 0x10);


constexpr float DotProduct(const CQuaternion& q1, const CQuaternion& q2) {
    return q1.x * q2.x +
           q1.y * q2.y +
           q1.z * q2.z +
           q1.w * q2.w;
}
constexpr CQuaternion operator+(const CQuaternion& left, const CQuaternion& right) {
    return { left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w };
}

constexpr CQuaternion operator-(const CQuaternion& left, const CQuaternion& right) {
    return { left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w };
}

constexpr CQuaternion operator*(const CQuaternion& left, float right) {
    return { left.x * right, left.y * right, left.z * right, left.w * right };
}

constexpr CQuaternion operator*(float left, const CQuaternion& right) {
    return { left * right.x, left * right.y, left * right.z, left * right.w };
}

constexpr CQuaternion operator/(const CQuaternion& left, float right) {
    return { left.x / right, left.y / right, left.z / right, left.w / right };
}