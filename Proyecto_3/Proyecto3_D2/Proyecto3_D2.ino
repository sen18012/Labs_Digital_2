/*
 * ------ Proyecto # 3 ------ 
 * Natalia de León 18193
 * Katharine Senn 18012
 * Sección 20
 * Abril 2021
 */

//***************************************************************************************************************************************
/* Librería para el uso de la pantalla ILI9341 en modo 8 bits
 * Basado en el código de martinayotte - https://www.stm32duino.com/viewtopic.php?t=637
 * Adaptación, migración y creación de nuevas funciones: Pablo Mazariegos y José Morales
 * Con ayuda de: José Guerra
 * IE3027: Electrónica Digital 2 - 2019
 */
//***************************************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

#include "bitmaps.h"
#include "font.h"
#include "lcd_registers.h"
#include "pitches.h"

#define LCD_RST PD_0
#define LCD_CS PD_1
#define LCD_RS PD_2
#define LCD_WR PD_3
#define LCD_RD PE_1

#define PUSH_LEFT_J1 PA_7
#define PUSH_RIGTH_J1 PE_3
#define PUSH_UP_J1 PA_6
#define PUSH_DOWN_J1 PE_2

#define PUSH_LEFT_J2 PD_7
#define PUSH_RIGTH_J2  PC_7
#define PUSH_UP_J2  PD_6
#define PUSH_DOWN_J2 PC_6
 
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7}; 

//***************************************************************************************************************************************
// Variables
//***************************************************************************************************************************************
//--- BANDERAS ---//
volatile byte flag_jugar = LOW; //BANDERA QUE INDICA QUE YA TERMINARON DE ESCOGER
volatile byte flag_boton_jugar = LOW;
volatile byte ganar_N1 = LOW;
volatile byte Listo_per_J1 = LOW;
volatile byte Listo_per_J2 = LOW;

//--- CONTADORES ---//
int cont_PUSH1 = 0;
int cont_personajes_J1 = 0;
int cont_personajes_J2 = 0;

//--- VARIABLES DE INFO ---//
int nivel = 0;
int num_personaje = 0;

//--- PUSH TIVA ---//
const byte interruptPin1 = PUSH1; 
const byte interruptPin2 = PUSH2;
const byte led_VERDE = GREEN_LED; 
 
//***************************************************************************************************************************************
// Functions Prototypes
//***************************************************************************************************************************************
//--- FUNCIONES LCD ---//
void LCD_Init(void);
void LCD_CMD(uint8_t cmd);
void LCD_DATA(uint8_t data);
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);
void LCD_Clear(unsigned int c);
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c);
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
void LCD_Print(String text, int x, int y, int fontSize, int color, int background);

void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);

//---- FUNCIONES PROPIAS ----//
void Static_Pantalla_Inicio(void); //lo que aparece en el menu y es fijo
void Mov_Pantalla_inicio(void); // lo que se mueve en la pagina de inicio
void Nivel_pantalla(int Num_Nivel); //para mostrar la pantalla del nivel que toca
void Linea_divisora(int nivel_line); //hacer la linea que divide los jugadores
void Posicion_inicial_munecos(int nivel_pos_i); //funcion para poner o munecos 
void Mapa_nivel(int nivel_mapa);
void Listo_personajes(void);
unsigned char Personaje_usar(int num_per);

//--- GRAFICOS ---//
extern uint8_t fondo[];
extern uint8_t fondo2[];
extern uint8_t Muneco_50[];
extern uint8_t Koala_50[];
extern uint8_t Calavera_50[];
extern uint8_t Mono_50[];
extern uint8_t dec_nivel_32[];

