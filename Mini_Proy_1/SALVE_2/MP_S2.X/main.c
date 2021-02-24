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
#include <stdint.h>
#include "SPI.h"

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
//Interrupciones
//****************************************************************************** 
void __interrupt() isr(void){
   if(SSPIF == 1){
//        PORTD = spiRead();
        spiWrite(contador);
        SSPIF = 0;
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
    setup();
    
    while (1) {
            if (PORTEbits.RE0 == 0) { 
                while (PORTEbits.RE0 == 0) { //ciclo para debouncing
                    contador = contador;
                }
                contador = contador + 1;
                PORTD = contador;
            }
        
            if (PORTEbits.RE1 == 0) {
                while (PORTEbits.RE1 == 0) { //ciclo para debouncing
                    contador = contador;
                }
                contador = contador - 1;
                PORTD = contador;

            }
    }
}

//******************************************************************************
//Configuracion
//******************************************************************************

void setup(void) {
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 1;
    TRISB = 0;
    TRISEbits.TRISE0 = 1;
    TRISEbits.TRISE1 = 1;
    TRISD = 0;
  
    //Limpio los puertos
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    PORTE = 0;
   
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW,SPI_IDLE_2_ACTIVE);
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    
    
    
}
//******************************************************************************
//Funciones
//******************************************************************************

