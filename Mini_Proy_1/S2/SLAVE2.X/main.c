//******************************************************************************
/*
 * File:   main.c
 * Author: katha
 *
 * Created on 16 de febrero de 2021, 11:24 PM
 */

//******************************************************************************
//Librerias
//******************************************************************************
#include <xc.h>

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
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off))

#define _XTAL_FREQ 8000000

//******************************************************************************
//Variables
//****************************************************************************** 
int contador = 0;

//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup(void);

//******************************************************************************
//Ciclo Principal
//******************************************************************************

void main(void) {
    setup();
    
    while (1) {
            if (PORTBbits.RB0 == 0) { 
                while (PORTBbits.RB0 == 0) { //ciclo para debouncing
                    contador = contador;
                }
                contador = contador + 1;
                PORTC = contador;
            }
        
            if (PORTBbits.RB1 == 0) {
                while (PORTBbits.RB1 == 0) { //ciclo para debouncing
                    contador = contador;
                }
                contador = contador - 1;
                PORTC = contador;

            }
    }
}

//******************************************************************************
//Configuracion
//******************************************************************************

void setup(void) {
    ANSEL = 0;
    ANSELH = 0;
    TRISC = 0;
    PORTC = 0;
    TRISD = 0;
    PORTD = 0;
    TRISE = 0;
    PORTE = 0;
    TRISB = 0b00000011;
    PORTB = 0;
    TRISA = 0; 
    PORTA = 0;
    
    
    
    
    
}
//******************************************************************************
//Funciones
//******************************************************************************

