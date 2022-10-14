#include "Scene_Zelda.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "Game_Engine.h"
#include "Components.h"
#include "Scene_Editor.h"

Scene_Zelda::Scene_Zelda(Game_Engine *game, const std::string &level_path)
	: Scene(game)
	, m_level_path(level_path)
{
	initialize(m_level_path);
}

void Scene_Zelda::initialize(const std::string &level_path)
{
	load_level(level_path);
	spawn_player();
	//m_game->play_sound("MusicLevel", true);

	// Register the actions required to play the game

	register_action(sf::Keyboard::Escape, "QUIT");
	register_action(sf::Keyboard::P, "PAUSE");
	register_action(sf::Keyboard::Y, "TOGGLE_FOLLOW");		// Toggle follow camera
	register_action(sf::Keyboard::T, "TOGGLE_TEXTURE");		// Toggle drawing (T)extures
	register_action(sf::Keyboard::C, "TOGGLE_COLLISION");	// Toggle drawing (C)ollision boxes
	register_action(sf::Keyboard::W, "UP");					// Move up
	register_action(sf::Keyboard::S, "DOWN");
	register_action(sf::Keyboard::A, "LEFT");
	register_action(sf::Keyboard::D, "RIGHT");
	register_action(sf::Keyboard::Space, "ATTACK");
	register_action(sf::Keyboard::Tilde, "OPEN_EDITOR");
}

void Scene_Zelda::load_level(const std::string &filename)
{
	m_entity_manager= Entity_Manager();

	// Load the level file and put all entities in the manager

	std::ifstream file(filename);
	std::string string, name;
	c_Vec2 room_pos, tile_pos;
	bool blocks_movement, blocks_vision;

	while (file.good())
	{
		file >> string;

		if (string == "Tile")
		{
			file >> name >> room_pos.x >> room_pos.y >> tile_pos.x >> tile_pos.y >> blocks_movement >> blocks_vision;

			auto tile= m_entity_manager.add_entity(e_Tag::Tile);
			tile->add_component<c_Animation>(m_game->assets().get_animation(name), true);
			tile->add_component<c_Transform>(get_position(room_pos.x, room_pos.y, tile_pos.x, tile_pos.y));
			tile->add_component<c_Bounding_box>(m_game->assets().get_animation(name).get_size(), blocks_movement, blocks_vision);

			if (name == "Black") { m_portals.push_back(tile); }
		}
		else if (string == "NPC")
		{
			int max_health, damage;
			std::string AI_behavior;

			file >> name >> room_pos.x >> room_pos.y >> tile_pos.x >> tile_pos.y >> blocks_movement >> blocks_vision
				>> max_health >> damage >> AI_behavior;

			auto npc= m_entity_manager.add_entity(e_Tag::Enemy);
			npc->add_component<c_Animation>(m_game->assets().get_animation(name), true);
			npc->add_component<c_Transform>(get_position(room_pos.x, room_pos.y, tile_pos.x, tile_pos.y));
			npc->add_component<c_Bounding_box>(m_game->assets().get_animation(name).get_size(), blocks_movement, blocks_vision);
			npc->add_component<c_Health>(max_health, max_health);
			npc->add_component<c_Damage>(damage);

			if (AI_behavior == "Follow") 
			{ 
				float follow_speed;
				file >> follow_speed;
				npc->add_component<c_Follow_player>(npc->get_component<c_Transform>().position, follow_speed);
			}
			else
			{
				int positions_quantity= 0;
				c_Vec2 patrol_position;

				npc->add_component<c_Patrol>();
				auto &npc_patrol= npc->get_component<c_Patrol>();

				file >> npc_patrol.speed >> positions_quantity;

				for (int i= 0; i < positions_quantity; i++)
				{
					file >> patrol_position.x >> patrol_position.y;
					patrol_position= get_position(room_pos.x, room_pos.y, patrol_position.x, patrol_position.y);
					npc_patrol.positions.push_back(patrol_position);
				}
			}
		}
		else if (string == "Player")
		{
			file >> m_player_config.X >> m_player_config.Y >> m_player_config.BX >> m_player_config.BY >> m_player_config.S >> m_player_config.H;
		}
	}
}

