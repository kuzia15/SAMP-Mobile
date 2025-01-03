//
// Created by x1y2z on 06.04.2023.
//

#pragma once

#include "game/SimpleTransform.h"
#include "game/Core/MatrixLink.h"
#define VALIDATE_SIZE(struc, size) static_assert(sizeof(struc) == size, "Invalid structure size of " #struc)

#pragma pack(push, 1)
class CPlaceable
{
public:
    //uint32_t            vtablee;
    CSimpleTransform    m_placement; // sa name - m_transform
    CMatrixLink         *m_matrix;

public:
    static void InjectHooks();

    CPlaceable();
    virtual ~CPlaceable();

    CMatrix& GetMatrix();
    void GetMatrix(RwMatrix* Matrix);

    static void ShutdownMatrixArray();
    static void InitMatrixArray();

    CVector GetRightVector();
    CVector GetForwardVector();
    CVector GetUpVector();

    void FreeStaticMatrix();
    void SetPosn(float x, float y, float z);
    void SetPosn(const CVector& posn);
    void SetOrientation(float x, float y, float z);
    void SetOrientation(CVector radians) { SetOrientation(radians.x, radians.y, radians.z); } // TODO: Replace method above with this
    void GetOrientation(float& x, float& y, float& z);
    void GetOrientationDeg(float& x, float& y, float& z);
    void SetHeading(float heading);
    float GetHeading();
    float GetRoll() const;
    bool IsWithinArea(float x1, float y1, float x2, float y2) const;
    bool IsWithinArea(float x1, float y1, float z1, float x2, float y2, float z2) const;
    void RemoveMatrix();
    void AllocateStaticMatrix();
    void AllocateMatrix();
    void SetMatrix(CMatrix& matrix);

    // NOTSA
    bool IsPointInRange(const CVector& point, float range);
    bool IsEntityInRange(const CPlaceable* entity, float range) { return IsPointInRange(entity->GetPosition(), range); }
public:
    static constexpr uint32_t NUM_MATRICES_TO_CREATE = 10000;

    inline CVector& GetRight() const { return m_matrix->GetRight(); }
    inline CVector& GetForward() const { return m_matrix->GetForward(); }
    inline CVector& GetUp() const { return m_matrix->GetUp(); }
    inline const CVector& GetPosition() const { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    inline CVector& GetPosition() { return m_matrix ? m_matrix->GetPosition() : m_placement.m_vPosn; }
    inline CVector2D GetPosition2D() { return { GetPosition() }; }
};
#pragma pack(pop)

VALIDATE_SIZE(CPlaceable, (VER_x32 ? 0x18 : 0x20));
