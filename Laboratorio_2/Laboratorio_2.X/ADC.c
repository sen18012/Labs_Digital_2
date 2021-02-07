#include "ADC.h"

uint8_t VALOR_ADC;
uint8_t VALOR_SWAP;
uint8_t VALOR_NIB1;
uint8_t VALOR_NIB2;


uint8_t ADC_val(uint8_t ADRES_L, uint8_t ADRES_H){
    VALOR_ADC = ((ADRESL << 8) | ADRESH);
    return VALOR_ADC;
}

uint8_t ADC_nib_1(uint8_t val_ADC_){
    VALOR_NIB1 = (val_ADC_ & 15);
    return VALOR_NIB1;
}

uint8_t ADC_nib_2(uint8_t val_ADC_1){
    VALOR_SWAP = (((val_ADC_1 & 0x0F)<<4) | ((val_ADC_1 & 0x0F)>>4));
    VALOR_NIB2 = (VALOR_SWAP & 15);
    return VALOR_NIB2;
}