c_Vec2 Scene_Zelda::get_position(int rx, int ry, int tx, int ty) const
{
	// This function takes in the room (rx, ry) coordinate
	// as well as the tile (tx, ty) coordinate, and returns the c_Vec2 game world
	// position of the center of the entity.

	int pixel_x= rx * 1280 + tx * 64; // room * 20 tiles * 64 pixels per tile + room tile * 64 pixels per tile
	int pixel_y= ry * 768 + ty * 64; // 12 tiles in y component

	return c_Vec2(pixel_x, pixel_y);
}

void Scene_Zelda::spawn_player()
{
	m_player= m_entity_manager.add_entity(e_Tag::Player);
	m_player->add_component<c_Transform>(c_Vec2(m_player_config.X, m_player_config.Y));
	m_player->add_component<c_Animation>(m_game->assets().get_animation("StandDown"), true);
	m_player->add_component<c_Bounding_box>(c_Vec2(m_player_config.BX, m_player_config.BY), true, false);
	m_player->add_component<c_Health>(m_player_config.H, m_player_config.H);
	m_player->add_component<c_Input>();
}

void Scene_Zelda::spawn_sword(std::shared_ptr<Entity> entity)
{
	if (m_entity_manager.get_entities(e_Tag::Sword).empty())
	{
		auto &entity_transform= entity->get_component<c_Transform>();
		auto &entity_animation= entity->get_component<c_Animation>();
		c_Vec2 sword_position;
		sword_position.x= entity_transform.position.x + entity_animation.animation.get_size().x * entity_transform.facing.x;
		sword_position.y= entity_transform.position.y + entity_animation.animation.get_size().y * entity_transform.facing.y;

		auto sword= m_entity_manager.add_entity(e_Tag::Sword);
		sword->add_component<c_Lifespan>(10, m_current_frame);
		sword->add_component<c_Transform>(sword_position);
		sword->add_component<c_Bounding_box>(c_Vec2(48, 48));
		sword->add_component<c_Damage>(1);
		m_game->play_sound("Slash");
	}
}

void Scene_Zelda::update()
{
	m_entity_manager.update();

	if (!m_paused)
	{
		s_AI();
		s_movement();
		s_status();
		s_collision();
	}
	s_animation();
	s_camera();

	m_current_frame++;
}

void Scene_Zelda::s_movement()
{
	auto &player_transform= m_player->get_component<c_Transform>();
	auto &player_input= m_player->get_component<c_Input>();

	float player_velocity_x= 0.0f;
	float player_velocity_y= 0.0f;

	// sets player velocity according to inputs and player speed
	player_velocity_x= (-player_input.left + player_input.right) * m_player_config.S;
	player_velocity_y= (-player_input.up + player_input.down) * m_player_config.S;

	// prioritizes the direction of the most recent input
	if (player_velocity_x != 0 && player_velocity_y != 0)
	{
		if (player_input.x_was_last)	{ player_velocity_y= 0; }
		else							{ player_velocity_x= 0; }
	}

	player_transform.velocity= c_Vec2 (player_velocity_x, player_velocity_y);

	// determine which way the player is facing
	if (player_velocity_x != 0)
	{
		player_transform.facing.x= player_velocity_x > 0 ? 1 : -1;
		player_transform.facing.y= 0;
	}
	else if (player_velocity_y != 0)
	{
		player_transform.facing.y= player_velocity_y > 0 ? 1 : -1;
		player_transform.facing.x= 0;
	}

	// update position of all entities
	for (auto &e : m_entity_manager.get_entities())
	{
		auto &transform= e->get_component<c_Transform>();

		transform.previous_position= transform.position;
		if (e->tag() != e_Tag::Sword)
		{
			transform.position+= transform.velocity;
		}
		else
		{
			auto &player_animation= m_player->get_component<c_Animation>();
			c_Vec2 sword_position;
			sword_position.x= player_transform.position.x + 56 * player_transform.facing.x;
			sword_position.y= player_transform.position.y + 56 * player_transform.facing.y;
			transform.position= sword_position;
		}
	}
}


