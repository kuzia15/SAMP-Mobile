#include "../main.h"
#include "../gui/gui.h"
#include "game.h"
#include "RW/RenderWare.h"
#include <cstring>

#define MY_FACTOR 120.0f

extern CSnapShotHelper* pSnapShotHelper;
extern UI* pUI;

extern uintptr_t TextDrawTexture[];

// 0.3.7
CTextDraw::CTextDraw(TEXT_DRAW_TRANSMIT* pTextDrawTransmit, const char* szText)
{

	memset(&m_TextDrawData, 0, sizeof(TEXT_DRAW_DATA));

	m_TextDrawData.fLetterWidth = pTextDrawTransmit->fLetterWidth;
	m_TextDrawData.fLetterHeight = pTextDrawTransmit->fLetterHeight;

	m_TextDrawData.dwLetterColor = pTextDrawTransmit->dwLetterColor;
	m_TextDrawData.byteUnk12 = 0;
	m_TextDrawData.byteCentered = pTextDrawTransmit->byteCenter;
	m_TextDrawData.byteBox = pTextDrawTransmit->byteBox;

	m_TextDrawData.fLineWidth = pTextDrawTransmit->fLineWidth;
	m_TextDrawData.fLineHeight = pTextDrawTransmit->fLineHeight;

	m_TextDrawData.dwBoxColor = pTextDrawTransmit->dwBoxColor;
	m_TextDrawData.byteProportional = pTextDrawTransmit->byteProportional;
	m_TextDrawData.dwBackgroundColor = pTextDrawTransmit->dwBackgroundColor;
	m_TextDrawData.byteShadow = pTextDrawTransmit->byteShadow;
	m_TextDrawData.byteOutline = pTextDrawTransmit->byteOutline;
	m_TextDrawData.byteAlignLeft = pTextDrawTransmit->byteLeft;
	m_TextDrawData.byteAlignRight = pTextDrawTransmit->byteRight;
	m_TextDrawData.dwStyle = pTextDrawTransmit->byteStyle;

	m_TextDrawData.fX = pTextDrawTransmit->fX;
	m_TextDrawData.fY = pTextDrawTransmit->fY;

	m_TextDrawData.dwParam1 = 0xFFFFFFFF;
	m_TextDrawData.dwParam2 = 0xFFFFFFFF;
	m_TextDrawData.byteSelectable = pTextDrawTransmit->byteSelectable;
	m_TextDrawData.wModelID = pTextDrawTransmit->wModelID;
	m_TextDrawData.vecRot.x = pTextDrawTransmit->vecRot.x;
	m_TextDrawData.vecRot.y = pTextDrawTransmit->vecRot.y;
	m_TextDrawData.vecRot.z = pTextDrawTransmit->vecRot.z;
	m_TextDrawData.fZoom = pTextDrawTransmit->fZoom;
	m_TextDrawData.wColor1 = pTextDrawTransmit->wColor1;
	m_TextDrawData.wColor2 = pTextDrawTransmit->wColor2;
	m_TextDrawData.bHasKeyCode = false;
	m_TextDrawData.iTextureSlot = -1;
	SetText(szText);

	if (m_TextDrawData.dwStyle == 4) {
		m_TextDrawData.iTextureSlot = GetFreeTextDrawTextureSlot();
		LoadTexture();
	}

	if (m_TextDrawData.dwStyle == 6)
	{
		//DrawTexture6();
		//return;
	}

	m_TextDrawData.bHasRectArea = false;
	m_rectArea.left = 0.0f;
	m_rectArea.right = 0.0f;
	m_rectArea.bottom = 0.0f;
	m_rectArea.top = 0.0f;
	m_bHovered = false;
	m_dwHoverColor = 0;
}
// 0.3.7
CTextDraw::~CTextDraw()
{
	DestroyTextDrawTexture(m_TextDrawData.iTextureSlot);
}

void CTextDraw::Draw()
{
	if(m_TextDrawData.iTextureSlot == -1)
	{
		if(m_TextDrawData.dwStyle == TEXTDRAW_TXD_SPRITE && m_TextDrawData.dwStyle == TEXTDRAW_MODEL_PREVIEW)
			DrawTextured();
		else DrawDefault();
	}
	else DrawTextured();
}

