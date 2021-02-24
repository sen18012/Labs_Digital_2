/*
 * File:   main.c
 * Author: katha
 *
 * Created on 22 de febrero de 2021, 10:54 PM
 */

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
// Definición e importación de librerías
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include "ADC.h"
#include "SPI.h"

//*****************************************************************************
// VARIABLES
//*****************************************************************************
#define _XTAL_FREQ 8000000
uint8_t ADC_res = 0;

//*****************************************************************************
// PROTOTIPOS DE FUNCIONES
//*****************************************************************************
void setup(void);
void ADC_CH(void);
//*****************************************************************************
// INTERRUPCIONES
//*****************************************************************************
void __interrupt() isr(void){
   if(SSPIF == 1){
//        PORTD = spiRead();
        spiWrite(ADC_res);
        SSPIF = 0;
    }
}
//*****************************************************************************
// CÓDIGO PRINCIPAL
//*****************************************************************************
void main(void) {
    setup();
    while(1){
        ADC_CH();
        }  
//    return;
}
//*****************************************************************************
// FUNCIONES
//*****************************************************************************
void setup(void){
    //IO CONFIG
    ANSEL = 0b00000001;
    ANSELH = 0;
    TRISA = 0b00000001; 
    TRISB = 0;
    TRISD = 0;
    
    PORTA = 0;
    PORTB = 0;
    PORTD = 0;
    
    ADC_ch(0); //channel 0
    ADCON0bits.ADCS0 = 1; // 
    ADCON0bits.ADCS1 = 0; //Fosc/8
    ADCON0bits.ADON = 1; //enable ADC
    
    spiInit(SPI_SLAVE_SS_EN, SPI_DATA_SAMPLE_MIDDLE, SPI_CLOCK_IDLE_LOW, SPI_IDLE_2_ACTIVE);
    
    INTCONbits.GIE = 1;         // Habilitamos interrupciones
    INTCONbits.PEIE = 1;        // Habilitamos interrupciones PEIE
    PIR1bits.SSPIF = 0;         // Borramos bandera interrupción MSSP
    PIE1bits.SSPIE = 1;         // Habilitamos interrupción MSSP
    TRISAbits.TRISA5 = 1;       // Slave Select
    
    //ADC 
    ADCON1 = 0x07; //Justificado a la izquierda

}

void ADC_CH(void) {
    __delay_ms(2);  
    ADCON0bits.GO = 1; //iniciamos conversión
    while (ADCON0bits.GO != 0) { //revisamos si ya se finalizó la conversión
//        ADC_res = ADC_val(ADRESL, ADRESH);
        ADC_res = ADRESH;
    }
}