void Scene_Zelda::s_do_action(const Action &action)
{
	// TODO
	//
	// Implement all actions described for the game here
	// Only the setting of the player's input component variables should be set here
	// Do minimal logic in this function

	if (action.type() == "START")
	{
			 if (action.name() == "PAUSE")				{ set_paused(); }
		else if (action.name() == "QUIT")				{ on_end(); }
		else if (action.name() == "TOGGLE_FOLLOW")		{ m_follow= !m_follow; }
		else if (action.name() == "TOGGLE_TEXTURE")		{ m_draw_textures= !m_draw_textures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_draw_collision= !m_draw_collision; }
		else if (action.name() == "UP")					{ m_player->get_component<c_Input>().up=	true; set_last_x(false); }
		else if (action.name() == "DOWN")				{ m_player->get_component<c_Input>().down=	true; set_last_x(false); }
		else if (action.name() == "LEFT")				{ m_player->get_component<c_Input>().left=	true; set_last_x(true);  }
		else if (action.name() == "RIGHT")				{ m_player->get_component<c_Input>().right= true; set_last_x(true);  }
		else if (action.name() == "ATTACK")				{ spawn_sword(m_player); }
		else if (action.name() == "OPEN_EDITOR")		{ open_editor(); }
	}
	else if (action.type() == "END")
	{
			 if (action.name() == "UP")		{ m_player->get_component<c_Input>().up=	false; }
		else if (action.name() == "DOWN")	{ m_player->get_component<c_Input>().down=	false; }
		else if (action.name() == "LEFT")	{ m_player->get_component<c_Input>().left=	false; }
		else if (action.name() == "RIGHT")	{ m_player->get_component<c_Input>().right= false; }
	}
}

void Scene_Zelda::set_last_x(bool was_last)
{
	m_player->get_component<c_Input>().x_was_last= was_last;
}

void Scene_Zelda::s_AI()
{
	// Follow behavior
	c_Vec2 player_position= m_player->get_component<c_Transform>().position;
	c_Vec2 player_room;
	player_room.x= floor(player_position.x / 1280);
	player_room.y= floor(player_position.y / 768);
	for (auto &npc : m_entity_manager.get_entities(e_Tag::Enemy))
	{
		c_Vec2 npc_position= npc->get_component<c_Transform>().position;
		c_Vec2 direction;
		c_Vec2 velocity= c_Vec2(0, 0);

		if (npc->get_component<c_Follow_player>().has)
		{
			c_Vec2 npc_room= c_Vec2(floor(npc_position.x / 1280), floor(npc_position.y / 768));
			bool has_sight= false;

			// If player and npc are in the same room
			if (player_room == npc_room)
			{
				has_sight= true;
				// Check sight line against all entities
				for (auto &e : m_entity_manager.get_entities())
				{
					// If the entity has a bounding box and blocks vision
					if (e != npc && e != m_player && e->get_component<c_Bounding_box>().has && e->get_component<c_Bounding_box>().block_vision)
					{
						c_Vec2 e_position= e->get_component<c_Transform>().position;
						// If the npc and entity are in the same room
						c_Vec2 e_room= c_Vec2(floor(e_position.x / 1280), floor(e_position.y / 768));
						if (npc_room == e_room)
						{
							// If there's an intersection then the npc does not have sight on the player
							if (Physics::entity_intersect(player_position, npc_position, e))
							{
								has_sight= false;
								break;
							}
						}
					}
				}
			}

			if (has_sight)
			{
				direction= m_player->get_component<c_Transform>().position - npc_position;
				velocity= direction * direction.quick_inverse_magnitude() * npc->get_component<c_Follow_player>().speed;
				npc->get_component<c_Transform>().velocity= velocity;
			}
			else if (npc->get_component<c_Follow_player>().home.get_distance_squared(npc_position) > 25)
			{
				direction= npc->get_component<c_Follow_player>().home - npc_position;
				velocity= direction * direction.quick_inverse_magnitude() * npc->get_component<c_Follow_player>().speed;
				npc->get_component<c_Transform>().velocity= velocity;
			}
			else
			{
				npc->get_component<c_Transform>().velocity= velocity;
			}
		}

		// Patrol behavior
		if (npc->get_component<c_Patrol>().has)
		{
			auto &npc_patrol= npc->get_component<c_Patrol>();
			if (npc_patrol.positions.at(npc_patrol.current_position).get_distance_squared(npc_position) < 25)
			{
				npc_patrol.current_position= (npc_patrol.current_position + 1) % npc_patrol.positions.size();
			}

			direction= npc_patrol.positions.at(npc_patrol.current_position) - npc_position;
			velocity= direction * direction.quick_inverse_magnitude() * npc->get_component<c_Patrol>().speed;
			npc->get_component<c_Transform>().velocity= velocity;
		}
	}
}

