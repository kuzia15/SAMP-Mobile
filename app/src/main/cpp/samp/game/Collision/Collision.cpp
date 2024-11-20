
#include "Collision.h"
#include "../vendor/armhook/patch.h"
#include "ColStore.h"

CCollision::DebugSettings CCollision::s_DebugSettings = {
        CCollision::DebugSettings{}.ShapeShapeCollision,
        false // AllowLineOriginInsideSphere
};

void CCollision::InjectHooks() {
    CHook::Write(g_libGTASA + (VER_x32 ? 0x00677A88 : 0x84D540), &ms_colModelCache);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006783D0 : 0x84E7C8), &ms_iProcessLineNumCrossings);
    CHook::Write(g_libGTASA + (VER_x32 ? 0x006773BC : 0x84C7C0), &ms_collisionInMemory);

    CHook::Write(g_libGTASA + (VER_x32 ? 0x00678D74 : 0x84FB18), &bCamCollideWithVehicles);
    CHook::Redirect("_ZN10CCollision4InitEv", &CCollision::Init);
}

void CCollision::Init() {
    ms_colModelCache.Init(300);
    ms_collisionInMemory = 0;
    CColStore::Initialise();
}

void CCollision::RemoveTrianglePlanes(CCollisionData* colData) {
    if (!colData->m_pTrianglePlanes)
        return;

    auto* link = colData->GetLinkPtr();
    ms_colModelCache.Remove(link);
    colData->RemoveTrianglePlanes();
}

bool CCollision::TestSphereSphere(CColSphere const& sphere1, CColSphere const& sphere2) { // Yes, it's __stdcall
    return (sphere1.m_vecCenter - sphere2.m_vecCenter).SquaredMagnitude() <= sq(sphere1.m_fRadius + sphere2.m_fRadius);
}

void CalculateColPointInsideBox(CBox const& box, CVector const& point, CColPoint& colPoint) {
    const auto pointToCenter = point - box.GetCenter();

    // Calculate point's component-wise distance to each corner on each axis
#define DoAxis(a) (pointToCenter.a <= 0 ? point.a - box.m_vecMin.a : box.m_vecMax.a - point.a)
    const CVector pointToClosest{ DoAxis(x), DoAxis(y), DoAxis(z) };
#undef DoAxis

    const auto CalcNormal = [](float a) {
        return a <= 0.f ? -1.f : 1.f;
    };

    colPoint = {};
    colPoint.m_vecPoint = point;

    if (pointToClosest.x >= pointToClosest.y || pointToClosest.x >= pointToClosest.z) {
        if (pointToClosest.y >= pointToClosest.x || pointToClosest.y >= pointToClosest.z) {
            colPoint.m_vecNormal.z = CalcNormal(pointToCenter.z);
            colPoint.m_fDepth = pointToClosest.z;
        } else {
            colPoint.m_vecNormal.y = CalcNormal(pointToCenter.y);
            colPoint.m_fDepth = pointToClosest.y;
        }
    } else {
        colPoint.m_vecNormal.x = CalcNormal(pointToCenter.x);
        colPoint.m_fDepth = pointToClosest.x;
    }
}

/*!
* @address 0x4120C0
* @brief Tests if the \a bb is fully inside \a sphere
*/
bool CCollision::TestSphereBox(CSphere const& sphere, CBox const& box) {
    for (auto i = 0u; i < 3u; i++) {
        if (sphere.m_vecCenter[i] + sphere.m_fRadius < box.m_vecMin[i] ||
            sphere.m_vecCenter[i] - sphere.m_fRadius > box.m_vecMax[i]
                ) {
            return false;
        }
    }
    return true;
}

