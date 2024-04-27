#include "InputTextWindow.h"

void InputTextWindow::DrawInputTextWindow()
{
	int BordersWidth = 3;
	Vector2 MousePos = GetMousePosition();
	if (CheckCollisionPointRec(MousePos, Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y })) {
		BordersWidth = 5;
	};
	DrawRectangleLinesEx(Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y }, BordersWidth, BLACK);
	DrawText(text, Position.x - MeasureText(text, FontSize) / 2, Position.y - FontSize / 2, FontSize, BLACK);
	DrawText(Description, Position.x - MeasureText(Description, DescriptionFontSize) - Size.x / 2 - 10, Position.y - DescriptionFontSize / 2, DescriptionFontSize, BLACK);
}
