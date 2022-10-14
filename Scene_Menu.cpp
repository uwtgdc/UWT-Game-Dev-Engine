#include "Scene_Menu.h"
#include "Scene_Zelda.h"
#include "Common.h"
#include "Assets.h"
#include "Game_Engine.h"
#include "Components.h"
#include "Action.h"

Scene_Menu::Scene_Menu(Game_Engine *game_engine)
	: Scene(game_engine)
{
	initialize();
}

void Scene_Menu::initialize()
{
	register_action(sf::Keyboard::W, "UP");
	register_action(sf::Keyboard::S, "DOWN");
	register_action(sf::Keyboard::D, "PLAY");
	register_action(sf::Keyboard::Escape, "QUIT");

	m_title= "Fairy Boy";
	m_menu_strings.push_back("Level 1");
	m_menu_strings.push_back("Level 2");
	m_menu_strings.push_back("Level 3");

	m_level_paths.push_back("level1.txt");
	m_level_paths.push_back("level2.txt");
	m_level_paths.push_back("level3.txt");

	m_menu_text.setFont(m_game->assets().get_font("Mario"));
	m_menu_text.setCharacterSize(60);
	m_menu_text.setFillColor(sf::Color(0, 0, 0));

	//m_game->play_sound("MusicTitle", true);
}

void Scene_Menu::on_end()
{
	m_game->quit();
}

void Scene_Menu::update()
{
	m_current_frame++;
	s_render();
}

void Scene_Menu::s_do_action(const Action &action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_menu_index > 0) { m_menu_index--; }
			else { m_menu_index= m_menu_strings.size() - 1; }
		}
		else if (action.name() == "DOWN")
		{
			m_menu_index= (m_menu_index + 1) % m_menu_strings.size();
		}
		else if (action.name() == "PLAY")
		{
			m_game->stop_sound("MusicTitle");
			m_game->change_scene("ZELDA", std::make_shared<Scene_Zelda>(m_game, m_level_paths[m_menu_index]));
		}
		else if (action.name() == "QUIT")
		{
			on_end();
		}
	}
}

void Scene_Menu::s_render()
{
	m_game->window().clear(sf::Color(157, 131, 8));

	m_menu_text.setString(m_title);
	m_menu_text.setCharacterSize(64);
	m_menu_text.setPosition(5, 8);
	m_game->window().draw(m_menu_text);

	m_menu_text.setPosition(5, 40);
	for (int i= 0; i < m_menu_strings.size(); i++)
	{
		m_menu_text.setString(m_menu_strings[i]);
		m_menu_text.setPosition(5, m_menu_text.getPosition().y + 80);

		if (i == m_menu_index)
		{
			m_menu_text.setFillColor(sf::Color(255, 255, 255));
		}
		else
		{
			m_menu_text.setFillColor(sf::Color(0, 0, 0));
		}
		m_game->window().draw(m_menu_text);
	}

	m_menu_text.setString("UP:W  DOWN:S  PLAY:D  BACK:ESC");
	m_menu_text.setFillColor(sf::Color(0, 0, 0));
	m_menu_text.setCharacterSize(24);
	m_menu_text.setPosition(5, (int)height() - 64);
	m_game->window().draw(m_menu_text);
}