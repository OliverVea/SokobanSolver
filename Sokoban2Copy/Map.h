#pragma once
#include "Defs.h"
#include "Dictionary.h"

#include <vector>
#include <unordered_map>
#include <queue>
#include <iostream>

class Map
{
public:
	enum eTileType { eNull, eFree, eWall, eTarget, ePlayer, eBox, eBoxOnTarget, eNobox };
	enum eHeuristic { eHeuNone, eHeuSparse };
	struct State {
		pos_t playerPos = 0; // Player position
		dir_t playerDir = 1;
		std::vector<pos_t> boxPositions = {}; // The position of boxes.

		heu_t currentExpense = 0; // Current expense.
		heu_t futureExpense = 0; // Estimate of future expense.

		const heu_t getHeuristic() const; // Returns currentExpense + futureExpense.

		const hash_t getHash() const;
		static State fromHash(hash_t val);

		static bool comparison(const State& a, const State& b);

		const bool operator==(const State& s) const;
		const bool containsBox(pos_t position) const;

		const State move(pos_t direction) const;
	}; 
	static Dictionary<tile_t, Map::eTileType> dic;

	struct StateComparison {
		bool operator()(const State& a, const State& b) {
			return Map::State::comparison(a, b);
		}
	};

	Map(const std::vector<eTileType>& map, const std::vector<pos_t>& targetPositions, const pos_t& width, const pos_t& height, const State& initialState);
	~Map();

	const bool hasWon(const State& state) const;

	static pos_t getFirstPosInConnectedArea(const pos_t& position, const State& state, std::vector<eTileType> map, const pos_t& width);
	static std::vector<pos_t> getConnectedArea(const pos_t& position, const State& state, std::vector<eTileType> map, const pos_t& width);

	pos_t getFirstPosInConnectedArea(const pos_t& position, const State& state);
	std::vector<pos_t> getConnectedArea(const pos_t& position, const State& state);

	const pos_t getDistance(pos_t a, pos_t b) const;
	const bool containsTarget(pos_t p) const;

	const std::vector<Map::State> getChildStates(const State& parentState, const eHeuristic& futureExpenseHeuristic) const;
	const pos_t getMaxPos() const;
	const State getInitialState() const;

	static const bool isValid(const pos_t& pos, const std::vector<eTileType>& map);
	const bool isValid(const pos_t& pos) const;

	static const bool isFree(const pos_t& pos, const std::vector<eTileType>& map);
	const bool isFree(const pos_t& pos) const;

	static Map loadMap(const std::vector<std::vector<tile_t>>& map);
	static void printMap(const Map& map, const State& state);
	static std::vector<pos_t> getNeighbors(const pos_t& position, const std::vector<eTileType>& map, const pos_t& width, const std::vector<eTileType>& tiles);
	static bool contains(const eTileType& tile, const std::vector<eTileType>& tiles);

protected:
	const std::vector<dir_t> directions;
	const pos_t width, height;
	const std::vector<pos_t> targetPositions;
	const std::vector<eTileType> map;
	const Map::State initialState;
};