#include "MapGenerator.h"


// Функции для работы с нодами
int MapGenerator::make_node(int i, int j, int index)
{
	return index + 4 * j + 4 * m * i;
}

int MapGenerator::get_i(int node)
{
	return (node - get_index(node) - 4 * get_j(node)) / (4 * m);
}

int MapGenerator::get_j(int node)
{
	return ((node - get_index(node)) % (4 * m)) / 4;
}

int MapGenerator::get_index(int node)
{
	return node % 4;
}

void MapGenerator::dfs(int v, std::map<int, std::set<int>>& graph, std::set<int>& component, std::vector<bool>& used)
{
	used[v] = true;
	component.insert(v);
	for (int u : graph[v]) {
		if (!used[u]) {
			dfs(u, graph, component, used);
		}
	}
}


MapGenerator::MapGenerator(int a, int b)
{
	// Подгрузили тайлы
	std::srand(std::time(nullptr));
	std::rand(); // Нужно чтобы избавиться от 1го не очень-то рандомного числа
	loadTiles();

	// Инициализировали клетки
	n = a;
	m = b;
	map.resize(n, std::vector<Cell>(m));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			map[i][j] = Cell(i, j);
		}
	}

	GenerateMap();
}

void MapGenerator::loadTiles()
{
	// Временная затычка, потом будем грузить текстуры (Нет ничего более вечного чем временное решение)
	Tiles.push_back(Tile('f', 30, 0, 0, 0, 0));
	Tiles.push_back(Tile('a', 10, 1, 0, 0, 0));
	Tiles.push_back(Tile('b', 30, 1, 0, 1, 0));
	Tiles.push_back(Tile('c', 4, 0, 1, 1, 1));
	Tiles.push_back(Tile('d', 10, 1, 1, 0, 0));
	Tiles.push_back(Tile('e', 1, 1, 1, 1, 1));

	// А тут вращаем базовые тайлы, чтобы получить все возможные комбинации
	int base_tiles_count = Tiles.size();
	for (int i = 0; i < base_tiles_count; i++) {
		Tile _u = Tiles[i];
		Tile _r = Tiles[i].rotateSelf(1);
		Tile _d = Tiles[i].rotateSelf(2);
		Tile _l = Tiles[i].rotateSelf(3);
		if (_r != _u) {
			Tiles.push_back(Tile(_r));
		}
		if (_d != _u && _d != _r) {
			Tiles.push_back(Tile(_d));
		}
		if (_l != _u && _l != _r && _l != _d) {
			Tiles.push_back(Tile(_l));
		}
	}

	Tiles.shrink_to_fit();
}

