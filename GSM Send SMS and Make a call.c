/*
 * GSM_Send_SMS_and_Make_a_call.c
 *
 * Created: 7/9/2014 3:34:56 AM
 *  Author: Saroj Shakya
 */ 


#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define LCD_DATAPORT PORTB
#define LCD_DATADDR DDRB
#define LCD_CMNDPORT PORTC
#define LCD_CMNDDDR DDRC
#define LCD_RS 0
#define LCD_RW 1
#define LCD_EN 2
void LCD_cmnd(unsigned char cmnd)
{
	LCD_DATAPORT=cmnd;
	LCD_CMNDPORT &= ~(1<<LCD_RS);
	LCD_CMNDPORT &= ~(1<<LCD_RW);
	LCD_CMNDPORT |= (1<<LCD_EN);
	_delay_us(5);
	LCD_CMNDPORT &= ~(1<<LCD_EN);
	_delay_us(200);
}
void LCD_data(unsigned char data)
{
	LCD_DATAPORT = data;
	LCD_CMNDPORT |= (1<<LCD_RS);
	LCD_CMNDPORT &= ~(1<<LCD_RW);
	LCD_CMNDPORT |= (1<<LCD_EN);
	_delay_us(5);
	LCD_CMNDPORT &= ~(1<<LCD_EN);
	_delay_us(200);
}
void LCD_initialize(void)
{
	LCD_DATADDR = 0xFF;
	LCD_CMNDDDR = 0xFF;
	LCD_CMNDPORT &= ~(1<<LCD_EN);
	_delay_ms(20);
	LCD_cmnd(0x38);
	_delay_ms(20);
	LCD_cmnd(0x0E);
	_delay_ms(20);
	LCD_cmnd(0x01);
	_delay_ms(20);
}
void LCD_clear(void)
{
	LCD_cmnd(0x01);
	_delay_ms(2);
}
void LCD_print(char * str)
{
	unsigned char i=0;
	while(str[i] != 0)
	{
		LCD_data(str[i]);
		i++;
		_delay_ms(5);
	}
}
void LCD_set_curser(unsigned char y, unsigned char x)
{
	if(y==1)
	LCD_cmnd(0x7F+x);
	else if(y==2)
	LCD_cmnd(0xBF+x);
}
void LCD_num(unsigned char num)
{
	//LCD_data(num/100 + 0x30);
	//num = num%100;
	LCD_data(num/10 + 0x30);
	LCD_data(num%10 + 0x30);
}
void usart_initialize()
{
	UCSRB = 1<<TXEN;
	UCSRC = (1<<UCSZ0)|(1<<UCSZ1)|(1<<URSEL);
	UBRRL = 0x67;
}
void usart_send_char(unsigned char txdata)
{
	while(!(UCSRA&(1<<UDRE)));
	UDR = txdata;
}
void usart_send_string(char *str)
{
	unsigned char i=0;
	while(str[i] != 0)
	{
		usart_send_char(str[i]);
		i++;
		_delay_ms(5);
	}
}
void send_message()
{
	usart_send_string("AT\r\n");		// Transmit AT to the module – GSM Modem sendsOK
	_delay_ms(500);
	usart_send_string("ATE0\r\n");		// Echo Off
	_delay_ms(500);
	usart_send_string("AT+CMGF=1\r\n");	// Switch to text mode
	_delay_ms(500);
	usart_send_string("AT+CMGS=\"+9779841491322\"\r\n"); // Send SMS to a cell number
	LCD_clear();
	LCD_print("    Sending    ");
	LCD_set_curser(2,1);
	LCD_print("    Message    ");
	//_delay_ms(500);
	LCD_set_curser(2,1);
	usart_send_string("Message From Home security System:\n"); // Input SMS Data
	//usart_send_char('X');
	usart_send_char(0x1A);					// Ctrl-Z indicates end of SMS
	LCD_clear();
	LCD_print(" Message Sent  ");
	LCD_set_curser(2,1);
	LCD_print("Successfully!!!");
	_delay_ms(1000);
	LCD_clear();
}
void voice_call()
{
	usart_send_string("AT\r\n");		// Transmit AT to the module – GSM Modem sendsOK
	_delay_ms(2000);
	usart_send_string("ATE0\r\n");		// Echo Off
	_delay_ms(2000);
	LCD_clear();
	LCD_print("    Calling    ");
	usart_send_string("ATD+9779841491322;\r\n"); // Send SMS to a cell number
	_delay_ms(2000);
	LCD_clear();
}
void call_abort()
{
	usart_send_string("AT\r\n");		// Transmit AT to the module – GSM Modem sendsOK
	_delay_ms(2000);
	usart_send_string("ATE0\r\n");		// Echo Off
	_delay_ms(2000);
	LCD_clear();
	usart_send_string("ATH\r\n");
	_delay_ms(2000);
	LCD_print("Calling canceled");
	LCD_clear();
}
int main(void)
{
	LCD_initialize();
	usart_initialize();
	voice_call();
	_delay_ms(4000);
	call_abort();
	_delay_ms(2000);
	send_message();
    while(1)
    {
		;
    }
}