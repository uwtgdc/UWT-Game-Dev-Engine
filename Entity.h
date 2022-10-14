#pragma once

#include "Common.h"

#include "Components.h"

class Entity_Manager;

typedef std::tuple<
	c_Transform,
	c_Lifespan,
	c_Input,
	c_Bounding_box,
	c_Animation,
	c_State,
	c_Damage,
	c_Follow_player,
	c_Health,
	c_Invincibility,
	c_Patrol
> ComponentTuple;

enum class e_Tag{Default, Player, Enemy, Sword, Tile, Button};

class Entity
{
	friend class Entity_Manager;

	bool			m_active= true;
	e_Tag			m_tag= e_Tag::Default;
	size_t			m_id= 0;
	ComponentTuple	m_components;

	// constructor is private so we can never create
	// entities outside the Entity_Manager which has friend access
	Entity(const size_t &id, const enum e_Tag &tag);

public:

	void		destroy();
	size_t		id()		const;
	bool		is_active() const;
	const e_Tag &tag()		const;

	template <typename T>
	bool has_component() const
	{
		return get_component<T>().has;
	}

	template <typename T, typename... T_args>
	T &add_component(T_args&&... m_args)
	{
		auto &component= get_component<T>();
		component= T(std::forward<T_args>(m_args)...);
		component.has= true;
		return component;
	}

	template<typename T>
	T &get_component()
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	const T &get_component() const
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	void remove_component()
	{
		auto &component= get_component<T>();
		component.has= false;
	}
};