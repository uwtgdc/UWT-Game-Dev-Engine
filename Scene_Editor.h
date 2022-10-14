#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "Entity_Manager.h"
#include "Scene_Zelda.h"

class Scene_Zelda;

typedef std::vector<std::shared_ptr<Entity>> tileSet;

class Scene_Editor : public Scene
{

protected:

	Scene_Zelda					   *m_scene;
	c_Input							m_camera_input;
	bool							m_draw_textures= true;
	bool							m_draw_collision= false;
	bool							m_draw_grid= false;
	std::shared_ptr<Entity>			m_dragged;
	std::shared_ptr<Entity>			m_selected;
	tileSet							m_tile_set;
	c_Vec2							m_pos;
	sf::View						m_gui;
	int								m_animation_set;

	void initialize();
	virtual void on_end();
	void create_buttons();

	virtual void s_do_action(const Action &action);
	virtual void s_render();
	void s_animation();
	void s_camera();
	void s_drag();

	void close_editor();
	void left_click_down();
	void left_click_up();

public:

	Scene_Editor(Game_Engine *game, Scene_Zelda *scene);

	virtual void update();
};