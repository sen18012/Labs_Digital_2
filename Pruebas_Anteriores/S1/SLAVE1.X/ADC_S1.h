/* 
 * File:ADC   
 * Author:KATHA SENN
 * Comments:-
 * Revision history: -
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_S1_H
#define	ADC_S1_H


#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//ADC
//Obtener Datos del ADC para Multiplexar
uint8_t ADC_val(uint8_t ADRES_L, uint8_t ADRES_);

unsigned ADC_ch(unsigned short channel); //FOSC/8


#endif	/* ADC_S1_H */