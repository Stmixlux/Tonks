#pragma once
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <ctime>
#include <raylib.h>
#include "UsefulStuff.h"

/// <summary>
/// Использую метод коллапса волновой функции для генерации карты
/// </summary>
class MapGenerator
{

	// Тайл чисто для генерации, не путать с тайлами из карты
	struct Tile {
		char icon;
		double weight;
		// Число показывает чем заканчивается тайл в этой стороне (0 - ничего, 1 - стена)
		// Стороны идут по часовой стрелке начиная сверху (0 - верх, 1 - право, 2 - низ, 3 - лево)
		int sidesStates[4];

		Tile(char i, double weight, int up, int right, int down, int left);

		// поворот себя n раз по 90
		Tile rotateSelf(int n) const;

		bool operator==(const Tile& other) const;
		bool operator!=(const Tile& other) const;
		void print() const;

	};

	// Клетка на карте, которая может быть сгенерирована
	struct Cell {
		bool isCollapsed;
		int finalTileIndex;
		std::set<int> options;
		int posx, posy;

		// Не рекомендуется к использованию (?)
		Cell(); 
		Cell(int x, int y);

		int pickRandom();
		void Collapse();
		void Update(int side, Cell& neighbour);
		void CutSide(int side);

		void print() const;
	};

	// Набор функций нужный для полировки генерации при удалении некоторых стен
	int make_node(int i, int j, int index);
	int get_i(int node);
	int get_j(int node);
	int get_index(int node);
	void dfs(int v, std::map<int, std::set<int>>& graph, std::set<int>& component, std::vector<bool>& used);


public:
	// Карта
	int n, m; // Размеры
	std::vector<std::vector<Cell>> map;

	// Таблица сверки индекс - тайл
	std::vector<Tile> Tiles;

	// Итоговая карта - набор прямоугольников
	std::vector<Rectangle> RealMap;


	MapGenerator(int a, int b);
	void loadTiles();
	void GenerateMap();

	//Заставите делать что угодно, специально для дебага
	void degug();
};

extern MapGenerator globalMapGenerator;