/*
 * File:   main.c
 * Author: katha
 *
 * Created on 7 de febrero de 2021, 03:30 PM
 */

//******************************************************************************
//Librerias
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include "ADC1.h"
#include "DISP7.h"

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

#define _XTAL_FREQ 8000000

//******************************************************************************
//Variables
//******************************************************************************
uint8_t contador;
uint8_t ADC_res;
uint8_t NIB1_res;
uint8_t NIB2_res;
uint8_t cont = 0;

uint8_t FLAG = 0;

void DISP(void);
void MULTIPLEX(void);

//******************************************************************************
//Interrupciones
//******************************************************************************

void __interrupt() ISR(void) {
    if (INTCONbits.RBIF == 1) { //se revisa que bandera se levantó
        if (PORTBbits.RB0 == 1) {
            while (PORTBbits.RB0 == 1) {
                contador = contador;
            }
            contador = contador + 1;
            PORTC = contador;
        }
        if (PORTBbits.RB1 == 1) {
            while (PORTBbits.RB1 == 1) {
                contador = contador;
            }
            contador = contador - 1;
            PORTC = contador;
        }
    }
    INTCONbits.RBIF = 0;

    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0; //bajamos manuelmente la bandera de interrup
        __delay_ms(2);
        ADCON0bits.GO = 1;
        while (ADCON0bits.GO != 0) { //Revisa si la conversación ya finalizó
            ADC_res = ADC_val(ADRESL, ADRESH);
            NIB1_res = ADC_nib_1(ADC_res);
            NIB2_res = ADC_nib_2(ADC_res);
            //PORTD = ADC_res; //Prueba antes de multiplexado
            DISP();
        }
    }
    if (TMR0IF) { //Configuracion del timer 0 
        TMR0IF = 0;
        TMR0 = 4;
        cont = cont +1;
    }
    
}

//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup(void);


//******************************************************************************
//Ciclo Principal
//******************************************************************************

void main(void) {
    contador = 0;
    setup();
    while (1) {
        //PIR1bits.ADIF = 1;
        if (cont >= 1){
            cont = 0;
            MULTIPLEX();
        }
        if (ADC_res > contador){
            PORTEbits.RE0 = 1;
        }
        else if (ADC_res < contador){
            PORTEbits.RE0 = 0;
        }
    }
    return;
}

//******************************************************************************
//Configuracion
//******************************************************************************

void setup(void) {
    OSCCON = 0b01100001;
    ANSEL = 0b00000100;
    ANSELH = 0;
    TRISC = 0;
    PORTC = 0;
    TRISD = 0;
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    TRISB = 0b0000011;
    PORTB = 0;
    TRISA = 0b00000100;
    PORTA = 0;
    INTCONbits.GIE = 1; //Interrup Globales
    INTCONbits.RBIE = 1; //Interrup On Change PORTB
    INTCONbits.RBIF = 0;
    IOCBbits.IOCB0 = 1;
    IOCBbits.IOCB1 = 1;
    INTCONbits.PEIE = 1; //Enable interrupciones 
    ADCON0 = 0b01001001; //Fosc/8   CH2   GoDone0  
    ADCON1 = 0b00000000;
    PIE1bits.ADIE = 1;
    PIR1bits.ADIF = 1;
}

//******************************************************************************
//Funciones
//******************************************************************************
void DISP(void) {
    PORTE = 0;
    if (FLAG == 0) {
        PORTD = DISPLAY7(NIB1_res);
        PORTEbits.RE1 = 1;
    } else if (FLAG == 1) {
        PORTD = DISPLAY7(NIB2_res);
        PORTEbits.RE2 = 1;
    }

}

void MULTIPLEX(void) {
    if (FLAG == 1) {
        FLAG = 0;
    } else if (FLAG == 0) {
        FLAG = 1;

    }
}
