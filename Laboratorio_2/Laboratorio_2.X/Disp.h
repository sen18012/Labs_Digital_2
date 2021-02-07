/* 
 * File:Disp  
 * Author:KATHA SENN
 * Comments:-
 * Revision history: -
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DISP_H
#define	DISP_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//DISPLAY 7 SEGMENTOS
//
void disp_val(uint8_t val_display);



#endif	/* DISP_H */
