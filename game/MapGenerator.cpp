#include "MapGenerator.h"

MapGenerator::MapGenerator(int a, int b)
{
	std::srand(std::time(nullptr));
	std::rand(); // Used to get rid of first not-so-random number

	loadTiles();

	// Initialize Cells-Map
	n = a;
	m = b;
	clearMap();
	generateMap();
}

void MapGenerator::regenerateMap()
{
	clearMap();
	generateMap();
}

void MapGenerator::Draw()
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			map[i][j].Draw();
		}
	}
}

Tile MapGenerator::getTileAt(Vector2 position)
{
	int x = (int)(position.x / screenWidth * n);
	int y = (int)(position.y / screenHeight * m);
	return map[x][y];
}

std::vector<Rectangle> MapGenerator::getNeighbourhoodRect(Vector2 position)
{
	std::vector<Rectangle> res{ Rectangle{-10, -1, 15, screenHeight + 2}, Rectangle{screenWidth - 5, -1, 10.1, screenHeight + 2},
		Rectangle{-1, -10, screenWidth + 2, 15}, Rectangle{-1, screenHeight-5, screenWidth + 2, 10.1} }; // Боковые стены
	int x = (int)(position.x / screenWidth * n);
	int y = (int)(position.y / screenHeight * m);
	map[x][y].getRectangles(res);

	if (0 <= x - 1)	map[x - 1][y].getRectangles(res);
	if (x + 1 < n)	map[x + 1][y].getRectangles(res);
	if (0 <= y - 1)	map[x][y - 1].getRectangles(res);
	if (y + 1 < m)	map[x][y + 1].getRectangles(res);
	if (0 <= x - 1 && 0 <= y - 1)	map[x - 1][y - 1].getRectangles(res);
	if (0 <= x - 1 && y + 1 < m)	map[x - 1][y + 1].getRectangles(res);
	if (x + 1 < n && y + 1 < m)		map[x + 1][y + 1].getRectangles(res);
	if (x + 1 < n && 0 <= y - 1)	map[x + 1][y - 1].getRectangles(res);

	return res;
}

void MapGenerator::loadTiles()
{
	// Temporary solution, supposed to load textures down the road (There's nothing more eternal than temporary solution)
	// Load base Tiles (configuration of 0/1 sides and their's generation probobalities (weights))
	Tileset.push_back(GenerationTile('f', 30, 0, 0, 0, 0));
	Tileset.push_back(GenerationTile('a', 10, 1, 0, 0, 0));
	Tileset.push_back(GenerationTile('b', 30, 1, 0, 1, 0));
	Tileset.push_back(GenerationTile('c', 4, 0, 1, 1, 1));
	Tileset.push_back(GenerationTile('d', 10, 1, 1, 0, 0));
	Tileset.push_back(GenerationTile('e', 1, 1, 1, 1, 1));

	// Rotate base Tiles to get all possible configurations
	int base_tiles_count = Tileset.size();
	for (int i = 0; i < base_tiles_count; i++) {
		GenerationTile _u = Tileset[i];
		GenerationTile _r = Tileset[i].rotateSelf(1);
		GenerationTile _d = Tileset[i].rotateSelf(2);
		GenerationTile _l = Tileset[i].rotateSelf(3);
		if (_r != _u) {
			Tileset.push_back(GenerationTile(_r));
		}
		if (_d != _u && _d != _r) {
			Tileset.push_back(GenerationTile(_d));
		}
		if (_l != _u && _l != _r && _l != _d) {
			Tileset.push_back(GenerationTile(_l));
		}
	}

	Tileset.shrink_to_fit();
}

void MapGenerator::clearMap()
{
	map.resize(n, std::vector<Tile>(m)); // This helps not to break this attrocity of vector<vector<>>
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			map[i][j] = Tile(i, j, Tileset[0].sidesStates);
		}
	}

	cellMap.resize(n, std::vector<Cell>(m)); // This helps not to break this attrocity of vector<vector<>>
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cellMap[i][j] = Cell(i, j, this);
		}
	}
}

