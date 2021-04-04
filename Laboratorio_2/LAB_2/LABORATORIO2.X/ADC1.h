/* 
 * File:ADC   
 * Author:KATHA SENN
 * Comments:-
 * Revision history: -
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC1_H
#define	ADC1_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//ADC
//Obtener Datos del ADC para Multiplexar
uint8_t ADC_val(uint8_t ADRESL_, uint8_t ADRESH_);
uint8_t ADC_nib_1(uint8_t val_ADC);
uint8_t ADC_nib_2(uint8_t val_ADC);



#endif	/* ADC1_H */
