#include "USART.h"
#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 8000000

uint8_t USART_LECTURA(){
  while(!RCIF);
  return RCREG;
} 


void USART_ESCRITURA(uint8_t a){
    while(!TRMT);
    TXREG = a;
}
void USART_STRING(char *a){
    uint8_t i;
    for(i=0;a[i]!='\0';i++){
        USART_ESCRITURA(a[i]); //ciclo for para crear un string de datos
    }
}

