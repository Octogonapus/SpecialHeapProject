#include "node.h"

#pragma once

class Head
{
public:
	Head() :
		rank(0),
		next(nullptr),
		prev(nullptr),
		suffix_min(nullptr),
		queue(nullptr) {}

	const bool operator==(const Head& other) const
	{
		rank == other.rank &&
		next == other.next &&
		prev == other.prev &&
		suffix_min == other.suffix_min &&
		queue == other.queue;
	}

	const bool operator!=(const Head& other) const { !(operator==(other)); }

	int rank;
	Head *next, *prev, *suffix_min;
	Node *queue;
};
