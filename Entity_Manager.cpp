#include "Entity_Manager.h"

Entity_Manager::Entity_Manager() {}
	

void Entity_Manager::update()
{
	// add all the entities that are pending
	for (auto &e : m_entities_to_add)
	{
		// add it to the vector of all entities 
		m_entities.push_back(e);

		// add it to the entity map in the correct place
		// map[key] will create an element at 'key' if it does not already exist 
		//			therefore we are not in danger of adding to a vector that doesn't exist
		m_entity_map[e->m_tag].push_back(e);
	}

	// clear the temporary vector since we have added everything
	m_entities_to_add.clear();

	// remove dead entities from the vector of all entities
	remove_dead_entities(m_entities);

	// clean up dead entities in all vectors
	for (auto &kv : m_entity_map)
	{
		// kv is a key-value pair contained in the map
		//	  key	(kv.first):	 the tag string
		//	  value (kv.second): the vector storing entities
		remove_dead_entities(kv.second);
	}
}

// iterates through a passed vector and erases any inactive entities
void Entity_Manager::remove_dead_entities(EntityVec &vec)
{
	auto v= vec.begin();
	while (v != vec.end())
	{
		if (!(*v)->is_active())
		{
			v= vec.erase(v);
		}
		else
		{
			v++;
		}
	}
}

// pushes an entity to the 'to_add' vector, which will be added to the entity vectors in the update() method
std::shared_ptr<Entity> Entity_Manager::add_entity(const enum e_Tag &tag)
{
	auto entity= std::shared_ptr<Entity>(new Entity(m_total_entities++, tag));

	m_entities_to_add.push_back(entity);
	
	return entity;
}

const EntityVec &Entity_Manager::get_entities()
{
	return m_entities;
}

const EntityVec &Entity_Manager::get_entities(const enum e_Tag &tag)
{
	return m_entity_map[tag];
}