// Очень перегруженный метод, может стоит разбить?
void MapGenerator::GenerateMap()
{
	// Генерируем клетки по правилам
	for (int step = 0; step < n * m; step++) {
		// Сначала находим клетку с наименьшей энтропией ( числом опций, минимум это 1, поэтому если нашли такую то пойдет)
		int min_num_opt = Tiles.size() + 2;
		Cell* cellToCollapse = &map[0][0];
		bool foundAbsMin = false;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				if (map[i][j].options.size() < min_num_opt && !map[i][j].isCollapsed) {
					cellToCollapse = &map[i][j];
					min_num_opt = map[i][j].options.size();
					if (map[i][j].options.size() == 1) {
						foundAbsMin = true;
						break;
					}
				}
			}
			if (foundAbsMin) {
				break;
			}
		}

		// Коллапсируем
		cellToCollapse->Collapse();

		// Уведомляем всех соседей об обновлении
		int x = cellToCollapse->posx;
		int y = cellToCollapse->posy;

		if (0 <= x - 1) {
			map[x - 1][y].Update(1, *cellToCollapse);
		}

		if (x + 1 < n) {
			map[x + 1][y].Update(3, *cellToCollapse);
		}

		if (0 <= y - 1) {
			map[x][y - 1].Update(0, *cellToCollapse);
		}

		if (y + 1 < m) {
			map[x][y + 1].Update(2, *cellToCollapse);
		}
	}


	// Мы ходим чтобы в карте небыло замкнутых фигур, поэтому мы должны находить и ломать преграды
	// Создадим граф, в котором каждый тайл разбит на 4 части (по углам изначального). 
	// Узлы связаны, если между ними нету стен
	// Узлы представляют собой число вида node = index + 4 * j + 4 * m * i
	// i, j - положение соотв. клетки на карте, index - положение внутри клетки:
	// |0 1|
	// |3 2|
	// У нас могут быть "пустые" узлы, т.к. это ничего не ломает и упрощает границы
	int num_nodes = n * m * 4;
	std::map<int, std::set<int>> graph;
	std::map<int, std::set<int>> neighbourhood;
	std::vector<std::set<int>> components;
	std::vector<bool> used;
	std::vector<int> wallsToBreak;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			Tile cell = Tiles[map[i][j].finalTileIndex];
			for (int index = 0; index < 4; index++) {
				// Добавили соседей по клетке
				if(cell.sidesStates[index] == 0) graph[make_node(i, j, index)].insert(make_node(i, j, (index + 1) % 4));
				if(cell.sidesStates[(index + 3) % 4] == 0) graph[make_node(i, j, index)].insert(make_node(i, j, (index + 3) % 4));
				neighbourhood[make_node(i, j, index)].insert(make_node(i, j, (index + 1) % 4));
				neighbourhood[make_node(i, j, index)].insert(make_node(i, j, (index + 3) % 4));
			}
			// Добавили из соседних клеток (проще влоб записать всё) (это точно имеет смысл, рисуй)
			if (i - 1 >= 0) graph[make_node(i, j, 0)].insert(make_node(i - 1, j, 1));
			if (j - 1 >= 0) graph[make_node(i, j, 0)].insert(make_node(i, j-1, 3));
			if (j - 1 >= 0) graph[make_node(i, j, 1)].insert(make_node(i, j-1, 2));
			if (i + 1 < n) graph[make_node(i, j, 1)].insert(make_node(i+1, j, 0));
			if (i + 1 < n) graph[make_node(i, j, 2)].insert(make_node(i+1, j, 3));
			if (j + 1 < m) graph[make_node(i, j, 2)].insert(make_node(i, j+1, 1));
			if (j + 1 < m) graph[make_node(i, j, 3)].insert(make_node(i, j+1, 0));
			if (i - 1 >= 0) graph[make_node(i, j, 3)].insert(make_node(i-1, j, 2));
		}
	}

	// Теперь проходимся по графу и ищем компоненты связности
	for (int v = 0; v < num_nodes; v++) {
		used.push_back(false);
	}
	for (int v = 0; v < num_nodes; v++) {
		if (!used[v]) {
			components.push_back(std::set<int>());
			dfs(v, graph, components.back(), used);
		}
	}
	for (int i = 0; i < components.size(); i++) {
		for (int node : components[i])
		{
			std::cout << node << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	// А теперь нужно связать все компоненты связности в одну большую компоненту связности
	// (Очень связный текст)
	bool* united = new bool[components.size()];
	for (int comp_index = 0; comp_index < components.size() - 1; comp_index++) {

		for (int diff_comp = 0; diff_comp < components.size(); diff_comp++) {
			united[diff_comp] = false;
		}
		
		for (int node : components[comp_index]) {
			for (int neighbour : neighbourhood[node]) {
				for (int diff_comp = comp_index + 1; diff_comp < components.size(); diff_comp++) {
					if (components[diff_comp].count(neighbour) == 1 && !united[diff_comp]) {
						united[diff_comp] = true;
						wallsToBreak.push_back(node < neighbour ? node : neighbour);
					}
				}
			}
		}
	}
	delete [] united;

	// Ура, у нас есть список стен подлежащих сносу, дело осталось за малым
	for (int node : wallsToBreak) {
		int i = get_i(node);
		int j = get_j(node);
		int side = get_index(node);

		map[i][j].finalTileIndex = 0;
	}



	// Переводим клетки в более реальные прямоугольники
	float cell_width = screenWidth / n;
	float cell_height = screenHeight / m;
	float line_width = (screenHeight / n + screenWidth / m) / 20;

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			Tile cell = Tiles[map[i][j].finalTileIndex]; // Глубоко...
			if (cell.sidesStates[0]) {
				RealMap.push_back(Rectangle{ float(i + 0.5) * cell_width - line_width / 2, float(j) * cell_height, line_width, cell_height / 2 + line_width / 2 });
			}
			if (cell.sidesStates[1]) {
				RealMap.push_back(Rectangle{ float(i + 0.5) * cell_width, float(j + 0.5) * cell_height - line_width / 2, cell_width / 2 + line_width / 2, line_width });
			}
			if (cell.sidesStates[2]) {
				RealMap.push_back(Rectangle{ float(i + 0.5) * cell_width - line_width / 2, float(j + 0.5) * cell_height - line_width / 2, line_width, cell_height / 2 + line_width / 2 });
			}
			if (cell.sidesStates[3]) {
				RealMap.push_back(Rectangle{ float(i) * cell_width, float(j + 0.5) * cell_height - line_width / 2, cell_width / 2, line_width });
			}
		}
	}
}

