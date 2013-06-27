// v0.8
#include"LCD.h"
#include<plib.h>

int main()
{
  OSCSetPBDIV(OSC_PB_DIV_1);
	MCU_init();
	LCD_init();
	LCD_puts(startStr1);
	LCD_goto(0x40);		//change to the second line
	LCD_puts(startStr2);
	while(1);
}

void MCU_init() {
/* setup I/O ports to connect to the LCD module */
//	TRISA = 0;
	DDPCONbits.JTAGEN = 0;
 	TRISA = 0xfff0;	//pin 0-1 output rs:RA0 E:RA1
	TRISE = 0xff00;	//pin 0-7 output
/* setup Timer to count for 1 us and ms */
	T2CON = 0;
	T2CONbits.TCKPS = 0; //prescaler is 1:1

	
/* Configure Timer interrupts */
	INTCONbits.MVEC = 1;
	IPC2bits.T2IP = 3;//Set priority level = 3
	IPC2bits.T2IS = 1;//Set subpriority level = 1
	IFS0bits.T2IF = 0;//Clear Timer2 interrupt flag

/* enable global and individual interrupts */
	asm("ei");
	IEC0bits.T2IE = 1;
}

/*initialize the LCD module */
void LCD_init() {
	DelayMsec(15); //wait for 15 ms
	RS = 0; //send command
	Data = LCD_IDLE; //function set - 8 bit interface
	DelayMsec(5); //wait for 5 ms
	Data = LCD_IDLE; //function set - 8 bit interface
	DelayUsec(100); //wait for 100 us
	Data = LCD_IDLE; //function set
	DelayMsec(5);
	Data = LCD_IDLE;
	DelayUsec(100);
	LCD_putchar(LCD_2_LINE_4_BITS);
	DelayUsec(40);

	LCD_putchar(LCD_DSP_CSR);
	DelayUsec(40);
	LCD_putchar(LCD_CLR_DSP);
	DelayMsec(5);
	LCD_putchar(LCD_CSR_INC);
	
	
}


/* Send one byte c (instruction or data) to the LCD */
void LCD_putchar(uchar c) {
	E = 1;
	Data = c; //sending higher nibble
	E = 0; //producing falling edge on E
	E = 1;
	Data <<= 4; //sending lower nibble through higher 4 ports
	E = 0; //producing falling edge on E
}

/*Display a string of characters *s by continuously calling LCD_putchar() */
void LCD_puts(const uchar *s) {
	RS=1;
	int i=0;
	while (s[i]!='\0')
	{
		LCD_putchar(s[i]);
		DelayUsec(40);
		i++;
	}
	RS=0;
}
/*void LCD_puts(const uchar *s) {
	RS=1;
	int i=0;
	while (s[i]!='\0')
	{
		if(i<16)
			LCD_putchar(s[i]);
		else
			{LCD_goto(0x40);LCD_putchar(s[i]);}
		DelayUsec(40);
		i++;
	}
	RS=0;
}*/
/* go to a specific DDRAM address addr */
void LCD_goto(uchar addr) {
	LCD_putchar(addr|0x80);
    DelayUsec(40);
}

/* configure timer SFRs to generate num us d elay*/
void DelayUsec(uchar num) {
	TMR2 = 0;
	PR2 = 4*num;	//4M
	T2CONSET = 0x8000;
	while (T2CONbits.ON == 1);
}

/* configure timer SFRs to generate 1 ms delay*/
void GenMsec() {
	TMR2 = 0;
	PR2 = 4000; //4M
	T2CONSET = 0x8000;
	while (T2CONbits.ON == 1);
}

/* Call GenMsec() num times to generate num ms delay*/
void DelayMsec(uchar num) {
	uchar i;
	for (i=0; i<num; i++) {
	GenMsec();
	}
}

/* timer interrupt handler */
#pragma interrupt Timer2_ISR ipl5 vector 8

void Timer2_ISR() {
	T2CONbits.ON = 0;
	IFS0bits.T2IF = 0;
}

/*************end of LCD.c**************/
/*int main()
{
	OSCSetPBDIV (OSC_PB_DIV_1);
	MCU_init();
	//LCD_init();
	DelayUsec(40);
	DelayMsec(40);
	return 0;
}*/
