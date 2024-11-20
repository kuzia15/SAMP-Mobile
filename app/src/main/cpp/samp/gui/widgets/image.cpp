#include "../gui.h"
#include "../../game/game.h"

Image::Image(const std::string& arch_name, const std::string& name)
{
	m_texture = (RwTexture*)CUtil::LoadTextureFromDB(arch_name.c_str(), name.c_str());
}

void Image::performLayout()
{
	if (m_texture == nullptr) return;
	this->setSize(ImVec2(m_texture->raster->width, m_texture->raster->height));
}

void Image::draw(ImGuiRenderer* renderer)
{
	if(m_texture != nullptr)
		renderer->drawImage(absolutePosition(), absolutePosition() + size(), (ImTextureID)m_texture->raster);
	Widget::draw(renderer);
}