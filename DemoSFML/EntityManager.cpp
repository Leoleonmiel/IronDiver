#include "EntityManager.h"
#include "Entity.h"
#include "common.h"
#include "CharacterController.h"
#include "GrabHook.h"
#include "Box2D_Shape.h"

EntityManager* EntityManager::Instance = nullptr;

EntityManager* EntityManager::GetInstance()
{
	if (Instance == nullptr)
	{
		Instance = new EntityManager();
	}

	return Instance;
}

void EntityManager::Add(Entity* _entity, bool _background)
{
	if (!_background)
	{
		if (m_updating)
		{
			m_waitingEntities.push_back(_entity);
		}
		else
		{
			m_entities.push_back(_entity);
		}
		
		for (auto it = m_onEntityAdded.begin(); it != m_onEntityAdded.end(); it++)
		{
			(*(*it))(_entity);
		}
	}
	else
	{
		m_backgroundEntities.push_back(_entity);
	}
}

void EntityManager::MarkForDeath(Entity* _entity)
{

	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		if (*it == _entity)
		{
			for (auto it = m_onEntityRemoved.begin(); it != m_onEntityRemoved.end(); it++)
			{
				(*(*it))(_entity);
			}
			m_markedForDeath.push(*it);
			return;
		}
	}
}

void EntityManager::ExecuteMarkedForDeath()
{
	Entity* tempPtr;

	while (!m_markedForDeath.empty())
	{
		tempPtr = m_markedForDeath.top();

		for (int i = 0; i < m_entities.size(); i++)
		{
			if (m_entities[i] == tempPtr)
			{
				m_entities.erase(m_entities.begin() + i);
				
				i = (int)m_entities.size();
			}
		}

		delete tempPtr;
		tempPtr = nullptr;
		m_markedForDeath.pop();
	}
}

void EntityManager::Update()
{
	m_updating = true;
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		(*it)->Update();
	}

	m_updating = false;

	for (auto it = m_waitingEntities.begin(); it != m_waitingEntities.end(); it++)
	{
		m_entities.push_back(*it);
	}

	m_waitingEntities.clear();

	for (auto it = m_backgroundEntities.begin(); it != m_backgroundEntities.end(); it++)
	{
		(*it)->Update();
	}
}

void EntityManager::Display()
{
	for (auto it = m_entities.begin(); it != m_entities.end(); it++)
	{
		(*it)->Display();
	}
}

void EntityManager::DisplayBackgroundEntities()
{
	for (auto it = m_backgroundEntities.begin(); it != m_backgroundEntities.end(); it++)
	{
		(*it)->Display();
	}
}

void EntityManager::FreeMemory()
{
	delete Instance;
	Instance = nullptr;
}

Entity* EntityManager::GetEntity(int _index)
{
	if (m_entities.size() <= _index)
	{
		return nullptr;
	}

	return m_entities[_index];
}
