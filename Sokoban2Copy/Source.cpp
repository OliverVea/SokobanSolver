#include "IDepthFirst.h"
#include "Dictionary.h"

int main() {
	std::vector<std::vector<tile_t>> mapString({
		{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'},
		{'#', 'X', ' ', 'X', '#', ' ', ' ', ' ', ' ', ' ', '#', '#'},
		{'#', ' ', ' ', '#', '#', 'C', 'C', ' ', 'C', ' ', '#', '#'},
		{'#', ' ', ' ', 'X', '#', ' ', ' ', '#', ' ', ' ', ' ', '#'},
		{'#', ' ', ' ', ' ', ' ', ' ', ' ', '#', 'C', ' ', ' ', '#'},
		{'#', ' ', ' ', 'X', '#', ' ', ' ', '#', 'P', ' ', ' ', '#'},
		{'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#'}
	});

	Dictionary<tile_t, Map::eTileType> dic;
	dic.add(' ', Map::eFree);
	dic.add('#', Map::eWall);
	dic.add('P', Map::ePlayer);
	dic.add('X', Map::eTarget);
	dic.add('V', Map::eTarget);
	dic.add('C', Map::eBox);
	dic.add('-', Map::eNobox);

	Map map = Map::loadMap(mapString, dic.getB());

	Map::printMap(map, dic.getA(), 12);

	Map::State state = map.getInitialState();

	size_t mapSize = sizeof(map);
	size_t stateSize = sizeof(state);

	std::vector<Map::State> solution = IDepthFirst::solve(map, state);

	return 0;
}