void Scene_Zelda::s_status()
{
	// Lifespan
	for (auto &e : m_entity_manager.get_entities())
	{
		auto &e_lifespan= e->get_component<c_Lifespan>();
		if (e_lifespan.has)
		{
			if (m_current_frame >= e_lifespan.frame_created + e_lifespan.lifespan)
			{
				e->destroy();
			}
		}
	}
	// Invincibility frames
	for (auto &e : m_entity_manager.get_entities())
	{
		auto &e_invincibility= e->get_component<c_Invincibility>();
		if (e_invincibility.has)
		{
			e_invincibility.iframes--;
			if (e_invincibility.iframes <= 0)
			{
				e->remove_component<c_Invincibility>();
			}
		}
	}
}

void Scene_Zelda::s_collision()
{
	entity_tile_collisions();
	player_enemy_collisions();
	sword_npc_collisions();
}

void Scene_Zelda::entity_tile_collisions()
{
	c_Vec2 overlap;
	c_Vec2 previous_overlap;

	// Check all tiles against the player
	for (auto &t : m_entity_manager.get_entities(e_Tag::Tile))
	{
		overlap= Physics::get_overlap(m_player, t);

		// If the two bounding boxes overlap
		if (overlap.x > 0 && overlap.y > 0)
		{
			// If the tile blocks movement
			if (t->get_component<c_Bounding_box>().block_move)
			{
				previous_overlap= Physics::get_previous_overlap(m_player, t);

				// If the overlap is horizontal
				if (previous_overlap.y > 0)
				{
					// If the player came from the left, push them out to the left
					if (m_player->get_component<c_Transform>().position.x < t->get_component<c_Transform>().position.x)
					{
						m_player->get_component<c_Transform>().position.x-= overlap.x;
					}
					// If the player came from the right push them out to the right
					else
					{
						m_player->get_component<c_Transform>().position.x+= overlap.x;
					}
				}
				// If the overlap is vertical
				if (previous_overlap.x > 0)
				{
					// If the player came from above push them up 
					if (m_player->get_component<c_Transform>().position.y < t->get_component<c_Transform>().position.y)
					{
						m_player->get_component<c_Transform>().position.y-= overlap.y;
					}
					// If the player came from below push them down
					else
					{
						m_player->get_component<c_Transform>().position.y+= overlap.y;
					}
				}
			}
			// Else if the tile is a heart item
			else if (t->get_component<c_Animation>().animation.get_name() == "Heart")
			{
				entity_heart_collisions(m_player, t);
			}
			// Else if the tile is a portal
			else if (t->get_component<c_Animation>().animation.get_name() == "Black")
			{
				teleporting(t);
			}

		}
		// If the player did not overlap the exit portal then reset the exit portal
		else if (t == m_exit_portal) { m_exit_portal= nullptr; }
		
	}

	// Check all enemies
	for (auto &e : m_entity_manager.get_entities(e_Tag::Enemy))
	{
		// Against all tiles
		for (auto &t : m_entity_manager.get_entities(e_Tag::Tile))
		{
			overlap= Physics::get_overlap(e, t);

			// If the bounding boxes overlap
			if (overlap.x > 0 && overlap.y > 0)
			{
				// If the tile blocks movement
				if (t->get_component<c_Bounding_box>().block_move)
				{
					previous_overlap= Physics::get_previous_overlap(e, t);

					// If the overlap is horizontal
					if (previous_overlap.y > 0)
					{
						// If the player came from the left, push them out to the left
						if (e->get_component<c_Transform>().position.x < t->get_component<c_Transform>().position.x)
						{
							e->get_component<c_Transform>().position.x-= overlap.x;
						}
						// If the player came from the right push them out to the right
						else
						{
							e->get_component<c_Transform>().position.x+= overlap.x;
						}
					}
					// If the overlap is vertical
					if (previous_overlap.x > 0)
					{
						if (e->get_component<c_Transform>().position.y < t->get_component<c_Transform>().position.y)
						{
							e->get_component<c_Transform>().position.y-= overlap.y;
						}
						// If the player came from the right push them out to the right
						else
						{
							e->get_component<c_Transform>().position.y+= overlap.y;
						}
					}
				}
				// If the tile is a heart item
				else if (t->get_component<c_Animation>().animation.get_name() == "Heart")
				{
					entity_heart_collisions(e, t);
				}
			}
		}
	}
}

