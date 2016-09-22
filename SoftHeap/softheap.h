#include "head.h"
#include "node.h"
#include "ilcell.h"

#pragma once

class SoftHeap
{
public:
	SoftHeap(int r) :
		m_r(r),
		m_header(Head()),
		m_tail(Head())
	{
		m_tail.rank = INFTY;
		m_header.next = &m_tail;
		m_tail.prev = &m_header;
		m_r = r;
	}

	//Inserts a new key into the given soft heap
	void insert(const int newKey);

	//Melds a node into the given heap
	void meld(Node *q);

	//Fixes minlists in the given soft heap, from header to h
	void fixMinlist(Head *h);

	//Deletes the minimum key from the soft heap
	Node* sift(Node* v);

	//Deletes the minimum key from the soft heap
	int deletemin();

	//Prints the soft heap
	void printSoftHeap();
private:
	Head m_header, m_tail;
	int m_r;
	static const int INFTY = 999999;

	void printnode(Node* toprint, int spaces, int length);
};