//**************************************************************************************************************
// Universidad del Valle de Guatemala                                                     Katharine Senn Salazar
// Electrónica Digital 2                                                                             Carné 18012
// Sección 20
//                                              Laboratorio 7
//
//**************************************************************************************************************
// LIBRERÍAS
//**************************************************************************************************************
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

//**************************************************************************************************************
// PROTOTIPOS DE FUNCIONES
//**************************************************************************************************************
void UART_fun(void);
void TMR0_fun(void);


//**************************************************************************************************************
// VARIABLES
//**************************************************************************************************************
int LUZ_ON = false;
char color='a';


int main(void)
{
    //**************************************************************************************************************
    // CONFIG
    //**************************************************************************************************************
    SysCtlClockSet ( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ ) ; //FREQ 40MHz
    SysCtlPeripheralEnable (SYSCTL_PERIPH_GPIOF ) ;     //PUERTO F
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)){}
    SysCtlPeripheralEnable (SYSCTL_PERIPH_UART0);   //CONFIG UART0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0)){}

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);        //CONFIG PUERTO A

    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_UART0))
    GPIOPinConfigure(GPIO_PA0_U0RX);    //CONFIG tX/RX
    GPIOPinConfigure(GPIO_PA1_U0TX);

    IntMasterEnable();
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1); //CONFIG PINES, BdRATE
    UARTDisable(UART0_BASE);
    UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    IntEnable (INT_UART0);
    UARTIntEnable (UART0_BASE, UART_INT_RX);
    UARTEnable (UART0_BASE);
    IntPrioritySet(INT_UART0, 0x0);
    IntRegister(INT_UART0, UART_fun);
    UARTFIFOEnable(UART0_BASE);
    UARTFIFOLevelSet(UART0_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);


    SysCtlPeripheralEnable (SYSCTL_PERIPH_TIMER0); //CONFIG TMR0
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)){}
    SysCtlPeripheralReset (SYSCTL_PERIPH_TIMER0);
    SysCtlDelay (5);
    TimerDisable(TIMER0_BASE, TIMER_A|TIMER_B);
    TimerConfigure (TIMER0_BASE,TIMER_CFG_PERIODIC); //PERIODICO
    TimerLoadSet (TIMER0_BASE, TIMER_A, 20000000 -1);
    TimerEnable (TIMER0_BASE, TIMER_A|TIMER_B);
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    TimerIntRegister(TIMER0_BASE, TIMER_A, TMR0_fun);//ESTAB. ISR
    IntEnable(INT_TIMER0A); // INTERUP ENABLE
    TimerEnable(TIMER0_BASE, TIMER_A); //TEMP ENABLE

    //CONFIG PINES
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE,GPIO_PIN_1) ;//RED
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_2) ;//BLUE
    GPIOPinTypeGPIOOutput (GPIO_PORTF_BASE, GPIO_PIN_3) ;//GREEN

    while(1){}
}
void TMR0_fun(){
    TimerIntClear(TIMER0_BASE,  TIMER_TIMA_TIMEOUT);
        if (LUZ_ON){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x0);
        }else{
            switch(color){
                            case 'o':
                                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x0);
                                // color_pre='o';
                                break;
                            case 'r':
                                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x02);
                                //color_pre='r';
                                break;
                            case 'b':
                                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x04);
                               // color_pre='b';
                                break;

                            case 'g':
                                GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,0x08);
                               // color_pre='g';
                                break;

                                }

        }
        LUZ_ON = !LUZ_ON;


}
void UART_fun(){
    uint32_t ui32Status;

        ui32Status = UARTIntStatus(UART0_BASE, true); //Revisar el estado de la bandera para iniciar interupción

        UARTIntClear(UART0_BASE, ui32Status); //Limpiar la bandera


        while(UARTCharsAvail(UART0_BASE)) //Mientras FIFO tenga datos
        {
            color=UARTCharGet(UART0_BASE);
            UARTCharPutNonBlocking(UART0_BASE,color);
        }


}