void Scene_Zelda::player_enemy_collisions()
{
	c_Vec2 overlap;
	c_Vec2 previous_overlap;

	// Check all enemies
	for (auto &e : m_entity_manager.get_entities(e_Tag::Enemy))
	{
		// Against the player
		overlap= Physics::get_overlap(m_player, e);

		// If the bounding boxes overlap
		if (overlap.x > 0 && overlap.y > 0)
		{
			if (!m_player->get_component<c_Invincibility>().has)
			{
				// Damage the player
				m_player->get_component<c_Health>().current-= e->get_component<c_Damage>().damage;
				// If the player's health is at 0 then kill and respawn the player
				if (m_player->get_component<c_Health>().current <= 0)
				{
					m_player->destroy();
					spawn_player();
					m_game->play_sound("LinkDie");
				}
				else { m_game->play_sound("LinkHurt"); }
				m_player->add_component<c_Invincibility>(30);
				
			}
		}
	}
}

void Scene_Zelda::sword_npc_collisions()
{
	c_Vec2 overlap;
	c_Vec2 previous_overlap;

	// Check all enemies
	for (auto &e : m_entity_manager.get_entities(e_Tag::Enemy))
	{
		// Against the player's sword
		for (auto &s : m_entity_manager.get_entities(e_Tag::Sword))
		{
			if (e->get_component<c_Bounding_box>().has && !e->get_component<c_Invincibility>().has)
			{
				overlap= Physics::get_overlap(e, s);
				if (overlap.x > 0 && overlap.y > 0)
				{
					e->get_component<c_Health>().current-= s->get_component<c_Damage>().damage;
					if (e->get_component<c_Health>().current <= 0)
					{
						e->add_component<c_Animation>(m_game->assets().get_animation("EnemyDeath"), false);
						e->remove_component<c_Bounding_box>();
						e->remove_component<c_Patrol>();
						e->remove_component<c_Follow_player>();
						e->get_component<c_Transform>().velocity= c_Vec2(0, 0);
						m_game->play_sound("EnemyDie");
					}
					else { m_game->play_sound("EnemyHit"); }
					auto &sword_lifespan=s->get_component<c_Lifespan>();
					e->add_component<c_Invincibility>(sword_lifespan.frame_created + sword_lifespan.lifespan - m_current_frame + 1);
				}
			}
		}
	}
}

