#pragma once

#include "Common.h"
#include "Action.h"
#include "Entity_Manager.h"

#include <memory>

class Game_Engine;

typedef std::map<int, std::string> Action_Map;

class Scene
{

protected:

	Game_Engine	   *m_game= nullptr;
	Entity_Manager	m_entity_manager;
	Action_Map		m_action_map;
	bool			m_paused= false;
	bool			m_has_ended= false;
	size_t			m_current_frame;

	virtual void on_end()= 0;
	void set_paused();

public:

	Scene();
	Scene(Game_Engine *game_engine);

	virtual void update()= 0;
	virtual void s_do_action(const Action &action)= 0;
	virtual void s_render()= 0;

	void simulate(int i);
	void do_action(const Action &action);
	void register_action(int input_key, std::string action_name);

	const Action_Map &get_action_map() const;

	size_t width() const;
	size_t height() const;
	size_t current_frame() const;
};