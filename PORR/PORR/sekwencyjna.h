#ifndef SEKWENCYJNA_H
#define SEKWENCYJNA_H

namespace SEK
{
int const n=5;

void DecryptECB(int , char* , long* );

void EncryptECB(int blockCount, char* prepText, long* k);

void DecryptCTP(int blockCount, char* prepText, long* k, long initCounter);

void EncryptCTP(int blockCount, char* prepText, long* k, long initCounter);

void btea(long* v, long n, long* k) ;
}
#endif //SEKWENCYJNA_H