void Scene_Zelda::entity_heart_collisions(std::shared_ptr<Entity> entity, std::shared_ptr<Entity> heart)
{
	entity->get_component<c_Health>().current= entity->get_component<c_Health>().max;
	heart->destroy();
	m_game->play_sound("GetItem");
}

void Scene_Zelda::teleporting(std::shared_ptr<Entity> portal)
{
	if (m_exit_portal != portal)
	{
		int random= rand() % m_portals.size();
		if (m_portals.at(random) == portal) { random= (random + 1) % m_portals.size(); }
		auto &player_transform= m_player->get_component<c_Transform>();
		player_transform.position= m_portals.at(random)->get_component<c_Transform>().position;
		player_transform.facing={ 0, 1 };
		player_transform.scale={ 1, 1 };
		m_exit_portal= m_portals.at(random);
	}
}

void Scene_Zelda::s_animation()
{
	// Build the animation name for the player based on direction and action
	std::string animation_name;
	auto &player_transform= m_player->get_component<c_Transform>();
	if (!m_entity_manager.get_entities(e_Tag::Sword).empty())
	{
		animation_name+= "Atk";
	}
	else if (player_transform.velocity.get_magnitude_squared() > 0)
	{
		animation_name+= "Run";
	}
	else
	{
		animation_name+= "Stand";
	}

	if (player_transform.facing.x != 0)
	{
		animation_name+= "Right";
		player_transform.scale.x= player_transform.facing.x;
	}
	else
	{
		animation_name+= player_transform.facing.y == 1 ? "Down" : "Up";
		player_transform.scale.x= 1;
	}

	// if the built animation name is different from the current then replace the current
	if (m_player->get_component<c_Animation>().animation.get_name() != animation_name)
	{
		m_player->add_component<c_Animation>(m_game->assets().get_animation(animation_name), true);
	}

	// update animation for all entities
	for (auto &e : m_entity_manager.get_entities())
	{
		if (e->get_component<c_Animation>().has)
		{
			if (!e->get_component<c_Animation>().repeat && e->get_component<c_Animation>().animation.has_ended())
			{
				e->destroy();
			}
			else
			{
				e->get_component<c_Animation>().animation.update();
			}
		}
	}
}

void Scene_Zelda::s_camera()
{
	// get the current view, which we will modify in the if-statement below
	sf::View view= m_game->window().getView();
	c_Vec2 player_position= m_player->get_component<c_Transform>().position;

	if (m_follow)
	{
		view.setCenter(player_position.x, player_position.y);
	}
	else
	{
		c_Vec2 player_room;
		player_room.x= floor(player_position.x / 1280);
		player_room.y= floor(player_position.y / 768);

		view.setCenter(player_room.x * 1280 + 640, player_room.y * 768 + 384);
	}

	// then set the window view
	m_game->window().setView(view);
}

void Scene_Zelda::on_end()
{
	m_game->stop_sound("MusicLevel");
	m_game->play_sound("MusicTitle", true);
	sf::View view= m_game->window().getDefaultView();
	m_game->window().setView(view);
	m_game->change_scene("MENU", nullptr, true);
}