void CTextDraw::DrawImage() //Custom TextDraw
{
	if (m_TextDrawData.dwStyle == 6)
	{
		//DrawTexture6();
		return;
	}

	if (m_TextDrawData.dwStyle == 7)
	{
		//DrawTexture7();
		return;
	}
}

/*
void CTextDraw::Draw()
{
	if (m_TextDrawData.dwStyle == 6)
	{
		DrawTexture6();
		return;
	}

	if (m_TextDrawData.dwStyle == 7)
	{
		DrawTexture7();
		return;
	}

	if (m_TextDrawData.iTextureSlot == -1 &&(m_TextDrawData.dwStyle != 4 && m_TextDrawData.dwStyle != 5))
		DrawDefault();
	else {
		DrawTextured();
	}
}*/

void CTextDraw::DrawDefault()
{
	if (!m_szText || !strlen(m_szText)) return;

	int iScreenWidth = RsGlobal->maximumWidth;
	int iScreenHeight = RsGlobal->maximumHeight;
	float fHorizHudScale = 1.0f / 640.0f;
	float fVertHudScale = 1.0f / 448.0f;

	float fScaleY = (float)iScreenHeight * fVertHudScale * m_TextDrawData.fLetterHeight * 0.5f;
	float fScaleX = (float)iScreenWidth * fHorizHudScale * m_TextDrawData.fLetterWidth;

	CFont::SetScale(fScaleX, fScaleY);

	if (m_bHovered)
		CFont::SetColor(&m_dwHoverColor);
	else
		CFont::SetColor(&m_TextDrawData.dwLetterColor);

	CFont::SetJustify(0);

	if (m_TextDrawData.byteAlignRight) CFont::SetOrientation(2);
	else if (m_TextDrawData.byteCentered) CFont::SetOrientation(0);
	else CFont::SetOrientation(1);

	float fLineWidth = iScreenWidth * m_TextDrawData.fLineWidth * fHorizHudScale;
	CFont::SetWrapX(fLineWidth);

	float fLineHeight = iScreenWidth * m_TextDrawData.fLineHeight * fHorizHudScale;
	CFont::SetCentreSize(fLineHeight);

	CFont::SetBackground(m_TextDrawData.byteBox, 0);
	CFont::SetBackgroundColor(&m_TextDrawData.dwBoxColor);
	CFont::SetProportional(m_TextDrawData.byteProportional);
	CFont::SetDropColor(&m_TextDrawData.dwBackgroundColor);

	if (m_TextDrawData.byteOutline) {
		CFont::SetEdge(m_TextDrawData.byteOutline);
	}
	else {
		CFont::SetDropShadowPosition(m_TextDrawData.byteShadow);
	}

	CFont::SetFontStyle(m_TextDrawData.dwStyle);

	if (m_TextDrawData.bHasKeyCode) {
		//  CMessages_InsertPlayerControlKeysInString(m_szString);
	}

	float fUseY = iScreenHeight - ((448.0 - m_TextDrawData.fY) * (iScreenHeight * fVertHudScale));
	float fUseX = iScreenWidth - ((640.0 - m_TextDrawData.fX) * (iScreenWidth * fHorizHudScale));
	CFont::PrintString(fUseX, fUseY, m_szText);

	CFont::SetEdge(0);

	if (m_TextDrawData.byteAlignRight)
	{
		m_rectArea.left = fUseX - (fLineWidth - fUseX);
		m_rectArea.right = fUseX;
		m_rectArea.bottom = fUseY + fLineHeight;
		m_rectArea.top = fUseY;
	}
	else if (m_TextDrawData.byteCentered)
	{
		m_rectArea.left = fUseX - (fLineHeight * 0.5f);
		m_rectArea.right = m_rectArea.left + fLineHeight;
		m_rectArea.bottom = fUseY + fLineHeight;
		m_rectArea.top = fUseY;
	}
	else
	{
		m_rectArea.left = fUseX;
		m_rectArea.right = fLineWidth - fUseX + fUseX;
		m_rectArea.bottom = fUseY + fLineHeight;
		m_rectArea.top = fUseY;
	}

	m_TextDrawData.bHasRectArea = true;
}