void MapGenerator::degug()
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			map[i][j].print();
		}
	}
}


// Зона Тайлов
MapGenerator::Tile::Tile(char i, double w, int up, int right, int down, int left)
{
	icon = i;
	weight = w;
	sidesStates[0] = up;
	sidesStates[1] = right;
	sidesStates[2] = down;
	sidesStates[3] = left;
}

MapGenerator::Tile MapGenerator::Tile::rotateSelf(int n) const
{
	n = n % 4;
	return MapGenerator::Tile(icon, weight, sidesStates[(4-n) % 4], sidesStates[(5 - n) % 4], sidesStates[(6 - n) % 4], sidesStates[(7 - n) % 4]);
}

bool MapGenerator::Tile::operator==(const Tile& other) const
{
	for (int i = 0; i < 4; i++) {
		if (sidesStates[i] != other.sidesStates[i]) {
			return false;
		}
	}
	return true;
}

bool MapGenerator::Tile::operator!=(const Tile& other) const
{
	return !(*this == other);
}

void MapGenerator::Tile::print() const
{
	for (int s : sidesStates) {
		std::cout << s << " ";
	}
}




// Зона для Клетки

MapGenerator::Cell::Cell()
{
	isCollapsed = false;
	finalTileIndex = -1;
	posx = 0;
	posy = 0;
}

MapGenerator::Cell::Cell(int x, int y)
{
	isCollapsed = false;
	finalTileIndex = -1;
	posx = x;
	posy = y;

	for (int i = 0; i < globalMapGenerator.Tiles.size(); i++) {
		options.emplace(i);
	}
}

//Полезная функция, хз куда именно её лучше запихать
int MapGenerator::Cell::pickRandom() {
	double norm = 0;
	std::vector<double> optionsWeights;
	std::vector<int> IhateOptimizedMemoryUse;
	for (int tileIndex : options) {
		norm += globalMapGenerator.Tiles[tileIndex].weight;
		IhateOptimizedMemoryUse.push_back(tileIndex);
	}

	// Теперь всё нормируем на 1
	optionsWeights.push_back(globalMapGenerator.Tiles[0].weight/norm);
	for (int tileIndex : options) {
		if (tileIndex == *options.begin()) continue;
		optionsWeights.push_back(globalMapGenerator.Tiles[tileIndex].weight / norm + optionsWeights[optionsWeights.size() - 1]);
	}

	// Генерируем число от 0 до 1 и ищем первый индекс который будет больше него
	double rnd = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
	for (int i = 0; i < optionsWeights.size(); i++) {
		if (rnd <= optionsWeights[i]) {
			return IhateOptimizedMemoryUse[i];
		}
	}

	return optionsWeights.size() - 1;
}

void MapGenerator::Cell::Collapse()
{
	if (isCollapsed) { std::cout << "WTF?!"; return; }

	int pickedTileIndex = pickRandom();
	options.clear();
	finalTileIndex = pickedTileIndex;
	isCollapsed = true;

	//print();
}

void MapGenerator::Cell::Update(int side, Cell& neighbour)
{
	std::set<int> newOptions;
	// Состояние которое мы ожидаем на этой стороне клетки должно совпасть с состоянием соотв. стороны сколапсированного соседа
	int expectedType = globalMapGenerator.Tiles[neighbour.finalTileIndex].sidesStates[(side + 2) % 4];
	for (int tileIndex : options) {
		if (globalMapGenerator.Tiles[tileIndex].sidesStates[side] == expectedType) {
			newOptions.emplace(tileIndex);
		}
	}

	options.clear();
	for (int tileIndex : newOptions) {
		options.emplace(tileIndex);
	}
}

void MapGenerator::Cell::CutSide(int side)
{
}

void MapGenerator::Cell::print() const
{
	if (isCollapsed) {
		std::cout << "(" << posx << ", " << posy << ") ";
		globalMapGenerator.Tiles[finalTileIndex].print();
		std::cout << std::endl;
		return;
	}

	std::cout << "(" <<posx << ", " << posy << ")\toptions: ";
	for (int tileIndex : options) {
		std::cout << globalMapGenerator.Tiles[tileIndex].icon << " ";
	}
	std::cout << std::endl;
}
