#pragma once

class ILCell
{
public:
	ILCell() :
		key(0),
		next(nullptr) {}

	ILCell(const int keyIn) :
		key(0),
		next(nullptr) {}

	int key;
	ILCell* next;
};