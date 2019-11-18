#pragma once
#include "BaseSolution.h"

#include <algorithm>
#include <thread>
#include <mutex>

class IDepthFirst :
	private BaseSolution
{
public:
	static std::vector<Map::State> solve(Map map, Map::State state);

private:

	static Map::State task(Map map, std::mutex& a, std::mutex& b, std::unordered_map<hash_t, std::pair<heu_t, hash_t>>& pastBest, std::priority_queue<Map::State, std::vector<Map::State>, Map::StateComparison>& openSet);
};

