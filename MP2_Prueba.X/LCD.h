//
///* 
// * File:LCD   
// * Author:KATHA SENN
// * Comments:
// * Revision history: 
// */
//
// * File: LCD.h  
// * Se utiliz� como plantilla el archivo en el repositorio compartido en clase
// * Se adaptar� para que funciones con 8bits
// * 
// * Comentarios anteriores
// * Se utiliz� y se adaptaron las librer�as de Ligo George 
// * de la p�gina www.electrosome.com
// * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
// * Revision history: 
// */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __LCD_H_
#define	__LCD_H_


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

#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000
#endif

#ifndef RS
#define RS RE0
#endif

//#ifndef RW
//#define RW RE1
//#endif

#ifndef EN
#define EN RE2
#endif

#ifndef D0
#define D0 RD0
#endif

#ifndef D1
#define D1 RD1
#endif

#ifndef D2
#define D2 RD2
#endif

#ifndef D3
#define D3 RD3
#endif

#ifndef D4
#define D4 RD4
#endif

#ifndef D5
#define D5 RD5
#endif

#ifndef D6
#define D6 RD6
#endif

#ifndef D7
#define D7 RD7
#endif

#include <xc.h> // include processor files - each processor file is guarded.  

//LCD Functions Developed by electroSome


void Lcd_Port(char a);

void Lcd_Cmd(char a);

void Lcd_Clear(void);

void Lcd_Set_Cursor(char a, char b);

void Lcd_Init(void);

void Lcd_Write_Char(char a);

void Lcd_Write_String(char *a);

void Lcd_Shift_Right(void);

void Lcd_Shift_Left(void);

#endif	/* __LCD_H_ */