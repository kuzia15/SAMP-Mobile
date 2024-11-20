#include "../main.h"
#include "font.h"
#include "../vendor/armhook/patch.h"

void CFont::Initialise() {
	CHook::CallFunction<void>(g_libGTASA + (VER_x32 ? 0x0046FD8C + 1 : 0x55BFEC));
}

void CFont::AsciiToGxtChar(const char* ascii, uint16_t* gxt)
{
	return ((void(*)(const char*, uint16_t*))(g_libGTASA + (VER_x32 ? 0x005A83C0 + 1 : 0x6CBEF0)))(ascii, gxt);
}

void CFont::SetScale(float x, float y)
{
	*(float*)(g_libGTASA + (VER_x32 ? 0xA297B8 : 0xCCC324 + 0x4)) = x;
	*(float*)(g_libGTASA + (VER_x32 ? 0xA297BC : 0xCCC324 + 0x8)) = y;
}

void CFont::SetColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + (VER_x32 ? 0x005AB038 + 1 : 0x6CEE0C)))(dwColor);
}

void CFont::SetJustify(uint8_t justify)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB364 + 1 : 0x6CF1CC)))(justify);
}

void CFont::SetOrientation(uint8_t orientation)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB374 + 1 : 0x6CF1DC)))(orientation);
}

void CFont::SetWrapX(float wrapX)
{
	return ((void(*)(float))(g_libGTASA + (VER_x32 ? 0x005AB248 + 1 : 0x6CF0B0)))(wrapX);
}

void CFont::SetCentreSize(float size)
{
	return ((void(*)(float))(g_libGTASA + (VER_x32 ? 0x005AB258 + 1 : 0x6CF0C0)))(size);
}
void Font_SetRightJustifyWrap(float wrap)
{
	((int (*)(float))(g_libGTASA + (VER_x32 ? 0x05AB268 + 1 : 0x0053384C)))(wrap);
	//0053384C
}

void CFont::SetBackground(uint8_t bBackground, uint8_t bOnlyText)
{
	return ((void(*)(uint8_t, uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB330 + 1 : 0x6CF18C)))(bBackground, bOnlyText);
}

void CFont::SetBackgroundColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + (VER_x32 ? 0x005AB340 + 1 : 0x6CF1A0)))(dwColor);
}

void CFont::SetProportional(uint8_t prop)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB320 + 1 : 0x6CF17C)))(prop);
}

void CFont::SetDropColor(uint32_t* dwColor)
{
	return ((void(*)(uint32_t*))(g_libGTASA + (VER_x32 ? 0x005AB320 + 1 : 0x6CF0F0)))(dwColor);
}

void CFont::SetDropShadowPosition(uint8_t pos)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005A8ADC + 1 : 0x6CC8E8)))(pos);
}

/*
void CFont::PrintString(float fX, float fY, const uint16_t* szText)
{
	return ((void(*)(float, float, const uint16_t*))(g_libGTASA + 0x5353B4 + 1))(fX, fY, szText);
}
*/

void CFont::PrintString(float posX, float posY, const char* string)
{
	uint16_t* gxt_string = new uint16_t[0xFF];
	CFont::AsciiToGxtChar(string, gxt_string);
	((void (*)(float, float, uint16_t*))(g_libGTASA + (VER_x32 ? 0x005AA200 + 1 : 0x6CDEB0)))(posX, posY, gxt_string);
	delete gxt_string;
	((void (*)())(g_libGTASA + (VER_x32 ? 0x005A9120 + 1 : 0x6CCEA0)))();//53411C ; _DWORD CFont::RenderFontBuffer(CFont *__hidden this)
}

void CFont::SetFontStyle(uint8_t style)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB1BC + 1 : 0x6CEFF8)))(style);
}

void CFont::SetEdge(uint8_t edge)
{
	return ((void(*)(uint8_t))(g_libGTASA + (VER_x32 ? 0x005AB2EC + 1 : 0x6CF148)))(edge);
}