void MapGenerator::generateMap()
{

	// Generate non-contradictory map
	for (int step = 0; step < n * m; step++) {
		// Find the cell with the least options
		int minOptions = Tileset.size() + 2;
		Cell* cellToCollapse = &cellMap[0][0];
		bool foundAbsMin = false;
		for (int i = 0; i < n; i++) {
			for (int j = 0; j < m; j++) {
				if (cellMap[i][j].options.size() < minOptions && !cellMap[i][j].isCollapsed) {
					cellToCollapse = &cellMap[i][j];
					minOptions = cellMap[i][j].options.size();
					if (cellMap[i][j].options.size() == 1) {
						foundAbsMin = true;
						break;
					}
				}
			}
			if (foundAbsMin) {
				break;
			}
		}

		cellToCollapse->Collapse();

		// Update all neighbours
		int x = cellToCollapse->posx;
		int y = cellToCollapse->posy;

		if (0 <= x - 1) {
			cellMap[x - 1][y].Update(1, *cellToCollapse);
		}

		if (x + 1 < n) {
			cellMap[x + 1][y].Update(3, *cellToCollapse);
		}

		if (0 <= y - 1) {
			cellMap[x][y - 1].Update(0, *cellToCollapse);
		}

		if (y + 1 < m) {
			cellMap[x][y + 1].Update(2, *cellToCollapse);
		}
	}

	pruneExtraWalls();

	// Transform cells into Tiles (The ones from the real map)
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			map[i][j].Update(i, j, Tileset[cellMap[i][j].finalTileIndex].sidesStates);
		}
	}
}

void MapGenerator::pruneExtraWalls()
{
	// We want to get rid of all encloced areas of map, therefore we need to find some key-walls and remove them
	// We create graph, in which we divide each cell into 4 nodes (in corners of initial cell). 
	// Nodes are connected if there's no wall between them
	// Nodes are represented by integer as node = index + 4 * j + 4 * m * i
	// Where i, j - position of cell on map, index - position inside cell:
	// |0 1|
	// |3 2|
	int num_nodes = n * m * 4;
	std::map<int, std::set<int>> graph;
	std::map<int, std::set<int>> neighbourhood;
	std::vector<std::set<int>> components;
	std::vector<bool> used;
	std::vector<int> wallsToBreak;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			GenerationTile cell = Tileset[cellMap[i][j].finalTileIndex];
			for (int index = 0; index < 4; index++) {
				// Added node's neigbours from inside the cell
				if (cell.sidesStates[index] == 0) graph[makeNode(i, j, index)].insert(makeNode(i, j, (index + 1) % 4));
				if (cell.sidesStates[(index + 3) % 4] == 0) graph[makeNode(i, j, index)].insert(makeNode(i, j, (index + 3) % 4));
				neighbourhood[makeNode(i, j, index)].insert(makeNode(i, j, (index + 1) % 4));
				neighbourhood[makeNode(i, j, index)].insert(makeNode(i, j, (index + 3) % 4));
			}
			// And from neighbouring cells (it's easier to just write it down) (It makes sense, just draw it)
			if (i - 1 >= 0) graph[makeNode(i, j, 0)].insert(makeNode(i - 1, j, 1));
			if (j - 1 >= 0) graph[makeNode(i, j, 0)].insert(makeNode(i, j - 1, 3));
			if (j - 1 >= 0) graph[makeNode(i, j, 1)].insert(makeNode(i, j - 1, 2));
			if (i + 1 < n) graph[makeNode(i, j, 1)].insert(makeNode(i + 1, j, 0));
			if (i + 1 < n) graph[makeNode(i, j, 2)].insert(makeNode(i + 1, j, 3));
			if (j + 1 < m) graph[makeNode(i, j, 2)].insert(makeNode(i, j + 1, 1));
			if (j + 1 < m) graph[makeNode(i, j, 3)].insert(makeNode(i, j + 1, 0));
			if (i - 1 >= 0) graph[makeNode(i, j, 3)].insert(makeNode(i - 1, j, 2));
		}
	}

	// Now we go through the graph and find all components
	for (int v = 0; v < num_nodes; v++) {
		used.push_back(false);
	}
	for (int v = 0; v < num_nodes; v++) {
		if (!used[v]) {
			components.push_back(std::set<int>());
			dfs(v, graph, components.back(), used);
		}
	}

	// And now we need to unite all components by locating and eliminating few walls
	bool* united = new bool[components.size()];
	for (int comp_index = 0; comp_index < components.size() - 1; comp_index++) {

		for (int diff_comp = 0; diff_comp < components.size(); diff_comp++) {
			united[diff_comp] = false;
		}

		for (int node : components[comp_index]) {
			for (int neighbour : neighbourhood[node]) {
				for (int diff_comp = comp_index + 1; diff_comp < components.size(); diff_comp++) {
					// It shall work, don't mind vs
					if (components[diff_comp].count(neighbour) == 1 && !united[diff_comp]) {
						united[diff_comp] = true;
						wallsToBreak.push_back(node < neighbour ? node : neighbour);
					}
				}
			}
		}
	}
	delete[] united;

	// Yuppy, now all we have to do is to clear all those extra walls
	for (int node : wallsToBreak) {
		int i = get_i(node);
		int j = get_j(node);
		int side = get_index(node);

		cellMap[i][j].finalTileIndex = 0;
	}
}

// Used for debug purpuses
void MapGenerator::degug() const
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			cellMap[i][j].print();
		}
	}
}


