#include <stdio.h>
#include <stdlib.h>
#include "softheap.h"

int main(int argc, char** args)
{
	int r;
	char op;
	int value;
	int inserts[50];
	int deletes[50];
	int icount = 0, dcount = 0;
	int i;
	softheap* heap;
	FILE *fp;

	printf ("Enter r: "); 
	scanf ("%d", &r);

	heap = create(r);
	
	/* read input */
	fp = fopen("input.txt", "r");
	while (fscanf(fp, "%c", &op) == 1){
		switch(op){
		case 'i':
			fscanf(fp," %d\n", &value);
			insert(heap, value);
			inserts[icount] = value;
			icount++;
			break;
		case 'd':
			fscanf(fp,"\n");
			value = deletemin(heap);
			deletes[dcount] = value;
			dcount++;
			printf("Minimum deleted: %d\n", value);
			break;
		case 'p':
			fscanf(fp,"\n");
			printsoftheap(heap);
			break;
		default:
			printf("unknown input\n");
			break;
		}
	}

	fclose(fp);

	printf("Printing order of delete mins\n");
	for (i = 0; i < dcount; i++){
		printf("%d\n", deletes[i]);
	}


	return 0;
}
