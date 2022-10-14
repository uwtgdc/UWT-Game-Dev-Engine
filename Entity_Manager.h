#pragma once

#include "Common.h"
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<enum e_Tag, EntityVec> EntityMap;

class Entity_Manager
{
	EntityVec m_entities;
	EntityVec m_entities_to_add;
	EntityMap m_entity_map;
	size_t	  m_total_entities= 0;

	void remove_dead_entities(EntityVec& vec);

public:

	Entity_Manager();

	void update();

	std::shared_ptr<Entity> add_entity(const enum e_Tag &tag);

	const EntityVec &get_entities();
	const EntityVec &get_entities(const enum e_Tag &tag);
};
