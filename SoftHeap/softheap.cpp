#include <cmath>
#include <iostream>
#include "softheap.h"

softheap* create(int r){
	softheap* h = (softheap*)malloc(sizeof(softheap));
	h->header = new_head();
	h->tail = new_head();
	h->tail->rank = INFTY;
	h->header->next = h->tail;
	h->tail->prev = h->header;
	h->r = r;
	return h;
}


head* new_head(){
	head* h = (head*)malloc(sizeof(head));
	h->queue = NULL;
	h->next = NULL;
	h->prev = NULL;
	h->suffix_min = NULL;
	h->rank = 0;
	return h;
}

node* new_node(){
	node* n = (node*)malloc(sizeof(node));
	n->next = NULL;
	n->child = NULL;
	n->il = NULL;
	n->il_tail = NULL;
	n->rank = 0;
	n->ckey = 0;
	return n;
}

void insert (softheap* heap, int newkey) { 
	node* q;
	ilcell* l;

	/* create a new itemlist node, assign it the key */
	l = (ilcell *) malloc(sizeof (ilcell));
	l->key = newkey;
	l->next = NULL;

	/* create a new node */
	q = new_node();
	q->rank = 0;
	q->ckey = newkey;
	q->il = l;
	q->il_tail = l;

	/* meld the node to the soft heap */
	meld (heap, q);
}

void meld (softheap* heap, node *q) {
	head* h;
	head* prevhead;
	head* tohead = heap->header->next;
	node* top;
	node* bottom;

	/* find the proper place to begin melding */
	/* look for a queue whose rank is >= the rank of q */
	while (q->rank > tohead->rank){
		tohead = tohead->next;
	}

	/* keep track of the previous head */
	prevhead = tohead->prev;

	/* start melding, while there are two heads of equal rank */
	while (q->rank == tohead->rank) {
		if (tohead->queue->ckey > q->ckey) {
			top = q;
			bottom = tohead->queue;
		}
		else {
			top = tohead->queue;
			bottom = q;
		}

		/* create a new node of higher rank */
		q = new_node ();
		q->ckey = top->ckey;
		q->rank = top->rank + 1;

		/* add child and next */
		q->child = bottom;
		q->next = top;
		q->il = top->il;
		q->il_tail = top->il_tail;

		/* iterate */
		tohead = tohead->next;
	}

	/* if no melding had to occur, create a new head node */
	if (prevhead == tohead->prev){ 
		h = new_head ();
	}

	/* otherwise, select the previoud headnode for minlist fixing */
	else{
		h = prevhead->next;
	}
	h->queue = q;
	h->rank = q->rank;
	h->prev = prevhead;
	h->next = tohead;
	prevhead->next = h;
	tohead->prev= h;

	/* fix min lists */
	fix_minlist (heap, h);
}

void fix_minlist (softheap* heap, head *h) {

	head *tmpmin;

	/* fix_minlist assumes all suffix_min pointers are correct except for 
	those from h back to header */

	/* find the temporary min value */
	if (h->next == heap->tail) {
		tmpmin = h;
	}
	else {
		tmpmin = h->next->suffix_min;
	}

	/* walk back to the head of the list of binomial queues */
	while (h != heap->header) {

		/* if the queue is */
		if (h->queue->ckey < tmpmin->queue->ckey) {
			tmpmin = h;
		}

		/* update the suffix min */
		h->suffix_min = tmpmin;

		/* iterate */
		h = h->prev;
	}
}

