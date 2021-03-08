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
Задача 2.
При запуске программы на экран терминала выводится сообщение «Задача №2».
При нажатии клавиши 1 клавиатуры PC предлагается ввести время и дату, кото-рые затем устанавливаются как текущие в RTC DS11507. При нажатии клавиши 2 на ЖКИ стенда в первой строке выводится текущее время, во второй дата. При нажатии клавиши 3 происходит очистка ЖКИ.
Нажатие на другие клавиши игнорируется.

*/
// â ïîñëåäîâàòåëüíûé ïîðò âûâîäèòüñÿ òåêóùåå âðåìÿ,
//			 ïîëó÷åííîå ñ äàò÷èêà DS1307

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

//â òåðìèíàëüíîé ïðîãðàììå ïîëüçîâàòåëü ââîäèò âðåìÿ è 
//           äàòó è îíè óñòàíàâëèâàþòñÿ êàê òåêóùèå â äàò÷èê DS1307

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


// Âûâîä ïðèâåòñòâèÿ íà LCD ýêðàí

void OutputStartLCDMessage()
{
	unsigned char mes_line1[] = { 45, 61, 67, 84, 69, 72, 224, 32, 32, 80, 75, 45, 49, 48, 61, 45, 0 };
	unsigned char mes_line2[] = "Zadacha 2";

	LCD_print( mes_line1 );
	LCD_gotoxy( 0, 1 );
	LCD_print( mes_line2 );
}


// Îáðàáîò÷èê ïðåðûâàíèé îò UART

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
    
    P0 = 0xFF;				// Íàñòðàèâàåì ïîðòû íà âûâîä 
	P1 = 0xFF;
	P2 = 0xFF;	
	P3 = 0xFF;

	SCON  = 0x50;			// 8-áèòîâûé UART
	TMOD  = 0x20;			// Òàéìåð 1: 8-áèòîâûé ðåæèì, àâòî-ïåðåçàãðóæàåìûé 
	TH1   = 250;			// Çàäàåì íà÷àëüíîå çíà÷åíèå òàéìåðà äëÿ ñêîðîñòè 9600 áîä
	TL1   = 250;			
	TR1   = 1;				// Çàïóñêàåì òàéìåð
	TI    = 1;
	ES=1;                   // Ðàçðåøàåì ïðåðûâàíèå îò UART

	printf ("                -=  STEND RK-10  =-\n");			// Ïîñûëàåì â UART ñîîáùåíèå 
	printf ("       Hardware ver. 1.0, Firmware ver. 1.0\n");

	DELAY_MCS( 174 );

	SPI_init();             // Èíèöèàëèçèðóåì SPI èíòåðôåéñ

	I2C_init();				// Èíèöèàëèçèðóåì I2C èíòåðôåéñ
	DS1307_init();          // Èíèöèàëèçèðóåì ÷àñû ðåàëüíîãî âðåìåíè
	DS1820_startconverttemp();
   
	LCD_init();				// Èíèöèàëèçèðóåì LCD äèñïëåé
	LCD_clrscr();			// Î÷èùàåì ýêðàí LCD äèñïëåÿ

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
