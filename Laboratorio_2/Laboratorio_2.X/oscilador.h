/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef OSCILADOR_H
#define	OSCILADOR_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>

//initOsc
//Inicia Osc interno
//Tiene par�metro IRCF (P�g 62 manual)
//Ejemplo uso: initOsc()
void initOsc(uint8_t IRCF);



#endif	/* OSCILADOR_H */

