#include "../main.h"
#include "imguiwrapper.h"
#include "uisettings.h"

#include "../game/game.h"
#include "../net/netgame.h"

// voice
#include "../voice_new/MicroIcon.h"
#include "../voice_new/SpeakerList.h"
#include "../voice_new/include/util/Render.h"


extern UI* pUI;
extern CNetGame* pNetGame;


ImGuiWrapper::ImGuiWrapper(const ImVec2& display_size, const std::string& font_path)
{
	m_displaySize = display_size;
	m_renderer = 0;
	m_fontRaster = nullptr;
	m_fontPath = font_path;

	m_vertexBuffer = nullptr;
	m_vertexBufferSize = 10000;
}

ImGuiWrapper::~ImGuiWrapper()
{
	shutdown();
}

bool ImGuiWrapper::initialize()
{
	Log::traceLastFunc("ImGuiWrapper::initialize");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = m_displaySize;
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

	Log::addParameter("Loading font: %s", m_fontPath.c_str());
	ImFontConfig fontCfg;
	fontCfg.GlyphExtraSpacing.x = UISettings::outlineSize();

	static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0400, 0x04FF, // Cyrillic + Cyrillic Supplement
		0x0E00, 0x0E7F, // Thai
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0xF020, 0xF0FF,
		0
    };
	
	ImFont* font = io.Fonts->AddFontFromFileTTF(m_fontPath.c_str(),
		UISettings::fontSize(), &fontCfg, ranges);

	if (font == nullptr)
	{
		Log::addParameter("font", font);
		return false;
	}

	createFontTexture();

	m_renderer = new ImGuiRenderer(ImGui::GetBackgroundDrawList(), font);

	// voice 
	for (const auto& deviceInitCallback : Render::deviceInitCallbacks) {
		if (deviceInitCallback != nullptr) {
			deviceInitCallback();
		}
	}

	return true;
}

void ImGuiWrapper::render()
{
	ImGui::NewFrame();


	// voice
	if (pNetGame) {

		if (pUI->dialog()->visible() || pUI->playertablist()->visible() || pNetGame->GetTextDrawPool()->GetState()) {
			SpeakerList::Hide();
			MicroIcon::Hide();
		}
		else if (MicroIcon::hasShowed) {
			SpeakerList::Show();
			MicroIcon::Show();
		}

		for (const auto& renderCallback : Render::renderCallbacks) {
			if (renderCallback != nullptr) {
				renderCallback();
			}
		}
	}

	drawList();
	ImGui::EndFrame();
	ImGui::Render();
	renderDrawData(ImGui::GetDrawData());
}

void ImGuiWrapper::shutdown()
{
	Log::traceLastFunc("ImGuiWrapper::shutdown");

	// voice
	for (const auto& deviceFreeCallback : Render::deviceFreeCallbacks) {
		if (deviceFreeCallback != nullptr) {
			deviceFreeCallback();
		}
	}

	destroyFontTexture();
}

void ImGuiWrapper::setupRenderState(ImDrawData* draw_data)
{
	RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)0);
	RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)0);
	RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)1);
	RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
	RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)0);
	RwRenderStateSet(rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE);
	RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, (void*)0);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER);
	RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, (void*)2);
	RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
	RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);

}

void ImGuiWrapper::checkVertexBuffer(ImDrawData* draw_data)
{
	if (m_vertexBuffer == nullptr || m_vertexBufferSize < draw_data->TotalVtxCount)
	{
		Log::traceLastFunc("ImGuiWrapper::checkVertexBuffer");

		if (m_vertexBuffer)
		{
			delete m_vertexBuffer;
			m_vertexBuffer = nullptr;
		}

		m_vertexBufferSize = draw_data->TotalVtxCount + 10000;
		m_vertexBuffer = new RwIm2DVertex[m_vertexBufferSize];
		if (!m_vertexBuffer)
		{
			Log::addParameter("Buffer allocation error. Size", m_vertexBufferSize);
			return;
		}

		Log::addParameter("Vertex buffer reallocated. Size", m_vertexBufferSize);
	}

}

