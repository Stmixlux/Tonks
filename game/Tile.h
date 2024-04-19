#pragma once
#include "raylib.h"
#include "UsefulStuff.h"
#include <vector>

class Tile
{
public:
	int x, y;
	int sidesStates[4];

	void Draw();
	// God forbid
	Tile();
	Tile(int i, int j, int* states);
	void Update(int i, int j, int* states);
	void getRectangles(std::vector<Rectangle>& vec);
};

