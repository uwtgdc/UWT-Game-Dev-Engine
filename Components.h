#pragma once

#include "Animation.h"
#include "Assets.h"

class Component
{
public:
	bool has= false;
};

class c_Transform : public Component
{
public: 
	c_Vec2 position=			{ 0.0, 0.0 };
	c_Vec2 previous_position=	{ 0.0, 0.0 };
	c_Vec2 scale=				{ 1.0, 1.0 };
	c_Vec2 velocity=			{ 0.0, 0.0 };
	c_Vec2 facing=				{ 0.0, 1.0 };
	float  angle=				0;

	c_Transform() {}
	c_Transform(const c_Vec2 &p)
		: position(p) {}
	c_Transform(const c_Vec2 &p, const c_Vec2 &sp, const c_Vec2 &sc, float a)
		: position(p), previous_position(p), velocity(sp), scale(sc), angle(a) {}
};

class c_Lifespan : public Component
{
public:
	int lifespan= 0;
	int frame_created= 0;

	c_Lifespan() {}
	c_Lifespan(int duration, int frame)
		: lifespan(duration), frame_created(frame) {}
};

class c_Damage : public Component
{
public:
	int damage= 1;
	
	c_Damage() {}
	c_Damage(int d)
		: damage(d) {}
};

class c_Invincibility : public Component
{
public:
	int iframes= 0;

	c_Invincibility() {}
	c_Invincibility(int f)
		: iframes(f) {}
};

class c_Health : public Component
{
public:
	int max= 1;
	int current= 1;

	c_Health() {}
	c_Health(int m, int c)
		: max(m), current(c) {}
};

class c_Input : public Component
{
public:
	bool up=		 false;
	bool down=		 false;
	bool left=		 false;
	bool right=		 false;
	bool attack=	 false;
	bool x_was_last= false;

	c_Input() {}
};

class c_Bounding_box : public Component
{
public:
	c_Vec2 size;
	c_Vec2 half_size;
	bool block_move= false;
	bool block_vision= false;

	c_Bounding_box() {}
	c_Bounding_box(const c_Vec2 &s)
		: size(s), half_size(s.x / 2, s.y / 2) {}
	c_Bounding_box(const c_Vec2 &s, bool m, bool v)
		: size(s), block_move(m), block_vision(v), half_size(s.x / 2, s.y / 2) {}
};

class c_Animation : public Component
{
public:
	Animation animation;
	bool repeat= false;
	
	c_Animation() {}
	c_Animation(const Animation &animation, bool r)
		: animation(animation), repeat(r) {}
};

class c_State : public Component
{
public:
	std::string state= "stand";
	
	c_State() {}
	c_State(const std::string &s) : state(s) {}
};

class c_Follow_player : public Component
{
public:
	c_Vec2 home= { 0, 0 };
	float speed= 0;

	c_Follow_player() {}
	c_Follow_player(c_Vec2 h, float s)
		: home(h), speed(s) {}
};

class c_Patrol : public Component
{
public:
	std::vector<c_Vec2> positions;
	size_t current_position= 0;
	float speed= 0;
	
	c_Patrol() {}
	c_Patrol(std::vector<c_Vec2> &pos, float s)
		: positions(pos), speed(s) {}
};