/*!
* @address 0x412130
*/
bool CCollision::ProcessSphereBox(CColSphere const& sph, CColBox const& box, CColPoint & colp, float& minDistSq) {
    // GTA's code is too complicated, uses a huge 3x3x3 if statement
    // we can simplify the structure a lot
    // Some of the original code, to give you an idea:
    /*
    if (sphere.m_vecCenter.x + sphere.m_fRadius < bb.m_vecMin.x)
        return false;

    if (sphere.m_vecCenter.x + sphere.m_fRadius > bb.m_vecMax.x)
        return false;

    CVector colPos{};

    if (sphere.m_vecCenter.x >= bb.m_vecMin.x) {
        if (sphere.m_vecCenter.x <= bb.m_vecMax.x) {
            if (sphere.m_vecCenter.y + sphere.m_fRadius > bb.m_vecMin.y)
                return false;

            if (sphere.m_vecCenter.y - sphere.m_fRadius > bb.m_vecMax.y)
                return false;

            if (sphere.m_vecCenter.y >= bb.m_vecMin.y) {
                if (sphere.m_vecCenter.y <= bb.m_vecMax.y) {
                    if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                        return false;

                    if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                        return false;

                    if (sphere.m_vecCenter.z >= bb.m_vecMin.z) {
                        if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                            CColPoint boxCP{};
                            CalculateColPointInsideBox(bb, sphere.m_vecCenter, boxCP);

                            diskColPoint.m_vecPoint = boxCP.m_vecPoint - boxCP.m_vecNormal * sphere.m_fRadius;
                            diskColPoint.m_fDepth = boxCP.m_fDepth;

                            diskColPoint.m_nLightingA = sphere.ligthing;
                            diskColPoint.m_nSurfaceTypeA = sphere.m_nMaterial;

                            diskColPoint.m_nLightingB = bb.ligthing;
                            diskColPoint.m_nSurfaceTypeB = bb.m_nMaterial;

                            maxTouchDistance = 0.f;

                            return true;
                        } else {
                            colPos.x = sphere.m_vecCenter.x;
                            colPos.y = sphere.m_vecCenter.y;
                            colPos.z = bb.m_vecMax.z;
                        }
                    } else {
                        colPos.x = sphere.m_vecCenter.x;
                        colPos.y = sphere.m_vecCenter.y;
                        colPos.z = bb.m_vecMin.z;
                    }
                } else {
                    if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                        return false;

                    if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                        return false;

                    if (sphere.m_vecCenter.z >= bb.m_vecMin.z) {
                        if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                            colPos.x = sphere.m_vecCenter.x;
                            colPos.y = bb.m_vecMax.y;

                            if (sphere.m_vecCenter.z > bb.m_vecMax.z)
                                colPos.y = bb.m_vecMax.y;

                            if (sphere.m_vecCenter.z <= bb.m_vecMax.z) {
                                colPos.z = sphere.m_vecCenter.z;
                                colPos.y = bb.m_vecMax.y;
                            } else {
                                colPos.z = bb.m_vecMax.z;
                            }
                        } else {

                        }
                    } else {
                        colPos.y = bb.m_vecMax.y;
                        colPos.x = sphere.m_vecCenter.x;
                        colPos.z = bb.m_vecMin.z;
                    }
                }
            } else {
                if (sphere.m_vecCenter.z + sphere.m_fRadius > bb.m_vecMin.z)
                    return false;

                if (sphere.m_vecCenter.z - sphere.m_fRadius > bb.m_vecMax.z)
                    return false;

                if (sphere.m_vecCenter.z)
            }
        }
    }
    */

    // First make sure we have a collision at all
    if (!TestSphereBox(sph, box))
        return false;

    // Now find out where the sphere center lies in relation to all the sides
    enum class ClosestCorner {
        INSIDE,
        MIN,
        MAX,
    };
    using enum ClosestCorner;

    ClosestCorner axies[3];
    for (auto i = 0; i < 3; i++) {
        axies[i] = sph.m_vecCenter[i] < box.m_vecMin[i] ? MIN :
                   sph.m_vecCenter[i] > box.m_vecMax[i] ? MAX :
                   INSIDE;
    }

    if(axies[0] == INSIDE && axies[1] == INSIDE && axies[2] == INSIDE) { // Sphere center is inside the bb
        const auto p{ box.GetCenter() };

        const auto dir = sph.m_vecCenter - p;
        const auto distSq = dir.SquaredMagnitude();
        if(distSq < minDistSq){
            colp.m_vecNormal = dir / sqrt(distSq); // Normalize direction
            colp.m_vecPoint = sph.m_vecCenter - colp.m_vecNormal;

            colp.m_nSurfaceTypeA = sph.m_Surface.m_nMaterial;
            colp.m_nLightingA = sph.m_Surface.m_nLighting;

            colp.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
            colp.m_nLightingB = box.m_Surface.m_nLighting;

            // find absolute distance to the closer side in each dimension
            const float dx = dir.x > 0.0f ?
                             box.m_vecMax.x - sph.m_vecCenter.x :
                             sph.m_vecCenter.x - box.m_vecMin.x;

            const float dy = dir.y > 0.0f ?
                             box.m_vecMax.y - sph.m_vecCenter.y :
                             sph.m_vecCenter.y - box.m_vecMin.y;

            const float dz = dir.z > 0.0f ?
                             box.m_vecMax.z - sph.m_vecCenter.z :
                             sph.m_vecCenter.z - box.m_vecMin.z;

            // collision depth is maximum of that:
            colp.m_fDepth = std::max({ dx, dy, dz });

            return true;
        }
    } else { // Sphere centre is outside on at least one axis

        // Position of closest corner:
        const CVector p{
                axies[0] == MIN ? box.m_vecMin.x :
                axies[0] == MAX ? box.m_vecMax.x :
                sph.m_vecCenter.x,

                axies[1] == MIN ? box.m_vecMin.y :
                axies[1] == MAX ? box.m_vecMax.y :
                sph.m_vecCenter.y,

                axies[2] == MIN ? box.m_vecMin.z :
                axies[2] == MAX ? box.m_vecMax.z :
                sph.m_vecCenter.z,
        };

        const auto dir = sph.m_vecCenter - p;
        const auto distSq = dir.SquaredMagnitude();
        if (distSq < minDistSq) {
            colp.m_vecNormal = dir / sqrt(distSq); // Normalize vector
            colp.m_vecPoint = p;

            colp.m_nSurfaceTypeA = sph.m_Surface.m_nMaterial;
            colp.m_nLightingA = sph.m_Surface.m_nLighting;

            colp.m_nSurfaceTypeB = box.m_Surface.m_nMaterial;
            colp.m_nLightingB = box.m_Surface.m_nLighting;

            minDistSq = distSq;

            return true;
        }
    }
    return false;
}

