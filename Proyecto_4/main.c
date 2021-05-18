//**************************************
// Universidad del Valle de Guatemala                                                     Katharine Senn Salazar
// Electrónica Digital 2                                                                             Carné 18012
// Sección 20
//                                              Proyecto 4
//                                   (Sistema de Control de Parqueos)
//
//**************************************
// LIBRERÍAS
//**************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/sysctl.c"
#include "driverlib/interrupt.h"
#include "driverlib/interrupt.c"
#include "driverlib/gpio.h"
#include "driverlib/gpio.c"
#include "driverlib/timer.h"
#include "driverlib/timer.c"
#include "driverlib/uart.h"
#include "driverlib/uart.c"
#include "driverlib/pin_map.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "grlib/grlib.h"
#include <string.h>

#define XTAL 16000000

//**************************************
// VARIABLES
//**************************************
uint32_t parqueo = 0;

//**************************************
// PROTOTIPOS DE FUNCIONES
//**************************************
void revisar(void);
void display_7seg(void);


/**
 * main.c
 */
int main(void)
{

//**************************************
// CONFIG
//**************************************
    //CONGIF DE FRECUENCIA (40 MHz)
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN| SYSCTL_XTAL_16MHZ);  //FREQ - 40MHz

    //CONFIG HABILITAR PUERTOS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);


    //VERIFICAR HABILITACIÓN DE PUERTO
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA)){
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB)){
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC)){
        }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD)){
        }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE)){
        }



    //IO CONFIG
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //Indicadores Parqueos 1 y 2
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4); //Indicadores Parqueos 3 y 4
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //Indicador 7seg

    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_2 | GPIO_PIN_3); //Sensores todos los parqueos

    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);



//**************************************
// MAIN LOOP
//**************************************
    while(1){
        revisar();
        display_7seg();
        }
    }

//**************************************************************************************************************
// FUNCIONES
//**************************************************************************************************************
void revisar(void)
{
    parqueo = 0;
    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) )  { //Revisamo Parqueo 1

        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_5); //Parqueo Desocupado
        parqueo = parqueo+1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) ) {
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_4); //Parqueo Ocupado

       }

    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) )  { //Revisamo Parqueo 2

        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_7); //Parqueo Desocupado
        parqueo = parqueo+1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) ) {
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_6); //Parqueo Ocupado

       }
    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) )  { //Revisamo Parqueo 3

        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_2); //Parqueo Desocupado
        parqueo = parqueo+1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) ) {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_1); //Parqueo Ocupado

       }
    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) )  { //Revisamo Parqueo 4

        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_4); //Parqueo Desocupado
        parqueo = parqueo+1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) ) {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_3); //Parqueo Ocupado

       }

    }

void display_7seg(void)
{

    if (parqueo == 0){ //0 parqueos libres
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x3F);
    }
    else if (parqueo == 1) {//1 parqueo libre
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x06);;
       }
    else if (parqueo == 2) {//2 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x5B);;
           }
    else if (parqueo == 3) {//3 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x4F);;
           }
    else if (parqueo == 4) {//4 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x66);;
           }


    }






