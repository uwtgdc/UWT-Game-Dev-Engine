#pragma once

#include "Common.h"
#include "Scene.h"
#include <vector> // ?
#include <memory> // ?

typedef std::vector<std::string> StringsVec;
class Scene_Menu : public Scene
{
	std::string m_title;
	StringsVec	m_menu_strings;
	StringsVec	m_level_paths;
	int			m_menu_index= 0;
	sf::Text	m_menu_text;

	void initialize();

	virtual void on_end();
	
public:
	
	Scene_Menu(Game_Engine *game_engine);

	virtual void update();
	virtual void s_do_action(const Action &action);
	virtual void s_render();
};