void ImGuiWrapper::renderDrawData(ImDrawData* draw_data)
{
	const RwReal nearScreenZ = *(RwReal*)(g_libGTASA + (VER_x32 ? 0x00675F8C : 0x849FA0));
	const RwReal recipNearClip = *(RwReal*)(g_libGTASA + (VER_x32 ? 0x006766AC : 0x84ADC8));

	checkVertexBuffer(draw_data);
	setupRenderState(draw_data);

	RwIm2DVertex* vtx_dst = m_vertexBuffer;
	int vtx_offset = 0;

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		const ImDrawIdx* idx_src = cmd_list->IdxBuffer.Data;

		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
		{
			RwIm2DVertexSetScreenX(vtx_dst, vtx_src->pos.x);
			RwIm2DVertexSetScreenY(vtx_dst, vtx_src->pos.y);
			RwIm2DVertexSetScreenZ(vtx_dst, nearScreenZ);
			RwIm2DVertexSetRecipCameraZ(vtx_dst, recipNearClip);
			vtx_dst->emissiveColor = vtx_src->col;
			RwIm2DVertexSetU(vtx_dst, vtx_src->uv.x, recipCameraZ);
			RwIm2DVertexSetV(vtx_dst, vtx_src->uv.y, recipCameraZ);

			vtx_dst++;
			vtx_src++;
		}

		const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				ImVec4 clip_rect;
				clip_rect.x = pcmd->ClipRect.x;
				clip_rect.y = pcmd->ClipRect.w;
				clip_rect.z = pcmd->ClipRect.z;
				clip_rect.w = pcmd->ClipRect.y;
				SetScissorRect((void*)&clip_rect);

				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)pcmd->TextureId);
				RwIm2DRenderIndexedPrimitive(rwPRIMTYPETRILIST,
					&m_vertexBuffer[vtx_offset], (RwInt32)cmd_list->VtxBuffer.Size,
					(RwImVertexIndex*)idx_buffer, pcmd->ElemCount);
				RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)0);

				clip_rect = { 0.0f, 0.0f, 0.0f, 0.0f };
				SetScissorRect((void*)&clip_rect);
			}

			idx_buffer += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

}

bool ImGuiWrapper::createFontTexture()
{
	Log::traceLastFunc("ImGuiWrapper::createFontTexture");

	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pxs;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pxs, &width, &height, &bytes_per_pixel);

	RwImage* font_img = RwImageCreate(width, height, bytes_per_pixel * 8);
	RwImageAllocatePixels(font_img);

	if (font_img == nullptr)
	{
		Log::addParameter("Font image pointer", font_img);
		return false;
	}

	RwUInt8* pixels = font_img->cpPixels;
	for (int y = 0; y < font_img->height; y++)
	{
		memcpy((void*)pixels, pxs + font_img->stride * y, font_img->stride);
		pixels += font_img->stride;
	}

	RwInt32 w, h, d, flags;
	RwImageFindRasterFormat(font_img, rwRASTERTYPETEXTURE, &w, &h, &d, &flags);
	m_fontRaster = RwRasterCreate(w, h, d, flags);
	if (m_fontRaster == nullptr)
	{
		Log::addParameter("font raster pointer", m_fontRaster);
		return false;
	}

	m_fontRaster = RwRasterSetFromImage(m_fontRaster, font_img);
	io.Fonts->TexID = (ImTextureID*)m_fontRaster;

	RwImageDestroy(font_img);

	Log::addParameter("Atlas width", width);
	Log::addParameter("Atlas height", height);
	return true;

}

void ImGuiWrapper::destroyFontTexture()
{
	if (m_fontRaster)
	{
		ImGuiIO& io = ImGui::GetIO();
		RwRasterDestroy(m_fontRaster);
		m_fontRaster = nullptr;
		io.Fonts->TexID = (ImTextureID)0;
	}

}