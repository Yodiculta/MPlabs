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
При нажатии клавиши 1 клавиатуры PC предлагается ввести время и дату, ко-торые затем устанавливаются как текущие в RTC DS1307. При нажатии кла-виши 2 на ЖКИ стенда в первой строке выводится текущее время, во второй дата. При нажатии клавиши 3 происходит очистка ЖКИ.
Нажатие на другие клавиши игнорируется.
*/
// в последовательный порт выводиться текущее время,
//			 полученное с датчика DS1307

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

//в терминальной программе пользователь вводит время и 
//           дату и они устанавливаются как текущие в датчик DS1307

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


// Вывод приветствия на LCD экран

void OutputStartLCDMessage()
{
	unsigned char mes_line1[] = { 45, 61, 67, 84, 69, 72, 224, 32, 32, 80, 75, 45, 49, 48, 61, 45, 0 };
	unsigned char mes_line2[] = "Zadacha 2";

	LCD_print( mes_line1 );
	LCD_gotoxy( 0, 1 );
	LCD_print( mes_line2 );
}


// Обработчик прерываний от UART

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
    
    P0 = 0xFF;				// Настраиваем порты на вывод 
	P1 = 0xFF;
	P2 = 0xFF;	
	P3 = 0xFF;

	SCON  = 0x50;			// 8-битовый UART
	TMOD  = 0x20;			// Таймер 1: 8-битовый режим, авто-перезагружаемый 
	TH1   = 250;			// Задаем начальное значение таймера для скорости 9600 бод
	TL1   = 250;			
	TR1   = 1;				// Запускаем таймер
	TI    = 1;
	ES=1;                   // Разрешаем прерывание от UART

	printf ("                -=  STEND RK-10  =-\n");			// Посылаем в UART сообщение 
	printf ("       Hardware ver. 1.0, Firmware ver. 1.0\n");

	DELAY_MCS( 174 );

	SPI_init();             // Инициализируем SPI интерфейс

	I2C_init();				// Инициализируем I2C интерфейс
	DS1307_init();          // Инициализируем часы реального времени
	DS1820_startconverttemp();
   
	LCD_init();				// Инициализируем LCD дисплей
	LCD_clrscr();			// Очищаем экран LCD дисплея

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
