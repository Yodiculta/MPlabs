#ifndef _SPI_H_
#define _SPI_H_

#include "PINDEF.H"

void SPI_init();
unsigned char SPI_rw(const unsigned char w_data);
void SPI_ReadArray(unsigned char num, unsigned char *data);
#endif
