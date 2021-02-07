/* 
 * File:ADC   
 * Author:KATHA SENN
 * Comments:-
 * Revision history: -
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef ADC_H
#define	ADC_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//ADC
//Obtener Datos del ADC para Multiplexar
uint8_t ADC_val(uint8_t ADRES_L, uint8_t ADRES_);
uint8_t ADC_nib_1(uint8_t val_ADC);
uint8_t ADC_nib_2(uint8_t val_ADC);



#endif	/* ADC_H */