node* sift (softheap* heap, node* v) { 
	node *tmp;

	/* empty the itemlist at v, as it is useless */
	v->il = NULL;
	v->il_tail = NULL;

	/* if v is a leaf, then set the ckey to infinity.
	 This ensures that the node will stay at the bottom of the heap */
	if (v->next == NULL && v->child == NULL) {
		v->ckey = INFTY;
		return v;
	}

	/* create recursive branch on the next node of v */
	v->next = sift (heap, v->next);

	/* the heap might not satisfy the heap property, as the ckey of v->next might be too large.
	 If so, perform a rotation of v and v->next, to satisfy the heap property. */
	if (v->next->ckey > v->child->ckey) {
		tmp = v->child;
		v->child = v->next;
		v->next = tmp;
	}

	/* pass the itemlist of v->next to v, and the ckey of v->next */
	v->il = v->next->il;
	v->il_tail = v->next->il_tail;
	v->ckey = v->next->ckey;

	/* if the rank of v is off and larger than the threshold (loop condition),
	 then sift again, creating a branching process in the sift recursion tree */
	if (v->rank > heap->r && 
			(v->rank % 2 == 1 || v->child->rank < v->rank-1)) {


		/* sift again */
		v->next = sift (heap, v->next);

		/* another rotation might be needed */
		if (v->next->ckey > v->child->ckey) {
			tmp = v->child;
			v->child = v->next;
			v->next = tmp;
		}

		/* concatenate the item-list at v->next with the one at v */
		if (v->next->ckey != INFTY && v->next->il != NULL) {
			v->next->il_tail->next = v->il;
			v->il = v->next->il;
			if (v->il_tail == NULL) {
				v->il_tail = v->next->il_tail;
			}
			v->ckey = v->next->ckey;
		}
	} /* end of second sift */

	/* clean of the soft queue by removing nodes with ckeys of inifity. */
	if (v->child->ckey == INFTY) {
		if (v->next->ckey == INFTY) {
			v->child = NULL; v->next = NULL;
		}
		else {
			v->child = v->next->child;
			v->next = v->next->next;
		}
	}

	return v;
}


int deletemin(softheap* heap) {
	node *s, *tmp;
	int min;
	int childcount; 
	head *h;

	h = heap->header->next->suffix_min;

	/* while the item-list at h is empty */
	while (h->queue->il == NULL) {

		tmp = h->queue;
		childcount = 0;

		/* count the number of children in the soft queue */
		while (tmp->next != NULL) {
			tmp = tmp->next; 
			childcount ++;
		}

		/* check if the rank invarient is violated */
		if (childcount < h->rank/2) {

			/* dismantle the root by melding its children back */
			h->prev->next = h->next;
			h->next->prev = h->prev;
			fix_minlist (heap, h->prev);
			tmp = h->queue;
			while (tmp->next != NULL) {
				meld (heap, tmp->child);
				tmp = tmp->next;
			}
		}

		/* if the rank invarient holds, we can refill the item list by sifting */
		else {
			h->queue = sift(heap, h->queue);
			if (h->queue->ckey == INFTY) {
				h->prev->next = h->next;
				h->next->prev = h->prev;
				h = h->prev;
			}
			fix_minlist (heap, h);
		}

		/* iterate to the next suffix_min */
		h = heap->header->next->suffix_min;
	}

	/* grab the minimum */
	min = h->queue->il->key;

	/* remove the minimum from its item-list */
	h->queue->il = h->queue->il->next;

	/* update tail pointer if the item-list is now empty */
	if (h->queue->il == NULL) {
		h->queue->il_tail = NULL;
	}

	return min;
}

void printnode(node* toprint, int spaces, int length){
	int i;
	ilcell* itmp;
	int addspaces = 0;

	/* add starting spaces */
	for (i = 0; i < spaces; i++){
	}
	addspaces += 2 + (int)(log10((double)toprint->rank)) + 1;


	/* add items */
	for (itmp = toprint->il; itmp != NULL; itmp = itmp->next){
		addspaces += (int)(log10((double)itmp->key)) + 1;
		if (itmp->next != NULL){
			addspaces++;
		}
	}

	addspaces += 2;

	/* add ckey */
	addspaces += 2 + (int)(log10((double)toprint->ckey)) + 1;

	/* print the next node */

	if (toprint->next != NULL){
		printnode(toprint->next, 0, length + addspaces + 2);
	}

	/* print the child */
	if (toprint->child != NULL) printnode(toprint->child,length + spaces, 0);

	
}

void printsoftheap(softheap* heap)
{
	head* head_tmp;
	std::cout << "Printing heap\nThreshold: " << heap->r << "\n" << std::endl;
	for (head_tmp = heap->header->next; head_tmp != heap->tail; head_tmp = head_tmp->next)
	{
		std::cout << "| Head Rank: " << head_tmp->rank << " Addr:" << head_tmp << " SuffixMin: " << head_tmp->suffix_min << "\n" << std::endl;
		if (head_tmp->queue != NULL)
			printnode(head_tmp->queue, 0, 0);
	}

}