///// Tile workshop
MapGenerator::GenerationTile::GenerationTile(char i, double w, int up, int right, int down, int left)
{
	icon = i;
	weight = w;
	sidesStates[0] = up;
	sidesStates[1] = right;
	sidesStates[2] = down;
	sidesStates[3] = left;
}

MapGenerator::GenerationTile MapGenerator::GenerationTile::rotateSelf(int n) const
{
	n = n % 4;
	return MapGenerator::GenerationTile(icon, weight, sidesStates[(4-n) % 4], sidesStates[(5 - n) % 4], sidesStates[(6 - n) % 4], sidesStates[(7 - n) % 4]);
}

bool MapGenerator::GenerationTile::operator==(const GenerationTile& other) const
{
	for (int i = 0; i < 4; i++) {
		if (sidesStates[i] != other.sidesStates[i]) {
			return false;
		}
	}
	return true;
}

bool MapGenerator::GenerationTile::operator!=(const GenerationTile& other) const
{
	return !(*this == other);
}

void MapGenerator::GenerationTile::print() const
{
	for (int s : sidesStates) {
		std::cout << s << " ";
	}
}




///// Cell workshop

MapGenerator::Cell::Cell()
{
	generator = nullptr;
	isCollapsed = false;
	finalTileIndex = -1;
	posx = 0;
	posy = 0;
}

MapGenerator::Cell::Cell(int x, int y, MapGenerator* gen)
{
	generator = gen;
	isCollapsed = false;
	finalTileIndex = -1;
	posx = x;
	posy = y;

	for (int i = 0; i < generator->Tileset.size(); i++) {
		options.emplace(i);
	}
}

// Seems useful, but not sure if it should be here or not
int MapGenerator::Cell::pickRandom() const {
	double norm = 0;
	std::vector<double> optionsWeights;
	std::vector<int> IhateOptimizedMemoryUse;
	for (int tileIndex : options) {
		norm += generator->Tileset[tileIndex].weight;
		IhateOptimizedMemoryUse.push_back(tileIndex);
	}

	// Normalize weights to sum up to 1
	optionsWeights.push_back(generator->Tileset[0].weight/norm);
	for (int tileIndex : options) {
		if (tileIndex == *options.begin()) continue;
		optionsWeights.push_back(generator->Tileset[tileIndex].weight / norm + optionsWeights[optionsWeights.size() - 1]);
	}

	// Generate number between 0 and 1 and take the first index which weight is bigger than that
	double rnd = static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX);
	for (int i = 0; i < optionsWeights.size(); i++) {
		if (rnd <= optionsWeights[i]) {
			return IhateOptimizedMemoryUse[i];
		}
	}

	return optionsWeights.size() - 1;
}

// Collapse itself by choosing random option from whats left
void MapGenerator::Cell::Collapse()
{
	if (isCollapsed) { std::cout << "WTF?!"; return; }

	int pickedTileIndex = pickRandom();
	options.clear();
	finalTileIndex = pickedTileIndex;
	isCollapsed = true;
}

// Update it's options
void MapGenerator::Cell::Update(int side,const Cell& neighbour)
{
	std::set<int> newOptions;
	// State that we expect on this side should be equal to that of the collapsed neighbour
	int expectedType = generator->Tileset[neighbour.finalTileIndex].sidesStates[(side + 2) % 4];
	for (int tileIndex : options) {
		if (generator->Tileset[tileIndex].sidesStates[side] == expectedType) {
			newOptions.emplace(tileIndex);
		}
	}

	options.clear();
	for (int tileIndex : newOptions) {
		options.emplace(tileIndex);
	}
}

// Use for degug
void MapGenerator::Cell::print() const
{
	if (isCollapsed) {
		std::cout << "(" << posx << ", " << posy << ") ";
		generator->Tileset[finalTileIndex].print();
		std::cout << std::endl;
		return;
	}

	std::cout << "(" <<posx << ", " << posy << ")\toptions: ";
	for (int tileIndex : options) {
		std::cout << generator->Tileset[tileIndex].icon << " ";
	}
	std::cout << std::endl;
}



//// Nodes workspace

// Sews position and index into node-code
int MapGenerator::makeNode(int i, int j, int index)
{
	return index + 4 * j + 4 * m * i;
}

// Gets node's x position
int MapGenerator::get_i(int node)
{
	return (node - get_index(node) - 4 * get_j(node)) / (4 * m);
}

// Gets node's y position
int MapGenerator::get_j(int node)
{
	return ((node - get_index(node)) % (4 * m)) / 4;
}

//Gets node's index (position inside cell)
int MapGenerator::get_index(int node)
{
	return node % 4;
}

// Depth-first-search. Used to go through graph
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