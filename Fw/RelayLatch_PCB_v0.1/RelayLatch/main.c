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
const int LATCH_TIME_ms = 75;	// TIEMPO MINIMO ms PARA LATCH DE LOS RELES

void init_uart(void){
	DDRD |= (1<<DDD1);	// D1 COMO SALIDA TX
	//UCSR0B |= (1<<TXEN0);	//ENABLE TX
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);	//ENABLE RX & TX
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
	_delay_ms(LATCH_TIME_ms);
	PORTD |= (1<<PIND2);	//ENABLE HIGH
	_delay_ms(LATCH_TIME_ms);
	PORTD &= ~(1<<PIND2);	//ENABLE LOW	
}



int main(void)
{
	char str_buffer[100];
	init_uart();
	send_string("\n\nEDS_RelayLatch_Control v0.1\nINPEL S.A\n\n\r");
	
  DDRB = 0x07; // B0->Bin2, B1->Bin3, B2->Bin4
  DDRC = 0x00; // TODO EL PUERTO COMO ENTRADA, RESET & ENTRADA DIGITALES C0->I1, C1->I2, C2->I3, C3->I4
  DDRD = 0XFC; // D2->Enable, D3->Ain1, D4->Ain2, D6->Ain3, D7->Bin1
  
   
  uint8_t Iexe=0, I1=0, I2=0, I3=0, I4=0; 
  uint8_t O1A=0, O1B=0, O2A=0, O2B=0, O3A=0, O3B=0, O4A=0, O4B=0;
 	 
  uint8_t LAST_PORTC = PINC & 0x0F;
		 	  
    while (1) 
    {
		if ( LAST_PORTC != (PINC & 0x0F) | Iexe == 0 )
		{
			LAST_PORTC = PINC & 0x0F;
			Iexe = 1;
		
//----------	***	LECTURA DE I1	***//
		if ( (PINC & 1<<PINC0) == 0 )
		{	I1 = 1;
			O1A = 0;
			O1B = 1;
			PORTB &= ~(1<<PINB2);	//Bin4 LOW
			PORTB |= (1<<PINB1);	//Bin3 HIGH
		}else 
		{	I1 = 0;
			O1A = 1;
			O1B = 0;			
			PORTB |= (1<<PINB2);	//Bin4 HIGH	
			PORTB &= ~(1<<PINB1);	//Bin3 LOW	
		}

//----------	***	LECTURA DE I2	***//
		if ( (PINC & 1<<PINC1) == 0 )
		{	I2 = 1;
			O2A = 0;
			O2B = 1;			
			PORTB &= ~(1<<PINB0);	//Bin2 LOW
			PORTD |= (1<<PIND7);	//Bin1 HIGH
		}
		else 
		{	I2 = 0;
			O2A = 1;
			O2B = 0;			
			PORTB |= (1<<PINB0);	//Bin2 HIGH
			PORTD &= ~(1<<PIND7);	//Bin1 LOW			
		}
		
//----------	***	LECTURA DE I3	***//
		if ( (PINC & 1<<PINC2) == 0 )
		{	I3 = 1;
			O3A = 0;
			O3B = 1;			
			PORTD &= ~(1<<PIND6);	//Ain4 LOW
			PORTD |= (1<<PIND5);	//Ain3 HIGH
		}
		else 
		{	I3 = 0;
			O3A = 1;
			O3B = 0;			
			PORTD |= (1<<PIND6);	//Ain4 HIGH
			PORTD &= ~(1<<PIND5);	//Ain3 LOW
		}

//----------	***	LECTURA DE I4	***//
		if ( (PINC & 1<<PINC3) == 0 )
		{	I4 = 1;
			O4A = 0;
			O4B = 1;			
			PORTD &= ~(1<<PIND4);	//Ain2 LOW
			PORTD |= (1<<PIND3);	//Ain1 HIGH
		}
		else 
		{	I4 = 0;
			O4A = 1;
			O4B = 0;			
			PORTD |= (1<<PIND4);	//Ain2 HIGH
			PORTD &= ~(1<<PIND3);	//Ain1 LOW
		}		
			ENABLE_EXE();

			sprintf(str_buffer, "Estados logicos:     I1  -   I2  -   I3  -   I4 \n\r");
			send_string(str_buffer);			
			sprintf(str_buffer, "Entrada: %14d %7d %7d %7d \n",I1 , I2, I3, I4);
			send_string(str_buffer);
			
			sprintf(str_buffer, "Estados Logicos: | 1A-1B | 2A-2B | 3A-3B | 4A-4B | \n\r");
			send_string(str_buffer);	
			sprintf(str_buffer, "Salida: %13d %d",O1A ,O1B);
			send_string(str_buffer);	
			sprintf(str_buffer, "%6d %d %5d %d %5d %d\n\n\r",O2A ,O2B, O3A, O3B, O4A, O4B);
			send_string(str_buffer);
			
		}		
		
	
		

			
				
		

	}	

}