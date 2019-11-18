#include "Map.h"

Dictionary<tile_t, Map::eTileType> Map::dic;

Map::Map(const std::vector<eTileType>& map, const std::vector<pos_t>& targetPositions, const pos_t& width, const pos_t& height, const State& initialState) : 
	map(map), directions({-1, 1, -width, width}), targetPositions(targetPositions), width(width), height(height), initialState(initialState)
{ }

Map::~Map()
{ }

const bool Map::hasWon(const State& state) const
{
	for (pos_t targetPosition : targetPositions) 
		if (!state.containsBox(targetPosition))
			return false;
	return true;
}

pos_t Map::getFirstPosInConnectedArea(const pos_t& position, const State& state, std::vector<eTileType> map, const pos_t& width)
{
	std::vector<pos_t> connectedArea = Map::getConnectedArea(position, state, map, width);
	if (connectedArea.empty()) return (pos_t)-1;
	std::sort(connectedArea.begin(), connectedArea.end());
	return connectedArea[0];
}

std::vector<pos_t> Map::getConnectedArea(const pos_t& position, const State& state, std::vector<eTileType> map, const pos_t& width)
{
	std::vector<dir_t> directions = { -1, 1, -width, width };
	std::vector<pos_t> result = { position };
	std::unordered_map<pos_t, bool> alreadyChecked{ {position, true} };

	for (size_t i = 0; i < result.size(); i++) {
		pos_t& currentPos = result[i];
		for (dir_t dir : directions) {
			const pos_t newPos = currentPos + dir;
			if (Map::isValid(newPos, map) && !alreadyChecked[newPos]) {
				if (Map::isFree(newPos, map) && !state.containsBox(newPos)) result.push_back(newPos);
				alreadyChecked[newPos] = true;
			}
		}
	}

	return result;
}

pos_t Map::getFirstPosInConnectedArea(const pos_t& position, const State& state)
{
	return Map::getFirstPosInConnectedArea(position, state, map, width);
}

std::vector<pos_t> Map::getConnectedArea(const pos_t& position, const State& state)
{
	return Map::getConnectedArea(position, state, map, width);
}

const std::vector<Map::State> Map::getChildStates(const State& parentState) const
{
	std::vector<Map::State> newStates;
	std::vector<pos_t> freeNeighbors = getNeighbors(parentState.playerPos, map, width, {eFree, eNobox});
	
	for (pos_t newPlayerPos : freeNeighbors) {
		dir_t dir = newPlayerPos - parentState.playerPos;
		State newState = parentState.move(dir);

		if (!parentState.containsBox(newPlayerPos)) newStates.push_back(newState);
		else {
			pos_t newBoxPos = newPlayerPos + dir;
			if (newBoxPos >= 0 && newBoxPos < map.size() && !parentState.containsBox(newBoxPos) && map[newBoxPos] == eFree ) {
				std::replace(newState.boxPositions.begin(), newState.boxPositions.end(), newPlayerPos, newBoxPos);
				newStates.push_back(newState);
			}
		}
	}

	return newStates;
}

const pos_t Map::getMaxPos() const
{
	return height * width;
}

const Map::State Map::getInitialState() const
{
	return initialState;
}

const bool Map::isValid(const pos_t& pos, const std::vector<eTileType>& map)
{
	return (pos >= 0 && pos < map.size());
}

const bool Map::isValid(const pos_t& pos) const
{
	return (pos >= 0 && pos <= map.size());
}

const bool Map::isFree(const pos_t& pos, const std::vector<eTileType>& map)
{
	return map[pos] != eWall;
}

const bool Map::isFree(const pos_t& pos) const
{
	return map[pos] != eWall;
}

Map Map::loadMap(const std::vector<std::vector<tile_t>>& mapVec)
{
	std::unordered_map<tile_t, eTileType> dic = Map::dic.getB();
	pos_t height = (pos_t)mapVec.size(), width;
	std::vector<eTileType> map;
	std::vector<pos_t> targetPositions;
	State initialState;

	if (height > 0) {
		width = (pos_t)mapVec[0].size();

		map = std::vector<eTileType>(width * height);

		for (pos_t x = 0; x < width; x++) {
			for (pos_t y = 0; y < height; y++) {
				pos_t index = y * width + x;
				eTileType tile = dic[mapVec[y][x]];

				map[index] = eFree;
				if (tile == eWall) map[index] = eWall;
				if (tile == eTarget || tile == eBoxOnTarget) targetPositions.push_back(index);
				if (tile == eBox || tile == eBoxOnTarget) initialState.boxPositions.push_back(index);
				if (tile == ePlayer) initialState.playerPos = index;
			}
		}
	}

	const std::vector<pos_t> w = { width, height };
	const std::vector<pos_t> d = { width, 1 };

	for (pos_t x = 0; x < width; x++) {
		for (pos_t y = 0; y < height; y++) {
			pos_t i = x + y * width;
			std::vector<pos_t> freeNeigbors = getNeighbors(i, map, width, { eFree });
			while (map[i] == eFree && (std::find(targetPositions.begin(), targetPositions.end(), i) == targetPositions.end()) && (freeNeigbors.size() == 1)) {
				map[i] = eWall;
				i = freeNeigbors[0];
				freeNeigbors = getNeighbors(i, map, width, { eFree });
			}
			if (std::find(targetPositions.begin(), targetPositions.end(), i) != targetPositions.end() && freeNeigbors.size() == 0) map[i] = eWall;
		}
	}

	// Removing edges without targets for boxes.
	for (int i = 0; i < 2; i++) {
		for (pos_t a = 0; a < w[1 - i]; a++) { // y, x
			std::vector<pos_t> row;
			bool freeTile[] = { false, false };

			for (pos_t b = 0; b < w[i]; b++) { // x, y
				
				// Calculating position from indeces.
				pos_t index;
				if (i == 0) index = a * width + b; // y * width + x
				else index = a + b * width; // x + y * width;

				eTileType tile = map[index];

				// Calculating neighbour positions perpendicular to the traveling direction.
				int posA = index - d[i], posB = index + d[i]; // (posA = i - width, posB = i + width), (posA = i - 1, posB = i + 1)

				freeTile[0] |= (posA >= 0) && (map[posA] == eFree || map[posA] == eNobox);
				freeTile[1] |= (posB < map.size()) && (map[posB] == eFree || map[posB] == eNobox);

				if ((tile == eFree) || (tile == eNobox)) row.push_back(index);
				if (tile == eWall || std::find(targetPositions.begin(), targetPositions.end(), index) != targetPositions.end()) {
					if (tile == eWall && !(freeTile[0] && freeTile[1])) for (pos_t p : row) map[p] = eNobox;
					else {
						freeTile[0] = freeTile[1] = false;
						while (b < w[i] && map[index] != eWall) {
							b++;
							if (i == 0) index = a * width + b;
							else index = a + b * width;
						}
					}
					row = {};
				}
			}
		}
	}

	for (pos_t x = 0; x < width; x++) {
		for (pos_t y = 0; y < height; y++) {
			pos_t i = x + y * width;

			bool isTarget = (std::find(targetPositions.begin(), targetPositions.end(), i) != targetPositions.end());

			if (map[i] == eFree && !isTarget) {
				std::vector<pos_t> neighbors = getNeighbors(i, map, width, {eFree, eNobox});

				if (neighbors.size() == 2) { // If point has 2 neighbors.
					if (abs(i - neighbors[0]) != abs(i - neighbors[1])) // If neighbors are perpendicular.
						map[i] = eNobox; 
				}
			}
		}
	}

	return Map(map, targetPositions, width, height, initialState);
}

