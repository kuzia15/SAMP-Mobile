#pragma once

#include "dialogs/tablistwidget.h"

class PlayerTabList : public Widget
{
public:
	PlayerTabList();

	void show();
	void hide();

	void assemble();

	virtual void performLayout() override;
	virtual void draw(ImGuiRenderer* renderer) override;
	virtual void activateEvent(bool active) override;

private:
	TabListWidget* m_tabList;
	Label* m_lServerName;
	Label* m_lTotalPlayers;
};
