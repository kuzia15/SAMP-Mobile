
#include "ColTriangle.h"
#include "game/Lines.h"
#include "Matrix.h"
#include "Vector.h"

// NOTSA
void CColTriangle::DrawWireFrame(CRGBA color, const CollisionVector* vertices, const CMatrix& transform) const {
    const CVector a = transform * vertices[vA];
    const CVector b = transform * vertices[vB];
    const CVector c = transform * vertices[vC];

    const auto colorARGB = color.ToInt();
    CLines::RenderLineNoClipping(a, b, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(a, c, colorARGB, colorARGB);
    CLines::RenderLineNoClipping(b, c, colorARGB, colorARGB);
}

auto CColTriangle::GetPlane(const CollisionVector* vertices) const -> CColTrianglePlane {
    return { *this, vertices };
}

auto CColTriangle::GetBoundingRect(const CVector& a, const CVector& b, const CVector& c) -> CRect {
    const auto [left, right] = std::minmax({ a.x, b.x, c.x });
    const auto [bottom, top] = std::minmax({ a.y, b.y, c.y });
    return CRect{ left, bottom, right, top };
}

auto CColTriangle::GetPoly(const CollisionVector* verts) const -> CStoredCollPoly {
    return CStoredCollPoly{
            .verts    = { verts[vA], verts[vB], verts[vC] },
            .valid    = true,
            .ligthing = m_nLight,
    };
}
