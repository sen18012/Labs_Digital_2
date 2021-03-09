/*
 * File:   main.c
 * Author: katha
 *
 * Created on 5 de marzo de 2021, 10:48 PM
 */

//*****************************************************************************
//*****************************************************************************
// Palabra de configuración
//*****************************************************************************
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (RCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, RC on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

//*****************************************************************************
// LIBRERÍAS
//*****************************************************************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "LCD.h"
#include "USART.h"
#include "I2C.h"

//*****************************************************************************
// VARIABLES
//*****************************************************************************
#define _XTAL_FREQ 8000000

#define RS RE0
#define EN RE1
#define D0 RD0
#define D1 RD1
#define D2 RD2
#define D3 RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

//char USART_LEER;
//uint8_t cont = 0;
char time[] = "TIME:  :  :  ";
char calendar[] = "DATE:  /  /  ";

unsigned int i;
unsigned int second;
unsigned int minute;
unsigned int hour;
unsigned int date;
unsigned int month;
unsigned int year;

//******************************************************************************
//INTERRUPCIONES
//****************************************************************************** 

//*****************************************************************************
// PROTOTIPOS DE FUNCIONES
//*****************************************************************************
void setup(void);
void DS3231(void);


//*****************************************************************************
// Código Principal
//*****************************************************************************

void main(void) {
    setup();
    TRISD = 0x00;
    Lcd_Init(); //Inicializar LCD
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1); //Elegimos localidad para escribir
    Lcd_Write_String(time);
    Lcd_Set_Cursor(2, 1); //Elegimos localidad para escribir
    Lcd_Write_String(calendar);
    
    while (1) {
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0);
        I2C_Master_Start();
        I2C_Master_Write(0xD1);
        second = I2C_Master_Read(1);
        minute = I2C_Master_Read(1);
        hour = I2C_Master_Read(1);
        I2C_Master_Read(1);
        date = I2C_Master_Read(1);
        month = I2C_Master_Read(1);
        year = I2C_Master_Read(0);
        I2C_Master_Stop();
        DS3231();
        __delay_ms(50);
        
        Lcd_Set_Cursor(1, 1); // Go to column 1 row 1
        Lcd_Write_String(time);
        Lcd_Set_Cursor(2, 1); // Go to column 1 row 2
        Lcd_Write_String(calendar);

        USART_STRING(time); //enviamos el string con la info
        USART_STRING(calendar); //enviamos el string con la info

        USART_ESCRITURA(13); //Salto de linea en la terminal, para que se entienda mejor
        USART_ESCRITURA(10);

    }
    return;
}
//*****************************************************************************
// Función de Inicialización
//*****************************************************************************

void setup(void) {
    ANSEL = 0;
    ANSELH = 0;
    TRISB = 0;
    TRISE = 0;
    TRISD = 0;

    PORTC = 0;
    PORTE = 0;
    PORTD = 0;
    PORTB = 0;
    
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C

    // USART CONFIG
    SPBRGH = 0;
    SPBRG = 12; //BAUD RATE DE 9600 A 8MHz

    TXSTAbits.TXEN = 1; //ENABLE
    TXSTAbits.SYNC = 0; // ASYNC

    RCSTAbits.SPEN = 1; // ENABLE
    RCSTAbits.CREN = 1;
    RCREG = 0;


}

void DS3231() {
//     Convert BCD to decimal
    second = (second >> 4) * 10 + (second & 0x0F);
    minute = (minute >> 4) * 10 + (minute & 0x0F);
    hour = (hour >> 4) * 10 + (hour & 0x0F);
    date = (date >> 4) * 10 + (date & 0x0F);
    month = (month >> 4) * 10 + (month & 0x0F);
    year = (year >> 4) * 10 + (year & 0x0F);
//     End conversion
    

    
    time[12] = second % 10 + 48;
    time[11] = second / 10 + 48;
    time[9] = minute % 10 + 48;
    time[8] = minute / 10 + 48;
    time[6] = hour % 10 + 48;
    time[5] = hour / 10 + 48;
    calendar[12] = year % 10 + 48;
    calendar[11] = year / 10 + 48;
    calendar[9] = month % 10 + 48;
    calendar[8] = month / 10 + 48;
    calendar[6] = date % 10 + 48;
    calendar[5] = date / 10 + 48;
   
}