//***************************************************************************************************************************************
// Inicialización
//***************************************************************************************************************************************
void setup() {
  pinMode(interruptPin1, INPUT_PULLUP);
  pinMode(interruptPin2, INPUT_PULLUP);
  pinMode(PUSH_LEFT_J1, INPUT_PULLUP);
  pinMode(PUSH_LEFT_J2, INPUT_PULLUP);
  pinMode(PUSH_RIGTH_J1, INPUT_PULLUP);
  pinMode(PUSH_RIGTH_J2, INPUT_PULLUP);
  pinMode(PUSH_UP_J1, INPUT_PULLUP);
  pinMode(PUSH_UP_J2, INPUT_PULLUP);
  pinMode(PUSH_DOWN_J1, INPUT_PULLUP);
  pinMode(PUSH_DOWN_J2, INPUT_PULLUP);
  pinMode(led_VERDE, OUTPUT); 
  
  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(9600);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");
  
  //--- INTERRUPCIONES ---//
  attachInterrupt(digitalPinToInterrupt(interruptPin1), IRS_PUSH1, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), IRS_PUSH2, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_LEFT_J1), LEFT_J1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_LEFT_J2), LEFT_J2, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_RIGTH_J1), RIGTH_J1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_RIGTH_J2), RIGTH_J2, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_UP_J1), UP_J1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_UP_J2), UP_J2, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_DOWN_J1), DOWN_J1, FALLING);
  attachInterrupt(digitalPinToInterrupt(PUSH_DOWN_J2), DOWN_J2, FALLING);
  
  LCD_Init();
  LCD_Clear(0x0000);

  //--- Fondo del juego ---//
  //LCD_Bitmap(0, 0, 320, 240, fondo);
  //delay(500);

  //--- Pantalla de inicio ---//
  Static_Pantalla_Inicio();
}

//***************************************************************************************************************************************
// Loop Infinito
//***************************************************************************************************************************************
void loop() {
while(flag_boton_jugar != HIGH){
  Mov_Pantalla_inicio();
  Listo_personajes();
  }  
  
if (flag_boton_jugar == HIGH){
  String text1 = "JUGAR";
  LCD_Print(text1, 111, 200, 2, 0x000, 0x07FF);
  flag_boton_jugar = LOW;
  delay(500);
  }

  //Personaje_usar(num_personaje);
  switch (nivel){
    case 1: //NIVEL 1
      Nivel_pantalla(1);
      delay(500);
      
      while (ganar_N1 != HIGH){
        FillRect(0, 0, 319, 239, 0x0000);
        Linea_divisora(1); //linea divisiora de jugadores
        Posicion_inicial_munecos(1); //poner a los munecos en su posicion inicial
        Mapa_nivel(1);
        
        } 
    }    
}

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//
//                                                        FUNCIONES PROPIAS                                                                  //
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%//

//-----------------------------------------------              INTERRUPCIONES              -----------------------------------------------//
 
//***************************************************************************************************************************************
// Función interrupcion para comenzar a jugar
//***************************************************************************************************************************************
void IRS_PUSH1() { //INTERRUPCION PUSH1: CAMBIAR PERSONAJES Y COMENZAR A JUGAR
  Listo_per_J1 = HIGH;
  cont_PUSH1++; //LUEGO DE PRESIONAR UNA VEZ PARA ESCOGER EL PERSONAJE SI SE VUELVE A PRESIONAR YA ES PARA JUGAR
  
  if(Listo_per_J1 == HIGH & Listo_per_J2 == HIGH & cont_PUSH1 !=1){
    flag_boton_jugar = HIGH;
    nivel = 1;
    cont_PUSH1 = 0;
    }
}

void IRS_PUSH2() { //INTERRUPCION PUSH2
   Listo_per_J2 = HIGH;
}

void LEFT_J1() { //AQUI TAMBIEN CAMBIA PARA ESCOGER PERSONAJE J1
  if (Listo_per_J1 != HIGH){
      cont_personajes_J1++;
      Rect(28,110,25,25,0x07FF);
    if (cont_personajes_J1 == 4){
      cont_personajes_J1 = 0;
    }
  }
}
  //digitalWrite(led_VERDE, HIGH);


void LEFT_J2() { 
  //digitalWrite(led_VERDE, HIGH);
}

void RIGTH_J1() { 
  //digitalWrite(led_VERDE, HIGH);
}

void RIGTH_J2() { //AQUI TAMBIEN CAMBIA PARA ESCOGER PERSONAJE J2
  if (Listo_per_J2 != HIGH){
      cont_personajes_J2++;
      Rect(258,110,25,25,0x07FF);
    if (cont_personajes_J2 == 4){
      cont_personajes_J2 = 0;
    }
  }
}

void UP_J1() { 
  //digitalWrite(led_VERDE, HIGH);
}

