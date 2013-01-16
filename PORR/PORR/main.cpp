#include "sekwencyjna.h"


#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

using namespace SEK;

int main(int argc, char ** argv[])
{
	//init
	char text[] = "My card number 2345678890-09786-09786, and password - 1234567890 --padding----padding----padding----padding----padding----padding----padding----pa";
	long k[] = {45, 23456, 367, 1245678};
	int blockCount;
	int restBytes;
	int restBytesOld;
	int temp = 0;
	int temp1=0;
	int temp2 = 0;
	long initCounter = 13;
	int oldLength = sizeof(text);
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
	printf ("Encrypt ECB  \n");
	EncryptECB(blockCount,  prepText,  k);
	printf ("%s \n", prepText);

	printf ("\nDecrypt ECB  \n");
	DecryptECB(blockCount,  prepText,  k);
	printf ("%s \n", prepText);	

	printf ("\nEncrypt CTP \n");
	EncryptCTP(blockCount, prepText, k, initCounter);
	
	printf ("%s \n", prepText);	
	printf ("\nDecrypt CTP\n");
	DecryptCTP(blockCount, prepText, k, initCounter);

	printf ("%s \n", prepText);	
	free(prepText);
	return 0;
}