void CTextDraw::DrawTextured()
{
	float scaleX = RsGlobal->maximumWidth * (1.0f / 640.0f);
	float scaleY = RsGlobal->maximumHeight * (1.0f / 448.0f);

	CRect rect;
	rect.left = m_TextDrawData.fX * scaleX;
	rect.top = m_TextDrawData.fY * scaleY;
	rect.right = (m_TextDrawData.fX + m_TextDrawData.fLineWidth) * scaleX;
	rect.bottom = (m_TextDrawData.fY + m_TextDrawData.fLineHeight) * scaleY;



	static float uv_reflected[8] = {
			0.0f, 1.0f,
			1.0f, 1.0f,
			0.0f, 0.0f,
			1.0f, 0.0f };

	static float uv_normal[8] = {
			0.0f, 0.0f,
			1.0f, 0.0f,
			0.0f, 1.0f,
			1.0f, 1.0f };

	if (m_bHovered) {
		DrawTextureUV(TextDrawTexture[m_TextDrawData.iTextureSlot], &rect, m_dwHoverColor,
					  m_TextDrawData.dwStyle == 5 ? uv_reflected : uv_normal);
	}
	else {
		DrawTextureUV(TextDrawTexture[m_TextDrawData.iTextureSlot], &rect, m_TextDrawData.dwLetterColor,
					  m_TextDrawData.dwStyle == 5 ? uv_reflected : uv_normal);
	}

	m_rectArea.left = rect.left;
	m_rectArea.right = rect.right;
	m_rectArea.bottom = rect.bottom;
	m_rectArea.top = rect.top;
	m_TextDrawData.bHasRectArea = true;
}
// � ��� ������
void CTextDraw::SetText(const char* szText)
{
	memset(m_szText, 0, 800);
	strncpy(m_szText, szText, 800);
	m_szText[800] = 0;

	if (m_TextDrawData.dwStyle == 4 && m_TextDrawData.iTextureSlot != -1) {
		DestroyTextDrawTexture(m_TextDrawData.iTextureSlot);
		LoadTexture();
	}
}

void CTextDraw::LoadTexture()
{
	char txdname[64 + 1];
	memset(txdname, 0, sizeof(txdname));
	char texturename[64 + 1];
	memset(texturename, 0, sizeof(texturename));

	char *szTexture = strchr(m_szText, ':');
	if (szTexture == nullptr) return;

	if (strlen(m_szText) < 64 && strchr(m_szText, '\\') == nullptr && strchr(m_szText, '/') == nullptr)
	{
		strncpy(txdname, m_szText, (size_t)(szTexture - m_szText));
		strcpy(texturename, ++szTexture);

		FLog("LOADTEXTURE: txdname: %s, texture: %s", txdname, texturename);

		if (!strcasecmp(txdname, "ld_shtr") && !strcasecmp(texturename, "ship"))
		{
			strcpy(texturename, "ship_ld_shtr");
		}
		else if (!strcasecmp(txdname, "ld_spac") && !strcasecmp(texturename, "shoot"))
		{
			strcpy(texturename, "shoot_LD_SPAC");
		}
		else if (!strcasecmp(txdname, "ld_spac") && !strcasecmp(texturename, "white"))
		{
			strcpy(texturename, "white_LD_SPAC");
		}
		else
		{
			static char* texture_samp[] = {
					"smokeii_3", "back2", "back3", "back4", "back5",
					"back6", "back7", "back8", "back8_right", "back8_top",
					"intro1", "intro2", "intro3", "intro4", "duality", "health", "thrustg",
					"ric1", "ric2", "ric3", "ric4", "ric5", "load0uk", "loadsc0", "loadsc1",
					"loadsc10", "loadsc11", "loadsc12", "loadsc13", "loadsc14", "loadsc2",
					"loadsc3", "loadsc4", "loadsc5", "loadsc6", "loadsc7", "loadsc8",
					"loadsc9", "eax", "nvidia", "title_pc_eu", "title_pc_us", "loadscuk",
					"tyre64a", "wheel_alloy64", "wheel_classic64", "wheel_lighttruck64",
					"wheel_lightvan64", "wheel_offroad64", "wheel_saloon64", "wheel_smallcar64",
					"wheel_sport64", "wheel_truck64", "whee_rim64", "outro" };

			for (int i = 0; i < (sizeof(texture_samp) / sizeof(char*)); i++)
			{
				// ���� �������� ��, ��� �� �������� � samp.txt
				if (!strcasecmp(texture_samp[i], texturename))
				{
					char buf[64];
					sprintf(buf, "%s_%s", texturename, txdname);
					strcpy(texturename, buf);
				}
			}
		}

		if (m_TextDrawData.iTextureSlot != -1) {
			TextDrawTexture[m_TextDrawData.iTextureSlot] = (uintptr_t)LoadTextureFromTxd(txdname, texturename);
		}
	}
}