/*!
* Check if point is within the triangle
* Unused function - Most likely inlined
* @address 0x412700
*/
bool __stdcall CCollision::PointInTriangle(CVector const& point, CVector const* triPoints) {
    // Make everything relative to 0th vertex of the triangle
    const auto v1 = triPoints[1] - triPoints[0];
    const auto v2 = triPoints[2] - triPoints[0];
    const auto p  = point        - triPoints[0];

    // NOTE:
    // Because no vectors are normalized all offset products are scaled.
    // In order to compensate for this they multiply values by either vector's squared magnitude.

    const auto v1_dot_v2 = DotProduct(v1, v2);

    const auto v2_dot_p = DotProduct(v2, p);
    const auto v2_magSq = v2.SquaredMagnitude();

    const auto v1_dot_p = DotProduct(v1, p);
    const auto v1_magSq = v1.SquaredMagnitude();

    const auto a = (v2_magSq * v1_dot_p) - (v1_dot_v2 * v2_dot_p);
    if (a >= 0.f) {
        const auto b = (v1_magSq * v2_dot_p) - (v1_dot_v2 * v1_dot_p);
        if (b >= 0.f) {
            const auto c = (v1_magSq * v2_magSq) - (v1_dot_v2 * v1_dot_v2);
            return c >= (a + b);
        }
    }
    return false;
}

