#include "USART.h"
#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 8000000

//char USART_Init(const long int baudrate){
//  unsigned int x;
//  x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);   //SPBRG for Low Baud Rate
//  if(x>255)                                       //If High Baud Rage Required
//  {
//    x = (_XTAL_FREQ - baudrate*16)/(baudrate*16); //SPBRG for High Baud Rate
//    BRGH = 1;                                     //Setting High Baud Rate
//  }
//  if(x<256)
//  {
//    SPBRG = x;                                    //Writing SPBRG Register
//    SYNC = 0;                                     //Setting Asynchronous Mode, ie UART
//    SPEN = 1;                                     //Enables Serial Port
//    TRISC7 = 1;                                   //As Prescribed in Datasheet
//    TRISC6 = 0;                                   //As Prescribed in Datasheet
//    CREN = 1;                                     //Enables Continuous Reception
//    TXEN = 1;                                     //Enables Transmission
//    return 1;                                     //Returns 1 to indicate Successful Completion
//  }
//  return 0;                                       //Returns 0 to indicate UART initialization failed
//}

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
        USART_ESCRITURA(a[i]);
    }
}

