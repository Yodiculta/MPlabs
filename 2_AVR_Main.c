#include <stdio.h>
#include <intrins.h>
#include <string.h>

#include "KEYBOARD.H"
#include "SPI.H"
#include "LCD_HD44780.H"
#include "DELAY.H"
#include "LED7.H"
#include "DS1307.H"
#include "DS1820.H"
#include "AT24CXX.H"
#include "BEEP.H"
#include "I2C.H"
#include "SHIFT.H"
/*
��� ������� ������� 1 ���������� PC ������������ ������ ����� � ����, ��-����� ����� ��������������� ��� ������� � RTC DS1307. ��� ������� ���-���� 2 �� ��� ������ � ������ ������ ��������� ������� �����, �� ������ ����. ��� ������� ������� 3 ���������� ������� ���.
������� �� ������ ������� ������������.
*/
// � ���������������� ���� ���������� ������� �����,
//			 ���������� � ������� DS1307

void DateTimeOutput()
{
    	char time[9], date[9];
	DS1307_gettime(time);
	DS1307_getdate(date);

	LCD_gotoxy( 6, 0 );
	LCD_print( time );

	LCD_gotoxy( 6, 1 );
	LCD_print( date );
}

//� ������������ ��������� ������������ ������ ����� � 
//           ���� � ��� ��������������� ��� ������� � ������ DS1307

void DateTimeInputAndSetCurrent()
{
    unsigned char hour, minute, second;
	unsigned char day, month, year;

    printf( "Enter hour: ");
	scanf( "%bu", &hour );
    printf( "Enter minute: ");
	scanf( "%bu", &minute );
    printf( "Enter second: ");
	scanf( "%bu", &second );

	DS1307_settime( hour, minute, second );
	
	printf( "Enter day: ");
	scanf( "%bu", &day );
    printf( "Enter month: ");
	scanf( "%bu", &month );
    printf( "Enter year: ");
	scanf( "%bu", &year );

	DS1307_setdate( day, month, year ); 	
}


// ����� ����������� �� LCD �����

void OutputStartLCDMessage()
{
	unsigned char mes_line1[] = { 45, 61, 67, 84, 69, 72, 224, 32, 32, 80, 75, 45, 49, 48, 61, 45, 0 };
	unsigned char mes_line2[] = "Zadacha 2";

	LCD_print( mes_line1 );
	LCD_gotoxy( 0, 1 );
	LCD_print( mes_line2 );
}


// ���������� ���������� �� UART

void Serial_ISR(void) interrupt 4 using 0 
{
    unsigned char task;

    if(RI){
  	
	scanf( "%bu", &task );

    switch( task ) {
		case 1:
			DateTimeInputAndSetCurrent();
			break;
		case 2:
			DateTimeOutput();
			break;
		case 3:
			LCD_clrscr();
			break;
		}

    printf( "Enter task number:" );}
}


void main()
{   
    
    P0 = 0xFF;				// ����������� ����� �� ����� 
	P1 = 0xFF;
	P2 = 0xFF;	
	P3 = 0xFF;

	SCON  = 0x50;			// 8-������� UART
	TMOD  = 0x20;			// ������ 1: 8-������� �����, ����-��������������� 
	TH1   = 250;			// ������ ��������� �������� ������� ��� �������� 9600 ���
	TL1   = 250;			
	TR1   = 1;				// ��������� ������
	TI    = 1;
	ES=1;                   // ��������� ���������� �� UART

	printf ("                -=  STEND RK-10  =-\n");			// �������� � UART ��������� 
	printf ("       Hardware ver. 1.0, Firmware ver. 1.0\n");

	DELAY_MCS( 174 );

	SPI_init();             // �������������� SPI ���������

	I2C_init();				// �������������� I2C ���������
	DS1307_init();          // �������������� ���� ��������� �������
	DS1820_startconverttemp();
   
	LCD_init();				// �������������� LCD �������
	LCD_clrscr();			// ������� ����� LCD �������

	OutputStartLCDMessage();

    printf( "Enter task number:" );

    Beep( 300 );

    while(1) 
	{ 
	     char keyname;
	     EA=0;	
	     
		 AutonomousMode();
			 
		 keyname = KEY_getkeynumber( KEY_getkey() );
         if( keyname <= 3 )
            LED7_setdigit( keyname );
		 else
		    SHIFT_write(0);

	     EA=1;

		 delay_ms( 10 );
	}
}   
