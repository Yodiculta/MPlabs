#include <stdio.h>
#include <intrins.h>
#include <string.h>
#define MAX_SCAN_STEP 4
#define MAX_KEY_COUNT 2
#define KEY_FREQ_DEVIDER 2
#define FREQ_DEVIDER 20
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
bit scan_start = 0;
bit key_pressed = 0;
bit key_prev = 0; 
unsigned char key_number = 0;
unsigned char scan_step = 0;
unsigned char scan_code[] = {0xFE,0xFD,0xFB,0xF7};
unsigned char key_count = 0;
unsigned char devider = 0;
unsigned char key_devider = 0;
unsigned char i; 

void Timer0_ISR(void) interrupt 1 using 1 {
    TF2 = 0;

    if( !scan_start){
        // ������ ������� �� ��� ������
        P2 = 0xF0;
        if( P2 != 0xF0 ){
            if( !key_pressed ){
                scan_start = 1;
            }
        }else{
            if( key_pressed ){
                ++key_count;
                if(key_count == MAX_KEY_COUNT){
                    key_count = 0;
                    key_prev = key_pressed;
                    key_pressed = 0;
                }
            }
        }
    }else{
        // ���������������� ������ ������� �� ������ �� �����
        ++key_devider;
        if(key_devider == KEY_FREQ_DEVIDER){
            key_devider = 0;
            P2 = scan_code[scan_step];
            if( P2 != scan_code[scan_step] ){
                key_number = KEY_getkeynumber(~P2);
                if((key_number <= 3) && (key_number > 0))
                {
                    ++key_count;
                    if(key_count == MAX_KEY_COUNT){
                        key_count = 0;
                        key_prev = key_pressed;
                        key_pressed = 1;
                    }
    
                }
            }    
            ++scan_step;
            if(scan_step == MAX_SCAN_STEP){
                scan_start = 0;
                scan_step = 0;
            }
        } else{
             P2 = 0xFF;
        }
    }

     // ���� ������� ������
    if( key_pressed && !key_prev ){ 
	i=0;
	while(i!=3)
	{						//]��������� �������
	signal_first();
	signal_second();
	i++;
	}
        	if( key_number <= 9 )   
		{	
			
			LCD_printchar( keynumber + '0' ); 	// ������� �� �� �������
			keyCount++;

			if( keyCount == LCD_WIDTH )
				LCD_gotoxy( 0, 1 );
			
			LED7_setdigit( KEY_getkeynumber( key ) ); // ������� �� �������������� ���������
			KEY_waitforkeyrelease();
		}
	if( key == KEY_ASTERISK )
		{

}			
	else
		LCD_printchar(key_number) 			// ������� �� �� �������

	
    }

    ++devider;

    if(devider == FREQ_DEVIDER){
         devider = 0;
     }
/*
    // ���� ������� ��������
    if(    !key_pressed && key_prev ){
        do_smth();    	//������ ���-�� ��� ������� (��������, ���������� �������)
        }
        key_prev = key_pressed;
    }
*/
} 


/*
��� ������� ��������� �� ����� ��������� ��������� ��������� ������� �1�. //////////+
��� ������� �� ����� ������ ���������� PC ������ ���� 3 ������� ���-���������� 100 �� � ������ 100 ��, +
����� ����� 300 �� � 3 ������� �������-������ 300 �� � ������ 100 ��, ����� ����� 300 �� - 3 ������� ���������-����  +
100 �� � ������ 100 ��, � ������ ��������� �� ��� ������. ��� ����-��� �� ������ 0-9 ��������������� �����, ������ 
��������� �������, ��-������� ��� �� �������������� ���������.
*/

void signal_first()
{
	Beep( 300 );
	DELAY_MCS(100);

}
void signal_second()
{
	Beep( 100 );
	DELAY_MCS(100);
}

// ����� ����������� �� LCD �����

void OutputStartLCDMessage()
{
	unsigned char mes_line1[] = { 45, 61, 67, 84, 69, 72, 224, 32, 32, 80, 75, 45, 49, 48, 61, 45, 0 };
	unsigned char mes_line2[] = "Zadacha 1";

	LCD_print( mes_line1 );
	LCD_gotoxy( 0, 1 );
	LCD_print( mes_line2 );
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
	printf( "Enter key:" );
	Beep( 300 );
}   
