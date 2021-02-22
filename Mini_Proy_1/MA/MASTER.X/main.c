/*
 * File:   main.c
 * Author: katha
 *
 * Created on 8 de febrero de 2021, 10:53 AM
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
#include "USART.h"
#include "SPI_M.h"

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

//uint8_t ADC_res1;
//uint8_t ADC_res2;
uint8_t cont = 0;
char data1[20];
char data2[16];
char USART_LEER;
//int VAL1;
//int VAL2;



//******************************************************************************
//Interrupciones
//****************************************************************************** 
void __interrupt() ISR(void){        
    if (RCIF == 1) {
        USART_LEER = RCREG;
        if (USART_LEER == '+') { //comparar lo ingresado a la terminal con '+'
            cont = cont + 1; //aumentar contador
            }
        else if (USART_LEER == '-') {//comparar lo ingresado a la terminal con '-'
            cont = cont - 1; //decrementar contador
        }
        USART_LEER = 0; //regresamos a 0 para que no siga realizando la función ni se sobre escriba
    }
}
//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup(void);
//void ADC_VALOR(void);
//void ADC_CH0(void);
//void ADC_CH1(void);
//void COM1(void);
//void COM2(void);

//******************************************************************************
//Ciclo Principal
//******************************************************************************

void main(void) {
    setup();
    //  unsigned int a;
    TRISD = 0x00;
    Lcd_Init(); //Inicializar LCD
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1); //Elegimos localidad para escribir
    Lcd_Write_String("S1:"); //Escribimos texto en LCD
    Lcd_Set_Cursor(1, 7);
    Lcd_Write_String("S2:");
    Lcd_Set_Cursor(1, 14);
    Lcd_Write_String("S3:");
    while (1) {
//        ADC_CH0();  //Leemos AN0 sensor 1
//        ADC_CH1();  //Leemos AN1 sensor 2
//
//
//        
        sprintf(data1, "S1   S2   S3"); //volvemos los valores un char para enviarlo a la terminal
        USART_STRING(data1); //enviamos el string con la info
        USART_ESCRITURA(13); //Salto de linea en la terminal, para que se entienda mejor
        USART_ESCRITURA(10);

//        sprintf(data2, "%dV   %dV    %d", VAL1, VAL2, cont); //volvemos los valores un char para enviar un solo dato a la LCD
//        Lcd_Set_Cursor(2, 1); //Elegimos posición del texto en la LCD
//        Lcd_Write_String(data2); //Enviamos los datos a la LCD



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
    TRISC7 = 1; //Como en el data sheet
    //ADC 
    ADCON1 = 0b00000000; //Justificado a la izquierda
    PIE1bits.ADIE = 0;
    PIR1bits.ADIF = 0;
    OPTION_REG = 0b00000000;
    INTCON = 0b00000000;

    //INTERRUPCION
    INTCONbits.PEIE = 1;
    PIE1bits.RCIE = 1;
    INTCONbits.GIE = 1;

    // USART CONFIG
    SPBRGH = 0;
    SPBRG = 12; //BAUD RATE DE 9600 A 8MHz

    TXSTAbits.TXEN = 1; //ENABLE
    TXSTAbits.SYNC = 0; // ASYNC

    RCSTAbits.SPEN = 1; // ENABLE
    RCSTAbits.CREN = 1;
    RCREG = 0;
    
    
    PORTCbits.RC2 = 1; //Iniciamos Com SPI Como MASTER
    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);



}

//void ADC_CH0(void) {
//    ADC_ch(0); //channel 0
//    ADCON0bits.ADCS0 = 1; // 
//    ADCON0bits.ADCS1 = 0; //Fosc/8
//    ADCON0bits.ADON = 1; //enable ADC
//    __delay_us(40); 
//    ADCON0bits.GO = 1; //iniciamos conversión
//    while (ADCON0bits.GO != 0) { //revisamos si ya se finalizó la conversión
//        ADC_res1 = ADC_val(ADRESL, ADRESH);
//        VAL1 = ((ADC_res1 * 50) / 255);
//
//    }
//}

//void ADC_CH1(void) {
//    ADC_ch(1); //channel 1
//    ADCON0bits.ADCS0 = 1; 
//    ADCON0bits.ADCS1 = 0; //Fosc/8
//    ADCON0bits.ADON = 1; //enable ADC
//    __delay_us(40); 
//    ADCON0bits.GO = 1; //iniciamos conversión
//    while (ADCON0bits.GO != 0) { //revisamos si ya se finalizó la conversión
//        ADC_res2 = ADC_val(ADRESL, ADRESH);
//        VAL2 = ((ADC_res2 * 50) / 255);
//
//    }
//}