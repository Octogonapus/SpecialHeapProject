#include "ilcell.h"

#pragma once

class Node
{
public:
	Node() :
		ckey(0),
		rank(0),
		next(nullptr),
		child(nullptr),
		il(nullptr),
		il_tail(nullptr) {}

	Node(const int ckeyIn, const int rankIn) :
		ckey(ckeyIn),
		rank(rankIn),
		next(nullptr),
		child(nullptr),
		il(nullptr),
		il_tail(nullptr) {}

	Node(const int ckeyIn, ILCell *ilIn, ILCell *il_tailIn) :
		ckey(ckeyIn),
		rank(0),
		next(nullptr),
		child(nullptr),
		il(ilIn),
		il_tail(il_tailIn) {}

	int ckey, rank;
	Node *next, *child;
	ILCell *il, *il_tail;
};