#pragma once

#include "Common.h"
#include "Scene.h"
#include <map>
#include <memory>

#include "Entity_Manager.h"
#include "Scene_Editor.h"

typedef std::vector<std::shared_ptr<Entity>> PortalsVec;

class Scene_Zelda : public Scene
{
	struct player_config 
	{ 
		int X, Y, BX, BY, H; 
		float S;
	};

protected:

	std::shared_ptr<Entity>			m_player;
	std::string						m_level_path;
	player_config					m_player_config;
	PortalsVec						m_portals;
	std::shared_ptr<Entity>			m_exit_portal;
	bool							m_draw_textures= true;
	bool							m_draw_collision= false;
	bool							m_draw_grid= false;
	bool							m_follow= false;
	const c_Vec2					m_grid_size={ 64, 64 };
	sf::Text						m_grid_text;

	void initialize(const std::string &level_path);

	void load_level(const std::string &filename);

	virtual void on_end();

	void spawn_player();
	void spawn_sword(std::shared_ptr<Entity> entity);
	void set_last_x(bool x_was_last);

	c_Vec2 get_position(int rx, int ry, int tx, int ty) const;

	void s_movement();
	virtual void s_do_action(const Action &action);
	void s_AI();
	void s_status();
	void s_collision();
	void s_animation();
	void s_camera();
	void s_render();

	void entity_tile_collisions();
	void player_enemy_collisions();
	void sword_npc_collisions();
	void teleporting(std::shared_ptr<Entity> entity);
	void entity_heart_collisions(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> heart);

	void open_editor();

	friend class Scene_Editor;

public:

	Scene_Zelda(Game_Engine *game, const std::string &level_path);

	virtual void update();
};