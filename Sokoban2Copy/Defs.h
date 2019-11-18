#pragma once
#include <string>

#define THREAD_COUNT 3

/* -- TYPEDEFS -- */
typedef unsigned short int pos_t;
typedef int dir_t;
typedef double heu_t;
//typedef long unsigned int hash_t;
typedef std::string hash_t;
typedef char tile_t;

namespace costs {
	const heu_t halfturn = 1;
	const heu_t fullturn = 2;
	const heu_t forward = 1;
}