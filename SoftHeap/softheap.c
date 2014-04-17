
#include "softheap.h"
#include <math.h>

softheap* create(int r){
	softheap* new = malloc(sizeof(softheap));
	new->header = new_head();
	new->tail = new_head();
	new->tail->rank = INFTY;
	new->header->next = new->tail;
	new->tail->prev = new->header;
	new->r = r;
	return new;
}


head* new_head(){
	head* new = malloc(sizeof(head));
	new->queue = NULL;
	new->next = NULL;
	new->prev = NULL;
	new->suffix_min = NULL;
	new->rank = 0;
	return new;
}

node* new_node(){
	node* new = malloc(sizeof(node));
	new->next = NULL;
	new->child = NULL;
	new->il = NULL;
	new->il_tail = NULL;
	new->rank = 0;
	new->ckey = 0;
	return new;
}

void insert (softheap* heap, int newkey) { 
	node* q;
	ilcell* l;

	printf("Inserting value %d\n", newkey);

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

	printf("Melding a queue of rank %d\n", q->rank);

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
		printf("No melding occured\n");
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

	printf("Fixing minlists\n");

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

	printf("sifting on node %p with rank %d and ckey %d\n", v, v->rank, v->ckey);

	/* empty the itemlist at v, as it is useless */
	v->il = NULL;
	v->il_tail = NULL;

	/* if v is a leaf, then set the ckey to infinity.
	 This ensures that the node will stay at the bottom of the heap */
	if (v->next == NULL && v->child == NULL) {
		v->ckey = INFTY;
		printf("set to infinity\n");
		return v;
	}

	/* create recursive branch on the next node of v */
	printf("first recursive sift!\n");
	v->next = sift (heap, v->next);

	/* the heap might not satisfy the heap property, as the ckey of v->next might be too large.
	 If so, perform a rotation of v and v->next, to satisfy the heap property. */
	if (v->next->ckey > v->child->ckey) {
		printf("perform rotation to satisfy heap property\n");
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
	printf("rank: %d, r: %d\n", v->rank, heap->r);	
	if (v->rank > heap->r && 
			(v->rank % 2 == 1 || v->child->rank < v->rank-1)) {

		printf("second recursive sift!\n");

		/* sift again */
		v->next = sift (heap, v->next);

		/* another rotation might be needed */
		if (v->next->ckey > v->child->ckey) {
			printf("second rotation needed\n");
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
			printf("Removed child and next\n");
			v->child = NULL; v->next = NULL;
		}
		else {
			printf("Removed child\n");
			v->child = v->next->child;
			v->next = v->next->next;
		}
	}

	printf("end of sift\n");
	return v;
}


int deletemin(softheap* heap) {
	node *sift (), *tmp;
	int min;
	int childcount; 
	head *h;

	printf("deleting min\n");

	h = heap->header->next->suffix_min;

	/* while the item-list at h is empty */
	while (h->queue->il == NULL) {

		printf("list at suffix_min %p is empty\n", h);

		tmp = h->queue;
		childcount = 0;

		/* count the number of children in the soft queue */
		while (tmp->next != NULL) {
			tmp = tmp->next; 
			childcount ++;
		}

		printf("childcount: %d rank: %d\n", childcount, h->rank);

		/* check if the rank invarient is violated */
		if (childcount < h->rank/2) {

			printf("Rank invarient violated, fix\n");

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
		printf(" ");
	}
	printf("--");

	printf("[%d]", toprint->rank);
	addspaces += 2 + (int)(log10((double)toprint->rank)) + 1;


	/* add items */
	printf("{");
	for (itmp = toprint->il; itmp != NULL; itmp = itmp->next){
		printf("%d", itmp->key);
		addspaces += (int)(log10((double)itmp->key)) + 1;
		if (itmp->next != NULL){
			printf(",");
			addspaces++;
		}
	}

	printf("}");
	addspaces += 2;

	/* add ckey */
	printf("(%d)", toprint->ckey);
	addspaces += 2 + (int)(log10((double)toprint->ckey)) + 1;

	/* print the next node */

	if (toprint->next != NULL){
		printnode(toprint->next, 0, length + addspaces + 2);
	}

	printf("\n");
	/* print the child */
	if (toprint->child != NULL) printnode(toprint->child,length + spaces, 0);

	
}

void printsoftheap(softheap* heap){
	head* head_tmp;

	printf("Printing heap\nThreshold: %d\n", heap->r);
	for (head_tmp = heap->header->next; head_tmp != heap->tail; head_tmp = head_tmp->next){
		printf("| Head Rank: %d Addr:%p SuffixMin: %p\n", head_tmp->rank, head_tmp, head_tmp->suffix_min);
		if (head_tmp->queue != NULL){
			printnode(head_tmp->queue, 0, 0);
		}
	}

}