/*!
* @address 0x412850
*
* @param ln0 Origin of line seg.
* @param ln1 End of line seg.
* @param pt  The point
*
* @returns Sq. dist. from `pt` to point closest to `pt` on the line segment (ln0, ln1)
*/
float CCollision::DistToLineSqr(CVector const& ln0, CVector const& ln1, CVector const& pt) {
    // Make line end (l) and pt (pl_ip) relative to ln0 (by this ln0 becomes the space origin)
    const auto l = ln1 - ln0;
    const auto p = pt - ln0;

    //        * P
    //      / |
    //   c /  | a
    //    /   |
    // O *----+--------* L
    //     pl  IP
    //
    // O    - Origin (line start)
    // L    - Line end
    // P    - Point
    // IP   - Intersection pt
    // b, c - Triangle sides
    // a    - The distance we want to find out :D

    const auto ll = l.Dot(l); // Line mag. sq.
    const auto pl = p.Dot(l);

    if (pl <= 0.f) { // Before origin
        return p.SquaredMagnitude(); // Dist to origin
    }

    if (pl >= ll) { // After end
        return (p - l).SquaredMagnitude(); // Dist to end
    }

    // Simple Pythagorean here, we gotta find `a^2`

    const auto cSq = p.Dot(p);

    // Clever trick to divide by |l| without taking it's sqrt
    // We have to do this, because `pl` is multiplied by |l|
    // (Result of the dot product)
    const auto bSq = pl * pl / ll;

    return cSq - bSq; // return a^2
}

// 0x417610
float CCollision::DistToLine(const CVector& lineStart, const CVector& lineEnd, const CVector& point) {
    return std::sqrt(DistToLineSqr(lineStart, lineEnd, point));
}

/*!
* @address 0x412970
* @brief Similar to \r DistToLineSqr it always returns the distance to the projected intersection point.
*/
float CCollision::DistToMathematicalLine(CVector const* lineStart, CVector const* lineEnd, CVector const* point) {
    const auto l = *lineEnd - *lineStart;
    const auto p = *point - *lineStart;

    // See `DistToLineSqr` for a nice illustration.
    // Simple Pythagorean here, we gotta find side `a`

    const auto pMagSq = p.SquaredMagnitude();
    const auto cSq = pMagSq;
    const auto bSq = (float)std::pow(DotProduct(p, l), 2) / pMagSq; // Dot product is scaled by `pMagSq` - Clever trick to get descale it without using sqrt

    const auto aSq = cSq - bSq;
    return aSq > 0.0f ? std::sqrt(aSq) : 0.0f; // Little optimization to not call `sqrt` if the dist is 0 (it wont ever be negative)
}

/*!
* @address 0x412A30
* @brief Same as \r DistToMathematicalLine but in 2D
*/
float CCollision::DistToMathematicalLine2D(float lineStartX, float lineStartY, float lineEndX, float lineEndY, float pointX, float pointY) {
    const float px{ pointX - lineStartX }, py{ pointY - lineStartY };
    const auto  dot = px * lineEndX + py * lineEndY;
    const auto distSq = px * px + py * py - dot * dot;
    return distSq > 0.f ? std::sqrt(distSq) : 0.f;
}

/*!
* @address 0x412A80
* @brief TODO
*/
float CCollision::DistAlongLine2D(float lineX, float lineY, float lineDirX, float lineDirY, float pointX, float pointY) {
    return (pointX - lineX) * lineDirX + (pointY - lineY) * lineDirY;
}

/*!
* Calculate point closest to `point` on line (l0 - l1)
*
* @notsa
*/
template<typename T>
auto lerp(const T& from, const T& to, float t) {
    return to * t + from * (1.f - t);
}
CVector CCollision::GetClosestPtOnLine(const CVector& l0, const CVector& l1, const CVector& point) {
    const auto lnMagSq = (l1 - l0).SquaredMagnitude();
    const auto dot = (point - l0).Dot(l1 - l0);
    if (dot <= 0.0f) {
        return l0;
    }
    if (dot >= lnMagSq) {
        return l1;
    }
    return lerp(l0, l1, dot / lnMagSq);
}

// 0x417FD0
void CCollision::ClosestPointOnLine(const CVector& l0, const CVector& l1, const CVector& point, CVector& closest) {
    closest = GetClosestPtOnLine(l0, l1, point);
}