void UP_J2() { 
  //digitalWrite(led_VERDE, HIGH);
}

void DOWN_J1() { 
  //digitalWrite(led_VERDE, HIGH);
}

void DOWN_J2() { 
  //digitalWrite(led_VERDE, HIGH);
}
//-----------------------------------------------              FUNCIONES DEL PROGRAMA              -----------------------------------------------//


//***************************************************************************************************************************************
// Función para el munú de inicio
//***************************************************************************************************************************************
void Static_Pantalla_Inicio(void){
  
    //FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
    FillRect(0, 0, 319, 239, 0x0000);
    String text_escoge_J = "ELIGE PERSONAJE";
    String text_boton_jugar = "JUGAR";
    String text_J1 = "J1";
    String text_J2 = "J2";
    // LCD_Print(String text, int x, int y, int fontSize, int color, int background);
    LCD_Print(text_escoge_J, 45, 20, 2,0x0000, 0xFF40);
    LCD_Print(text_J1, 75, 60, 2, 0xFF40, 0x0000);
    LCD_Print(text_J2, 205, 60, 2, 0xFF40, 0x0000);
    LCD_Print(text_boton_jugar, 111, 200, 2, 0x000, 0xFF40); //TEXTO DE BOTON JUGAR
    //void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
    Rect(63,95,56,56,0xFF40);
    Rect(193,95,56,56,0xFF40);
    for(int y = 0; y <239; y++){
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado); // en medio +-4 a partir del 156
          LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
    
}
  
void Mov_Pantalla_inicio(void){
    //LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]);
    LCD_Sprite(29, 111,24,24,next_amarillo_24,1,0,1,0);
    LCD_Bitmap(259, 111, 24, 24, next_amarillo_24);
    switch(cont_personajes_J1){
      case 0:
        LCD_Bitmap(66, 98, 50, 50, Muneco_50);
        Rect(28,110,25,25,0x0000);
        num_personaje = 0;
        break;
      case 1:
        LCD_Bitmap(66, 98, 50, 50, Calavera_50);
        Rect(28,110,25,25,0x0000);
        num_personaje = 1;
        break;
      case 2:
        LCD_Bitmap(66, 98, 50, 50, Koala_50);
        Rect(28,110,25,25,0x0000);
        num_personaje = 2;
        break;
      case 3:
        LCD_Bitmap(66, 98, 50, 50, Mono_50);
        Rect(28,110,25,25,0x0000);
        num_personaje = 3;
        break;
      }

      
    switch(cont_personajes_J2){
      case 0:
        LCD_Bitmap(196, 98, 50, 50, Muneco_50);
        Rect(258,110,25,25,0x0000);
        num_personaje = 0;
        break;
      case 1:
        LCD_Bitmap(196, 98, 50, 50, Calavera_50);
        Rect(258,110,25,25,0x0000);
        num_personaje = 1;
        break;
      case 2:
        LCD_Bitmap(196, 98, 50, 50, Koala_50);
        Rect(258,110,25,25,0x0000);
        num_personaje = 2;
        break;
      case 3:
        LCD_Bitmap(196, 98, 50, 50, Mono_50);
        Rect(258,110,25,25,0x0000);
        num_personaje = 3;
        break;
      }
}

//***************************************************************************************************************************************
// Función escribir que estan listos cuando escogen su personaje
//***************************************************************************************************************************************
void Listo_personajes(void){
  if (Listo_per_J1 ==HIGH){
    String text_listo_J1 = "LISTO";
    LCD_Print(text_listo_J1, 72, 160, 1, 0x000, 0x07FF);
    }

  if (Listo_per_J2 ==HIGH){
    String text_listo_J2 = "LISTO";
    LCD_Print(text_listo_J2, 202, 160, 1, 0x000, 0x07FF);
    }
  }
  
