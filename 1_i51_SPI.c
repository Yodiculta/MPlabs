#include "SPI.h"

void SPI_init()
{
	SPCR = 0x5C;			// �������� SPI: ����� �������, ������� �������� ��������
							// �� �������� ���� � ��������
}

unsigned char SPI_rw(const unsigned char w_data)
{
	SPDR = w_data;			//	��������� SPI �����
	while(!(SPSR & 0x80));	// ���� ���� �������� ������ ����� ���������
	return SPDR;
}

void SPI_ReadArray(unsigned char num, unsigned char *data)
{
   SPI_PORTX &= ~(1<<SPI_SS); 
   while(num--){
      SPDR = *data;
      while(!(SPSR & (1<<SPIF)));
      *data++ = SPDR; 
   }
   SPI_PORTX |= (1<<SPI_SS); 
}


