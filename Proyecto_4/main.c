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
uint32_t parqueo = 0;    //Valor para el Display

uint32_t P1 = 0;         //Banderas para cada parqueo
uint32_t P2 = 0;
uint32_t P3 = 0;
uint32_t P4 = 0;

char val = '0';         //Valor para enviar

//**************************************
// PROTOTIPOS DE FUNCIONES
//**************************************
void revisar(void);             //Revisar que Parqueos están dispobibles
void display_7seg(void);        //Tambla para 7 segmentos
void datos_uart(void);          //Elegir que dato debe enviarse
void InitUART(void);            //Inicialización UART

/**
 * main.c
 */
int main(void)
{

//**************************************
// CONFIG
//**************************************
    //CONGIF DE FRECUENCIA (40 MHz)
    SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_16MHZ);  //FREQ - 40MHz

    //CONFIG HABILITAR PUERTOS
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOA)){
    }
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOB)){
        }
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOC)){
            }
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOD)){
        }
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOE)){
        }
    while (!SysCtlPeripheralReady (SYSCTL_PERIPH_GPIOF)){
        }


    //IO CONFIG
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7); //Indicadores Parqueo 1 y 2
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4); //Indicadores Parqueos 3 y 4
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7); //Indicador 7seg

    GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_0| GPIO_PIN_1| GPIO_PIN_2 | GPIO_PIN_3); //Sensores todos los parqueos

    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);
    GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_3, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPD);

    InitUART();

    P1 = 0;
    P2 = 0;
    P3 = 0;
    P4 = 0;


//**************************************
// MAIN LOOP
//**************************************
    while(1){
        revisar();
        display_7seg();
        datos_uart();
        UARTCharPut(UART1_BASE, val);
        }
    }

//**************************************************************************************************************
// FUNCIONES
//**************************************************************************************************************
void revisar(void)
{
    parqueo = 0;
    P1 = 0;
    P2 = 0;
    P3 = 0;
    P4 = 0;


    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) )  { //Revisamo Parqueo 1

        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_7); //Parqueo Desocupado
        parqueo = parqueo+1;
        P1 = 1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_0) ) {
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_6); //Parqueo Ocupado
            P1 = 0;
       }

    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) )  { //Revisamo Parqueo 2

        GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_3); //Parqueo Desocupado
        parqueo = parqueo+1;
        P2 = 1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_1) ) {
            GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2 | GPIO_PIN_3, GPIO_PIN_2); //Parqueo Ocupado
            P2 = 0;
    }
    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) )  { //Revisamo Parqueo 3

        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_2); //Parqueo Desocupado
        parqueo = parqueo+1;
        P3 = 1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_2) ) {
        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_1); //Parqueo Ocupado
        P3 = 0;
    }
    if ( !GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) )  { //Revisamo Parqueo 4

        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_4); //Parqueo Desocupado
        parqueo = parqueo+1;
        P4 = 1;
    }

    else if ( GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_3) ) {
            GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3 | GPIO_PIN_4, GPIO_PIN_3); //Parqueo Ocupado
            P4 = 0;
    }

   }

void display_7seg(void)
{

    if (parqueo == 0){ //0 parqueos libres
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, 0x3F);
    }
    else if (parqueo == 1) {//1 parqueo libre
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x06);
       }
    else if (parqueo == 2) {//2 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x5B);
           }
    else if (parqueo == 3) {//3 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x4F);
           }
    else if (parqueo == 4) {//4 parqueos libres
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7,0x66);
           }


    }


void datos_uart(void){
    if (P1 == 0 && P2 == 0 && P3 == 0 && P4 == 0){
        val = 'P';
    }
    else if (P1 == 0 && P2 == 0 && P3 == 0 && P4 == 1){
        val = 'O';
    }
    else if (P1 == 0 && P2 == 0 && P3 == 1 && P4 == 0){
        val = 'N';
    }
    else if (P1 == 0 && P2 == 0 && P3 == 1 && P4 == 1){
        val = 'M';
    }
    else if (P1 == 0 && P2 == 1 && P3 == 0 && P4 == 0){
        val = 'L';
    }
    else if (P1 == 0 && P2 == 1 && P3 == 0 && P4 == 1){
        val = 'K';
    }
    else if (P1 == 0 && P2 == 1 && P3 == 1 && P4 == 0){
        val = 'J';
    }
    else if (P1 == 0 && P2 == 1 && P3 == 1 && P4 == 1){
        val = 'I';
    }
    else if (P1 == 1 && P2 == 0 && P3 == 0 && P4 == 0){
        val = 'H';
    }
    else if (P1 == 1 && P2 == 0 && P3 == 0 && P4 == 1){
        val = 'G';
    }
    else if (P1 == 1 && P2 == 0 && P3 == 1 && P4 == 0){
        val = 'F';
    }
    else if (P1 == 1 && P2 == 0 && P3 == 1 && P4 == 1){
        val = 'E';
    }
    else if (P1 == 1 && P2 == 1 && P3 == 0 && P4 == 0){
        val = 'D';
    }
    else if (P1 == 1 && P2 == 1 && P3 == 0 && P4 == 1){
        val = 'C';
    }
    else if (P1 == 1 && P2 == 1 && P3 == 1 && P4 == 0){
        val = 'B';
    }
    else if (P1 == 1 && P2 == 1 && P3 == 1 && P4 == 1){
        val = 'A';
    }
}



void InitUART(void){

    /*Enable the peripheral UART Module 1*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART1)){
    }

    /*Enable the GPIO Port C*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinConfigure(GPIO_PC4_U1RX);
    GPIOPinConfigure(GPIO_PC5_U1TX);

    // Se habilitan las interrupciones Globales
    IntMasterEnable();

    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    UARTDisable(UART1_BASE);
    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(
            UART1_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    IntEnable (INT_UART1);

    UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);
    UARTEnable (UART1_BASE);
}


