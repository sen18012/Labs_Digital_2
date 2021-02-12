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
char data[16];
float VAL1;
float VAL2;

//******************************************************************************
//Interrupciones
//****************************************************************************** 

//void __interrupt() ISR(void) { 
//   if (PIR1bits.ADIF == 1) {
//        PIR1bits.ADIF = 0; //bajamos manuelmente la bandera de interrup
//        __delay_ms(2);
//        ADCON0bits.ADON = 1; // Enable ADC
//        __delay_us(40); // Time to charge the cap
//        ADCON0bits.GO_nDONE = 1; // Enable convertion
////        ADCON0bits.GO_nDONE = 1;
//        while (ADCON0bits.GO != 0) { //Revisa si la conversación ya finalizó
//            ADC_res = ADC_val(ADRESL, ADRESH);
//            PORTB = ADC_res;
//            //PORTC = ADC_res; //Prueba antes de multiplexado
//        }
////        PIE1bits.ADIE = 1; //enable interrupcion ADC
//        
//
//    }
// 
//}



//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup (void);
void ADC_VALOR (void);
void ADC_CH0 (void);
void ADC_CH1 (void);

//******************************************************************************
//Ciclo Principal
//******************************************************************************
void main(void) {
  setup ();
//  unsigned int a;
  TRISD = 0x00;
  Lcd_Init();
  
     Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1:");
    Lcd_Set_Cursor(1,7);
    Lcd_Write_String("S2:");
    Lcd_Set_Cursor(1,14);
    Lcd_Write_String("S3:");
  while(1)
  {
    ADC_CH0();
    ADC_CH1();

    sprintf(data, "%1.2fV " "%1.2fV", VAL1, VAL2);
    Lcd_Set_Cursor(2, 1); 
    Lcd_Write_String(data);
    
  }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************
void setup (void){
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
//ADC CONFIG
    ADCON1 = 0b00000000;//Justificado a la izquierda
    PIE1bits.ADIE = 0;
    PIR1bits.ADIF = 0;
    OPTION_REG = 0b00000000;
    INTCON = 0b00000000;

    
    
}

void ADC_CH0(void){
    ADC_ch(0); //channel 0
    //Cinfiguracion bits ADCON0
    ADCON0bits.ADCS0 = 1;//Clock ADC conversion
    ADCON0bits.ADCS1 = 0;//Fosc
    ADCON0bits.ADON = 1;//Se habilita el ADC
    __delay_us(40);//Para conversion
    ADCON0bits.GO = 1;  //Inicia la conversión
    while(ADCON0bits.GO != 0){ //Waiting for conversion to complete
        ADC_res1 = ADC_val(ADRESL, ADRESH);
        VAL1 = ((ADC_res1 * 5.0)/255);

    }
}

void ADC_CH1(void){
    ADC_ch(1); //channel 1
    ADCON0bits.ADCS0 = 1;//Clock ADC conversion
    ADCON0bits.ADCS1 = 0;//Fosc
    ADCON0bits.ADON = 1;//Se habilita el ADC
    __delay_us(40);//Para conversion
    ADCON0bits.GO = 1;  //Inicia la conversión
    while(ADCON0bits.GO != 0){ //Waiting for conversion to complete
        ADC_res2 = ADC_val(ADRESL, ADRESH);
        VAL2 = ((ADC_res2 * 5.0)/255);

    }
}