//***************************************************************************************************************************************
// Función para pantalla con titulo del nivel que toca
//***************************************************************************************************************************************
void Nivel_pantalla(int Num_Nivel){\
    FillRect(0, 0, 319, 239, 0x0000);
    String Str_nivel = String(Num_Nivel); //convertir valor INT de cantidad estrellas a STRING
    String text_nivel = "NIVEL " + Str_nivel;
    LCD_Print(text_nivel, 100, 110, 2, 0x07FF, 0x0000);
    switch (Num_Nivel){
      case 1: 
         LCD_Bitmap(138, 160, 32, 32, dec_nivel_32);
         break;

      case 2:
         LCD_Bitmap(113, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(155, 160, 32, 32, dec_nivel_32);
         break;

      case 3:
         LCD_Bitmap(92, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(134, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(176, 160, 32, 32, dec_nivel_32);
         break;
        
      }
    delay(300);   
}

//***************************************************************************************************************************************
// Función para seleccionar el bitmap del personaje a usar
//***************************************************************************************************************************************
//unsigned char Personaje_usar(int num_per){
//    switch (num_per){
//      case 0:
//        bitmap_usar = Muneco_24;
//        break;
//        
//      case 1:
//        bitmap_usar = Calavera_24;
//        break;
//        
//      case 3:
//        bitmap_usar = Koala_24;
//        break;
//
//     case 3:
//        bitmap_usar = Mono_24;
//        break;
//      }
//   return bitmap_usar
//  }
//***************************************************************************************************************************************
// Función hacer linea divisora en cada mapa
//***************************************************************************************************************************************
void Linea_divisora(int nivel_line){      
  switch (nivel_line){
    case 1:
        for(int y = 0; y <239; y++){
          LCD_Bitmap(152, y, 8, 8, Bloque_8_celeste); // en medio +-4 a partir del 156
          y += 7;
          }
         break;
    case 2:
        for(int y = 0; y <239; y++){
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado); // en medio +-4 a partir del 156
          y += 7;
          }
        break; 
    case 3:
        for(int y = 0; y <239; y++){
          LCD_Bitmap(152, y, 8, 8, Bloque_8_celeste); // en medio +-4 a partir del 156
          y += 7;
          }
        break;
  }
}

//***************************************************************************************************************************************
// Función hacer colocar los muñecos en su posicion incial en cada mapa
//***************************************************************************************************************************************
void Posicion_inicial_munecos(int nivel_pos_i){ //SEGUN EL NUMERO DE MAPA SE COLOCAN LOS MUNECOS
  
  switch (nivel_pos_i){
    case 1:
      LCD_Bitmap(288, 216, 24, 24, Muneco_24); 
      LCD_Bitmap(120, 216, 24, 24, Koala_24); 
      break;
    case 2:
      LCD_Bitmap(296, 224, 24, 24, Muneco_24); 
      LCD_Bitmap(296, 224, 24, 24, Koala_24); 
      break; 
    case 3:
      LCD_Bitmap(296, 224, 24, 24, Muneco_24);
      LCD_Bitmap(296, 224, 24, 24, Koala_24);  
      break;
  }
}

//***************************************************************************************************************************************
// Función para de los mapas de los niveles
//***************************************************************************************************************************************
void Mapa_nivel(int nivel_mapa){
  switch (nivel_mapa){
    case 1:
      for(int y = 144; y <240; y++){
          LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);//320 - 8
          LCD_Bitmap(144, y, 8, 8, Bloque_8_morado); //152-8
          y += 7;
          }

          for(int y = 208; y <240; y++){
          LCD_Bitmap(280, y, 8, 8, Bloque_8_morado); //320 - 32
          LCD_Bitmap(112, y, 8, 8, Bloque_8_morado); //152-32
          y += 7;
          }
          
        for(int x = 272; x <312; x++){ //312-40
          LCD_Bitmap(x, 144, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x-168, 144, 8, 8, Bloque_8_morado);
          x += 7;
          }
        delay(1000);
      break;
    case 2:
      LCD_Bitmap(296, 224, 24, 24, Muneco_24); 
      LCD_Bitmap(296, 224, 24, 24, Muneco_24); 
      break; 
    case 3:
      LCD_Bitmap(296, 224, 24, 24, Muneco_24);
      LCD_Bitmap(296, 224, 24, 24, Muneco_24);  
      break;
  }
  }

  
//---------------------------------------------------FUNCIONES LIBRERÍA-----------------------------------------------------------------------//
//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  pinMode(LCD_WR, OUTPUT);
  pinMode(LCD_RD, OUTPUT);
  for (uint8_t i = 0; i < 8; i++){
    pinMode(DPINS[i], OUTPUT);
  }
  //****************************************
  // Secuencia de Inicialización
  //****************************************
  digitalWrite(LCD_CS, HIGH);
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, HIGH);
  digitalWrite(LCD_RD, HIGH);
  digitalWrite(LCD_RST, HIGH);
  delay(5);
  digitalWrite(LCD_RST, LOW);
  delay(20);
  digitalWrite(LCD_RST, HIGH);
  delay(150);
  digitalWrite(LCD_CS, LOW);
  //****************************************
  LCD_CMD(0xE9);  // SETPANELRELATED
  LCD_DATA(0x20);
  //****************************************
  LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
  delay(100);
  //****************************************
  LCD_CMD(0xD1);    // (SETVCOM)
  LCD_DATA(0x00);
  LCD_DATA(0x71);
  LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0xD0);   // (SETPOWER) 
  LCD_DATA(0x07);
  LCD_DATA(0x01);
  LCD_DATA(0x08);
  //****************************************
  LCD_CMD(0x36);  // (MEMORYACCESS)
  LCD_DATA(0x40|0x80|0x20|0x08); // LCD_DATA(0x19);
  //****************************************
  LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
  LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
  //****************************************
  LCD_CMD(0xC1);    // (POWERCONTROL2)
  LCD_DATA(0x10);
  LCD_DATA(0x10);
  LCD_DATA(0x02);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
  LCD_DATA(0x00);
  LCD_DATA(0x35);
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x02);
  //****************************************
  LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
  LCD_DATA(0x04); // 72Hz
  //****************************************
  LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
  LCD_DATA(0x01);
  LCD_DATA(0x44);
  //****************************************
  LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
  LCD_DATA(0x04);
  LCD_DATA(0x67);
  LCD_DATA(0x35);
  LCD_DATA(0x04);
  LCD_DATA(0x08);
  LCD_DATA(0x06);
  LCD_DATA(0x24);
  LCD_DATA(0x01);
  LCD_DATA(0x37);
  LCD_DATA(0x40);
  LCD_DATA(0x03);
  LCD_DATA(0x10);
  LCD_DATA(0x08);
  LCD_DATA(0x80);
  LCD_DATA(0x00);
  //****************************************
  LCD_CMD(0x2A); // Set_column_address 320px (CASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0x3F);
  //****************************************
  LCD_CMD(0x2B); // Set_page_address 480px (PASET)
  LCD_DATA(0x00);
  LCD_DATA(0x00);
  LCD_DATA(0x01);
  LCD_DATA(0xE0);
