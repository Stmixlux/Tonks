#pragma once
#include "UsefulStuff.h"

class InputTextWindow
{
public:
	Vector2 Position;		// Position of the center of the button
	Vector2 Size;
	char text[30]{' '};
	const char* Description;

	int FontSize = 30;
	int DescriptionFontSize;
	bool IsActive = false;
	int TextLen = 0;

	InputTextWindow(const Vector2& _p, const Vector2& _d, const char* _desc, int _fs) : Position(_p), Size(_d), Description(_desc), DescriptionFontSize(_fs){};

	void DrawInputTextWindow();
	void UpdateState();
	void UpdateText();

	std::string GetIp();
	int GetPort();
};

