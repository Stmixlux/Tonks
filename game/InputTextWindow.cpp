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

void InputTextWindow::UpdateState()
{
	Vector2 MousePos = GetMousePosition();
	if (CheckCollisionPointRec(MousePos, Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		IsActive = true;
		SetMouseCursor(MOUSE_CURSOR_IBEAM);
	}
	else if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(MousePos, Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y }) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))) {
		IsActive = false;
		SetMouseCursor(MOUSE_CURSOR_DEFAULT);
	}
}

std::string InputTextWindow::GetIp()
{
	if (TextLen != 0) {
		std::string IP_port{ text };
		if (IP_port.find(':') < 30) {
			return IP_port.substr(0, IP_port.find(':'));
		}
	}
	return std::string("0");
}

int InputTextWindow::GetPort()
{
	if (TextLen != 0) {
		std::string IP_port{ text };
		if (IP_port.find(':') < 30 && IP_port[IP_port.find(':') + 1] != '\0') {
			return stoi(IP_port.substr(IP_port.find(':') + 1, IP_port.find('\0')));
		}
	}
	return 0;
	
}


void InputTextWindow::UpdateText()
{
	if (IsActive) {
		int key = GetKeyPressed();
		if (key > 0 && TextLen < 23) {
			if (IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_SEMICOLON)) {
				text[TextLen] = ':';
				text[TextLen + 1] = '\0';
				TextLen++;
			}
			else if (key == 46 || (key >= 48 && key <= 57)) {
				text[TextLen] = (char)key;
				text[TextLen + 1] = '\0';
				TextLen++;
			}
			int key = GetKeyPressed();
		}
		if (IsKeyPressed(KEY_BACKSPACE))
		{
			TextLen--;
			if (TextLen < 0) TextLen = 0;
			text[TextLen] = '\0';
		}
	}
}

