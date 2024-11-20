#pragma once

class CPlayerTags
{
public:
	CPlayerTags();
	~CPlayerTags();

	void Render(ImGuiRenderer* renderer);

private:
	void Draw(ImGuiRenderer* renderer, CVector* vec, const char* szNick, uint32_t dwColor, float fDist, float fHealth, float fArmour, bool bAfk, bool bMicro);

	RwTexture* m_pAFKIconTexture;
	RwTexture* m_pMicroIconTexture;
};