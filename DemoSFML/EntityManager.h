#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <stack>
#include <vector>
#include <list>
#include <functional>

#include "common.h"

class Entity;

typedef std::function<void(Entity*)> EntityCallback;

class EntityManager
{
protected:
	static EntityManager* Instance;

	std::stack<Entity*> m_markedForDeath;
	std::vector<Entity*> m_entities;
	std::vector<Entity*> m_waitingEntities;
	std::vector<Entity*> m_backgroundEntities;

	std::list<EntityCallback*> m_onEntityAdded;
	std::list<EntityCallback*> m_onEntityRemoved;

	float m_gameSpeedGoal = 1.f;
	float m_gameSpeedChange = 0.25f;

	bool m_updating = false;

public:
	static EntityManager* GetInstance();
	static void SetGameSpeed(float _value) { GameData::GameSpeed = _value; }
	static void SetGameSpeedGoal(float _value) { Instance->m_gameSpeedGoal = _value; }
	static void ClearCallbacks() { Instance->m_onEntityAdded.clear(); Instance->m_onEntityRemoved.clear(); }
	static void Clear() {ClearCallbacks(); Instance->m_entities.clear(); Instance->m_backgroundEntities.clear(); }

	void Add(Entity* _entity, bool _background = false);
	void MarkForDeath(Entity* _entity);
	void ExecuteMarkedForDeath();

	void Update();
	void Display();
	void DisplayBackgroundEntities();

	static void FreeMemory();

	int GetEntityCount() { return (int)m_entities.size(); }
	Entity* GetEntity(int _index);
	std::vector<Entity*>& GetEntities() { return m_entities; }

	void AddOnEntityAdded(EntityCallback* _callback) { m_onEntityAdded.push_back(_callback); }
	void AddOnEntityRemoved(EntityCallback* _callback) { m_onEntityRemoved.push_back(_callback); }
};

#endif