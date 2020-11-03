/*
 * RelayLatch.c
 *
 * Created: 10/08/2020 4:56:24 p.m.
 * Author : imagredo
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
const int LATCH_TIME_ms = 1000;	// TIEMPO MINIMO ms PARA LATCH DEL RELE

void init_uart(void){
	DDRD |= (1<<DDD1);	// D1 COMO SALIDA TX
	UCSR0B |= (1<<TXEN0);	//ENABLE TX
	UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);	//bit size 8-bit
	UBRR0 =	F_CPU/16/BAUD-1;	//103;	//
	
}

void send_char(unsigned char c){
	while( !(UCSR0A & (1<<UDRE0)) );	//FLAG UDRE0 ready to receive new data
	UDR0 = c;
	
}
	 
void send_string(char *str){
	while(*str != 0){
		send_char(*str);
		str++;
	}
}


//*** TODAS LAS SALIDAS EN HIGH POR 1SEGUNDO Y DE NUEVO EN LOW
void test_hw(void){
	send_string("Test HW\n\r");
   	PORTD |= (1<<2); //D2 HIGH
   	_delay_ms(1000);
   	PORTB = 0X07;
   	PORTD = 0XFC;
   	_delay_ms(1000);
   	PORTB = 0X00;
   	PORTD = 0X00;	
	   
	   //**********
	   send_string("ok\n\r");
}


void ENABLE_EXE(void){
	_delay_ms(1000);
	PORTD |= (1<<PIND2);	//ENABLE HIGH
	_delay_ms(LATCH_TIME_ms);
	PORTD &= ~(1<<PIND2);	//ENABLE LOW	
}

int main(void)
{
	char str_buffer[20];
	init_uart();
	send_string("EDS_RelayLatch_Control v1.0\nINPEL S.A\n\n\r");
	
  DDRB = 0x07; // B0,B1,B2 COMO SALIDA RST3,SET4,RST4
  DDRC = 0x00; // TODO EL PUERTO COMO ENTRADA, RESET & ENTRADA DIGITALES PULL UP EXT
  DDRD = 0XFC; // D2,D4,D4,D5,D6,D7 COMO SALIDA ENABLE, SET1,RST1,SET2,RST2,SET3
  
//  int SETT = 0, RSTT = 1;	// PULL UP -> {SETT=0 & RSTT=1}	PULL DOWN -> {SETT=1 & RSTT=0}
  int Iexe=0, I1=0, I2=0, I3=0, I4=0; 
 	
  uint8_t LAST_PORTC = PINC & 0x0F; 
			sprintf(str_buffer, "primer read PINC: %X \n\r",LAST_PORTC);
			send_string(str_buffer);		

		 	  
    while (1) 
    {
		if ( LAST_PORTC != (PINC & 0x0F) | Iexe == 0 )
		{
			LAST_PORTC = PINC & 0x0F;
			Iexe = 1;
			sprintf(str_buffer, "PINC actual: %X \n\r",LAST_PORTC);
			send_string(str_buffer);
			
			sprintf(str_buffer, "ENTRADA actual: %X \n\r",LAST_PORTC);
			send_string(str_buffer);			

		//	***	LECTURA DE I1	***//
		if ( (PINC & 1<<PINC0) == 0 )
		{
			PORTD |= (1<<PIND3);	//SET1 HIGH
			PORTD &= ~(1<<PIND4);	//RST1 LOW
		}else //if ( (PINC & 1<<PINC0) == 1 )
		{
			PORTD &= ~(1<<PIND3);	//SET1 LOW
			PORTD |= (1<<PIND4);	//RST1 HIGH
		}

		//	***	LECTURA DE I2	***//
		if ( (PINC & 1<<PINC1) == 0 )
		{
			PORTD |= (1<<PIND5);	//SET2 HIGH
			PORTD &= ~(1<<PIND6);	//RST2 LOW
		}
		else //if ( (PINC & 1<<PINC1) == 2 )
		{
			PORTD &= ~(1<<PIND5);	//SET2 LOW
			PORTD |= (1<<PIND6);	//RST2 HIGH
		}
		
		//	***	LECTURA DE I3	***//
		if ( (PINC & 1<<PINC2) == 0 )
		{
			PORTD |= (1<<PIND7);	//SET3 HIGH
			PORTB &= ~(1<<PINB0);	//RST3 LOW
		}
		else //if ( (PINC & 1<<PINC2) == 4 )
		{
			PORTD &= ~(1<<PIND7);	//SET3 LOW
			PORTB |= (1<<PINB0);	//RST3 HIGH
		}

		//	***	LECTURA DE I4	***//
		if ( (PINC & 1<<PINC3) == 0 )
		{
			PORTB |= (1<<PINB1);	//SET4 HIGH
			PORTB &= ~(1<<PINB2);	//RST4 LOW
		}
		else //if ( (PINC & 1<<PINC2) == 4 )
		{
			PORTB &= ~(1<<PINB1);	//SET4 LOW
			PORTB |= (1<<PINB2);	//RST4 HIGH
		}		
			ENABLE_EXE();

			
		}		
		
	
		

			
				
		

	}	

}