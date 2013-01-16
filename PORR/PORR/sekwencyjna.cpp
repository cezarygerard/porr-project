#include "sekwencyjna.h"


#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>

#define MX  ( (((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[(p&3)^e]^z)) )

using namespace SEK;

void SEK::btea(long* v, long n, long* k) 
{
	unsigned long z /* = v[n-1] */, y=v[0], sum=0, e, DELTA=0x9e3779b9;
	long p, q ;
	if (n > 1) {          /* Coding Part */
		z=v[n-1];           /* Moved z=v[n-1] to here, else segmentation fault in decode when n < 0 */
		q = 6+52/n ;
		while (q-- > 0) {
			sum += DELTA;
			e = sum >> 2&3 ;
			for (p=0; p<n-1; p++) y = v[p+1], z = v[p] += MX;
			y = v[0];
			z = v[n-1] += MX;
		}
	} else if (n < -1) {  /* Decoding Part */
		n = -n ;
		q = 6+52/n ;
		sum = q*DELTA ;
		while (sum != 0) {
			e = sum>>2 & 3;
			for (p=n-1; p>0; p--) z = v[p-1], y = v[p] -= MX;
			z = v[n-1];
			y = v[0] -= MX;
			sum -= DELTA;
		}
	}
}

void SEK::DecryptECB(int blockCount, char* prepText, long* k)
{
	int temp = 0;
	int temp1=0;
	//Decrypt ECB
	for(int i=0; i<blockCount; i++)
	{
		char blockText[n*4];
		for(int j = 0; j< n*4; j++)
		{
			blockText[j] = prepText[temp];
			temp++;
		}
		//convert to long[]
		long *cipherkArray = (long *)blockText;
		btea(cipherkArray, n, k);
		//Save
		char* cipherkArrayChar = (char*)cipherkArray;
		for(int j = 0; j< n*4; j++)
		{
			prepText[temp1] = cipherkArrayChar[j];
			temp1++;
		}
	}
}

void SEK::EncryptECB(int blockCount, char* prepText, long* k)
{
	int temp = 0;
	int temp1 = 0;
	for(int i=0; i<blockCount; i++)
	{
		char blockText[n*4];
		for(int j = 0; j< n*4; j++)
		{
			blockText[j] = prepText[temp];
			temp++;
		}
		//convert to long[]
		long *cipherkArray = (long *)blockText;
		btea(cipherkArray, -n, k);
		//Save
		char* cipherkArrayChar = (char*)cipherkArray;
		for(int j = 0; j< n*4; j++)
		{
			prepText[temp1] = cipherkArrayChar[j];
			temp1++;
		}
	}

}

void SEK::DecryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
	{
		for(i=0; i < blockCount; i++)
		{
			char blockText[n*4];
			for(j = 0; j< n*4; j++)
			{
				blockText[j] = prepText[i*n*4+j];
			}
			//convert to long[]
			long *cipherkArray = (long *)blockText;
			long preparedKey[4];
			for(int pk=0; pk<4; pk++)
			{
				preparedKey[pk] = k[pk]^(initCounter*(i+1));
			}
			btea(cipherkArray, n, preparedKey);
			//Save
			char* cipherkArrayChar = (char*)cipherkArray;
			for(j = 0; j< n*4; j++)  
			{
				prepText[i*n*4+j] = cipherkArrayChar[j];
			}
		}	
	}
}

void SEK::EncryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
	{
		for(i=0; i<blockCount; i++)
		{
			char blockText[n*4];
			for(j = 0; j< n*4; j++)
			{   
				blockText[j] = prepText[i*n*4+j];
			}
			//convert to long[]
			long *cipherkArray = (long *)blockText;
			long preparedKey[4];
			for(int pk=0; pk<4; pk++)
			{
				preparedKey[pk] = k[pk]^(initCounter*(i+1));
			}
			btea(cipherkArray, -n, preparedKey);
			//Save
			char* cipherkArrayChar = (char*)cipherkArray;
			for(j = 0; j< n*4; j++)
			{ 
				prepText[i*n*4+j] = cipherkArrayChar[j];
			}
		}
	}
}