//  LCD_DATA(0x8F);
  LCD_CMD(0x29); //display on 
  LCD_CMD(0x2C); //display on

  LCD_CMD(ILI9341_INVOFF); //Invert Off
  delay(120);
  LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
  delay(120);
  LCD_CMD(ILI9341_DISPON);    //Display on
  digitalWrite(LCD_CS, HIGH);
  Serial.println("Listo init");
}

//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
  digitalWrite(LCD_RS, LOW);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = cmd;
  digitalWrite(LCD_WR, HIGH);
}

//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_WR, LOW);
  GPIO_PORTB_DATA_R = data;
  digitalWrite(LCD_WR, HIGH);
}

//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2) {
  LCD_CMD(0x2a); // Set_column_address 4 parameters
  LCD_DATA(x1 >> 8);
  LCD_DATA(x1);   
  LCD_DATA(x2 >> 8);
  LCD_DATA(x2);   
  LCD_CMD(0x2b); // Set_page_address 4 parameters
  LCD_DATA(y1 >> 8);
  LCD_DATA(y1);   
  LCD_DATA(y2 >> 8);
  LCD_DATA(y2);   
  LCD_CMD(0x2c); // Write_memory_start
}

//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c){  
  unsigned int x, y;
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);   
  SetWindows(0, 0, 319, 239); // 479, 319);
  for (x = 0; x < 320; x++)
    for (y = 0; y < 240; y++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
    }
  digitalWrite(LCD_CS, HIGH);
} 

