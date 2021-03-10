/*
 * File:   main.c
 * Author: katha
 *
 * Created on 5 de marzo de 2021, 10:48 PM
 */

//***************************
//***************************
// Palabra de configuración
//***************************
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

//***************************
// LIBRERÍAS
//***************************
#include <xc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic16f887.h>
#include "LCD.h"
#include "USART.h"
#include "I2C.h"

//***************************
// VARIABLES
//***************************
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

char USART_LEER;
char time[] = "TIME:   :  :  ";
char date1[] = "DATE:   /  /20  ";
char datos[];
int led;



/*Set the current value of date and time below*/
int sec = 00;
int min = 00;
int hour = 00;
int date = 00;
int month = 00;
int year = 00;
/*Time and Date Set*/

//**************************
//INTERRUPCIONES
//************************** 
//void __interrupt() ISR(void) {
//    if (PIR1bits.RCIF == 1) {
//        PIR1bits.RCIF = 0; //Resetea bandera RCIF
//        led = RCREG;       //Se lee el registro y se guarda
//    }
//    if (led == 0X0A) {      //Dependiendo del dato que entra en RX enciende o apaga las leds
//        PORTE = 0;          //depende de los botones en el IOT cloud
//    } else if (led == 0X0B) {
//        PORTE = 1;
//    } else if (led == 0X0C) {
//        PORTE = 0;
//    } else if (led == 0X0D) {
//        PORTE = 2;
//    }
//    return;
//}
//***************************
// PROTOTIPOS DE FUNCIONES
//***************************
void setup(void);
void DS3231_time(void);
uint8_t bcd_to_decimal(uint8_t number);

//***************************
// Código Principal
//***************************

void main(void) {
    setup();
    TRISD = 0x00;
    Lcd_Init(); //Inicializar LCD
    Lcd_Clear();
    Lcd_Set_Cursor(1, 1); //Elegimos localidad para escribir
    Lcd_Write_String(time);
    Lcd_Set_Cursor(2, 1); //Elegimos localidad para escribir
    Lcd_Write_String(date1);

    I2C_Master_Start();
    I2C_Master_Write(0xD0);
    I2C_Master_Write(0);
    I2C_Master_Write(0);
    I2C_Master_Write(0);
    I2C_Master_Write(1);
    I2C_Master_Write(0);
    I2C_Master_Write(10);
    I2C_Master_Write(3);
    I2C_Master_Write(27);
    I2C_Master_Stop();
    __delay_ms(200);

    while (1) {
        I2C_Master_Start();
        I2C_Master_Write(0xD0);
        I2C_Master_Write(0);
        I2C_Master_RepeatedStart();
        I2C_Master_Write(0xD1);
        sec = I2C_Master_Read(1);
        min = I2C_Master_Read(1);
        hour = I2C_Master_Read(1);
        I2C_Master_Read(1);
        date = I2C_Master_Read(1);
        month = I2C_Master_Read(1);
        year = I2C_Master_Read(0);
        I2C_Master_Stop();

        DS3231_time();
        __delay_ms(200);

        Lcd_Set_Cursor(1, 1); // Go to column 1 row 1
        Lcd_Write_String(time);
        Lcd_Set_Cursor(2, 1); // Go to column 1 row 2
        Lcd_Write_String(date1);

        sprintf(datos, "%.0i", sec);
//        USART_STRING(time); //enviamos el string con la info
//        USART_STRING(date1); //enviamos el string con la info
        USART_STRING(datos); //enviamos el string con la info

        USART_ESCRITURA(13); //Salto de linea en la terminal, para que se entienda mejor
        USART_ESCRITURA(10);

    }
//    return;
}
//***************************
// Función de Inicialización
//***************************

void setup(void) {
    ANSEL = 0;
    ANSELH = 0;
    TRISA = 0;
    TRISB = 0;
    TRISCbits.TRISC3 = 0; //RC3 salida para SCL
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;
    TRISD = 0;
    TRISE = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
    I2C_Master_Init(100000);        // Inicializar Comuncación I2C

    // USART CONFIG
    SPBRGH = 0;
    SPBRG = 12; //BAUD RATE DE 9600 A 8MHz

    TXSTAbits.TXEN = 1; //ENABLE
    TXSTAbits.SYNC = 0; // ASYNC

    RCSTAbits.SPEN = 1; // ENABLE
    RCSTAbits.CREN = 1;
    RCREG = 0;
    
    //INTERRUP
    PIE1bits.RCIE = 1;
    PIE1bits.TXIE = 0; //No se habilitan interrupciones en el envio
    PIR1bits.RCIF = 0; //Se apaga la interrupcion

    INTCONbits.GIE = 1; //Interrupciones del timer
    
//        //Transmision
//    TXSTAbits.TXEN = 1; //Se habilita TX
//    TXSTAbits.SYNC = 0; //modo Asíncrono
//    RCSTAbits.SPEN = 1; //Se habilita RX
//    TXSTAbits.TX9 = 0; //Se transmiten 8 bits
//
//    //Lectura
//    RCSTAbits.CREN = 1; //Se habilita recibir datos
//    RCSTAbits.RX9 = 0; //Se reciben solo 8 bits
//
//    //ENCENDEMOS INTERRUPCIONES
//    PIE1bits.RCIE = 1;
//    PIE1bits.TXIE = 0; //No se habilitan interrupciones en el envio
//    PIR1bits.RCIF = 0; //Se apaga la interrupcion
//
//    INTCONbits.GIE = 1; //Interrupciones del timer
   
}
// convert BCD to decimal function
uint8_t bcd_to_decimal(uint8_t number) {
  return((number >> 4) * 10 + (number & 0x0F));
}

void DS3231_time() {
    

  // convert data from BCD format to decimal format
  sec = bcd_to_decimal(sec);
  min = bcd_to_decimal(min);
  hour   = bcd_to_decimal(hour);
  date  = bcd_to_decimal(date);
  month  = bcd_to_decimal(month);
  year   = bcd_to_decimal(year);
  // end conversion
 
  // update time
  time[6]  = hour / 10 + '0';
  time[7]  = hour % 10 + '0';
  time[9]  = min / 10 + '0';
  time[10] = min % 10 + '0';
  time[12] = sec / 10 + '0';
  time[13] = sec % 10 + '0';
  // update date
  date1[6]  = date / 10 + '0';
  date1[7]  = date % 10 + '0';
  date1[9]  = month / 10 + '0';
  date1[10] = month % 10 + '0';
  date1[14] = year / 10 + '0';
  date1[15] = year % 10 + '0';

}