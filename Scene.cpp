#include "Scene.h"
#include "Game_Engine.h"

Scene::Scene() 
{
	m_current_frame= 0;
}

Scene::Scene(Game_Engine *game_engine)
	: m_game(game_engine)
{
	m_current_frame= 0;
}

void Scene::set_paused()
{
	m_paused= !m_paused;
}

void Scene::simulate(int i)
{
	for (int j= 0; j < i; j++)
	{
		update();
	}
}

void Scene::do_action(const Action &action)
{
	s_do_action(action); // ???
}

void Scene::register_action(int input_key, std::string action_name)
{
	m_action_map[input_key]= action_name;
}

const Action_Map &Scene::get_action_map() const
{
	return m_action_map;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

size_t Scene::current_frame() const
{
	return m_current_frame;
}