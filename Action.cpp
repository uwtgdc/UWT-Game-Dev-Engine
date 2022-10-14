#include "Action.h"

Action::Action() {}

Action::Action(const std::string &name, const std::string &type)
	: m_name(name), m_type(type) {}

Action::Action(const std::string &type, c_Vec2 pos)
	: m_name(type), m_pos(pos) {}

Action::Action(const std::string &name, const std::string &type, c_Vec2 pos)
	: m_name(name), m_type(type), m_pos(pos) {}

const std::string &Action::name() const
{
	return m_name;
}
const std::string &Action::type() const
{
	return m_type;
}

const c_Vec2 &Action::pos() const
{
	return m_pos;
}

std::string Action::to_string() const
{
	std::stringstream stream;
	stream << m_name << " " << m_type << " " << (int)pos().x << " " << (int)pos().y;
	return stream.str();
}