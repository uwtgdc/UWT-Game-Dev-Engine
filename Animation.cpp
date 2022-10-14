#include "Animation.h"
#include <cmath>

Animation::Animation()
{
}

Animation::Animation(const std::string &name, const sf::Texture &t)
	: Animation(name, t, 1, 0)
{

}

Animation::Animation(const std::string &name, const sf::Texture &t, size_t frame_count, size_t speed)
	: m_name			(name)
	, m_sprite			(t)
	, m_frame_count		(frame_count)
	, m_current_frame	(0)
	, m_speed			(speed)
{
	m_size= c_Vec2((float)t.getSize().x / frame_count, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.0f, m_size.y / 2.0f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_current_frame) * m_size.x, 0, m_size.x, m_size.y));
}

// updates the animation to show the next frame, depending on its speed
// animation loops when it reaches the end
void Animation::update()
{
	m_current_frame++;

	if (m_speed > 0)
	{
		size_t frame= (m_current_frame / m_speed) % m_frame_count;
		sf::IntRect rectangle= sf::IntRect(std::floor(frame) * m_size.x, 0, m_size.x, m_size.y);
		m_sprite.setTextureRect(rectangle);
	}
}

bool Animation::has_ended() const
{
	bool end= false;

	if (m_current_frame == (m_speed * m_frame_count) - 1)
	{
		end= true;
	}
	return end;
}

const c_Vec2 &Animation::get_size() const
{
	return m_size;
}

const std::string &Animation::get_name() const
{
	return m_name;
}

sf::Sprite &Animation::get_sprite()
{
	return m_sprite;
}