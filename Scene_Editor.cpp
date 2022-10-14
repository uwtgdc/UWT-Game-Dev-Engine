#include "Scene_Editor.h"
#include "Common.h"
#include "Physics.h"
#include "Assets.h"
#include "Game_Engine.h"
#include "Components.h"

Scene_Editor::Scene_Editor(Game_Engine *game, Scene_Zelda *scene)
	: Scene(game), m_scene(scene)
{
	initialize();
}

void Scene_Editor::initialize() 
{
	m_current_frame= m_scene->current_frame();
	m_gui.setSize(1280, 768);
	m_gui.setCenter(m_game->window().getView().getCenter());

	register_action(sf::Keyboard::Escape, "QUIT");
	register_action(sf::Keyboard::T, "TOGGLE_TEXTURE");		// Toggle drawing (T)extures
	register_action(sf::Keyboard::C, "TOGGLE_COLLISION");	// Toggle drawing (C)ollision boxes
	register_action(sf::Keyboard::W, "UP");					
	register_action(sf::Keyboard::S, "DOWN");
	register_action(sf::Keyboard::A, "LEFT");
	register_action(sf::Keyboard::D, "RIGHT");
	register_action(sf::Keyboard::Tilde, "CLOSE_EDITOR");

	create_buttons();
}

void Scene_Editor::create_buttons()
{
	auto up_button= m_entity_manager.add_entity(e_Tag::Button);
	up_button->add_component<c_Animation>(m_game->assets().get_animation("Arrow"), true);
	up_button->add_component<c_Transform>(c_Vec2(m_game->window().getSize().x - 16, 20));
	auto down_button= m_entity_manager.add_entity(e_Tag::Button);
	down_button->add_component<c_Animation>(m_game->assets().get_animation("Arrow"), true);
	down_button->add_component<c_Transform>(c_Vec2(m_game->window().getSize().x - 16, 60));
	down_button->get_component<c_Transform>().scale.y= -1;

	std::vector<std::string> animations= m_game->assets().get_animation_names();
	std::string animation_name;
	m_animation_set= 0;
	for (int i= 0; i < 15; i++)
	{
		animation_name= animations.at(i);
		auto entity= m_entity_manager.add_entity(e_Tag::Tile);
		entity->add_component<c_Animation>(m_game->assets().get_animation(animation_name), true);
		entity->add_component<c_Transform>(c_Vec2(48.0f + i * 80.0f, 40.0f));

		auto &animation_size= entity->get_component<c_Animation>().animation.get_size();

		if (animation_size.x > 64 || animation_size.y > 64)
		{
			float scale= animation_size.x >= animation_size.y ? animation_size.x : animation_size.y;
			entity->get_component<c_Transform>().scale= c_Vec2(64 / scale, 64 / scale);
		}
		
		m_tile_set.push_back(entity);
	}
}

void Scene_Editor::s_do_action(const Action &action)
{
	if (action.type() == "START")
	{
		if		(action.name() == "QUIT")				{ on_end(); }
		else if (action.name() == "TOGGLE_TEXTURE")		{ m_draw_textures= !m_draw_textures; }
		else if (action.name() == "TOGGLE_COLLISION")	{ m_draw_collision= !m_draw_collision; }
		else if (action.name() == "UP")					{ m_camera_input.up= true; }
		else if (action.name() == "DOWN")				{ m_camera_input.down= true; }
		else if (action.name() == "LEFT")				{ m_camera_input.left= true; }
		else if (action.name() == "RIGHT")				{ m_camera_input.right= true; }
		else if (action.name() == "CLOSE_EDITOR")		{ close_editor(); }
		else if (action.name() == "LEFT_CLICK")			{ left_click_down(); }
	}
	else if (action.type() == "END")
	{
		if		(action.name() == "UP")			{ m_camera_input.up= false; }
		else if (action.name() == "DOWN")		{ m_camera_input.down= false; }
		else if (action.name() == "LEFT")		{ m_camera_input.left= false; }
		else if (action.name() == "RIGHT")		{ m_camera_input.right= false; }
		else if (action.name() == "LEFT_CLICK") { left_click_up(); }
	}
	else if (action.name() == "MOUSE_MOVE") {   }
}

void Scene_Editor::s_animation() 
{
	// update animation for all entities
	for (auto &e : m_scene->m_entity_manager.get_entities())
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
	for (auto &e : m_entity_manager.get_entities())
	{
		if (e->get_component<c_Animation>().has)
		{
			
			e->get_component<c_Animation>().animation.update();
			
		}
	}
}

void Scene_Editor::s_camera() 
{
	sf::View view= m_game->window().getView();

	if (m_camera_input.up)		{ view.setCenter(view.getCenter().x, view.getCenter().y - 5); }
	if (m_camera_input.down)	{ view.setCenter(view.getCenter().x, view.getCenter().y + 5); }
	if (m_camera_input.left)	{ view.setCenter(view.getCenter().x - 5, view.getCenter().y); }
	if (m_camera_input.right)	{ view.setCenter(view.getCenter().x + 5, view.getCenter().y); }

	m_game->window().setView(view);
} 

