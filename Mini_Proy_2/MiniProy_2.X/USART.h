/* 
 * File:USART 
 * Author:KATHA SENN
 * Comments:-
 * Revision history: -
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __USART_H_
#define	__USART_H_

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

uint8_t USART_LECTURA (void);
//void Read_USART_String(char *Output, unsigned int length);


void USART_ESCRITURA (uint8_t a);
void USART_STRING(char *a);


#endif	/* __USART_H_ */
