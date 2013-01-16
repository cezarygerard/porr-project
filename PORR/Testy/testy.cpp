#include "sekwencyjna.h"
#include  "rownoleglaPamiecWspolna.h"

#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>
#include <time.h>
#include <windows.h>
#include <iostream>

void test(long textSize, int numberOfThreads)
{
	time_t start,end;
	double dif;

	const int n = SEK::n;
	double PCFreq = 0.0;
	LARGE_INTEGER startPQ, stopPQ, freq;
	QueryPerformanceFrequency(&freq);
	FILE * pFile;
	printf("\ntextsize: %d numberOfThreads: %d", textSize, numberOfThreads);



	char * file_buffer = (char  *)malloc(textSize);
	if (NULL == file_buffer) {
		printf("\ntext malloc failed with textsize: %d", textSize);
		return;
	}

	pFile = fopen ( "test_file_in" , "rb" );
	if (NULL == pFile) {
		printf("\nblad odczytu pliku  test_file_in, nacisnij enter aby kontynuowac z losowymi danymi");
		getchar();		
	}
	else
	{
		size_t  result = fread (file_buffer,1,textSize,pFile);
		if (ferror (pFile)) 
		{
			printf ("Reading error, %d",errno);
		}
	}

	//OMP settings
	//threads count
	omp_set_num_threads(numberOfThreads);
	//Dynamic threads count change allow
	omp_set_dynamic(1);
	//Nested parallel sections allow
	omp_set_nested(1);

	int t = omp_get_max_threads();
	//init
	long k[] = {45, 23456, 367, 1245678};
	int blockCount;
	int restBytes;
	int restBytesOld;

	long initCounter = 13;
	int oldLength = textSize;
	//define block count
	if(oldLength >= n*4)
	{
		blockCount = (oldLength/(n*4));
		restBytesOld = oldLength - (n*4*blockCount);
		restBytes = oldLength - (n*4*blockCount);
		if(restBytes>0)
		{
			while(restBytes % (4*n) != 0)
			{
				restBytes++;
			}
		}
	}
	else
	{
		//log error
		return;
	}

	//Prepare text
	char *prepText = (char  *)malloc((blockCount*n*4 + restBytes));
	if (NULL == prepText) {
		printf("\nprepText malloc failed with textsize: %d", textSize);
		return;
	}

	for(int i = 0; i < (blockCount*n*4 + restBytes); i++)
	{
		if( i < (blockCount*n*4+restBytesOld - 1))
		{
			prepText[i] = file_buffer[i];
		}
		else if( i == ((blockCount*n*4 + restBytes) - 1))
		{
			prepText[i] = '\0';
		}
		else 
		{
			prepText[i] = '!';
		}
	}

	if(restBytes>0)
	{
		blockCount++;
	}

	if(numberOfThreads==2)
	{
		QueryPerformanceCounter(&startPQ);
		SEK::EncryptECB(blockCount,  prepText,  k);
		QueryPerformanceCounter(&stopPQ);
		dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
		printf("\n SEK EncryptECB  time: %f", dif);

		QueryPerformanceCounter(&startPQ);
		SEK::DecryptECB(blockCount,  prepText,  k);
		QueryPerformanceCounter(&stopPQ);
		dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
		printf("\n SEK DecryptECB  time: %f", dif);

		QueryPerformanceCounter(&startPQ);
		SEK::EncryptCTP(blockCount, prepText, k, initCounter);
		QueryPerformanceCounter(&stopPQ);
		dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
		printf("\n SEK EncryptCTP  time: %f", dif);

		QueryPerformanceCounter(&startPQ);
		SEK::DecryptCTP(blockCount, prepText, k, initCounter);
		QueryPerformanceCounter(&stopPQ);
		dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
		printf("\n SEK DecryptCTP  time: %f", dif);
	}

	//t = omp_get_max_threads();

	QueryPerformanceCounter(&startPQ);
	RPW::EncryptECB(blockCount,  prepText,  k);
	QueryPerformanceCounter(&stopPQ);
	dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
	printf("\n RPW EncryptECB  time: %f", dif);

	QueryPerformanceCounter(&startPQ);
	RPW::DecryptECB(blockCount,  prepText,  k);
	QueryPerformanceCounter(&stopPQ);
	dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
	printf("\n RPW DecryptECB  time: %f", dif);

	QueryPerformanceCounter(&startPQ);
	RPW::EncryptCTP(blockCount, prepText, k, initCounter);
	QueryPerformanceCounter(&stopPQ);
	dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
	printf("\n RPW EncryptCTP  time: %f", dif);

	QueryPerformanceCounter(&startPQ);
	RPW::DecryptCTP(blockCount, prepText, k, initCounter);
	QueryPerformanceCounter(&stopPQ);
	dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
	printf("\n RPW DecryptCTP  time: %f", dif);

	free(file_buffer);
	free(prepText);
}

int main(int argc, char ** argv[])
{

	assert(SEK::n==RPW::n);
	const int n = SEK::n;
	long long textSize = 64;
	long long maxTextSize = 1e9;
	int numberOfThreads = 1;
	int maxNumberOfThreads = 16;

	//omp_set_num_threads(100);
	//int t = omp_get_max_threads();
	for( ; textSize <= maxTextSize && textSize >0; textSize*=2)
	{
		for(; numberOfThreads <=maxNumberOfThreads; numberOfThreads+=1)
		{
			test(textSize,numberOfThreads);
		}
		numberOfThreads = 1;
	}

	int a;
	std::cin>>a;
}
