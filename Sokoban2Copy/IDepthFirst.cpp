#include "IDepthFirst.h"

std::vector<Map::State> IDepthFirst::solve(Map map, Map::State state)
{
	std::mutex a, b;
	std::unordered_map<hash_t, std::pair<heu_t, hash_t>> pastBest;
	std::priority_queue<Map::State, std::vector<Map::State>, Map::StateComparison> openSet;

	openSet.push(state);

	Map::State currentState;

	std::vector<Map::State> result;

	return result;
}

Map::State IDepthFirst::task(Map map)
{
	Map::State currentState;
	do {
		// Mutex A
		a.lock();
		currentState = openSet.top();
		openSet.pop();
		a.unlock();
		// Mutex A

		std::vector<Map::State> newStates = map.getChildStates(currentState);

		for (Map::State newState : newStates) {
			hash_t hash = newState.getHash(map.getMaxPos());

			// Mutex B
			b.lock();
			std::pair<heu_t, hash_t>& past = pastBest[hash];

			if (past.first == 0 || past.first > newState.getHeuristic()) {
				// Mutex A
				a.lock();
				openSet.push(newState);
				a.unlock();
				// Mutex B
				past = { newState.getHeuristic(), currentState.getHash(map.getMaxPos()) };
			}
			b.unlock();
			// Mutex B
		}
	} while (openSet.size() > 0 && !map.hasWon(currentState));
	return currentState;
}