void Map::printMap(const Map& map, const State& state, const pos_t & width)
{
	std::unordered_map<eTileType, tile_t> dic = Map::dic.getA();

	for (pos_t i = 0; i < map.map.size(); i++) {
		pos_t x = i % width, y = i / width;

		bool isTarget = (std::find(map.targetPositions.begin(), map.targetPositions.end(), i) != map.targetPositions.end());
		bool isBox = state.containsBox(i);

		if (isTarget && isBox) std::cout << dic[eBoxOnTarget];
		else if (isTarget) std::cout << dic[eTarget];
		else if (isBox) std::cout << dic[eBox];
		else if (i == state.playerPos) std::cout << dic[ePlayer];
		else std::cout << dic[map.map[i]];
		if (x == width - 1) std::cout << std::endl;
	}
}

std::vector<pos_t> Map::getNeighbors(const pos_t& i, const std::vector<eTileType>& map, const pos_t& width, const std::vector<eTileType>& tiles)
{
	std::vector<pos_t> neighbors, freeNeighbors;

	if (i >= width) neighbors.push_back(i - width);
	if (i < (map.size() - width)) neighbors.push_back(i + width);
	if ((i % width) > 0) neighbors.push_back(i - 1);
	if ((i % width) < (width - 1)) neighbors.push_back(i + 1);

	for (dir_t p : neighbors) if (std::find(tiles.begin(), tiles.end(), map[p]) != tiles.end()) freeNeighbors.push_back(p);
	return freeNeighbors;
}

bool Map::contains(const eTileType& tile, const std::vector<eTileType>& tiles)
{
	return std::find(tiles.begin(), tiles.end(), tile) != tiles.end();
}

/* -- STATE -- */

const heu_t Map::State::getHeuristic() const
{
	return currentExpense + futureExpense;
}

const hash_t Map::State::getHash() const
{
	hash_t result = std::to_string(playerPos);
	result += "," + std::to_string(playerDir);
	for (size_t i = 0; i < boxPositions.size(); i++) {
		result += "," + std::to_string(boxPositions[i]);
	}
	return result;
}

Map::State Map::State::fromHash(hash_t val)
{
	if (val == "") return Map::State();
	State result;
	std::vector<std::string> elements;
	std::string temp = "";
	for (char c : val) {
		if (c == ',') {
			elements.push_back(temp);
			temp = "";
		}
		else temp += c;
	}
	elements.push_back(temp);
	result.playerPos = std::stoi(elements[0]);
	result.playerDir = std::stoi(elements[1]);
	for (int i = 2; i < elements.size(); i++) result.boxPositions.push_back(std::stoi(elements[i]));
	return result;
}

bool Map::State::comparison(const Map::State& a, const Map::State& b)
{
	return a.getHeuristic() > b.getHeuristic();
}

const bool Map::State::operator==(const Map::State& s) const
{
	if (playerPos != s.playerPos) return false;

	std::vector<pos_t> temp = s.boxPositions;

	for (pos_t boxPos : boxPositions) {
		bool found = false;
		for (size_t i = 0; i < temp.size(); i++) {
			if (boxPos == temp[i]) {
				found = true;
				temp.erase(temp.begin() + i);
			}
		}
		if (!found) return false;
	}

	return true;
}

const bool Map::State::containsBox(pos_t position) const
{
	for (pos_t i : boxPositions) if (i == position) return true;
	return false;
}

const Map::State Map::State::move(dir_t direction) const
{
	State newState = *this;

	newState.playerPos += direction;

	if (newState.playerDir == direction) newState.currentExpense += costs::forward;
	else if (newState.playerDir == -direction) newState.currentExpense += costs::forward + costs::fullturn;
	else newState.currentExpense += costs::forward + costs::halfturn;

	newState.playerDir = direction;

	return newState;
}
