#ifndef CUTSCENE_PLAYER_TELEPORT_H
#define CUTSCENE_PLAYER_TELEPORT_H

#include "Cutscenes.h"
#include "SFML/Graphics.hpp"


class CutscenePlayerTeleport : public CutsceneInstruction
{
private:
	sf::Vector2f m_pos;

public:
	CutscenePlayerTeleport(sf::Vector2f _position);
	void Init() override;
	bool InstructionDone() override;
};

#endif // !
