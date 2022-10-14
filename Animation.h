#pragma once

#include "Common.h"
#include <vector>

class Animation
{
	sf::Sprite	m_sprite;
	size_t		m_frame_count=		1;			// total number of frames of animation
	size_t		m_current_frame=	0;			// the current frame of animation being played
	size_t		m_speed=			0;			// the speed to play this animation
	c_Vec2		m_size=				{ 1,1 };	// the size of the animation frame
	std::string m_name=				"none";

public:

	Animation();
	Animation(const std::string &name, const sf::Texture &t);
	Animation(const std::string &name, const sf::Texture &t, size_t frameCount, size_t speed);

	void update();
	bool has_ended() const;
	const std::string &get_name() const;
	const c_Vec2 &get_size() const;
	sf::Sprite &get_sprite();
};