//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i, j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + x;
  SetWindows(x, y, l, y);
  j = l;// * 2;
  for (i = 0; i < l; i++) {
      LCD_DATA(c >> 8); 
      LCD_DATA(c); 
  }
  digitalWrite(LCD_CS, HIGH);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//*************************************************************************************************************************************** 
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {  
  unsigned int i,j;
  LCD_CMD(0x02c); //write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW);
  l = l + y;
  SetWindows(x, y, x, l);
  j = l; //* 2;
  for (i = 1; i <= j; i++) {
    LCD_DATA(c >> 8); 
    LCD_DATA(c);
  }
  digitalWrite(LCD_CS, HIGH);  
}

//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  H_line(x  , y  , w, c);
  H_line(x  , y+h, w, c);
  V_line(x  , y  , h, c);
  V_line(x+w, y  , h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  unsigned int i;
  for (i = 0; i < h; i++) {
    H_line(x  , y  , w, c);
    H_line(x  , y+i, w, c);
  }
}
*/

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+w;
  y2 = y+h;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = w*h*2-1;
  unsigned int i, j;
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      LCD_DATA(c >> 8);
      LCD_DATA(c);
      
      //LCD_DATA(bitmap[k]);    
      k = k - 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background) 
//***************************************************************************************************************************************
void LCD_Print(String text, int x, int y, int fontSize, int color, int background) {
  int fontXSize ;
  int fontYSize ;
  
  if(fontSize == 1){
    fontXSize = fontXSizeSmal ;
    fontYSize = fontYSizeSmal ;
  }
  if(fontSize == 2){
    fontXSize = fontXSizeBig ;
    fontYSize = fontYSizeBig ;
  }
  
  char charInput ;
  int cLength = text.length();
  Serial.println(cLength,DEC);
  int charDec ;
  int c ;
  int charHex ;
  char char_array[cLength+1];
  text.toCharArray(char_array, cLength+1) ;
  for (int i = 0; i < cLength ; i++) {
    charInput = char_array[i];
    Serial.println(char_array[i]);
    charDec = int(charInput);
    digitalWrite(LCD_CS, LOW);
    SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1, y + fontYSize );
    long charHex1 ;
    for ( int n = 0 ; n < fontYSize ; n++ ) {
      if (fontSize == 1){
        charHex1 = pgm_read_word_near(smallFont + ((charDec - 32) * fontYSize) + n);
      }
      if (fontSize == 2){
        charHex1 = pgm_read_word_near(bigFont + ((charDec - 32) * fontYSize) + n);
      }
      for (int t = 1; t < fontXSize + 1 ; t++) {
        if (( charHex1 & (1 << (fontXSize - t))) > 0 ) {
          c = color ;
        } else {
          c = background ;
        }
        LCD_DATA(c >> 8);
        LCD_DATA(c);
      }
    }
    digitalWrite(LCD_CS, HIGH);
  }
}

//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width, unsigned int height, unsigned char bitmap[]){  
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 
  
  unsigned int x2, y2;
  x2 = x+width;
  y2 = y+height;
  SetWindows(x, y, x2-1, y2-1);
  unsigned int k = 0;
  unsigned int i, j;

  for (int i = 0; i < width; i++) {
    for (int j = 0; j < height; j++) {
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      //LCD_DATA(bitmap[k]);    
      k = k + 2;
     } 
  }
  digitalWrite(LCD_CS, HIGH);
}

//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset){
  LCD_CMD(0x02c); // write_memory_start
  digitalWrite(LCD_RS, HIGH);
  digitalWrite(LCD_CS, LOW); 

  unsigned int x2, y2;
  x2 =   x+width;
  y2=    y+height;
  SetWindows(x, y, x2-1, y2-1);
  int k = 0;
  int ancho = ((width*columns));
  if(flip){
  for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width -1 - offset)*2;
      k = k+width*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k - 2;
     } 
  }
  }else{
     for (int j = 0; j < height; j++){
      k = (j*(ancho) + index*width + 1 + offset)*2;
     for (int i = 0; i < width; i++){
      LCD_DATA(bitmap[k]);
      LCD_DATA(bitmap[k+1]);
      k = k + 2;
     } 
  }
    
    
    }
  digitalWrite(LCD_CS, HIGH);
}
