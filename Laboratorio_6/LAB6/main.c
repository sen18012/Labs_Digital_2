//**************************************************************************************************************
// Universidad del Valle de Guatemala                                                     Katharine Senn Salazar
// Electrónica Digital 2                                                                             Carné 18012
// Sección 20
//                                              Laboratorio 6
//
//**************************************************************************************************************
// LIBRERÍAS
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/systick.h"

#define XTAL 16000000

//**************************************************************************************************************
// DEF. PINES
//**************************************************************************************************************
#define Rojo GPIO_PIN_1
#define Verde GPIO_PIN_3
#define Amarillo GPIO_PIN_1|GPIO_PIN_3
#define OFF 0x00000000

//**************************************************************************************************************
// VARIABLES
//**************************************************************************************************************
uint32_t i = 0;
uint32_t msec_FIJO = 1000;
uint32_t msec_OFF = 250;
uint32_t msec_ON = 500;


//**************************************************************************************************************
// PROTOTIPOS DE FUNCIONES
//**************************************************************************************************************
void delay(uint32_t msec);
void delay1ms(void);

//**************************************************************************************************************
// INICIO PROGRAMA
//**************************************************************************************************************
int main(void)
{
    //CONGIF DE FRECUENCIA (DISTINTOS VALORES)
    //SysCtlClockSet(SYSCTL_SYSDIV_2 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_32MHZ);  //FREQ - 32MHz
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN| SYSCTL_XTAL_16MHZ);  //FREQ - 16MHz

    // CONFIG RELOJ EN F / PINES PB Y LEDS
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); //Configuramos en F
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3); //Leds - OUTPUTS
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4); //PB - INPUT
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

   //**********************************************************************************************************
   // LOOP PRINCIPAL
   //**********************************************************************************************************
   while (1) {

       if ( !GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) )  { //Si el PB está presionado se inicia la secuencia

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Verde); //Verde - Fijo
           delay(msec_FIJO);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Verde); //Verde - Parpadeo 1
           delay(msec_ON);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Verde); //Verde - Parpadeo 2
           delay(msec_ON);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Verde); //Verde - Parpadeo 3
           delay(msec_ON);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Amarillo); //Amarillo - Fijo
           delay(msec_FIJO);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, Rojo); //Rojo - Fijo
           delay(msec_FIJO);

           GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, OFF); //Apagamos LED
           delay(msec_OFF);

       }

    }
}
//**************************************************************************************************************
// FUNCIONES
//**************************************************************************************************************
void delay(uint32_t msec)
{
    for (i = 0; i < msec; i++)
    {
        delay1ms();
    }

}

void delay1ms(void)
{
    SysTickDisable();
    SysTickPeriodSet(16000);
    SysTickEnable();

    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0);

}
