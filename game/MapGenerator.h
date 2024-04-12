#pragma once
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <ctime>
#include <raylib.h>
#include "UsefulStuff.h"

/// <summary>
/// ��������� ����� �������� �������� ������� ��� ��������� �����
/// </summary>
class MapGenerator
{

	// ���� ����� ��� ���������, �� ������ � ������� �� �����
	struct Tile {
		char icon;
		double weight;
		// ����� ���������� ��� ������������� ���� � ���� ������� (0 - ������, 1 - �����)
		// ������� ���� �� ������� ������� ������� ������ (0 - ����, 1 - �����, 2 - ���, 3 - ����)
		int sidesStates[4];

		Tile(char i, double weight, int up, int right, int down, int left);

		// ������� ���� n ��� �� 90
		Tile rotateSelf(int n) const;

		bool operator==(const Tile& other) const;
		bool operator!=(const Tile& other) const;
		void print() const;

	};

	// ������ �� �����, ������� ����� ���� �������������
	struct Cell {
		bool isCollapsed;
		int finalTileIndex;
		std::set<int> options;
		int posx, posy;

		// �� ������������� � ������������� (?)
		Cell(); 
		Cell(int x, int y);

		int pickRandom();
		void Collapse();
		void Update(int side, Cell& neighbour);
		void CutSide(int side);

		void print() const;
	};

	// ����� ������� ������ ��� ��������� ��������� ��� �������� ��������� ����
	int make_node(int i, int j, int index);
	int get_i(int node);
	int get_j(int node);
	int get_index(int node);
	void dfs(int v, std::map<int, std::set<int>>& graph, std::set<int>& component, std::vector<bool>& used);


public:
	// �����
	int n, m; // �������
	std::vector<std::vector<Cell>> map;

	// ������� ������ ������ - ����
	std::vector<Tile> Tiles;

	// �������� ����� - ����� ���������������
	std::vector<Rectangle> RealMap;


	MapGenerator(int a, int b);
	void loadTiles();
	void GenerateMap();

	//��������� ������ ��� ������, ���������� ��� ������
	void degug();
};

extern MapGenerator globalMapGenerator;