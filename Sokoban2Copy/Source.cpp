#include "IDepthFirst.h"
#include "JSONReading.h"

int main() {
	Map::dic = Dictionary<tile_t, Map::eTileType>();
	Map::dic.add(' ', Map::eFree);
	Map::dic.add('#', Map::eWall);
	Map::dic.add('P', Map::ePlayer);
	Map::dic.add('X', Map::eTarget);
	Map::dic.add('V', Map::eBoxOnTarget);
	Map::dic.add('C', Map::eBox);
	Map::dic.add('-', Map::eNobox);
	
	auto vsMap = json::readJSONMapFromFile(mapPath);
	auto vcMap = json::reduceToChars(vsMap);
	Map map = Map::loadMap(vcMap);

	Map::State state = map.getInitialState();

	Map::printMap(map, state);

	size_t mapSize = sizeof(map);
	size_t stateSize = sizeof(state);

	std::vector<Map::State> solutionNone = IDepthFirst::solve(map, state, Map::eHeuNone);
	//std::vector<Map::State> solutionSparse = IDepthFirst::solve(map, state, Map::eHeuSparse);

	system("pause");

	for (Map::State s : solutionNone) {
		system("cls");
		Map::printMap(map, s);
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}

	system("pause");

	return 0;
}