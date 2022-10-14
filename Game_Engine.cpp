#include "Game_Engine.h"
#include "Assets.h"
#include "Scene_Zelda.h"
#include "Scene_Menu.h"

Game_Engine::Game_Engine(const std::string &path)
{
	initialize(path);
}

void Game_Engine::initialize(const std::string &path)
{
	srand(time(NULL));

	m_assets.load_from_file(path);

	m_window.create(sf::VideoMode(1280, 768), "Definitely Not Zelda");
	m_window.setFramerateLimit(60);

	change_scene("MENU", std::make_shared<Scene_Menu>(this));
}

std::shared_ptr<Scene> Game_Engine::current_scene()
{
	return m_scene_map[m_current_scene];
}

bool Game_Engine::is_running()
{
	return m_running && m_window.isOpen();
}

Assets &Game_Engine::assets()
{
	return m_assets;
}

sf::RenderWindow &Game_Engine::window()
{
	return m_window;
}

void Game_Engine::run()
{
	while (is_running())
	{
		update();
	}
}

void Game_Engine::s_user_input()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			// if the current scene does not have an action associated with this key, skip the event
			if (current_scene()->get_action_map().find(event.key.code) == current_scene()->get_action_map().end()) { continue; }

			// determine the start or end action by whether it was key press or release
			const std::string action_type= (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send the action to the scene
			current_scene()->do_action(Action(current_scene()->get_action_map().at(event.key.code), action_type));
		}

		// Mouse Actions
		auto mpos= sf::Mouse::getPosition(m_window);
		c_Vec2 pos(mpos.x, mpos.y);
		if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:	{ current_scene()->do_action(Action("LEFT_CLICK",   "START", pos)); break; }
				case sf::Mouse::Middle: { current_scene()->do_action(Action("MIDDLE_CLICK", "START", pos)); break; }
				case sf::Mouse::Right:	{ current_scene()->do_action(Action("RIGHT_CLICK",  "START", pos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left:	{ current_scene()->do_action(Action("LEFT_CLICK",   "END", pos)); break; }
				case sf::Mouse::Middle: { current_scene()->do_action(Action("MIDDLE_CLICK", "END", pos)); break; }
				case sf::Mouse::Right:	{ current_scene()->do_action(Action("RIGHT_CLICK",  "END", pos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			current_scene()->do_action(Action("MOUSE_MOVE", c_Vec2(event.mouseMove.x, event.mouseMove.y)));
		}
	}
}

void Game_Engine::change_scene(const std::string &scene_name, std::shared_ptr<Scene> scene, bool end_current_scene)
{
	// If a scene was passed, add it to the map with the scene name
	if (scene)
	{
		m_scene_map[scene_name]= scene;
	}
	else
	{
		// If no scene was passed and the scene name is not in the map then return a warning and exit
		if (m_scene_map.find(scene_name) == m_scene_map.end())
		{
			std::cerr << "Warning: Scene does not exist: " << scene_name << std::endl;
			exit(-1);
		}
	}

	if (end_current_scene)
	{
		m_scene_map.erase(m_scene_map.find(m_current_scene));
	}

	m_current_scene= scene_name;
}

bool Game_Engine::scene_exists(const std::string &scene_name) { return m_scene_map[scene_name] != nullptr; }

void Game_Engine::update()
{
	if (!is_running())		 { return; }

	if (m_scene_map.empty()) { return; }

	s_user_input();
	current_scene()->update();
	current_scene()->s_render();
	m_window.display();
}

void Game_Engine::quit()
{
	m_running= false; 
}

void Game_Engine::play_sound(const std::string &sound_name, bool repeat)
{
	m_assets.get_sound(sound_name).setLoop(repeat);
	m_assets.get_sound(sound_name).play();
}

void Game_Engine::stop_sound(const std::string &sound_name)
{
	m_assets.get_sound(sound_name).stop();
}