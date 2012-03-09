// Nathan Zimmerman
// MSP430G2553 GUI COMM
// 3/8/12
//Rev 0.3

// A GUI to control all basic IO functions on the msp430G2553

#include <msp430g2553.h>
#include "stdbool.h"


////////////////Defines////////////////

#define Green_LED	BIT6
#define Button		BIT3
#define ERROR		BIT0

unsigned int buffer = 0;
unsigned int command = 0;
unsigned int sub_command = 0;
unsigned int data1 =0;
unsigned int data2 =0;
bool Command_Ready = false;
unsigned int comm_p=0;

////////////////Function Protos////////////////

void TX(char *tx_message);	// Transmit char string
void DIR_P1(); //Set P1DIR
void OUT_P1(); //Set P1OUT
void REN_P1();
void SEL_P1();
void SEL2_P1();
void DIR_P2();
void OUT_P2();
void REN_P2();
void SEL_P2();
void SEL2_P2();


void process_command(); 	//Main function for doing actions based upon recieved data

////////////////Main Program////////////////

void main(void)
{
//unsigned char buffer[2];


WDTCTL = WDTPW + WDTHOLD;			// Stop WDT

BCSCTL1 = CALBC1_1MHZ;				// Set DCO to 1MHz
DCOCTL = CALDCO_1MHZ;

////////////////USCI setup////////////////


P1SEL = BIT1 + BIT2;				// Set P1.1 to RXD and P1.2 to TXD
P1SEL2 = BIT1 + BIT2;				//

UCA0CTL1 |= UCSSEL_2;				// Have USCI use SMCLK AKA 1MHz main CLK

UCA0BR0 = 104;						// Baud: 9600, N= CLK/Baud, N= 10^6 / 9600

UCA0BR1 = 0;						// Set upper half of baud select to 0

UCA0MCTL = UCBRS_1;					// Modulation UCBRSx = 1

UCA0CTL1 &= ~UCSWRST; 				// Start USCI

IE2 |= UCA0RXIE;					//Enable interrupt for USCI

P1OUT = BIT1 + BIT2;				//Debug LEDs

 __enable_interrupt();				//Guess

////////////////General GPIO Defines////////////////


////////////////Main Loop////////////////


while(1)
{
	process_command();
}



} // End Main Function


////////////////Interrupts////////////////

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	buffer =UCA0RXBUF;

	switch(comm_p)
	{
	case 0:
		command = buffer;

		comm_p++;
		if(buffer == '1')
				{
					TX("PING");
					comm_p=0;
					command = 0;
				}
		break;

	case 1:
		data1 = buffer;
		comm_p++;
		break;

	case 2:
		data2 = buffer;
		Command_Ready = true;
		comm_p++;
		break;

	default:
		Command_Ready = false;
	} // End Switch

}

////////////////Functions////////////////

void TX(char *tx_message)
{
	unsigned int i=0; //Define end of string loop int
	char *message; // message variable
	unsigned int message_num; // define ascii int version variable
	message = tx_message; // move tx_message into message

	while(1)
	{


	message_num = (int)message[i]; //Cast string char into a int variable

	UCA0TXBUF = message_num; // write INT to TX buffer

	i++; // increase string index

	__delay_cycles(1500); //transmission delay

	if(i>20) //prevent infinite transmit
	{
	//P1OUT |= ERROR;
	break;
	}

	if(message[i]==0) // If end of input string is reached, break loop.
	{
		__delay_cycles(500);
		UCA0TXBUF = 0;
		break;
	}

	} // End TX Main While Loop
} // End TX Function




////////////////DIR_P1()////////////////

void DIR_P1()
{

if((data1 & BIT0)== BIT0)
{
	P1DIR &=(~data2)|(BIT1 + BIT2);
}
else
{
	P1DIR |=data2;
}

} // End Function


///////////////////////////////////////////


////////////////OUT_P1()////////////////

void OUT_P1()
{

if((data1 & BIT0)== BIT0)
{
	P1OUT &=(~data2)|(BIT1 + BIT2);
}
else
{
	P1OUT |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////REN_P1()////////////////

void REN_P1()
{

if((data1 & BIT0)== BIT0)
{
	P1REN &=(~data2)|(BIT1 + BIT2);
}
else
{
	P1REN |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////SEL1_P1()////////////////

void SEL_P1()
{

if((data1 & BIT0)== BIT0)
{
	P1SEL &=(~data2)|(BIT1 + BIT2);
}
else
{
	P1SEL |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////SEL2_P1()////////////////

void SEL2_P1()
{

if((data1 & BIT0)== BIT0)
{
	P1SEL2 &=(~data2)|(BIT1 + BIT2);
}
else
{
	P1SEL2 |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////DIR_P2()////////////////

void DIR_P2()
{

if((data1 & BIT0)== BIT0)
{
	P2DIR &=(~data2);
}
else
{
	P2DIR |=data2;
}

} // End Function


////////////////OUT_P2()////////////////

void OUT_P2()
{

if((data1 & BIT0)== BIT0)
{
	P2OUT &=(~data2)|(BIT1 + BIT2);
}
else
{
	P2OUT |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////REN_P2()////////////////

void REN_P2()
{

if((data1 & BIT0)== BIT0)
{
	P2REN &=(~data2)|(BIT1 + BIT2);
}
else
{
	P2REN |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////SEL1_P2()////////////////

void SEL_P2()
{

if((data1 & BIT0)== BIT0)
{
	P2SEL &=(~data2)|(BIT1 + BIT2);
}
else
{
	P2SEL |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////SEL2_P2()////////////////

void SEL2_P2()
{

if((data1 & BIT0)== BIT0)
{
	P2SEL2 &=(~data2)|(BIT1 + BIT2);
}
else
{
	P2SEL2 |=data2;
}

} // End Function

///////////////////////////////////////////

////////////////process_command////////////////

void process_command()	// Main processing function
{

while(!(Command_Ready));	//Wait till 2 packets are recieved
Command_Ready = false;
sub_command = command;
command = 0;

	switch(sub_command)
	{

	case 0x0:					// Command has already been proccessed

		TX("ERORR");
		break;

	case 0x1: 				// IF Command = Connect
		TX("Connected");
		command = 0;
		comm_p=0;
		break;

	case 0x2:	// IF command = digital output port 1
		DIR_P1();
		TX("DIR");
		command = 0;
		comm_p = 0;
		break;

	case 0x3:
		OUT_P1();
		TX("OUT");
		command = 0;
		comm_p = 0;
		break;

	case 0x4:
		REN_P1();
		TX("REN");
		command = 0;
		comm_p = 0;
		break;

	case 0x5:
		SEL_P1();
		TX("SEL");
		command = 0;
		comm_p = 0;
		break;

	case 0x6:
		SEL2_P1();
		TX("SEL2");
		command = 0;
		comm_p = 0;
		break;

	case 0x7:
		DIR_P2();
		TX("DIR2");
		command = 0;
		comm_p = 0;
		break;

	case 0x8:
		OUT_P2();
		TX("OUT2");
		command = 0;
		comm_p = 0;
		break;

	case 0x9:
		REN_P2();
		TX("REN2");
		command = 0;
		comm_p = 0;
		break;

	case 0xA:
		SEL_P2();
		TX("SEL1p2");
		command = 0;
		comm_p = 0;
		break;

	case 0xB:
		SEL2_P2();
		TX("SEL2p2");
		command = 0;
		comm_p = 0;
		break;


	default:
	command =0;
	TX("ERORR");			//Error Case

	} // End Main Switch

} // End Sub Command Function

///////////////////////////////////////////

