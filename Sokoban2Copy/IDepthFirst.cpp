#include "IDepthFirst.h"

std::vector<Map::State> IDepthFirst::solve(Map map, Map::State state)
{
	Stopwatch sw;
	sw.start("totalTime");
	std::mutex a, b;
	std::unordered_map<hash_t, std::pair<heu_t, hash_t>> pastBest;
	std::priority_queue<Map::State, std::vector<Map::State>, Map::StateComparison> openSet;

	std::vector<std::mutex> vmStopThread(THREAD_COUNT);
	std::vector<int> vbStopThread(THREAD_COUNT);
	for (int i = 0; i < THREAD_COUNT; i++) vbStopThread[i] = 0;
	std::vector<std::future<Map::State>> results(THREAD_COUNT);

	openSet.push(state);

	for (int i = 0; i < THREAD_COUNT; i++) {
		std::mutex& mStopThread = vmStopThread[i];
		int& bStopThread = vbStopThread[i];
		results[i] = std::async(std::launch::async, [map, &a, &b, &mStopThread, &bStopThread, &pastBest, &openSet, i] { return task(map, a, b, mStopThread, bStopThread, pastBest, openSet, i == 0); });
	}

	bool gotSolution = false;
	while (!gotSolution) {
		for (int i = 0; i < THREAD_COUNT; i++) gotSolution |= results[i]._Is_ready();
	}

	for (int i = 0; i < THREAD_COUNT; i++) {
		vmStopThread[i].lock();
		vbStopThread[i] = true;
		vmStopThread[i].unlock();
	}

	Map::State bestState;

	std::vector<Map::State> r(THREAD_COUNT);
	std::vector<bool> hW(THREAD_COUNT);

	for (int i = 0; i < THREAD_COUNT; i++) {
		r[i] = results[i].get();
		hW[i] = map.hasWon(r[i]);
	}
	for (int i = 0; i < THREAD_COUNT; i++) { if (bestState == Map::State() || (!(r[i] == Map::State()) && r[i].getHeuristic() < bestState.getHeuristic())) bestState = r[i]; }

	std::vector<Map::State> solution;

	while (!(bestState == Map::State())) {
		solution.push_back(bestState);
		hash_t bestHash = bestState.getHash();
		auto pastBestPair = pastBest[bestHash];
		hash_t parentHash = pastBestPair.second;
		bestState = Map::State::fromHash(parentHash);
	}

	std::reverse(solution.begin(), solution.end());

	sw.stop("totalTime");
	std::cout << "Found solution with " << solution.size() << " steps after searching " << pastBest.size() << " states. Time to calculate: " << sw.getString("totalTime") << std::endl;

	return solution;
}

Map::State IDepthFirst::task(const Map& map, std::mutex& a, std::mutex& b, std::mutex& mStopThread, int& bStopThread, std::unordered_map<hash_t, std::pair<heu_t, hash_t>>& pastBest, std::priority_queue<Map::State, std::vector<Map::State>, Map::StateComparison>& openSet, bool printStates)
{
	Map::State currentState;
	int i = 0;
	do {
		mStopThread.lock();
		a.lock();
		while (openSet.size() == 0 && !bStopThread) {
			a.unlock();
			mStopThread.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			mStopThread.lock();
			a.lock();
		}
		mStopThread.unlock();

		if (bStopThread) {
			a.unlock();
			return Map::State();
		}

		currentState = openSet.top();
		openSet.pop();
		a.unlock();

		if (false) {
			system("cls");
			Map::printMap(map, currentState, 12);
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
		}

		std::vector<Map::State> newStates = map.getChildStates(currentState);

		for (Map::State newState : newStates) {
			hash_t hash = newState.getHash();

			b.lock();
			const std::pair<heu_t, hash_t> past = pastBest[hash];
			b.unlock();

			if (past.first == 0 || (past.first > newState.getHeuristic())) {
				a.lock();
				openSet.push(newState);
				a.unlock();
				b.lock();
				pastBest[hash] = { newState.getHeuristic(), currentState.getHash() };
				b.unlock();
			}
		}
		i++;
	} while (!map.hasWon(currentState));
	return currentState;
}