// 0x415950
void CCollision::Closest3(CVector* arg0, CVector* arg1) {

}

/*!
* Computes closest points C1 and C2 of S1(s)=P1+s*d1 and
* S2(t)=P2+t*d2, returning s and t.
*
* @param       s1p0 Origin of ln. seg 1
* @param       d1 Direction of ln. seg. 1 (Not normalized!)
* @param       a  Sq. mag. of ln. seg 1
* @param       s2p1 As s1p0 ln. seg. 2
* @param       d1 As d1 ln. seg. 2
* @param       e  As a for ln. seg. 2
* @param [out] s  Intersection parameter on ln. seg. 1
* @param [out] t  Intersection parameter on ln. seg. 2
* @param [out] c1 Point on ln. seg. 1
* @param [out] c2 Point on ln. seg. 2
*
* @returns squared distance between between S1(s) and S2(t)
*
* Credit: Code from "Real-Time Collision Detection" by Christer Ericson, published by Morgan Kaufmann Publishers, В© 2005 Elsevier Inc
*/
float CCollision::ClosestPtSegmentSegment(
        CVector p1, CVector d1, float a,
        CVector p2, CVector d2, float e,
        float& s, float& t,
        CVector& c1, CVector& c2
) {
    // Must be non-negative!
    assert(a >= 0.f);
    assert(e >= 0.f);

    constexpr auto EPSILON = std::numeric_limits<float>::epsilon();

    const auto r = p1 - p2;
    const auto f = d2.Dot(r);

    // Check if either or both segments degenerate into points
    if (a <= EPSILON && e <= EPSILON) {
        // Both segments degenerate into points
        s = t = 0.0f;
        c1 = p1;
        c2 = p2;
        return (c1 - c2).SquaredMagnitude();
    }

    if (a <= EPSILON) {
        // First segment degenerates into a point
        s = 0.0f;
        t = f / e;
        // s = 0 => t = (b*s + f) / e = f / e
        t = std::clamp(t, 0.0f, 1.0f);
    } else {
        const auto c = d1.Dot(r);
        if (e <= EPSILON) {
            // Second segment degenerates into a point
            t = 0.0f;
            s = std::clamp(-c / a, 0.0f, 1.0f);
            // t = 0 => s = (b*t - c) / a = -c / a
        } else {
            // The general nondegenerate case starts here
            const auto b = d1.Dot(d2);
            const auto denom = a * e - b * b;
            // Always nonnegative
            // If segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary s (here 0)
            if (denom != 0.0f) {
                s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
            } else {
                s = 0.0f;
            }
            // Compute point on L2 closest to S1(s) using
            // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
            t = (b * s + f) / e;
            // If t in [0,1] done. Else clamp t, recompute s for the new value
            // of t using s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
            // and clamp s to [0, 1]
            if (t < 0.0f) {
                t = 0.0f;
                s = std::clamp(-c / a, 0.0f, 1.0f);
            } else if (t > 1.0f) {
                t = 1.0f;
                s = std::clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    c1 = p1 + d1 * s;
    c2 = p2 + d2 * t;
    return (c1 - c2).SquaredMagnitude();
}

/*!
* This one took me quite a bit to figure out.
* Of course they weren't logical at all and did something weird :)
* Turns out, they used an algorithm out of a book by Dan Sunday,
* see here (https://web.archive.org/web/20210330143700/http://geomalgorithms.com/a07-_distance.html)
* I opted to use something different, but it remains to be seen how much more efficient it is (or isnt)
*
* @param s1p0 Seg. 1 origin
* @param s2p0 Seg. 2 origin
* @param s2p1 Seg. 2 end
* @param u    Dir. of seg. 1 (Unnormalized!)
* @param a    Sq. mag. of seg 1
*
* @returns Sq. dist. of the closest points on the 2 line segments
*/
float ClosestSquaredDistanceBetweenFiniteLines(
        const CVector& s1p0,
        const CVector& s2p0, const CVector& s2p1,
        const CVector& u, float a
) {
    float s, t;
    CVector c1, c2;
    CVector d2 = s2p1 - s2p0;
    return CCollision::ClosestPtSegmentSegment(
            s1p0, u, a,
            s2p0, d2, d2.Dot(d2),
            s, t,
            c1, c2
    );

    /* Original code below:
    constexpr auto EPSILON = 1e-5f;

    // For completeness sake I'll include the copyright:
    //
    // Copyright 2001 softSurfer, 2012 Dan Sunday
    // This code may be freely used and modified for any purpose
    // providing that this copyright notice is included with it.
    // SoftSurfer makes no warranty for this code, and cannot be held
    // liable for any real or imagined damage resulting from its use.
    // Users of this code must verify correctness for their application.

    //CVector u = s1p1 - s1p0; //S1.P1 - S1.P0;
    CVector v = s2p1 - s2p0; //S2.P1 - S2.P0;
    CVector w = s1p0 - s2p0; //S1.P0 - S2.P0;
    //float   a = u.Dot(u);         // always >= 0
    float   b = u.Dot(v);
    float   c = v.Dot(v);// v.Dot(v);         // always >= 0
    float   d = u.Dot(w);
    float   e = v.Dot(w);
    float   D = a*c - b*b;        // always >= 0
    float   sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    float   tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < EPSILON) { // the lines are almost parallel
        sN = 0.f;         // force using point P0 on segment S1
        sD = 1.f;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    } else {                 // get the closest points on the infinite lines
        sN = (b*e - c*d);
        tN = (a*e - b*d);
        if (sN < 0.f) {        // sc < 0 => the s=0 edge is visible
            sN = 0.f;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.f) {            // tc < 0 => the t=0 edge is visible
        tN = 0.f;
        // recompute sc for this edge
        if (-d < 0.f)
            sN = 0.f;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (abs(sN) < EPSILON ? 0.f : sN / sD);
    tc = (abs(tN) < EPSILON ? 0.f : tN / tD);

    // get the difference of the two closest points
    CVector dP = w + (sc * u) - (tc * v);  // =  S1(sc) - S2(tc)
    return dP.SquaredMagnitude(); //return norm(dP);   // return the closest distance
    */
}

/*!
* Calculate clamped barycentric coordinates of a point (p) on a triangle (a, b, c)
* See: https://gamedev.stackexchange.com/a/23745
*
* @notsa
*/
CVector CCollision::GetBaryCoordsOnTriangle(CVector a, CVector b, CVector c, CVector p) {
    const auto vab = b - a, vac = c - a, vap = p - a;
    const auto bb  = vab.Dot(vab);
    const auto bc  = vab.Dot(vac);
    const auto cc  = vac.Dot(vac);
    const auto pb  = vap.Dot(vab);
    const auto pc  = vap.Dot(vac);
    const auto d   = bb * cc - bc * bc;
    const auto v   = (cc * pb - bc * pc) / d;
    const auto w   = (bb * pc - bc * pb) / d;
    const auto u   = 1.0f - v - w;
    return { u, v, w };
}

/*!
* Get barycentric coords of point (p) clamped onto triangle (a, b, c)
* See: https://stackoverflow.com/a/37923949
*
* @notsa
*/
CVector CCollision::GetClampedBaryCoordsIntoTriangle(CVector a, CVector b, CVector c, CVector p) {
    // Calculate barycentric coords
    const auto [u, v, w] = GetBaryCoordsOnTriangle(a, b, c, p);

    // Calculate new `t`
    const auto Get = [&](CVector v1, CVector v2) {
        const auto d = v1 - v2;
        return std::clamp((p - v2).Dot(d) / d.Dot(d), 0.f, 1.f);
    };

    if (u < 0.f) {
        const auto t = Get(c, b);
        return { 0.0f, 1.0f - t, t };
    }

    if (v < 0.f) {
        const auto t = Get(a, c);
        return { t, 0.0f, 1.0f - t };
    }


    if (w < 0.f) {
        const auto t = Get(b, a);
        return { 1.0f - t, t, 0.0f };
    }

    return { u, v, w }; // Point was in the triangle
}
