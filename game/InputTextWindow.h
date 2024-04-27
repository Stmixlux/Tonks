#pragma once
#include "UsefulStuff.h"

class InputTextWindow
{
public:
	Vector2 Position;		// Position of the center of the button
	Vector2 Size;
	const char* text = "\0";
	const char* Description;

	int FontSize = 14;
	int DescriptionFontSize;

	InputTextWindow(const Vector2& _p, const Vector2& _d, const char* _desc, int _fs) : Position(_p), Size(_d), Description(_desc), DescriptionFontSize(_fs){};

	void DrawInputTextWindow();
	void UpdateText();


};

