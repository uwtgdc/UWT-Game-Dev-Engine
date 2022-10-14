#pragma once

#include "Common.h"
#include "Entity.h"

class Action
{
	std::string m_name= "NONE";
	std::string m_type= "NONE";
	c_Vec2		m_pos= c_Vec2(0, 0);

public:

	Action();
	Action(const std::string &name, const std::string &type);
	Action(const std::string &name, c_Vec2 pos);
	Action(const std::string &name, const std::string &type, c_Vec2 pos);

	const std::string &name() const;
	const std::string &type() const;
	const c_Vec2	  &pos()  const;
	std::string to_string() const;
};