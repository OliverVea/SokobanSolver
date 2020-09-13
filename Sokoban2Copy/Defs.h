#pragma once
#include "Stopwatch.h"

#include <string>
#include <vector>

#define THREAD_COUNT 16

#define MAX_STATES 50000000

#define DEFAULT_MAP_DELIMITER_START '['
#define DEFAULT_MAP_DELIMITER_END ']'
#define DEFAULT_MAP_CELL_DELIMITER '"'
#define DEFAULT_MAP_CONTAINER_DELIMITER_START '{'
#define DEFAULT_MAP_CONTAINER_DELIMITER_END '}'

/* -- TYPEDEFS -- */
typedef unsigned short int pos_t;
typedef int dir_t;
typedef double heu_t;
typedef std::string hash_t;
typedef char tile_t;

namespace costs {
	const heu_t halfturn = 0.6;
	const heu_t fullturn = 1;
	const heu_t forward = 0.8;
}

const std::string mapPath = "maps/lastcompetitionmap.json";