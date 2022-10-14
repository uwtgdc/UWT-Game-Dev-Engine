#pragma once

#include "Common.h"
#include "Entity.h"

struct Intersect { bool does_intersect; c_Vec2 pos; };

namespace Physics
{
	c_Vec2 get_overlap(std::shared_ptr<Entity> a, std::shared_ptr <Entity> b);
	c_Vec2 get_previous_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);
	bool is_inside(const c_Vec2 &pos, std::shared_ptr<Entity> e);
	Intersect line_intersect(const c_Vec2 &a, const c_Vec2 &b, const c_Vec2 &c, const c_Vec2 &d);
	bool entity_intersect(const c_Vec2 &a, const c_Vec2 &b, std::shared_ptr<Entity> e);
}