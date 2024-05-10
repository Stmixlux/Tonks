#include "Switch.h"

void Switch::DrawSwitch()
{
	this->DrawButton();
	DrawText(Function, Position.x - MeasureText(Function, FunctionFontSize) - Size.x/2 - 10, Position.y - FunctionFontSize / 2, FunctionFontSize, BLACK);
}

void Switch::UpdateSwitch()
{
	Vector2 MousePos = GetMousePosition();
	if (DumbCheck(Rectangle{ Position.x - Size.x / 2, Position.y - Size.y / 2, Size.x, Size.y }, MousePos) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
		if (state == 0) {
			PlaySound(soundBoard[SoundButtonClick]);
			state = 1;
			text = "ON";
			color = GREEN;
		}
		else { 
			PlaySound(soundBoard[SoundButtonClick]);
			state = 0; 
			text = "OFF";
			color = RED;
		}
	}
}

void Switch::ChangeState()
{
	if (state == 0) {
		state = 1;
		text = "ON";
		color = GREEN;
	}
	else {
		state = 0;
		text = "OFF";
		color = RED;
	}
}

int Switch::GetState() {
	return state;
}
