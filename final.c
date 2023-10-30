#include <xc.h>
#include <pic18f4520.h>

#pragma config OSC = INTIO67    // Oscillator Selection bits
#pragma config WDT = OFF        // Watchdog Timer Enable bit 
#pragma config PWRT = OFF       // Power-up Enable bit
#pragma config BOREN = ON       // Brown-out Reset Enable bit
#pragma config PBADEN = OFF     // Watchdog Timer Enable bit 
#pragma config LVP = OFF        // Low Voltage (single -supply) In-Circute Serial Pragramming Enable bit
#pragma config CPD = OFF        // Data EEPROM?Memory Code Protection bit (Data EEPROM code protection off)
#define _XTAL_FREQ 4000000

int flag = 0;
int id;
int cnt = 0;
int cnt2 = 0;

void __interrupt(high_priority)H_ISR(){
    
    //step4
    int value = ADRESH*4+ADRESL/64;  //ADRESH:ADRESL<7:6>
    //do things
    
    PORTD = ~cnt;
    if(PORTBbits.RB1==0){   //cnt
        __delay_ms(5);
        cnt++;
//        PORTD = ~cnt;
    }
    if(PORTBbits.RB2==0){   //cnt2
        __delay_ms(5);
        cnt2++;
    }
    
    if(value<341){
        CCPR1L = 19;
    }
    else if(value < 511){
        CCPR1L = 15;
    }
    else if(value < 682){
        CCPR1L = 11;
    }
    else if(value < 852){
        CCPR1L = 7;
    }
    else{
        CCPR1L = 3;
    }
    __delay_us(4);
    
    ADCON0bits.GODONE = 1;
    //clear flag bit
    PIR1bits.ADIF = 0;
    
    return;
}

void main(void)
{
    //configure OSC and port
    OSCCONbits.IRCF = 0b110; //4MHz
    TRISAbits.RA0 = 1;       //analog input port
    
    PORTD = 0;
    
    //step1
    ADCON1bits.VCFG0 = 0;
    ADCON1bits.VCFG1 = 0;
    ADCON1bits.PCFG = 0b1110; //AN0 ?analog input,???? digital
    ADCON0bits.CHS = 0b0000;  //AN0 ?? analog input
    ADCON2bits.ADCS = 0b100;  //4*Tosc
    ADCON2bits.ACQT = 0b010;  //Tad = 0.8us 0.8*4 = 3.2 > 2.4 
    ADCON0bits.ADON = 1;
    ADCON2bits.ADFM = 0;    //left justified 
    //step2
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 0;
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;
    //step3
    ADCON0bits.GODONE = 1;
    
    PORTC = 0X00; //INIT PORTC
    TRISC = 0;  //PORTC AS OUTPUT
    //
    RCONbits.IPEN = 1;
    INTCONbits.INT0IE = 1;
    // Timer2 -> On, prescaler -> 4
    T2CONbits.TMR2ON = 0b1;
    T2CONbits.T2CKPS = 0b01;

    // Internal Oscillator Frequency, Fosc = 125 kHz, Tosc = 8 탎
    OSCCONbits.IRCF = 0b001;
    
    // PWM mode, P1A, P1C active-high; P1B, P1D active-high
    CCP1CONbits.CCP1M = 0b1100;
    
    // CCP1/RC2 -> Output
    TRISC = 0;
    LATC = 0;
    //TRISB = 0x07;   //RB0:button RB1:sensor
    TRISBbits.RB1 = 1;
    TRISBbits.RB3 = 0;
    TRISD = 0;
    // Set up PR2, CCP to decide PWM period and Duty Cycle
    /** 
     * PWM period
     * = (PR2 + 1) * 4 * Tosc * (TMR2 prescaler)
     * = (0x9b + 1) * 4 * 8탎 * 4
     * = 0.019968s ~= 20ms
     */
    PR2 = 0x9b;
    
    /**
     * Duty cycle
     * = (CCPR1L:CCP1CON<5:4>) * Tosc * (TMR2 prescaler)
     * = (0x0b*4 + 0b01) * 8탎 * 4
     * = 0.00144s ~= 1450탎
     */
    
    // init
    CCPR1L = 0x04;
    CCP1CONbits.DC1B = 0b0;
    int state = 0;
    int cnt = 0;
    while(1){
        PORTDbits.RD0 = 1;
    }
    return;
}