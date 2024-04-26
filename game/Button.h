#pragma once
#include "UsefulStuff.h"


class Button
{
public:
	Vector2 Position;		// Position of the center of the button
	Vector2 Size;
	const char* text;
	int FontSize;
	Color color;

	Button(const Vector2& _p, const Vector2& _d, const char* _text, int _fs, Color _c) : Position(_p), Size(_d), text(_text), FontSize(_fs), color(_c) {};

	void DrawButton();
	bool IsPressed();


	};
