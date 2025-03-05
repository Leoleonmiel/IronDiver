#ifndef DIALOG_MODULE_H
#define DIALOG_MODULE_H

#include "SFML/System/Clock.hpp"
#include <string>
class GUI;

class Dialog
{
public:
	std::string name;
	std::string text;

	float speed = 0.f;
	bool instant = false;
};

class DialogMaster
{
public:
	static void InitialiseDialogMaster();

	static void SetEnabled(bool _enabled);

	static bool TrySetDialog(Dialog& _dialog);
	static void UpdateDialog();
	static void PassDialog();
	static bool GetDialogRunning()
	{
		return m_dialogRunning;
	}
	static bool DialogActive();
	static Dialog* GetCurrentDialog() { return m_currentDialog; }
private:
	static float m_clock;
	static Dialog* m_currentDialog;
	static GUI* m_dialogPannel;

	static bool m_dialogActive;
	static bool m_dialogRunning;
};

#endif // !DIALOG_MODULE_H
