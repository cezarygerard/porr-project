#include <limits.h>
#include <conio.h>
#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "mpi.h"
#include <time.h>
#include <windows.h>

#define MX  ((((z>>5)^(y<<2))+((y>>3)^(z<<4)))^((sum^y)+(k[(p&3)^e]^z)))

int const n=2;

//XXTEA n>1 - Decrypt; n<-1 - Encrypt;
void btea(long* v, long n, long* k) 
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

void DecryptECB(int blockCount, char* prepText, long* k)
{
	int i, j;
	for(i=0; i < blockCount; i++)
	{
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
}

void EncryptECB(int blockCount, char* prepText, long* k)
{
	int i, j;
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
}


void DecryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
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

void EncryptCTP(int blockCount, char* prepText, long* k, long initCounter)
{
	int i, j;
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


int main(int argc, char * argv[])
{
	//MPI init
	int Tag = 0;
	int rank;
	int size;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
    //END MPI init

	if(rank == 0) // ONLY for main thread
	{

		LARGE_INTEGER startPQ, stopPQ, freq;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&startPQ);
		
		//Text
		//char text[] = "My card number 1234567890, and password - 0987654321";	
		int textSize =  atoi(argv[ 1 ]);
		char * file_buffer = (char  *)malloc(textSize);
		
		if (NULL == file_buffer) {

			printf("\ntext malloc failed with textsize: %d", textSize);
			return 1 ;
		}
		FILE * pFile = fopen ( "test_file_in" , "rb" );
		if (NULL == pFile) {
			printf("\nblad odczytu pliku  test_file_in, losowe danymi");
		
		}
		else
		{
			size_t  result = fread (file_buffer,1,textSize,pFile);
			if (ferror (pFile)) 
			{
				printf ("Reading error, %d",errno);
			}

		}
		
		
		
		//Key
		long k[] = {45, 23456, 367, 1245678};
		//init counter for CTP
		long initCounter = 3;
		//Temp variables
		int blockCount;
		int restBytes;
		int restBytesOld;
		int oldLength = textSize;

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

		//Send key
		Tag = 0;
		for(int proc=1; proc < size; proc++)
		{

			MPI_Send (k, 4, MPI_LONG, proc, Tag/*=0*/, MPI_COMM_WORLD);
		}

		//send Init Counter
		for(int proc=1; proc < size; proc++)
		{

			MPI_Send (&initCounter, 1, MPI_LONG, proc, 10/*=10*/, MPI_COMM_WORLD);
		}

		//Define and send block count and text for each process
		Tag = 1;
		int blockCountForProcess;
		int blockCountForLastProcess;
		if(blockCount<=size-1)
		{
			blockCountForProcess = 1;
			for(int proc=1; proc < size; proc++)
			{   
				if(blockCount>0)
				{
				  char *prepTextForSending = (char  *)malloc(n*4);
				  for(int b = 0; b < n*4; b++)
				  {
					  prepTextForSending[b] = prepText[((proc-1)*n*4+b)];
				  }
				  MPI_Send (&blockCountForProcess, 1, MPI_INT, proc, Tag/*=1*/, MPI_COMM_WORLD);
				  MPI_Send (prepTextForSending, n*4, MPI_CHAR, proc, 2/*=2*/, MPI_COMM_WORLD);
				}
				else
				{
				  blockCountForProcess = 0;
                  MPI_Send (&blockCountForProcess, 1, MPI_INT, proc, Tag/*=1*/, MPI_COMM_WORLD);
				  MPI_Send (NULL, 0, MPI_CHAR, proc, 2/*=2*/, MPI_COMM_WORLD);
				}
			    blockCount--;
			}
		}
		else
		{
			if((blockCount % (size-1)) == 0)
			{
				blockCountForProcess = blockCount/(size-1);
				blockCountForLastProcess = blockCount/(size-1);
			}
			else
			{
				blockCountForProcess = blockCount/(size-2);
				blockCountForLastProcess = blockCount - blockCountForProcess*(size-2); 
			}
			
			for(int proc=1; proc < size; proc++)
			{   
				if(proc < size-1)
				{
				  char *prepTextForSending = (char  *)malloc(n*4*blockCountForProcess);
				  for(int b = 0; b < n*4*blockCountForProcess; b++)
				  {
					  prepTextForSending[b] = prepText[((proc-1)*n*4*blockCountForProcess+b)];
				  }
					 MPI_Send(&blockCountForProcess, 1, MPI_INT, proc, Tag/*=1*/, MPI_COMM_WORLD);
				     MPI_Send (prepTextForSending, n*4*blockCountForProcess, MPI_CHAR, proc, 2/*=2*/, MPI_COMM_WORLD);
				}
				else
				{
				  char *prepTextForSending = (char  *)malloc(n*4*blockCountForLastProcess);
				  for(int b = 0; b < n*4*blockCountForLastProcess; b++)
				  {
					  prepTextForSending[b] = prepText[((proc-1)*n*4*blockCountForProcess+b)];
				  }
					MPI_Send(&blockCountForLastProcess, 1, MPI_INT, proc, Tag/*=1*/, MPI_COMM_WORLD);
					MPI_Send (prepTextForSending, n*4*blockCountForLastProcess, MPI_CHAR, proc, 2/*=2*/, MPI_COMM_WORLD);
				}
			}
		}
		//Get responses from processes
		int temp = 0;
		for(int proc=1; proc < size; proc++)
		{
			MPI_Status s;
			int bufElems;
			MPI_Probe(proc, 100, MPI_COMM_WORLD,&s);
			MPI_Get_count( &s, MPI_CHAR, &bufElems );
			char *response = (char  *)malloc(bufElems);
			MPI_Recv(response, bufElems, MPI_CHAR, proc,  100,  MPI_COMM_WORLD, &s);

			//concat response from all threads
			for(int cb=0; cb < bufElems; cb++)
			{
				prepText[temp] = response[cb];
				temp++;
			}
		}

		QueryPerformanceCounter(&stopPQ);
		double dif = ((double)(stopPQ.QuadPart-startPQ.QuadPart))/((double)(freq.QuadPart));
		printf("\n time: %f", dif);
		//printf("text='%s'\n", prepText)

		
	}
	else //only for not main threads
	{
		//init
		MPI_Status s;
		long k[4];
		long initCounter;
		int blockCount;

		//Get key
		MPI_Recv(k, 4, MPI_LONG , 0,  0,  MPI_COMM_WORLD, &s); 
		//Get block countd
		MPI_Recv(&blockCount, 1, MPI_INT , 0,  1,  MPI_COMM_WORLD, &s);
		//Get init counter
		MPI_Recv(&initCounter, 1, MPI_LONG , 0,  10,  MPI_COMM_WORLD, &s);
		
		//Get text for process
		char* prepText = (char  *)malloc(blockCount*n*4);
		MPI_Recv(prepText, n*4*blockCount, MPI_CHAR, 0,  2,  MPI_COMM_WORLD, &s);
		//printf("Thread %d, got text %s", rank, prepText);
		
		//Decrypt ECB
		DecryptECB(blockCount, prepText, k);
		//Encrypt ECB
		EncryptECB(blockCount, prepText, k);
		//Decrypt CTP
		DecryptCTP(blockCount, prepText, k, initCounter);
		//Decrypt CTP
		EncryptCTP(blockCount, prepText, k, initCounter);

		//Send response
		MPI_Send (prepText, n*4*blockCount, MPI_CHAR, 0, 100/*=2*/, MPI_COMM_WORLD);
	}

			


	MPI_Finalize();
	return 0;
}

