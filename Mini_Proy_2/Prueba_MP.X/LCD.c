/* 
 * File: LCD.h  
 * Se utiliz� como plantilla el archivo en el repositorio compartido en clase
 * Se adaptar� para que funciones con 8bits
 * 
 * Comentarios anteriores
 * Se utiliz� y se adaptaron las librer�as de Ligo George 
 * de la p�gina www.electrosome.com
 * Enlace: https://electrosome.com/lcd-pic-mplab-xc8/
 * Revision history: 
 */

//LCD Functions Developed by electroSome

#include "LCD.h"

void Lcd_Port(char a)
{
    PORTD = a;

}
void Lcd_Cmd(char a)
{
	RS = 0;             // => RS = 0
	Lcd_Port(a);
	EN  = 1;             // => E = 1
        __delay_ms(4);
        EN  = 0;             // => E = 0
}

void Lcd_Clear(void)
{
	Lcd_Cmd(0);
	Lcd_Cmd(1);
}

void Lcd_Set_Cursor(char a, char b)
{
	char temp;
	if(a == 1)
	{
	  temp = 0x80 + b - 1;
//		z = temp>>8;
//		y = temp & 0x0F;
//		Lcd_Cmd(z);
		Lcd_Cmd(temp);
	}
	else if(a == 2)
	{
		temp = 0xC0 + b - 1;
//		z = temp>>8;
//		y = temp & 0x0F;
//		Lcd_Cmd(z);
		Lcd_Cmd(temp);
	}
}

void Lcd_Init(void)
{
  Lcd_Port(0x00);
   __delay_ms(20);
  Lcd_Cmd(0x30);
	__delay_ms(5);
  Lcd_Cmd(0x30);
	__delay_ms(11);
  Lcd_Cmd(0x30);
  /////////////////////////////////////////////////////
  Lcd_Cmd(0x38);
  Lcd_Cmd(0x0C);
  Lcd_Cmd(0x01);
  Lcd_Cmd(0x06);
  //Lcd_Cmd(0x0C);
  //Lcd_Cmd(0x00);
  //Lcd_Cmd(0x06);
}

void Lcd_Write_Char(char a)
{
 
//   char temp;
//   temp = a;
//   y = a&0xF0;
   RS = 1;             // => RS = 1
//   Lcd_Port(y>>4);             //Data transfer
//   EN = 1;
//   __delay_us(40);
//   EN = 0;
   Lcd_Port(a);
   EN = 1;
   __delay_us(40);
   EN = 0;
}

void Lcd_Write_String(char *a)
{
	int i;
	for(i=0;a[i]!='\0';i++)
	   Lcd_Write_Char(a[i]);
}



