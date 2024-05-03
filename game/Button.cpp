#include "Button.h"


void Button::DrawButton()
{
	DrawRectangleV(Position - Vector2{Size.x / 2, Size.y/2}, Size, color);
	Vector2 MousePos = GetMousePosition();
	DrawText(text, Position.x - MeasureText(text, FontSize) / 2, Position.y - FontSize/2, FontSize, BLACK);
	if (DumbCheck(Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y }, MousePos)) {
		DrawRectangleV(Position - Vector2{ Size.x / 2, Size.y / 2 }, Vector2{ Size.x, 4 }, BLACK);
		DrawRectangleV(Position - Vector2{ Size.x / 2, Size.y / 2 }, Vector2{ 4, Size.y }, BLACK);
		DrawRectangleV(Position + Vector2{ -Size.x / 2, Size.y / 2 - 4}, Vector2{ Size.x, 4 }, BLACK);
		DrawRectangleV(Position + Vector2{ Size.x / 2 - 4, -Size.y / 2 }, Vector2{ 4, Size.y }, BLACK);
	}

}

bool Button::IsPressed() {
	Vector2 MousePos = GetMousePosition();
	if (DumbCheck(Rectangle{ Position.x - Size.x/2, Position.y - Size.y/2, Size.x, Size.y }, MousePos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		PlaySound(soundBoard[SoundButtonClick]);
		return true;
	}
	return false;
}


