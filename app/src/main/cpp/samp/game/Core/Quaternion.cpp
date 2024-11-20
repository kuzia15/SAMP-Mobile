#include "main.h"
#include "game/game.h"
#include <cmath>

void CQuaternion::SetFromMatrix(RwMatrix* mat)
{
	w = sqrt( std::max( (float)0, 1.0f + mat->right.x + mat->up.y + mat->at.z ) ) * 0.5f;
	x = sqrt( std::max( (float)0, 1.0f + mat->right.x - mat->up.y - mat->at.z ) ) * 0.5f;
	y = sqrt( std::max( (float)0, 1.0f - mat->right.x + mat->up.y - mat->at.z ) ) * 0.5f;
	z = sqrt( std::max( (float)0, 1.0f - mat->right.x - mat->up.y + mat->at.z ) ) * 0.5f;

	x = copysignf(x, mat->at.y - mat->up.z);
	y = copysignf(y, mat->right.z - mat->at.x);
	z = copysignf(z, mat->up.x - mat->right.y);
}

void CQuaternion::GetMatrix(RwMatrix* mat)
{
	if(!mat) return;

	float sqw = w * w; // v13 = a1 * a1;
	float sqx = x * x; // v14 = a2 * a2;
	float sqy = y * y; // v15 = a3 * a3;
	float sqz = z * z; // v16 = a4 * a4;

	mat->right.x = ( sqx - sqy - sqz + sqw); 	// a5 = v14 - v15 - v16 + v13;
	mat->up.y = (-sqx + sqy - sqz + sqw);		// a9 = v15 - v14 - v16 + v13;
	mat->at.z = (-sqx - sqy + sqz + sqw);		// a13 = v16 - (v15 + v14) + v13;

	float tmp1 = x * y;				// v17 = a2 * a3;
	float tmp2 = z * w;				// v18 = a1 * a4;
	mat->up.x = 2.0 * (tmp1 + tmp2);		// a8 = v18 + v17 + v18 + v17;
	mat->right.y = 2.0 * (tmp1 - tmp2);		// a6 = v17 - v18 + v17 - v18;

	tmp1 = x*z;						// v20 = a2 * a4;
	tmp2 = y*w;						// v21 = a1 * a3;
	mat->at.x = 2.0 * (tmp1 - tmp2);		// a11 = v20 - v21 + v20 - v21;
	mat->right.z = 2.0 * (tmp1 + tmp2);		// a7 = v21 + v20 + v21 + v20;
	tmp1 = y*z;						// v22 = a3 * a4;
	tmp2 = x*w;						// v23 = a1 * a2;
	mat->at.y = 2.0 * (tmp1 + tmp2);		// a12 = v23 + v22 + v23 + v22;
	mat->up.z = 2.0 * (tmp1 - tmp2);		// a10 = v22 - v23 + v22 - v23;
}

void CQuaternion::Normalize()
{
	double n = sqrt(x*x + y*y + z*z + w*w);

	w /= n;
	x /= n;
	y /= n;
	z /= n;
}

#define SLERP_DELTA 0.1
void CQuaternion::Slerp(CQuaternion *pQ1, CQuaternion *pQ2, float t)
{

	if(!pQ1 || !pQ2) return;
	if(t > 1) return;

	float p1[4];
	double omega, cosom, sinom, scale0, scale1;
	cosom = pQ1->x*pQ2->x + pQ1->y*pQ2->y + pQ1->z*pQ2->z + pQ1->w*pQ2->w;

	if(cosom < 0.0)
	{
		cosom = -cosom;
		p1[0] = - pQ2->x;  p1[1] = - pQ2->y;
		p1[2] = - pQ2->z;  p1[3] = - pQ2->w;
	}	
	else
	{
		p1[0] = pQ2->x;    p1[1] = pQ2->y;
		p1[2] = pQ2->z;    p1[3] = pQ2->w;
	}

	if((1.0 - cosom) > SLERP_DELTA)
	{
		// стандартный случай (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;
	}
	else
	{
		// если маленький угол - линейная интерполяция
		scale0 = 1.0 - t;
		scale1 = t;
	}
	Set(scale0 * pQ1->x + scale1 * p1[0],
		scale0 * pQ1->y + scale1 * p1[1],
		scale0 * pQ1->z + scale1 * p1[2],
		scale0 * pQ1->w + scale1 * p1[3]);
}