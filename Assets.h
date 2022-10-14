#pragma once

#include "Common.h"
#include "Animation.h"

class Assets
{
	std::map<std::string, sf::Texture>		m_texture_map;
	std::map<std::string, Animation>		m_animation_map;
	std::map<std::string, sf::Font>			m_font_map;
	std::map<std::string, sf::Sound>		m_sound_map;
	std::map<std::string, sf::SoundBuffer>	m_buffer_map;
	std::vector<std::string>				m_animation_names;

	void add_texture(const std::string &texture_name, const std::string &path, bool smooth= true);
	void add_animation(const std::string &animation_Name, const std::string &texture_name, size_t frameCount, size_t speed);
	void add_font(const std::string &font_name, const std::string &path);
	void add_sound(const std::string &sound_name, const std::string &path);

public:

	Assets();

	void load_from_file(const std::string &path);

	const sf::Texture				&get_texture(const std::string &texture_name) const;
	const Animation					&get_animation(const std::string &animation_name) const;
	const sf::Font					&get_font(const std::string &font_name) const;
	sf::Sound						&get_sound(const std::string &sound_name);
	const std::vector<std::string>	&get_animation_names();
};