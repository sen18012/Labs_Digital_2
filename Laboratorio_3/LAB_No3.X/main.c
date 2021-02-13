/*
 * File:   main.c
 * Author: katha
 *
 * Created on 7 de febrero de 2021, 02:03 PM
 */

//******************************************************************************
//Librerias
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "LCD.h"
#include "ADC.h"
#include "USART.h"

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
//Variables
//******************************************************************************
#define _XTAL_FREQ 8000000
#define RS RE0
#define EN RE1
#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

uint8_t ADC_res1;
uint8_t ADC_res2;
uint8_t cont = 0;
char data1[20];
char data2[16];
char USART_LEER;
int VAL1;
int VAL2;



//******************************************************************************
//Interrupciones
//****************************************************************************** 
void __interrupt() ISR(void){        
    if (RCIF == 1) {
        USART_LEER = RCREG;
        if (USART_LEER == '+') {
            cont = cont + 1;
            }
        else if (USART_LEER == '-') {
            cont = cont - 1;
        }
        USART_LEER = 0;
    }
}
//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup(void);
void ADC_VALOR(void);
void ADC_CH0(void);
void ADC_CH1(void);
void COM1(void);
void COM2(void);

//******************************************************************************
//Ciclo Principal
//******************************************************************************

void main(void) {
    setup();
    //  unsigned int a;
    TRISD = 0x00;
    Lcd_Init();
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1);
    Lcd_Write_String("S1:");
    Lcd_Set_Cursor(1, 7);
    Lcd_Write_String("S2:");
    Lcd_Set_Cursor(1, 14);
    Lcd_Write_String("S3:");
    while (1) {
        ADC_CH0();
        ADC_CH1();


        
        sprintf(data1, "%dV   %dV   %d", VAL1, VAL2, cont); //VALORES SE PASAN A CHAR PARA QUE LOS LEA LA TERMINAL VIRTUAL
        USART_STRING(data1); //enviar el string con los valores a la pc
        USART_ESCRITURA(13); //13 y 10 la secuencia es para dar un salto de linea 
        USART_ESCRITURA(10);

        sprintf(data2, "%dV   %dV    %d", VAL1, VAL2, cont);
        Lcd_Set_Cursor(2, 1);
        Lcd_Write_String(data2);



    }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************

void setup(void) {
    ANSEL = 0b00000011; //IO CONFIG
    ANSELH = 0;
    TRISA = 0b00000011;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    TRISC7 = 1;
    //ADC 
    ADCON1 = 0b00000000; //Justificado a la izquierda
    PIE1bits.ADIE = 0;
    PIR1bits.ADIF = 0;
    OPTION_REG = 0b00000000;
    INTCON = 0b00000000;

    //INTERRUPCION
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;
//    PIR1bits.RCIF = 1;
    INTCONbits.GIE = 1;

    // USART CONFIG
    SPBRGH = 0;
    SPBRG = 12; //BAUD RATE DE 9600 A 8MHz
    
//    TXSTAbits.CSRC = 0;
//    TXSTAbits.TX9 = 0; //8 bits
    TXSTAbits.TXEN = 1; //ENABLE
    TXSTAbits.SYNC = 0; // ASYNC
//    TXSTAbits.BRGH = 0; //LOW SPEED
//    TXSTAbits.TRMT = 0;

    RCSTAbits.SPEN = 1; // ENABLE
//    RCSTAbits.RX9 = 0; //8 bits
    RCSTAbits.CREN = 1;
    RCREG = 0;



}

void ADC_CH0(void) {
    ADC_ch(0); //channel 0
    //Cinfiguracion bits ADCON0
    ADCON0bits.ADCS0 = 1; //Clock ADC conversion
    ADCON0bits.ADCS1 = 0; //Fosc
    ADCON0bits.ADON = 1; //Se habilita el ADC
    __delay_us(40); //Para conversion
    ADCON0bits.GO = 1; //Inicia la conversión
    while (ADCON0bits.GO != 0) { //Waiting for conversion to complete
        ADC_res1 = ADC_val(ADRESL, ADRESH);
        VAL1 = ((ADC_res1 * 50) / 255);

    }
}

void ADC_CH1(void) {
    ADC_ch(1); //channel 1
    ADCON0bits.ADCS0 = 1; //Clock ADC conversion
    ADCON0bits.ADCS1 = 0; //Fosc
    ADCON0bits.ADON = 1; //Se habilita el ADC
    __delay_us(40); //Para conversion
    ADCON0bits.GO = 1; //Inicia la conversión
    while (ADCON0bits.GO != 0) { //Waiting for conversion to complete
        ADC_res2 = ADC_val(ADRESL, ADRESH);
        VAL2 = ((ADC_res2 * 50) / 255);

    }
}