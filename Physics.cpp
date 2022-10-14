#include "Physics.h"
#include "Components.h"

c_Vec2 Physics::get_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	c_Vec2 overlap= c_Vec2(0, 0);

	if (a->get_component<c_Bounding_box>().has && b->get_component<c_Bounding_box>().has)
	{
		c_Vec2 a_position= a->get_component<c_Transform>().position;
		c_Vec2 b_position= b->get_component<c_Transform>().position;

		c_Vec2 a_half_size= a->get_component<c_Bounding_box>().half_size;
		c_Vec2 b_half_size= b->get_component<c_Bounding_box>().half_size;

		c_Vec2 delta= c_Vec2(abs(a_position.x - b_position.x), abs(a_position.y - b_position.y));

		float ox= a_half_size.x + b_half_size.x - delta.x;
		float oy= a_half_size.y + b_half_size.y - delta.y;

		overlap= c_Vec2(ox, oy);
	}

	return overlap;
}

c_Vec2 Physics::get_previous_overlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	c_Vec2 overlap= c_Vec2(0, 0);

	if (a->get_component<c_Bounding_box>().has && b->get_component<c_Bounding_box>().has)
	{
		c_Vec2 a_position= a->get_component<c_Transform>().previous_position;
		c_Vec2 b_position= b->get_component<c_Transform>().previous_position;

		c_Vec2 a_half_size= a->get_component<c_Bounding_box>().half_size;
		c_Vec2 b_half_size= b->get_component<c_Bounding_box>().half_size;

		c_Vec2 delta= c_Vec2(abs(a_position.x - b_position.x), abs(a_position.y - b_position.y));

		float ox= a_half_size.x + b_half_size.x - delta.x;
		float oy= a_half_size.y + b_half_size.y - delta.y;

		overlap= c_Vec2(ox, oy);
	}

	return overlap;
}

bool Physics::is_inside(const c_Vec2 &pos, std::shared_ptr<Entity> e)
{
	bool inside= false;
	// If the entity doesn't have an animation, we can't be inside it
	if (e->has_component<c_Animation>())
	{
		// Checks if a point is contained within a bounding box
		c_Vec2 e_position= e->get_component<c_Transform>().position;
		c_Vec2 e_half_size= e->get_component<c_Animation>().animation.get_size() / 2;

		c_Vec2 delta= c_Vec2(abs(e_position.x - pos.x), abs(e_position.y - pos.y));

		bool overlap_x= e_half_size.x > delta.x;
		bool overlap_y= e_half_size.y > delta.y;

		inside= overlap_x && overlap_y;
	}

	return inside;
}

Intersect Physics::line_intersect(const c_Vec2 &a, const c_Vec2 &b, const c_Vec2 &c, const c_Vec2 &d)
{
	Intersect result;

	c_Vec2 r= (b - a);
	c_Vec2 s= (d - c);
	float rxs= r.cross_product(s);
	c_Vec2 cma= c - a;
	float t= cma.cross_product(s) / rxs;
	float u= cma.cross_product(r) / rxs;

	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		result= { true, c_Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	else
	{
		result={ false, c_Vec2(0,0) };
	}

	return result;
}

bool Physics::entity_intersect(const c_Vec2 &a, const c_Vec2 &b, std::shared_ptr<Entity> e)
{
	float half_size_x= e->get_component<c_Bounding_box>().half_size.x;
	float half_size_y= e->get_component<c_Bounding_box>().half_size.y;
	c_Vec2 p1= e->get_component<c_Transform>().position;
	c_Vec2 p2= p1, p3= p1, p4= p1;

	p1+= c_Vec2(half_size_x, half_size_y);
	p2+= c_Vec2(half_size_x, -half_size_y);
	p3+= c_Vec2(-half_size_x, -half_size_y);
	p4+= c_Vec2(-half_size_x, half_size_y);
	

	if (Physics::line_intersect(a, b, p1, p2).does_intersect) { return true; }
	if (Physics::line_intersect(a, b, p2, p3).does_intersect) { return true; }
	if (Physics::line_intersect(a, b, p3, p4).does_intersect) { return true; }
	if (Physics::line_intersect(a, b, p1, p4).does_intersect) { return true; }

	return false;
}