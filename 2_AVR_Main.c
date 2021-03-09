#include "BEEP.H"
#include "DELAY.H"
#include "SPI.H"
#include "LCD_HD44780.H"
#include "I2C.H"
#include "KEYBOARD.H"
#include "LED7.H"
#include "UART.H"
#include "DS1307.H"
#include "DS1820.H"
#include "AT24CXX.H"
#include "CONVERT.H"
#include "SHIFT.H"
#include "ADC.H"

// Строковые константы, которые храняться в flash-памяти, чтобы избежать
// переполнения оперативной памяти

flash char UartMessage[] = "                -=  STEND RK-10  =-\r\n"
						   "       Hardware ver. 1.0, Firmware ver. 2.0\r\n";

flash unsigned char LcdMessageLine1[] = { 45, 61, 67, 84, 69, 72, 224, 32, 32, 80, 75, 45, 49, 48, 61, 45, 0 };
flash unsigned char LcdMessageLine2[] = "Zadacha 2";

flash char timeStr[] = "Time: ";
flash char dateStr[] = "Date: ";
flash char enterAddressStr[] = "Enter address(0..255): ";
flash char enterValueStr[] = "Enter value(0..255): ";
flash char valueStr[] = "Value: ";
flash char temperatureStr[] = "Temperature: ";

flash char enterHourStr[] = "Enter hour: ";
flash char enterMinuteStr[] = "Enter minute: ";
flash char enterSecondStr[] = "Enter second: ";
flash char enterDayStr[] = "Enter day: ";
flash char enterMonthStr[] = "Enter month: ";
flash char enterYearStr[] = "Enter year: ";
flash char sensorNotConnectedStr[] = "Sensor is not connected";


void DateTimeOutput()
{
    	char time[9], date[9];
	DS1307_gettime(time);
	DS1307_getdate(date);

	UART_sendstring_flash( timeStr );
	UART_sendstring( time );
	UART_sendcrlf();
	UART_sendstring_flash( dateStr );
	UART_sendstring( date );
	UART_sendcrlf();
}

void DateTimeInputAndSetCurrent()
{
    unsigned char hour, minute, second;
	unsigned char day, month, year;
	
	UART_sendstring_flash( enterHourStr );
	hour = UART_receivevalue();	
	UART_sendstring_flash( enterMinuteStr );
	minute = UART_receivevalue();	
	UART_sendstring_flash( enterSecondStr );
	second = UART_receivevalue();	
	
	DS1307_settime( hour, minute, second );

	UART_sendstring_flash( enterDayStr );
	day = UART_receivevalue();	
	UART_sendstring_flash( enterMonthStr );
	month = UART_receivevalue();	
	UART_sendstring_flash( enterYearStr );
	year = UART_receivevalue();	
	
	DS1307_setdate( day, month, year );	
}


void OutputStartLCDMessage()
{
	LCD_print_flash( LcdMessageLine1 );
	LCD_gotoxy( 0, 1 );
	LCD_print_flash( LcdMessageLine2 );
}

// Функция-обработчик прерывания, возникающего когда во входном буфере UART
// имеются данные.
// В этой функции мы получаем номер задания и выполняем его.

#pragma vector = UART_RXC_vect
__interrupt void UART0_RX_interrupt()
{
	unsigned char task;
	task = UART_receivevalue();
	if(RI){	
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
		UART_sendstring( "Enter task number: " );
	}

// Функция автономного режима, то есть режима, когда нет подключения к
// компьютеру через UART-интерфейс.
// На LCD-экран выводятся:
// 		в первой строке - текущая температура и текущее время
//		во второй строке - текущая дата
// LED индикаторы меняют свое состояние
void AutonomousMode()
{
	LCD_clrscr();
	
	static char strTemp[ 5 ] = { 0 };
	LCD_gotoxy( 0, 0 );

	switch( DS1820_gettemperature( strTemp ) ) {	// Получаем температуру
	case 0:											// Сенсор не подключен
		strTemp[ 0 ] = 0;
		break;
	case 2:											// Данные не готовы
		DS1820_startconverttemp();
		break;
	}
	LCD_print( strTemp );

	char time[9];
	DS1307_gettime(time);
	
	LCD_gotoxy( 6, 0 );
	LCD_print( time );

	
	LCD_gotoxy( 0, 1 );
	char keyname = KEY_getkeyname( KEY_getkey() );
	if( keyname != 0 )
		LCD_printchar( keyname );
	
	
	LCD_gotoxy( 3, 1 );
	char strADC1[ 5 ];
	ShortToString( ADC_get( 0 ), strADC1 );
	LCD_print( strADC1 );
	
	LCD_gotoxy( 9, 1 );
	char strADC2[ 5 ];
	ShortToString( ADC_get( 1 ), strADC2 );
	LCD_print( strADC2 );
	
	static unsigned char ledCount = 0;
	if( ledCount % 3 == 0 )							// Чтобы индикаторы слишком
	{									// часто не мигали, меняем
		LED1_PORT = !LED1_PORT;						// их состояние один раз на
		LED2_PORT = !LED2_PORT;						// три вызова функции
		LED3_PORT = !LED3_PORT;
	}
	ledCount++;
}



int main()
{
	LED1_DDR = 1;
	LED2_DDR = 1;
	LED3_DDR = 1;
	
	I2C_init();
	SPI_init();
	LCD_init();
	LCD_clrscr();

	UART_init( CALC_UBRR( 57600 ) );
	__enable_interrupt();

	DS1820_startconverttemp();
	
	UART_sendstring_flash( UartMessage );
	OutputStartLCDMessage();

	UART_sendstring( "Enter task number: " );

	Beep( 500 );

	DELAY_MS( 1000 );

	while(1)
	{
		AutonomousMode();
                SHIFT_write(0);
		DELAY_MS( 200 );
	}

}
