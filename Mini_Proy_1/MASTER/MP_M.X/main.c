/*
 * File:   main.c
 * Author: katha
 *
 * Created on 22 de febrero de 2021, 10:48 PM
 */

//*****************************************************************************
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = EXTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// LIBRERÍAS
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "SPI.h"
#include "LCD.h"
#include "USART.h"

//*****************************************************************************
// VARIABLES
//*****************************************************************************
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


uint8_t S1 = 0;
uint8_t S2 = 0;
uint8_t S3 = 0;
char data[20];
char data_S1[8];
char data_S2[8];
char data_S3[8];
int ADC;
int TEMP;
char USART_LEER;
uint8_t cont = 0;
//
//char data1[20];
//char data2[16];
//char USART_LEER;
uint8_t hablar = 0;
//int S1;

//******************************************************************************
//INTERRUPCIONES
//****************************************************************************** 
//void __interrupt() ISR(void){        
//    if (RCIF == 1) {
//        USART_LEER = RCREG;
//        if (USART_LEER == '+') { //comparar lo ingresado a la terminal con '+'
//            cont = cont + 1; //aumentar contador
//            }
//        else if (USART_LEER == '-') {//comparar lo ingresado a la terminal con '-'
//            cont = cont - 1; //decrementar contador
//        }
//        USART_LEER = 0; //regresamos a 0 para que no siga realizando la función ni se sobre escriba
//    }
//}

//*****************************************************************************
// PROTOTIPOS DE FUNCIONES
//*****************************************************************************
void setup(void);
void LEER_S1(void);
void LEER_S2(void);
void LEER_S3(void);

//*****************************************************************************
// Código Principal
//*****************************************************************************
void main(void) {
    setup();
    TRISD = 0x00;
    Lcd_Init(); //Inicializar LCD
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1); //Elegimos localidad para escribir
    Lcd_Write_String("S1:"); //Escribimos texto en LCD
    Lcd_Set_Cursor(1, 7);
    Lcd_Write_String("S2:");
    Lcd_Set_Cursor(1, 14);
    Lcd_Write_String("S3:");
    
    while(1){
        LEER_S1();
        LEER_S2();
        LEER_S3();
        PORTB = S3;
//
//       sprintf(data_S1, "%1.2fV", ADC);
//       sprintf(data_S2, "%.3i", S2);
//       sprintf(data_S3, "%.3i", S3);
       
       sprintf(data, "%.2iV   %.3i    %.2i", ADC, S2, TEMP); //volvemos los valores un char para enviarlo a la terminal
       
       USART_STRING(data); //enviamos el string con la info
//       USART_STRING("  \n");
//       USART_STRING(data_S2);
//       USART_STRING("  \n");
//       USART_STRING(data_S3);
       USART_ESCRITURA(13); //Salto de linea en la terminal, para que se entienda mejor
       USART_ESCRITURA(10);
//       
//       
////       sprintf(data, "%d  %d  %d", ADC, S2, S3); //volvemos los valores un char para enviar un solo dato a la LCD
       Lcd_Set_Cursor(2, 1); //Elegimos posición del texto en la LCD
       Lcd_Write_String(data); //Enviamos los datos a la LCD
    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************
void setup(void){
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISE = 0;
    TRISD = 0;
   
    PORTC = 0;
    PORTE = 0;
    PORTD = 0;
    PORTB = 0;
    
    TRISC0 = 0;
    PORTCbits.RC0 = 1;
    TRISC1 = 0;
    PORTCbits.RC1 = 1;
    TRISC2 = 0;
    PORTCbits.RC2 = 1;
//    PORTCbits.RC7 = 1;

    spiInit(SPI_MASTER_OSC_DIV4, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
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


}

void LEER_S1(void){
    
    PORTCbits.RC0 = 0;       //Slave Select
    __delay_ms(hablar);
       
    spiWrite(1);
    S1 = spiRead();
    ADC = ((S1 * 50) / 255);
       
    __delay_ms(1);
    PORTCbits.RC0 = 1;       //Slave Deselect 
    __delay_ms(100);
} 

void LEER_S2(void){
    PORTCbits.RC1 = 0;
    __delay_ms(1);
       
    spiWrite(hablar);
    S2 = spiRead();
       
    __delay_ms(1);
    PORTCbits.RC1 = 1; 
    __delay_ms(100);
}   

void LEER_S3(void){
    PORTCbits.RC2 = 0;
    __delay_ms(1);
       
    spiWrite(hablar);
    S3 = spiRead();
    TEMP = ((S3*150)/255);
       
    __delay_ms(1);
    PORTCbits.RC2 = 1; 
    
    __delay_ms(100);
}