void CTextDraw::SnapshotProcess()
{
	if (m_TextDrawData.dwStyle != 5 || m_TextDrawData.iTextureSlot != -1) {
		return;
	}

	/*uintptr_t snapshot = 0;

	// PED MODEL
	if (IsValidPedModel(m_TextDrawData.wModelID))
	{
		if (!IsExistInfoForModel(m_TextDrawData.wModelID))
			m_TextDrawData.wModelID = 0;

		snapshot = pSnapShotHelper->CreatePedSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom);
	}
		// VEHICLE MODEL
	else if (m_TextDrawData.wModelID >= 400 && m_TextDrawData.wModelID <= 611)
	{
		snapshot = pSnapShotHelper->CreateVehicleSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom,
				m_TextDrawData.wColor1,
				m_TextDrawData.wColor2
		);
	}
		// OBJECT MODEL
	else if ((m_TextDrawData.wModelID <= 320 || m_TextDrawData.wModelID > 373) && m_TextDrawData.wModelID < 615)
	{
		if (m_TextDrawData.wModelID >= 0) return;

		if (!IsValidModel(m_TextDrawData.wModelID))
			m_TextDrawData.wModelID = 18631;

		snapshot = pSnapShotHelper->CreateObjectSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom
		);
	}
	else
	{
		if (!IsExistInfoForModel(m_TextDrawData.wModelID))
			m_TextDrawData.wModelID = 18631;

		snapshot = pSnapShotHelper->CreateObjectSnapShot(
				m_TextDrawData.wModelID,
				m_TextDrawData.dwBackgroundColor,
				&m_TextDrawData.vecRot,
				m_TextDrawData.fZoom
		);
	}

	if (snapshot) {
		m_TextDrawData.iTextureSlot = GetFreeTextDrawTextureSlot();
		TextDrawTexture[m_TextDrawData.iTextureSlot] = snapshot;
	}*/
}

/*
bool ProcessInlineHexColor(const char* start, const char* end, ImVec4& color);

void lTextWithColors(ImVec2 pos, ImColor col, const char* szStr, bool b)
{
	char tempStr[256];

	ImVec2 vecPos = pos;

	strcpy(tempStr, szStr);
	tempStr[sizeof(tempStr) - 1] = '\0';

	bool pushedColorStyle = false;
	const char* textStart = tempStr;
	const char* textCur = tempStr;
	while (textCur < (tempStr + sizeof(tempStr)) && *textCur != '\0')
	{
		if (*textCur == '{')
		{
			if (textCur != textStart)
			{
				pGUI->RenderText(vecPos, col, b, textStart, textCur);
				vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
			}

			const char* colorStart = textCur + 1;
			do
			{
				++textCur;
			} while (*textCur != '\0' && *textCur != '}');

			if (pushedColorStyle)
			{
				pushedColorStyle = false;
			}

			ImVec4 textColor;
			if (ProcessInlineHexColor(colorStart, textCur, textColor))
			{
				col = textColor;
				pushedColorStyle = true;
			}

			textStart = textCur + 1;
		}
		else if (*textCur == '\n')
		{
			pGUI->RenderText(vecPos, col, b, textStart, textCur);
			vecPos.x = pos.x;
			vecPos.y += pGUI->GetFontSize();
			textStart = textCur + 1;
		}

		++textCur;
	}

	if (textCur != textStart)
	{
		pGUI->RenderText(vecPos, col, true, textStart, textCur);
		vecPos.x += ImGui::CalcTextSize(textStart, textCur).x;
	}
	else
		vecPos.y += pGUI->GetFontSize();
}*/

void CTextDraw::DrawTexture7()
{


}

void CTextDraw::DrawTexture6()
{
}