#ifndef ROWNOLEGLA_PAMIEC_WSPOLNA_H
#define ROWNOLEGLA_PAMIEC_WSPOLNA_H


namespace RPW
{

int const n=5;

void DecryptECB(int blockCount, char* prepText, long* k);

void EncryptECB(int blockCount, char* prepText, long* k);

void DecryptCTP(int blockCount, char* prepText, long* k, long initCounter);

void EncryptCTP(int blockCount, char* prepText, long* k, long initCounter);

void btea(long* v, long n, long* k) ;
}

#endif //ROWNOLEGLA_PAMIEC_WSPOLNA_H