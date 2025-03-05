#include "CutscenePlayerTeleport.h"
#include "common.h"
#include "CharacterController.h"

CutscenePlayerTeleport::CutscenePlayerTeleport(sf::Vector2f _position)
{
	m_pos = _position;
}

void CutscenePlayerTeleport::Init()
{
	CharaControl* chara = GameData::CharaCtrl;
	chara->SetPos(m_pos);
}

bool CutscenePlayerTeleport::InstructionDone()
{
	return true;
}
