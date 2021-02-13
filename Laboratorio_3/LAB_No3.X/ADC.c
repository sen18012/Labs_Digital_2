#include "ADC.h"
#include <xc.h>
#include <stdint.h>

#define _XTAL_FREQ 8000000

uint8_t VALOR_ADC;
unsigned short channel;



uint8_t ADC_val(uint8_t ADRES_L, uint8_t ADRES_H){
    VALOR_ADC = ((ADRESL << 8) | ADRESH);

    return VALOR_ADC;
}

unsigned ADC_ch(unsigned short channel){
     switch (channel) {
        case 0:
            ADCON0bits.CHS3 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS0 = 0; //CH0

            break;
        case 1:
            ADCON0bits.CHS3 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS0 = 1; //CH1
            break;
        default:
            ADCON0bits.CHS3 = 0;
            ADCON0bits.CHS2 = 0;
            ADCON0bits.CHS1 = 0;
            ADCON0bits.CHS0 = 0; //CH0  
            break;
     }
}