void Scene_Editor::s_render() 
{
	// RENDERING DONE

	m_game->window().clear(sf::Color(255, 192, 122));
	sf::RectangleShape tick({ 1.0f, 6.0f });
	tick.setFillColor(sf::Color::Black);

	// draw all Entity textures / animations
	if (m_draw_textures)
	{
		// draw entity animations
		for (auto e : m_scene->m_entity_manager.get_entities())
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
		for (auto e : m_scene->m_entity_manager.get_entities())
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
		for (auto e : m_scene->m_entity_manager.get_entities())
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
				lines[1].position.x= m_scene->m_player->get_component<c_Transform>().position.x;
				lines[1].position.y= m_scene->m_player->get_component<c_Transform>().position.y;
				lines[1].color= sf::Color::Black;
				m_game->window().draw(lines);
				m_game->window().draw(lines);
				dot.setPosition(e->get_component<c_Follow_player>().home.x, e->get_component<c_Follow_player>().home.y);
				m_game->window().draw(dot);
			}
		}
	}

	sf::View world_view= m_game->window().getView();
	m_game->window().setView(m_gui);
	c_Vec2 window_size= c_Vec2(m_game->window().getSize().x, m_game->window().getSize().y);
	sf::RectangleShape drop_down({ window_size.x, 80 });
	drop_down.setPosition(0, 0);
	drop_down.setFillColor(sf::Color(24, 24, 24));
	m_game->window().draw(drop_down);
	
	for (auto e : m_entity_manager.get_entities())
	{
		if (e->has_component<c_Animation>())
		{
			auto &animation= e->get_component<c_Animation>().animation;
			auto &transform= e->get_component<c_Transform>();
			animation.get_sprite().setRotation(transform.angle);
			animation.get_sprite().setPosition(transform.position.x, transform.position.y);
			animation.get_sprite().setScale(transform.scale.x, transform.scale.y);
			m_game->window().draw(animation.get_sprite());
		}
	}

	m_game->window().setView(world_view);
}

void Scene_Editor::s_drag() 
{ 
	auto mpos= sf::Mouse::getPosition(m_game->window());
	float x_diff= m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2;
	float y_diff= m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2;
	m_pos= c_Vec2(mpos.x + x_diff, mpos.y + y_diff);
	
	if (m_dragged)
	{
		m_dragged->get_component<c_Transform>().position= m_pos;
	}
}

void Scene_Editor::on_end() {}

void Scene_Editor::update() 
{
	m_entity_manager.update();
	m_scene->m_entity_manager.update();

	s_animation();
	s_camera();
	s_drag();

	m_current_frame++;
	m_scene->m_current_frame++;
}

void Scene_Editor::close_editor() {}

void Scene_Editor::left_click_down()
{

	auto mpos= sf::Mouse::getPosition(m_game->window());
	c_Vec2 pos= c_Vec2(mpos.x, mpos.y);
	std::cout << m_pos.x << ", " << m_pos.y << " : " << mpos.x << ", " << mpos.y << "\n";
	for (auto e : m_entity_manager.get_entities())
	{
		if (Physics::is_inside(pos, e))
		{
			if (e->tag() == e_Tag::Tile)
			{
				auto &animation= e->get_component<c_Animation>();
				auto entity= m_scene->m_entity_manager.add_entity(e_Tag::Tile);
				entity->add_component<c_Animation>(m_game->assets().get_animation(animation.animation.get_name()), true);
				entity->add_component<c_Transform>(m_pos);

				m_dragged= entity;
				m_selected= entity;
				break;
			}
			else if (e->tag() == e_Tag::Button)
			{
				std::vector<std::string> animations= m_game->assets().get_animation_names();
				std::string animation_name;
				if (e->get_component<c_Transform>().scale.y == 1)
				{
					m_animation_set= (m_animation_set - 15) % animations.size();
				}
				else if (e->get_component<c_Transform>().scale.y == -1)
				{
					m_animation_set= (m_animation_set + 15) % animations.size();
				}

				int i= m_animation_set;
				for (auto e : m_tile_set)
				{
					animation_name= animations.at(i);
					e->add_component<c_Animation>(m_game->assets().get_animation(animation_name), true);
					i= (i + 1) % animations.size();

					auto &animation_size= e->get_component<c_Animation>().animation.get_size();
					if (animation_size.x > 64 || animation_size.y > 64)
					{
						float scale= animation_size.x >= animation_size.y ? animation_size.x : animation_size.y;
						e->get_component<c_Transform>().scale= c_Vec2(64 / scale, 64 / scale);
					}
					else { e->get_component<c_Transform>().scale= c_Vec2(1, 1); }
				}
			}
		}
	}

	if (!m_dragged)
	{
		for (auto e : m_scene->m_entity_manager.get_entities())
		{
			if (Physics::is_inside(m_pos, e))
			{
				m_dragged= e;
				m_selected= e;
				break;
			}
		}
	}
}

void Scene_Editor::left_click_up()
{
	m_dragged= nullptr;
}