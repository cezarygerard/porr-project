//#include "stdafx.h"

#include "rownoleglaPamiecWspolna.h"

#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define MX  ((((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[(p&3)^e]^z)))

using namespace RPW;

//XXTEA n>1 - Decrypt; n<-1 - Encrypt;
void RPW::btea(long* v, long n, long* k) 
{
	unsigned long z, y=v[0], sum=0, e, DELTA=0x9e3779b9;
	long p, q ;
	if (n > 1) 
	{          
		z=v[n-1];           
		for (q = 6+52/n; q > 0;q--) 
		{
			sum += DELTA;
			e = sum >> 2&3 ;
			for (p = 0; p < n-1; p++)
			{
				y = v[p+1];
				z = v[p] += MX;
			}
			y = v[0];
			z = v[n-1] += MX;
		}
	} else if (n < -1) 
	{  
		n = -n ;
		q = 6+52/n ;
		for (sum = q*DELTA ; sum != 0; sum -= DELTA) 
		{
			e = sum>>2 & 3;
			for (p = n-1; p > 0; p--) 
			{
				z = v[p-1];
				y = v[p] -= MX;
			}
			z = v[n-1];
			y = v[0] -= MX;
		}
	}
}

void RPW::DecryptECB(int blockCount, char* prepText, long* k)
{
	int i, j;
	#pragma omp parallel shared(n, k, prepText, blockCount, i) private(j)
	{
		//int t = omp_get_max_threads();
		//t = omp_get_num_threads();
		#pragma omp for
		for(i=0; i < blockCount; i++)
		{
		//	t = omp_get_num_threads();
			char blockText[n*4];
			for(j = 0; j< n*4; j++)
			{
				blockText[j] = prepText[i*n*4+j];
			}
			//convert to long[]
			long *cipherkArray = (long *)blockText;
			btea(cipherkArray, n, k);
			//Save
			char* cipherkArrayChar = (char*)cipherkArray;
			for(j = 0; j< n*4; j++)  
			{
				prepText[i*n*4+j] = cipherkArrayChar[j];
			}
		}
		#pragma omp barrier
		
	}
}

void RPW::EncryptECB(int blockCount, char* prepText, long* k)
{
	int i, j;
	#pragma omp parallel shared(n, k, prepText, blockCount, i) private(j)
	{
		#pragma omp for
		for(i=0; i<blockCount; i++)
		{
			char blockText[n*4];
			for(j = 0; j< n*4; j++)
			{   
				blockText[j] = prepText[i*n*4+j];
			}
			//convert to long[]
			long *cipherkArray = (long *)blockText;
			btea(cipherkArray, -n, k);
			//Save
			char* cipherkArrayChar = (char*)cipherkArray;
			for(j = 0; j< n*4; j++)
			{
				prepText[i*n*4+j] = cipherkArrayChar[j];
			}
		}
		#pragma omp barrier
	}
}


void RPW::DecryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
	#pragma omp parallel shared(n, k, prepText, blockCount, i) private(j)
	{
		#pragma omp for
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
		#pragma omp barrier
		
	}
}

void RPW::EncryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
	#pragma omp parallel shared(n, k, prepText, blockCount, i) private(j)
	{
		#pragma omp for
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
		#pragma omp barrier
	}
}

