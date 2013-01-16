#include "rownoleglaPamiecWspolna.h"

#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

using namespace RPW;

int main(int argc, char ** argv[])
{
	  //Text
	//char text[] = "My card number 2345678890-09786-09786, and password - 1234567890 --padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding----padding--";
	char text[] = "My card number 2345678890-09786-09786, and password - 1234567890 --padding----padding";

	  //Key
	long k[] = {45, 23456, 367, 1245678};


	
	//init counter for CTP
	 long initCounter = 3;
	
	//OMP settings
	//Dynamic threads count change allow
	omp_set_dynamic(1);
	 //threads count
	omp_set_num_threads(30);

	//Nested parallel sections allow
	omp_set_nested(1);

	//Temp variables
	int blockCount;
	int restBytes;
	int restBytesOld;
	int oldLength = sizeof(text);


	//Define block count
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
		return 1;
	}


	//Prepare text
	char *prepText = (char  *)malloc((blockCount*n*4 + restBytes));
	for(int i = 0; i < (blockCount*n*4 + restBytes); i++)
	{
		if( i < (blockCount*n*4+restBytesOld - 1))
		{
			prepText[i] = text[i];
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

	printf ("%s \n", prepText);
	//Encrypt ECB
	EncryptECB(blockCount, prepText, k);
	printf ("%s \n", prepText);
	//Decrypt ECB
	DecryptECB(blockCount, prepText, k);
	printf ("%s \n", prepText);
	//Encrypt CTP
	EncryptCTP(blockCount, prepText, k, initCounter);
	printf ("%s \n", prepText);
	//Decrypt CTP
	DecryptCTP(blockCount, prepText, k,initCounter);
	printf ("%s \n", prepText);

	free(prepText);
	return 0;
}

