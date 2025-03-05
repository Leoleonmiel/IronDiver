#ifndef CHARACTER_STATS_H
#define CHARACTER_STATS_H

#include <string>
#include <SFML/Graphics.hpp>

struct GrabHookStats;

class CharacterStats
{
public:
	CharacterStats(std::string, std::string );
	void Update();
	float GetMidAirMaxSpeed() const { return m_midAirMaxSpeed; }
	float GetMidAirSpeed() const { return m_midAirSpeed; }
	float GetJumpStrenght() const { return m_jumpStrenght; }
	float GetJumpMinTime() const { return m_jumpMinTime; }
	float GetJumpMaxTime() const { return m_jumpMaxTime; }
	float GetSpeed() const { return m_walkSpeed; }
	float GetMaxSpeed() const { return m_maxWalkSpeed; }
	int GetMaxHp() const { return m_maxHp; }
	int GetHp() const { return m_currentHp; }
	int GetDamage() const { return m_damage; }
	int GetWalkBoostLevel() const { return m_walkBoostLevel; }
	int GetDamageBoostLevel() const { return m_damageBoostLevel; }
	bool CanUseSpecialSpell() const { return m_canUseSpecialSpell; }
	GrabHookStats* GetGrabHookStats() const { return m_grabHookStats; }

	void SetSpeed(float _speed);
	void SetMaxHp(int _maxHp);
	void SetWalkBoostLevel(int _level);
	void SetCanUseSpecialSpell(bool _value);
	void SetDamageBoostLevel(int _level);
	void SetHp(int _hp);
	void AddHealth(int _hp);
	void AddDamage(int _hp);

	void LoadStatsJSON(std::string _pathFile);
	void LoadBoostJSON(std::string _pathFile);


private:
	friend class CharaControl;

	int m_maxHp = 0;
	int m_damage = 0;

	float m_attackSideSpeed = 0.f;
	float m_attackUpDuration = 0.f;
	float m_attackDownDuration = 0.f;
	float m_attackCooldown = 0.f;

	float m_friction = 0.f;
	float m_density = 0.f;

	float m_walkSpeed = 0.f;
	float m_maxWalkSpeed = 50.f;
	float m_jumpMinTime = 0.f;
	float m_jumpMaxTime = 0.f;
	float m_jumpMaxVelocity = 0.f;
	float m_jumpStrenght = 0.f;
	float m_midAirSpeed = 0.f;
	float m_midAirMaxSpeed = 0.f;
	float m_attackImpulse = 0.f;

	sf::Vector2f m_coreSize;
	sf::Vector2f m_headSize;
	sf::Vector2f m_feetSize;
	sf::Vector2f m_wallSensorSize;
	sf::Vector2f m_attackSize;

	GrabHookStats* m_grabHookStats = nullptr;

	std::vector<std::string> m_shadowWhiteList;

	int m_walkBoostLevel = 0;
	int m_damageBoostLevel = 0;
	bool m_canUseSpecialSpell = false;
	
	int m_currentHp = 0;
};

#endif // CHARACTER_H
