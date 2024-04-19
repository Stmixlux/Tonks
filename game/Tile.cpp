#include "Tile.h"

void Tile::Draw()
{
	float cellWidth = screenWidth / XCellCount;
	float cellHeight = screenHeight / YCellCount;
	float lineWidth = (screenHeight / XCellCount + screenWidth / YCellCount) / 20;

	if (sidesStates[0]) {
		DrawRectangle(float(x + 0.5) * cellWidth - lineWidth / 2, float(y) * cellHeight, lineWidth, cellHeight / 2 + lineWidth / 2, BLACK);
	}
	if (sidesStates[1]) {
		DrawRectangle(float(x + 0.5) * cellWidth, float(y + 0.5) * cellHeight - lineWidth / 2, cellWidth / 2 + lineWidth / 2, lineWidth, BLACK);
	}
	if (sidesStates[2]) {
		DrawRectangle(float(x + 0.5) * cellWidth - lineWidth / 2, float(y + 0.5) * cellHeight - lineWidth / 2, lineWidth, cellHeight / 2 + lineWidth / 2, BLACK);
	}
	if (sidesStates[3]) {
		DrawRectangle(float(x) * cellWidth, float(y + 0.5) * cellHeight - lineWidth / 2, cellWidth / 2, lineWidth, BLACK);
	}
}

Tile::Tile()
{
	x = -1;
	y = -1;
	for (int k = 0; k < 4; k++) {
		sidesStates[k] = -1;
	}
}

Tile::Tile(int i, int j, int* states)
{
	x = i;
	y = j;
	for (int k = 0; k < 4; k++) {
		sidesStates[k] = states[k];
	}
}

void Tile::Update(int i, int j, int* states)
{
	x = i;
	y = j;
	for (int k = 0; k < 4; k++) {
		sidesStates[k] = states[k];
	}
}

void Tile::getRectangles(std::vector<Rectangle>& vec)
{

	float cellWidth = screenWidth / XCellCount;
	float cellHeight = screenHeight / YCellCount;
	float lineWidth = (screenHeight / XCellCount + screenWidth / YCellCount) / 20;

	if (sidesStates[0]) {
		vec.push_back(Rectangle{ float(x + 0.5) * cellWidth - lineWidth / 2, float(y) * cellHeight, lineWidth, cellHeight / 2 + lineWidth / 2 });
	}
	if (sidesStates[1]) {
		vec.push_back(Rectangle{ float(x + 0.5) * cellWidth, float(y + 0.5) * cellHeight - lineWidth / 2, cellWidth / 2 + lineWidth / 2, lineWidth });
	}
	if (sidesStates[2]) {
		vec.push_back(Rectangle{ float(x + 0.5) * cellWidth - lineWidth / 2, float(y + 0.5) * cellHeight - lineWidth / 2, lineWidth, cellHeight / 2 + lineWidth / 2 });
	}
	if (sidesStates[3]) {
		vec.push_back(Rectangle{ float(x) * cellWidth, float(y + 0.5) * cellHeight - lineWidth / 2, cellWidth / 2, lineWidth });
	}
}
