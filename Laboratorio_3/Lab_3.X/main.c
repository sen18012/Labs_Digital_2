/*
 * File:   main.c
 * Author: katha
 *
 * Created on 7 de febrero de 2021, 02:03 PM
 */

//******************************************************************************
//Librerias
//******************************************************************************
#include <xc.h>
#include <stdint.h>
#include "LCD.h"

// CONFIG1
#pragma config FOSC = HS        // Oscillator Selection bits (XT oscillator: Crystal/resonator on RA6/OSC2/CLKOUT and RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = ON      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

//******************************************************************************
//Variables
//******************************************************************************

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


//******************************************************************************
//Prototipos de funciones
//******************************************************************************
void setup (void);

//******************************************************************************
//Ciclo Principal
//******************************************************************************
void main(void) {
  setup ();
  unsigned int a;
  TRISD = 0x00;
  Lcd_Init();
  while(1)
  {
    Lcd_Clear();
    Lcd_Set_Cursor(1,1);
    Lcd_Write_String("S1:");
    Lcd_Set_Cursor(1,5);
    Lcd_Write_String("S2:");
    __delay_ms(2000);
//    Lcd_Clear();
//    Lcd_Set_Cursor(1,1);
//    Lcd_Write_String("Developed By");
//    Lcd_Set_Cursor(2,1);
//    Lcd_Write_String("electroSome");
//    __delay_ms(2000);
//    Lcd_Clear();
//    Lcd_Set_Cursor(1,1);
//    Lcd_Write_String("www.electroSome.com");
//
//    for(a=0;a<15;a++)
//    {
//        __delay_ms(300);
//        Lcd_Shift_Left();
//    }
//
//    for(a=0;a<15;a++)
//    {
//        __delay_ms(300);
//        Lcd_Shift_Right();
//    }
//
//    Lcd_Clear();
//    Lcd_Set_Cursor(2,1);
//    Lcd_Write_Char('e');
//    Lcd_Write_Char('S');
//    __delay_ms(2000);
  }
    return;
}

//******************************************************************************
//Funciones
//******************************************************************************
void setup (void){
    ANSEL = 0b00000011;
    ANSELH = 0;
    TRISA = 0b00000011;
    TRISB = 0;
    TRISC = 0;
    TRISD = 0;
    TRISE = 0;
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    PORTD = 0;
    PORTE = 0;
 
    
    
}