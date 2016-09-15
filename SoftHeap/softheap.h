/*
 * softheap.c
 * Description: An implementation of the soft heap described by Bernard
 * 	Chazelle in his paper "The Soft Heap: An Approximate Priority 
 *	Queue with Optimal Error Rate". This code is based off of his work.
 *	I take no credit for any of the implementation.
 *
 * A soft heap is a sequence of modified binomial trees of distint ranks, called soft queues.
 * Soft queues are altered binomial trees. Soft queues can have multiple keys per node, and nodes
 * may be missing.
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>

/* ILCELL stands for Item List Cell. An Item List is a singly linked list of keys */
typedef struct ILCELL {
	int key;
	struct ILCELL *next;
} ilcell;

/* a node of the soft heap. A node stores an itemlist of keys. A node has a ckey (common key), which is an upper bound on all of the keys in its itemlist. It also has a rank. (TODO: explain rank better)*/
typedef struct NODE {
	int ckey;
	int rank;
	struct NODE *next;
	struct NODE *child;
	struct ILCELL *il;
	struct ILCELL *il_tail;
} node;

typedef struct HEAD {
	int rank;
	struct HEAD *next;
	struct HEAD *prev;

	/* queue points to the root of the queue of the head node */
	struct NODE *queue;
	
	/* suffix min points to the root of minimum ckey among all head nodes of rank j
	 where j >= rank */
	struct HEAD *suffix_min;
	
} head;

/* the structre for a soft heap. A soft heap is a list of soft queues, ordered by increasing rank.
 the constant r is related to the error constant e.*/
typedef struct SOFTHEAP {
	head *header;
	head *tail;
	int r;
} softheap;

#define INFTY 999999

/* creates a new soft heap */
softheap* create(int r);

/* creates a new head node (root of a soft queue) in the trivial manner */
head* new_head();

/* creates a new node in the trivial manner */
node* new_node();

/* inserts a new key into the given soft heap */
void insert (softheap* heap, int newkey);

/* melds a node into the given heap */
void meld (softheap* heap, node *q);

/* fixes minlists in the given soft heap, from header to h */
void fix_minlist (softheap* heap, head *h);

node* sift (softheap* heap, node* v);

/* deletes the minimum key from the soft heap */
int deletemin(softheap* heap);

/* prints the soft heap */
void printsoftheap(softheap* heap);
