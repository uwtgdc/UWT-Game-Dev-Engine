#include "Entity.h"

Entity::Entity(const size_t &id, const enum e_Tag &t)
	: m_id(id), m_tag(t) {}

bool Entity::is_active() const
{
	return m_active;
}

void Entity::destroy()
{
	m_active= false;
}

size_t Entity::id() const
{
	return m_id;
}

const e_Tag &Entity::tag() const
{
	return m_tag;
}
