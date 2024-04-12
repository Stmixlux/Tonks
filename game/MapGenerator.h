#pragma once
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <ctime>
#include <raylib.h>
#include "UsefulStuff.h"

/// <summary>
/// Using wave function collapse to generate map
/// </summary>
class MapGenerator
{
	// Hold information about structure of a Tile (not to confuse with Tile from the real Map (if they ever appear))
	struct Tile {
		char icon;
		double weight;

		// Number shows what is on the end of this side of Tile (0 - void, 1 - wall)
		// Sides go in clock-order from 0 to 3 (0 - up, 1 - right, 2 - down, 3 - left)
		int sidesStates[4];

		Tile(char i, double weight, int up, int right, int down, int left);

		// Return copy of itself rotated by 90deg. n times
		Tile rotateSelf(int n) const;

		bool operator==(const Tile& other) const;
		bool operator!=(const Tile& other) const;
		void print() const;

	};

	// Cell on map, representing current state of the map-tile. Should only be used during map generation
	struct Cell {
		bool isCollapsed;
		int finalTileIndex;
		std::set<int> options;
		int posx, posy;
		MapGenerator* generator;

		// DO NOT, UNDER ANY CIRCUMSTANCES, USE IT (breaks the state) (?)
		Cell(); 
		Cell(int x, int y, MapGenerator* gen);

		int pickRandom() const;
		void Collapse();
		void Update(int side,const Cell& neighbour);

		void print() const;
	};

	// Used to polish generation by deleting some extra walls
	int makeNode(int i, int j, int index);
	// Used snake case, to differentiate i and j
	int get_i(int node);
	int get_j(int node);
	int get_index(int node);
	void dfs(int v, std::map<int, std::set<int>>& graph, std::set<int>& component, std::vector<bool>& used);

	// Map size
	int n, m;
	std::vector<std::vector<Cell>> cellMap;

	// Reference Table for indexTile <-> Tile
	std::vector<Tile> Tileset;

	void loadTiles();
	void clearMap();
	void generateMap();
	void pruneExtraWalls();

public:

	// Real gameMap (for now just rectangles)
	std::vector<Rectangle> map;
	MapGenerator(int a, int b);

	void regenerateMap();

	//Here it comes, the one, the only - degug
	void degug() const;
};