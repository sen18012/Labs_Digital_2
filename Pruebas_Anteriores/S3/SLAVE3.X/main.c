/*
 * File:   main.c
 * Author: katha
 *
 * Created on 16 de febrero de 2021, 23:55 PM
 */

//******************************************************************************
//Librerias
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "ADC_S3.h"


#define _XTAL_FREQ 8000000

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
uint8_t ADC_res;
uint8_t ADC_con;

//******************************************************************************
//Interrupciones
//****************************************************************************** 

//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup(void);
void ADC_CH(void);
void sem(void);
//******************************************************************************
//Ciclo Principal
//******************************************************************************

void main(void) {
    setup();
    while (1) {
        ADC_CH();
        //PORTD = ADC_con;
    }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************

void setup(void) {
    ANSEL = 0b00000001; //IO CONFIG
    ANSELH = 0;
    TRISA = 0b00000001;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    //ADC 
    ADCON1 = 0b00010000; //Justificado a la izquierda
    PIE1bits.ADIE = 0;
    PIR1bits.ADIF = 0;
    OPTION_REG = 0b00000000;
    INTCON = 0b00000000;

}

void sem(void){
    if (ADC_con < 26){
        PORTDbits.RD0 = 0;
        PORTDbits.RD1 = 0;
        PORTDbits.RD2 = 1;
    }
    else if (25 <= ADC_con && ADC_con <= 36){
        PORTDbits.RD0 = 0;
        PORTDbits.RD1 = 1;
        PORTDbits.RD2 = 0;
    }
    else if (ADC_con >36){
        PORTDbits.RD0 = 1;
        PORTDbits.RD1 = 0;
        PORTDbits.RD2 = 0;
    }
}

void ADC_CH(void) {
    ADC_chT(0); //channel 0
    ADCON0bits.ADCS0 = 1; // 
    ADCON0bits.ADCS1 = 0; //Fosc/8
    ADCON0bits.ADON = 1; //enable ADC
    __delay_us(40); 
    ADCON0bits.GO = 1; //iniciamos conversión
    while (ADCON0bits.GO != 0) { //revisamos si ya se finalizó la conversión
        ADC_res = ADRESH;
        
    }
    ADC_con = ((ADRESH*150)/255);
    sem();
}