void Scene_Zelda::s_render()
{
	// RENDERING DONE

	m_game->window().clear(sf::Color(255, 192, 122));
	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);

	// draw all Entity textures / animations
	if (m_draw_textures)
	{
		// draw entity animations
		for (auto e : m_entity_manager.get_entities())
		{
			auto &transform= e->get_component<c_Transform>();
			sf::Color c= sf::Color::White;
			if (e->has_component<c_Invincibility>())
			{
				c= sf::Color(255, 255, 255, 128);
			}

			if (e->has_component<c_Animation>())
			{
				auto &animation= e->get_component<c_Animation>().animation;
				animation.get_sprite().setRotation(transform.angle);
				animation.get_sprite().setPosition(transform.position.x, transform.position.y);
				animation.get_sprite().setScale(transform.scale.x, transform.scale.y);
				animation.get_sprite().setColor(c);
				m_game->window().draw(animation.get_sprite());
			}
		}

		// draw entity health bars
		for (auto e : m_entity_manager.get_entities())
		{
			auto &transform= e->get_component<c_Transform>();
			if (e->has_component<c_Health>())
			{
				auto &h= e->get_component<c_Health>();
				c_Vec2 size(64, 6);
				sf::RectangleShape rect({ size.x, size.y });
				rect.setPosition(transform.position.x - 32, transform.position.y - 48);
				rect.setFillColor(sf::Color(96, 96, 96));
				rect.setOutlineColor(sf::Color::Black);
				rect.setOutlineThickness(2);
				m_game->window().draw(rect);

				float ratio= (float)(h.current) / h.max;
				size.x*= ratio;
				rect.setSize({ size.x, size.y });
				rect.setFillColor(sf::Color(255, 0, 0));
				rect.setOutlineThickness(0);
				m_game->window().draw(rect);

				for (int i= 0; i < h.max; i++)
				{
					tick.setPosition(rect.getPosition() + sf::Vector2f(i * 64 * 1 / h.max, 0));
					m_game->window().draw(tick);
				}
			}
		}
	}

	// draw all Entity collision bounding boxes with a rectangleShape
	if (m_draw_collision)
	{
		sf::CircleShape dot(4);
		dot.setFillColor(sf::Color::Black);
		for (auto e : m_entity_manager.get_entities())
		{
			if (e->has_component<c_Bounding_box>())
			{
				auto &box= e->get_component<c_Bounding_box>();
				auto &transform= e->get_component<c_Transform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.half_size.x, box.half_size.y));
				rect.setPosition(transform.position.x, transform.position.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));

				if (box.block_move && box.block_vision) { rect.setOutlineColor(sf::Color::Black); }
				if (box.block_move && !box.block_vision) { rect.setOutlineColor(sf::Color::Blue); }
				if (!box.block_move && box.block_vision) { rect.setOutlineColor(sf::Color::Red); }
				if (!box.block_move && !box.block_vision) { rect.setOutlineColor(sf::Color::White); }
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}

			if (e->has_component<c_Patrol>())
			{
				auto &patrol= e->get_component<c_Patrol>().positions;
				for (size_t p= 0; p < patrol.size(); p++)
				{
					dot.setPosition(patrol[p].x, patrol[p].y);
					m_game->window().draw(dot);
				}
			}

			if (e->has_component<c_Follow_player>())
			{
				sf::VertexArray lines(sf::LinesStrip, 2);
				lines[0].position.x= e->get_component<c_Transform>().position.x;
				lines[0].position.y= e->get_component<c_Transform>().position.y;
				lines[0].color= sf::Color::Black;
				lines[1].position.x= m_player->get_component<c_Transform>().position.x;
				lines[1].position.y= m_player->get_component<c_Transform>().position.y;
				lines[1].color= sf::Color::Black;
				m_game->window().draw(lines);
				m_game->window().draw(lines);
				dot.setPosition(e->get_component<c_Follow_player>().home.x, e->get_component<c_Follow_player>().home.y);
				m_game->window().draw(dot);
			}
		}
	}
}

void Scene_Zelda::open_editor() 
{
	m_paused= true;
	if (!m_game->scene_exists("Zelda_Editor"))
	{
		m_game->change_scene("Zelda_Editor", std::make_shared<Scene_Editor>(m_game, this), false);
	}
	else
	{
		m_game->change_scene("Zelda_Editor", nullptr, false);
	}
}