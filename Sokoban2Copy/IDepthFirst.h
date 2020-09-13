#pragma once
#include "BaseSolution.h"

#include <algorithm>
#include <chrono>
#include <thread>
#include <mutex>
#include <future>

class IDepthFirst :
	private BaseSolution
{
public:
	static std::vector<Map::State> solve(Map map, Map::State state, Map::eHeuristic heuristic);

private:

	static Map::State task(const Map& map, std::mutex& a, std::mutex& b, std::mutex& mStopThread, int& bStopThread, std::unordered_map<hash_t, std::pair<heu_t, hash_t>>& pastBest, std::priority_queue<Map::State, std::vector<Map::State>, Map::StateComparison>& openSet, Map::eHeuristic heuristic);
};

