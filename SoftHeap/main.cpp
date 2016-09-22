#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <queue>
#include "softheaps.h"

int main(int argc, char** args)
{
	int r = 0.01;
	char op;
	int value;
	static const int size = 100000;
	static const int reps = 1;
	int inserts[size];
	int deletes[size];
	int icount = 0, dcount = 0;
	int i, z;
	softheap* heap;
	/*FILE *fp;

	std::cout << "Enter r:\n" << std::endl;
	std::cin >> r;*/

	heap = create(r);
	srand(time(NULL));
	std::priority_queue<int> pq;

	for (i = 0; i < size; i++)
	{
		inserts[i] = rand();
	}

	std::chrono::system_clock::time_point t(std::chrono::system_clock::now());

	for (z = 0; z < reps; z++)
	{
		for (i = 0; i < size; i++)
			insert(heap, inserts[i]);

		std::cout << z << std::endl;

		for (i = 0; i < size; i++)
			deletes[i] = deletemin(heap);
	}

	std::chrono::system_clock::time_point t2(std::chrono::system_clock::now());

	std::cout << "duration: " << (t2 - t).count() << std::endl;

	//------------------------------------------------------------------------

	std::chrono::system_clock::time_point t3(std::chrono::system_clock::now());

	for (z = 0; z < reps; z++)
	{
		for (i = 0; i < size; i++)
			pq.push(inserts[i]);

		std::cout << z << std::endl;

		for (i = 0; i < size; i++)
		{
			deletes[i] = pq.top();
			pq.pop();
		}
	}

	std::chrono::system_clock::time_point t4(std::chrono::system_clock::now());

	std::cout << "duration: " << (t4 - t3).count() << std::endl;

	///* read input */
	//fp = fopen("input.txt", "r");
	//while (fscanf(fp, "%c", &op) == 1){
	//	switch(op){
	//	case 'i':
	//		fscanf(fp," %d\n", &value);
	//		insert(heap, value);
	//		inserts[icount] = value;
	//		icount++;
	//		break;
	//	case 'd':
	//		fscanf(fp,"\n");
	//		value = deletemin(heap);
	//		deletes[dcount] = value;
	//		dcount++;
	//		break;
	//	case 'p':
	//		fscanf(fp,"\n");
	//		printsoftheap(heap);
	//		break;
	//	default:
	//		break;
	//	}
	//}

	//fclose(fp);

	//std::remove("output.txt");
	//std::ofstream output("output.txt", std::ios_base::app);

	//for (i = 0; i < 10; i++)
	//{
	//	//output << deletes[i] << "\n";
	//	std::cout << deletes[i] << std::endl;
	//}

	//output.close();

	return 0;
}
