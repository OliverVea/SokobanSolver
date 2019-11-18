#include "IDepthFirst.h"

int main() {

	Map::dic = Dictionary<tile_t, Map::eTileType>();
	Map::dic.add(' ', Map::eFree);
	Map::dic.add('#', Map::eWall);
	Map::dic.add('P', Map::ePlayer);
	Map::dic.add('X', Map::eTarget);
	Map::dic.add('V', Map::eBoxOnTarget);
	Map::dic.add('C', Map::eBox);
	Map::dic.add('-', Map::eNobox);

	Map map = Map::loadMap(mapString);

	Map::State state = map.getInitialState();

	size_t mapSize = sizeof(map);
	size_t stateSize = sizeof(state);

	std::vector<Map::State> solution = IDepthFirst::solve(map, state);

	for (Map::State s : solution) {
		Map::printMap(map, s, mapWidth);
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
		system("cls");
	}

	return 0;
}