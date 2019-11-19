#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <iostream>
#include <sstream>
#include <iomanip>

// Chrono wrapper-ish class as it made a mess in the source file.

class Stopwatch
{
public:
	typedef std::chrono::nanoseconds ns;

	enum Unit {Nanosecond, Microsecond, Millisecond, Second, Kilosecond, Megasecond, Terasecond};

	Stopwatch();
	~Stopwatch();

	void addWatch(std::string watchName = "");
	void start(std::string watchName = "");
	void stop(std::string watchName = "");
	double getTime(std::string watchName = "", Unit unit = Millisecond);
	std::string getString(std::string watchName = "", int decimals = 3);
	void print(std::string watchName = "");

private:
	int getIndex(std::string watchName);

	// This could have been a binary tree.
	std::vector<std::string> watchNames;
	std::vector<std::pair<ns, ns>> times;
};

