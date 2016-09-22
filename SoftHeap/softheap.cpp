#include "softheap.h"

void SoftHeap::insert(const int newKey)
{
	ILCell *l = new ILCell(newKey);
	Node *q = new Node(newKey, l, l);
	meld(q);
}

void SoftHeap::meld(Node * q)
{
	Head *h;
	Head *prevhead;
	Head *tohead = m_header.next;
	Node *top;
	Node *bottom;

	/* find the proper place to begin melding */
	/* look for a queue whose rank is >= the rank of q */
	while (q->rank > tohead->rank) {
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
		q = new Node(top->ckey, top->rank + 1);

		/* add child and next */
		q->child = bottom;
		q->next = top;
		q->il = top->il;
		q->il_tail = top->il_tail;

		/* iterate */
		tohead = tohead->next;
	}

	/* if no melding had to occur, create a new head node */
	if (prevhead == tohead->prev) {
		h = new Head();
	}

	/* otherwise, select the previoud headnode for minlist fixing */
	else {
		h = prevhead->next;
	}
	h->queue = q;
	h->rank = q->rank;
	h->prev = prevhead;
	h->next = tohead;
	prevhead->next = h;
	tohead->prev = h;

	/* fix min lists */
	fixMinlist(h);
}

void SoftHeap::fixMinlist(Head *h)
{
	Head *tmpmin;

	/* fix_minlist assumes all suffix_min pointers are correct except for
	those from h back to header */

	/* find the temporary min value */
	if (h->next == &m_tail)
		tmpmin = h;
	else
		tmpmin = h->next->suffix_min;

	/* walk back to the head of the list of binomial queues */
	while (h != &m_header)
	{

		/* if the queue is */
		if (h->queue->ckey < tmpmin->queue->ckey)
			tmpmin = h;

		/* update the suffix min */
		h->suffix_min = tmpmin;

		/* iterate */
		h = h->prev;
	}
}

Node* SoftHeap::sift(Node* v)
{
	Node *tmp;

	/* empty the itemlist at v, as it is useless */
	v->il = nullptr;
	v->il_tail = nullptr;

	/* if v is a leaf, then set the ckey to infinity.
	This ensures that the node will stay at the bottom of the heap */
	if (v->next == nullptr && v->child == nullptr)
	{
		v->ckey = INFTY;
		return v;
	}

	/* create recursive branch on the next node of v */
	v->next = sift(v->next);

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
	if (v->rank > m_r &&
		(v->rank % 2 == 1 || v->child->rank < v->rank - 1)) {


		/* sift again */
		v->next = sift(v->next);

		/* another rotation might be needed */
		if (v->next->ckey > v->child->ckey) {
			tmp = v->child;
			v->child = v->next;
			v->next = tmp;
		}

		/* concatenate the item-list at v->next with the one at v */
		if (v->next->ckey != INFTY && v->next->il != nullptr) {
			v->next->il_tail->next = v->il;
			v->il = v->next->il;
			if (v->il_tail == nullptr) {
				v->il_tail = v->next->il_tail;
			}
			v->ckey = v->next->ckey;
		}
	} /* end of second sift */

	  /* clean of the soft queue by removing nodes with ckeys of inifity. */
	if (v->child->ckey == INFTY)
		if (v->next->ckey == INFTY)
		{
			v->child = nullptr;
			v->next = nullptr;
		}
		else 
		{
			v->child = v->next->child;
			v->next = v->next->next;
		}

	return v;
}