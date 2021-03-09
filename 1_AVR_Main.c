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
flash unsigned char LcdMessageLine2[] = "Zadacha 1";

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

void Timer0_ISR(void) interrupt 1 using 1 {
    TF2 = 0;

    if( !scan_start){
        // ïîäà÷à ñèãíàëà íà âñå ñòðîêè
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
        // ïîñëåäîâàòåëüíàÿ ïîäà÷à ñèãíàëà íà êàæäóþ èç ñòðîê
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

     // åñëè êëàâèøà íàæàòà
    if( key_pressed && !key_prev ){ 
	i=0;
	while(i!=3)
	{						//]âêëþ÷åíèå çóììåðà
	signal_first();
	signal_second();
	i++;
	}
        	if( key_number <= 9 )   
		{	
			
			LCD_printchar( keynumber + '0' ); 	// âûâîäèì íà ÆÊ äèñïëåé
			keyCount++;

			if( keyCount == LCD_WIDTH )
				LCD_gotoxy( 0, 1 );
			
			LED7_setdigit( KEY_getkeynumber( key ) ); // âûâîäèì íà ñåìèñåãìåíòíûé èíäèêàòîð
			KEY_waitforkeyrelease();
		}
	if( key == KEY_ASTERISK )
		{

}			
	else
		LCD_printchar(key_number) 			// âûâîäèì íà ÆÊ äèñïëåé

	
    }

    ++devider;

    if(devider == FREQ_DEVIDER){
         devider = 0;
     }

} 


/*
Ïðè çàïóñêå ïðîãðàììû íà ýêðàí òåðìèíàëà âûâîäèòñÿ ñîîáùåíèå «Çàäà÷à ¹1». //////////+
Ïðè íàæàòèè íà ëþáóþ êíîïêó êëàâèàòóðû PC çóììåð äàåò 3 ñèãíàëà äëè-òåëüíîñòüþ 100 ìñ ñ ïàóçîé 100 ìñ, +
ïîñëå ïàóçû 300 ìñ – 3 ñèãíàëà äëèòåëü-íîñòüþ 300 ìñ ñ ïàóçîé 100 ìñ, ïîñëå ïàóçû 300 ìñ - 3 ñèãíàëà äëèòåëüíî-ñòüþ  +
100 ìñ ñ ïàóçîé 100 ìñ, è ñèìâîë âûâîäèòñÿ íà ÆÊÈ ñòåíäà. Ïðè íàæà-òèè æå êëàâèø 0-9 ñîîòâåòñòâóþùèå öèôðû, ïîìèìî 
çâóêîâîãî ñèãíàëà, âû-âîäÿòñÿ åùå íà ñåìèñåãìåíòíûé èíäèêàòîð.
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

void OutputStartLCDMessage()
{
	LCD_print_flash( LcdMessageLine1 );
	LCD_gotoxy( 0, 1 );
	LCD_print_flash( LcdMessageLine2 );
}
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
