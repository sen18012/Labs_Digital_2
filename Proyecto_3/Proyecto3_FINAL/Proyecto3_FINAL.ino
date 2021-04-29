#include <stdint.h>
#include <stdbool.h>
#include <TM4C123GH6PM.h>
#include <SPI.h>
#include <SD.h>
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

#define BUZZER PC_5
 
int DPINS[] = {PB_0, PB_1, PB_2, PB_3, PB_4, PB_5, PB_6, PB_7}; 

File myFile;


//--- BANDERAS ---//
volatile byte flag_jugar = LOW; //BANDERA QUE INDICA QUE YA TERMINARON DE ESCOGER
volatile byte flag_boton_jugar = LOW;
volatile byte ganar_N1 = LOW;
volatile byte ganar_N2 = LOW;
volatile byte ganar_N3 = LOW;
volatile byte Listo_per_J1 = LOW;
volatile byte Listo_per_J2 = LOW;
volatile byte JUEGO_EN_PROGRESO = LOW;

//--- BANDERAS PUSH MOVIMIENTO ---//
volatile byte b_LEFT_J1 = LOW;
volatile byte b_LEFT_J2 = LOW;
volatile byte b_RIGHT_J1 = LOW;
volatile byte b_RIGHT_J2 = LOW;
volatile byte b_UP_J1 = LOW;
volatile byte b_UP_J2 = LOW;
volatile byte b_DOWN_J1 = LOW;
volatile byte b_DOWN_J2 = LOW;

//--- BANDERAS ESTRELLAS ---//
volatile byte STAR_1_J1 = LOW;
volatile byte STAR_1_J2 = LOW;
volatile byte STAR_2_J1 = LOW;
volatile byte STAR_2_J2 = LOW;
volatile byte STAR_3_J1 = LOW;
volatile byte STAR_3_J2 = LOW;
volatile byte STAR_4_J1 = LOW;
volatile byte STAR_4_J2 = LOW;
volatile byte STAR_5_J1 = LOW;
volatile byte STAR_5_J2 = LOW;

//--- CONTADORES ---//
int cont_PUSH1 = 0;
int cont_personajes_J1 = 0;
int cont_personajes_J2 = 0;

//--- VARIABLES DE INFO ---//
int nivel = 0;
int num_personaje_J1 = 0;
int num_personaje_J2 = 0;
int posicion_J1 = 0;
int posicion_J2 = 0;
int STARS_J1 = 0;
int STARS_J2 = 0;
int STARS_N1_J1 = 0;
int STARS_N1_J2 = 0;
int STARS_N2_J1 = 0;
int STARS_N2_J2 = 0;
int STARS_N3_J1 = 0;
int STARS_N3_J2 = 0;
int Juegos_ganados_J1 = 0;
int Juegos_ganados_J2 = 0;

//--- PUSH TIVA ---//
const byte interruptPin1 = PUSH1; 
const byte interruptPin2 = PUSH2;
const byte led_VERDE = GREEN_LED; 

//--- ARREGLOS EN SD ---//
unsigned char personajes_inicio_50[10000]={0};

unsigned char J1_Abajo_Derecha[2304]={0};
unsigned char J2_Abajo_Derecha[2304]={0};


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

//---- FUNCIONES PANTALLA INICIO ----//
void Static_Pantalla_Inicio(void); //lo que aparece en el menu y es fijo
void Mov_Pantalla_inicio(void); // lo que se mueve en la pagina de inicio
void Listo_personajes(void); //Mostrar texto de listo cuando se elige personaje

//---- FUNCIONES PANTALLAS ----//
void Nivel_pantalla(int Num_Nivel); //para mostrar la pantalla del nivel que toca
void Marcador_pantalla(void);
void Pantalla_instrucciones(void);

//---- FUNCIONES APARECER EN LOS MAPAS ----//
void Posicion_inicial_munecos(int nivel_pos_i); //funcion para poner o munecos 
void Posicion_meta(int nivel_pos_i); //funcion para poner la meta en cada mapa
void Posicion_estrellas(int nivel_pos_i); //funcion par aponer las estrellas en cada nivel
void Mostrar_stars_en_mapa(int nivel_pos_i);

//---- FUNCION DE CADA MAPA (BLOQUES, ESTRELLAS, ENEMIGOS) ----//
void Mapa_nivel(int nivel_mapa);

//---- FUNCIONES PARA ESCOGER PERSONAJE EN TODO EL JEUGO ----//
void Personajes_usar(int num_per_J1, int num_per_J2);

//--- FUNCIONES PARA SD ---//
void open_SD_bitmap(unsigned char Bitmap_SD[], unsigned long Size_bitmap, char* filename);
int ACII_to_HEX(char *puntero);

//--- FUNCIONES AL APACHAR CADA BOTON EN CADA JUGADOR ---//
void switch_posicion_LEFT_J1(int num_nivel);
void switch_posicion_RIGHT_J1(int num_nivel);
void switch_posicion_UP_J1(int num_nivel);
void switch_posicion_DOWN_J1(int num_nivel);

void switch_posicion_LEFT_J2(int num_nivel);
void switch_posicion_RIGHT_J2(int num_nivel);
void switch_posicion_UP_J2(int num_nivel);
void switch_posicion_DOWN_J2(int num_nivel);

//--- FUNCIONES BORRAR BANDERA ESTRELLAS ---//
void bajar_banderas_stars(void);

//--- GRAFICOS ---//
extern uint8_t fondo[];
extern uint8_t dec_nivel_32[];
extern uint8_t next_amarillo_24[];
extern uint8_t Bloque_8_celeste[];
extern uint8_t Bloque_8_morado[];
extern uint8_t estrella_16[];

extern uint8_t Muneco_50[];
extern uint8_t Koala_50[];
extern uint8_t Calavera_50[];
extern uint8_t Mono_50[];

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
  pinMode(BUZZER, OUTPUT);

  SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  SPI.setModule(0);
  GPIOPadConfigSet(GPIO_PORTB_BASE, 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
  Serial.println("Inicio");

  //--- INICIALIZACION SD ---//
  pinMode(PA_3, OUTPUT);

  if (!SD.begin(PA_3)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  myFile = SD.open("/"); //abrir archivos
  //printDirectory(myFile, 0); //funcion que muestra archivos dentro de SD
  Serial.println("done!");

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
  digitalWrite(BUZZER, LOW);
  delay(300);
  digitalWrite(BUZZER, HIGH);
  LCD_Init();
  LCD_Clear(0x0000);
  LCD_Bitmap(20, 90, 279, 60, fondo);
  delay(3000);
  Pantalla_instrucciones();
  
}

void loop() {

  if (flag_boton_jugar == LOW && JUEGO_EN_PROGRESO == LOW){
    Static_Pantalla_Inicio();
    Mov_Pantalla_inicio();
    Listo_personajes();
    }
  else if (flag_boton_jugar == HIGH && JUEGO_EN_PROGRESO == LOW) {
    String text1 = "JUGAR";
    LCD_Print(text1, 111, 200, 2, 0x000, 0x07FF);
    
    Personajes_usar(num_personaje_J1, num_personaje_J2);

    JUEGO_EN_PROGRESO = HIGH;
    delay(500);
    }
 //else {}

  while (JUEGO_EN_PROGRESO == HIGH){
    //digitalWrite(BUZZER, HIGH);
    switch (nivel){
      case 1:
        digitalWrite(BUZZER, LOW);
        Nivel_pantalla(1); //Titulo del nivel al que pasan
        delay(500);
        digitalWrite(BUZZER, HIGH);
        Mapa_nivel(1); //bloques del nivel 
        bajar_banderas_stars();
        while (ganar_N1 != HIGH){
          Mostrar_stars_en_mapa(1); //muestra valor de las etrellas agarradas en el nivel
       
        //-- MOVIMIENTOS JUGADOR 1 --//  
        if (b_LEFT_J1 == HIGH){
          switch_posicion_LEFT_J1(1);
          }

        if (b_RIGHT_J1 == HIGH){
          //Rect(8,8,24,24,0xFFFF);
          switch_posicion_RIGHT_J1(1);
          }

        if (b_UP_J1 == HIGH){
          switch_posicion_UP_J1(1);
          } 
          
       if (b_DOWN_J1 == HIGH){
          switch_posicion_DOWN_J1(1);
          } 

          
        //-- MOVIMIENTOS JUGADOR 2 --//  
       if (b_LEFT_J2 == HIGH){
          switch_posicion_LEFT_J2(1);
          }

        if (b_RIGHT_J2 == HIGH){
          switch_posicion_RIGHT_J2(1);
          }

        if (b_UP_J2 == HIGH){
          switch_posicion_UP_J2(1);
          } 
          
       if (b_DOWN_J2 == HIGH){
          switch_posicion_DOWN_J2(1);
          } 
      }

      break;
      case 2:
        Marcador_pantalla(); //muestra juegos ganados y cantidad de estrellas
        delay(2500);
        digitalWrite(BUZZER, LOW);
        Nivel_pantalla(2); //Titulo del nivel al que pasan
        delay(500);
        digitalWrite(BUZZER, HIGH);
        Mapa_nivel(2); //bloques del nivel 
        bajar_banderas_stars();
        while(ganar_N2 != HIGH){
          Mostrar_stars_en_mapa(2);
          
          if (b_LEFT_J1 == HIGH){
          switch_posicion_LEFT_J1(2); //muestra valor de las etrellas agarradas en el nivel
          }

        if (b_RIGHT_J1 == HIGH){
          //Rect(8,8,24,24,0xFFFF);
          switch_posicion_RIGHT_J1(2);
          }

        if (b_UP_J1 == HIGH){
          switch_posicion_UP_J1(2);
          } 
          
       if (b_DOWN_J1 == HIGH){
          switch_posicion_DOWN_J1(2);
          } 

          
        //-- MOVIMIENTOS JUGADOR 2 --//  
       if (b_LEFT_J2 == HIGH){
          switch_posicion_LEFT_J2(2);
          }

        if (b_RIGHT_J2 == HIGH){
          switch_posicion_RIGHT_J2(2);
          }

        if (b_UP_J2 == HIGH){
          switch_posicion_UP_J2(2);
          } 
          
       if (b_DOWN_J2 == HIGH){
          switch_posicion_DOWN_J2(2);
          }
        }

      break;
      case 3:
        Marcador_pantalla(); //muestra juegos ganados y cantidad de estrellas
        delay(2500);
        digitalWrite(BUZZER, LOW);
        Nivel_pantalla(3); //Titulo del nivel al que pasan
        delay(500);
        digitalWrite(BUZZER, HIGH);
        Mapa_nivel(3); //bloques del nivel 
        bajar_banderas_stars();
        
        while(ganar_N3 != HIGH){
          Mostrar_stars_en_mapa(3); //muestra valor de las etrellas agarradas en el nivel
          
          //-- MOVIMIENTOS JUGADOR 1 --//  
        if (b_LEFT_J1 == HIGH){
          switch_posicion_LEFT_J1(3);
          }

        if (b_RIGHT_J1 == HIGH){
          //Rect(8,8,24,24,0xFFFF);
          switch_posicion_RIGHT_J1(3);
          }

        if (b_UP_J1 == HIGH){
          switch_posicion_UP_J1(3);
          } 
          
       if (b_DOWN_J1 == HIGH){
          switch_posicion_DOWN_J1(3);
          } 

          
        //-- MOVIMIENTOS JUGADOR 2 --//  
       if (b_LEFT_J2 == HIGH){
          switch_posicion_LEFT_J2(3);
          }

        if (b_RIGHT_J2 == HIGH){
          switch_posicion_RIGHT_J2(3);
          }

        if (b_UP_J2 == HIGH){
          switch_posicion_UP_J2(3);
          } 
          
       if (b_DOWN_J2 == HIGH){
          switch_posicion_DOWN_J2(3);
          }
          }
      break;

      case 4:
      while (JUEGO_EN_PROGRESO == HIGH){
         Mapa_nivel(4);
        }
       
        //delay(1000);
        
      break;
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
  if (Listo_per_J1 == LOW && JUEGO_EN_PROGRESO == LOW){
      cont_personajes_J1++;
      Rect(28,110,25,25,0x07FF);
    if (cont_personajes_J1 == 4){
      cont_personajes_J1 = 0;
    }
  }
  if (JUEGO_EN_PROGRESO == HIGH){
    b_LEFT_J1 = HIGH;
  }
}

void LEFT_J2() { 
  b_LEFT_J2 = HIGH;
  //ganar_N1 = HIGH;
  //nivel = 2; 
}

void RIGTH_J1() { 
  b_RIGHT_J1 = HIGH;
}

void RIGTH_J2() { //AQUI TAMBIEN CAMBIA PARA ESCOGER PERSONAJE J2
  if (Listo_per_J2 != HIGH && JUEGO_EN_PROGRESO == LOW){
      cont_personajes_J2++;
      Rect(258,110,25,25,0x07FF);
    if (cont_personajes_J2 == 4){
      cont_personajes_J2 = 0;
    }
  }
    else if (Listo_per_J2 == HIGH && JUEGO_EN_PROGRESO == HIGH){
    b_RIGHT_J2 = HIGH;
  }
}

void UP_J1() { 
  b_UP_J1 = HIGH;
}

void UP_J2() { 
  b_UP_J2 = HIGH;
}

void DOWN_J1() { 
  b_DOWN_J1 = HIGH;
}

void DOWN_J2() { 
  b_DOWN_J2 = HIGH;
}

//***************************************************************************************************************************************
// Función para las instrucciones
//***************************************************************************************************************************************
void Pantalla_instrucciones(void){
  FillRect(0, 0, 320, 240, 0x0000);
    for(int y = 0; y <240; y++){  //lineas decoracion 
      LCD_Bitmap(0, y, 8, 8, Bloque_8_morado);
      LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
      y += 7;
     }

     for(int x = 0; x <320; x++){  //lineas decoracion 
      LCD_Bitmap(x, 0, 8, 8, Bloque_8_morado);
      LCD_Bitmap(x, 232, 8, 8, Bloque_8_morado);
      x += 7;
     }

    String text_instrucciones = "INSTRUCCIONES";
    LCD_Print(text_instrucciones, 60, 30, 2, 0x0000, 0xFF40);
    
    String text_1 = "Se el primero en";
    String text_2 = "  agarrar las 5";
    String text_3 = "   estrellas y ";
    String text_4 = "llegar a la meta ";
    String text_5 = "Evita las paredes ";
    String text_6 = "    celeste";
    LCD_Print(text_1, 30, 50+15, 2, 0xFF40, 0x0000);
    LCD_Print(text_2, 30, 70+15, 2, 0xFF40, 0x0000);
    LCD_Print(text_3, 30, 90+15, 2, 0xFF40, 0x0000);
    LCD_Print(text_4, 30, 110+15, 2, 0xFF40, 0x0000);
    LCD_Print(text_5, 25, 170, 2, 0xFF40, 0x0000);
    LCD_Print(text_6, 30, 195, 2, 0xFF40, 0x0000);
    delay(5000);
    FillRect(0, 0, 320, 240, 0x0000);
  }

//***************************************************************************************************************************************
// Función para el menú de inicio
//***************************************************************************************************************************************
void Static_Pantalla_Inicio(void){
    Juegos_ganados_J1 = 0;
    Juegos_ganados_J2 = 0;
    //FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
    //FillRect(0, 0, 319, 239, 0x0000);         //FONDO
    String text_escoge_J = "ELIGE PERSONAJE"; //TEXTOS DE LA PANTALLA
    String text_boton_jugar = "JUGAR";
    String text_J1 = "J1";
    String text_J2 = "J2";
    // LCD_Print(String text, int x, int y, int fontSize, int color, int background);
    LCD_Print(text_escoge_J, 45, 20, 2,0x0000, 0xFF40); //MOSTRAR LOS TEXTOS
    LCD_Print(text_J1, 75, 60, 2, 0xFF40, 0x0000);
    LCD_Print(text_J2, 205, 60, 2, 0xFF40, 0x0000);
    LCD_Print(text_boton_jugar, 111, 200, 2, 0x000, 0xFF40); //TEXTO DE BOTON JUGAR
    //void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c);
    Rect(63,95,56,56,0xFF40); // MARCOS DE LOS PERSONAJES
    Rect(193,95,56,56,0xFF40);
    for(int y = 0; y <239; y++){ //DECORACION DE LOS LADOS
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado); // en medio +-4 a partir del 156
          LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
          y += 7;
          }   
}

void Mov_Pantalla_inicio(void){ //Todo lo que va cambiando de la pantalla de inicio
    LCD_Sprite(29, 111,24,24,next_amarillo_24,1,0,1,0); //FLECHAS PARA CAMBIAR DE PERSONAJE
    LCD_Bitmap(259, 111, 24, 24, next_amarillo_24);
    switch(cont_personajes_J1){

      case 0:                                                       //EN BASE A LA CANTIDAD DE VECES QUE PRESIONA EL BOTON SE MUESTRA EL PERSONAJE
        //open_SD_bitmap(personajes_inicio_50, 10001, "Mun_50.txt");//POR DEFAULT PERONAJE QUE SE OBTIENE DE LA SD
        LCD_Bitmap(66, 98, 50, 50, Muneco_50);                      //SE MUESTRA EL PERSONAJE
        Rect(28,110,25,25,0x0000);
        num_personaje_J1 = 0;                                       //VARIABLE QUE INDICA CUAL PERSONAJE SE ESCOGIO
        break;
      case 1:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Cal_50.txt");
        LCD_Bitmap(66, 98, 50, 50, Calavera_50);
        Rect(28,110,25,25,0x0000);
        num_personaje_J1 = 1;
        break;
      case 2:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Koala_50.txt");
        LCD_Bitmap(66, 98, 50, 50, Koala_50);
        Rect(28,110,25,25,0x0000);
        num_personaje_J1 = 2;
        break;
      case 3:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Mono_50.txt");
        LCD_Bitmap(66, 98, 50, 50, Mono_50);
        Rect(28,110,25,25,0x0000);
        num_personaje_J1 = 3;
        break;
      }

    // PERSONAJE 2 
    switch(cont_personajes_J2){
      case 0:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Mun_50.txt");
        LCD_Bitmap(196, 98, 50, 50, Muneco_50);
        Rect(258,110,25,25,0x0000);
        num_personaje_J2 = 0;
        break;
      case 1:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Cal_50.txt");
        LCD_Bitmap(196, 98, 50, 50, Calavera_50);
        Rect(258,110,25,25,0x0000);
        num_personaje_J2 = 1;
        break;
      case 2:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Koala_50.txt");
        LCD_Bitmap(196, 98, 50, 50, Koala_50);
        Rect(258,110,25,25,0x0000);
        num_personaje_J2 = 2;
        break;
      case 3:
        //open_SD_bitmap(personajes_inicio_50, 10001, "Mono_50.txt");
        LCD_Bitmap(196, 98, 50, 50, Mono_50);
        Rect(258,110,25,25,0x0000);
        num_personaje_J2 = 3;
        break;
      }
}

//***************************************************************************************************************************************
// Función para seleccionar el bitmap del personaje a usar 
//***************************************************************************************************************************************
void Personajes_usar(int num_per_J1, int num_per_J2){ //EN BASE A LO QUE ESCOGIERON EN EL INICIO SE OBTIENE Y GUARDA DE SD EL PERSONAJE A USAR EN EL MAPA
    //J1//
    switch (num_per_J1){
      case 0:
        open_SD_bitmap(J1_Abajo_Derecha, 2305, "Mun_24.txt");
        break;
      case 1:
        open_SD_bitmap(J1_Abajo_Derecha, 2305, "Cal_24.txt");
        break;
        
      case 2:
        open_SD_bitmap(J1_Abajo_Derecha, 2305, "Koala_24.txt");
        break;

     case 3:
        open_SD_bitmap(J1_Abajo_Derecha, 2305, "Mono_24.txt");
        break;
      }

  //J2//
   switch (num_per_J2){
      case 0:
        open_SD_bitmap(J2_Abajo_Derecha, 2305, "Mun_24.txt");
        break;
      case 1:
        open_SD_bitmap(J2_Abajo_Derecha, 2305, "Cal_24.txt");
        break;
        
      case 2:
        open_SD_bitmap(J2_Abajo_Derecha, 2305, "Koala_24.txt");
        break;

     case 3:
        open_SD_bitmap(J2_Abajo_Derecha, 2305, "Mono_24.txt");
        break;
      }
  }
  
//***************************************************************************************************************************************
// Función escribir que estan listos cuando escogen su personaje
//***************************************************************************************************************************************
void Listo_personajes(void){ //MUESTRAN LA PALABRA LISTO DEBAJO DEL PERSONAJE PARA INDICAR QUE YA PUEDEN JUGAR
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
void Nivel_pantalla(int Num_Nivel){ //MOSTRAR EL NIVEL
    FillRect(0, 0, 320, 240, 0x0000);
    String Str_nivel = String(Num_Nivel); //convertir valor INT de cantidad estrellas a STRING
    String text_nivel = "NIVEL " + Str_nivel;
    LCD_Print(text_nivel, 100, 110, 2, 0x07FF, 0x0000);
    
    switch (Num_Nivel){ //DECORACION SEGUN EL NIVEL
      case 1: 
         LCD_Bitmap(144, 160, 32, 32, dec_nivel_32);
      break;

      case 2:
         LCD_Bitmap(118, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(170, 160, 32, 32, dec_nivel_32);
      break;

      case 3:
         LCD_Bitmap(102, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(144, 160, 32, 32, dec_nivel_32);
         LCD_Bitmap(186, 160, 32, 32, dec_nivel_32);
      break;
    }    
    delay(300);   

}


//***************************************************************************************************************************************
// Función para pantalla con marcador luego de cada nivel
//***************************************************************************************************************************************
void Marcador_pantalla(void){ //MOSTRAR EL NIVEL

    FillRect(0, 0, 320, 240, 0x0000);
    for(int y = 0; y <240; y++){  //lineas decoracion 
      LCD_Bitmap(0, y, 8, 8, Bloque_8_morado);
      LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
      y += 7;
     }

     for(int x = 0; x <320; x++){  //lineas decoracion 
      LCD_Bitmap(x, 120, 8, 8, Bloque_8_morado);
      LCD_Bitmap(x, 0, 8, 8, Bloque_8_morado);
      LCD_Bitmap(x, 232, 8, 8, Bloque_8_morado);
      x += 7;
     }

    String text_strellas = "ESTRELLAS";
    LCD_Print(text_strellas, 85, 20, 2, 0x0000, 0xFF40);
    String text_juegosganados = "JUEGOS GANADOS";
    LCD_Print(text_juegosganados, 45, 150, 2, 0x0000, 0xFF40);
    
    String text_J1_estrellas = "J1";
    String text_J2_estrellas = "J2";
    LCD_Print(text_J1_estrellas, 65, 55, 2, 0x07FF, 0x0000);
    LCD_Print(text_J2_estrellas, 215, 55, 2, 0x07FF, 0x0000);
    H_line(60,75,40,0xFF40);
    H_line(210,75,40,0xFF40);

    if (STARS_J1 <=9 && STARS_J2 <= 9){
      String Str_estrellas_J1 = String(STARS_J1);
      String Str_estrellas_J2 = String(STARS_J2);
      LCD_Print(Str_estrellas_J1, 60, 85, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_J2, 210, 85, 2, 0x07FF, 0x0000);
     
      LCD_Bitmap(80, 85, 16, 16, estrella_16);
      LCD_Bitmap(230, 85, 16, 16, estrella_16);
      }
      
    else if (STARS_J1 <=9 && STARS_J2 >= 10){
      String Str_estrellas_J1 = String(STARS_J1);
      String Str_estrellas_J2 = String(STARS_J2);
      LCD_Print(Str_estrellas_J1, 60, 85, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_J2, 205, 85, 2, 0x07FF, 0x0000);
     
      LCD_Bitmap(80, 85, 16, 16, estrella_16);
      LCD_Bitmap(240, 85, 16, 16, estrella_16);
      }
      
    else if (STARS_J1 >=10 && STARS_J2 <= 9){
      String Str_estrellas_J1 = String(STARS_J1);
      String Str_estrellas_J2 = String(STARS_J2);
      LCD_Print(Str_estrellas_J1, 55, 85, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_J2, 210, 85, 2, 0x07FF, 0x0000);
     
      LCD_Bitmap(90, 85, 16, 16, estrella_16);
      LCD_Bitmap(230, 85, 16, 16, estrella_16);
      }

    else if (STARS_J1 >=10 && STARS_J2 >= 10){
      String Str_estrellas_J1 = String(STARS_J1);
      String Str_estrellas_J2 = String(STARS_J2);
      LCD_Print(Str_estrellas_J1, 55, 85, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_J2, 205, 85, 2, 0x07FF, 0x0000);
     
      LCD_Bitmap(90, 85, 16, 16, estrella_16);
      LCD_Bitmap(240, 85, 16, 16, estrella_16);
      }
      

    String Str_juegos_gnados_J1 = String(Juegos_ganados_J1);
    String Str_juegos_gnados_J2 = String(Juegos_ganados_J2);
    String text_guion = "-";
    
    LCD_Print(Str_juegos_gnados_J1, 65, 190, 2, 0xFF40, 0x0000);//juegos ganados J1
    LCD_Print(text_guion, 145, 190, 2, 0xFF40, 0x0000);//guion
    LCD_Print(Str_juegos_gnados_J2, 230, 195, 2, 0xFF40, 0x0000);//JUEGOS ganadas J2

   
    
    delay(300);   
}

//***************************************************************************************************************************************
// Función hacer colocar los muñecos en su posicion incial en cada mapa
//***************************************************************************************************************************************
void Posicion_inicial_munecos(int nivel_pos_i){ //SEGUN EL NUMERO DE MAPA SE COLOCAN LOS MUNECOS AL INICIO
  
  switch (nivel_pos_i){
    case 1:
      LCD_Sprite(128, 216, 24, 24, J1_Abajo_Derecha,1,0,1,0);
      LCD_Sprite(128+160, 216, 24, 24, J2_Abajo_Derecha,1,0,1,0);
      break;
    case 2:
      LCD_Bitmap(8, 216, 24, 24, J1_Abajo_Derecha); 
      LCD_Bitmap(168, 216, 24, 24, J2_Abajo_Derecha); 
      break; 
    case 3:
      LCD_Sprite(128, 216, 24, 24, J1_Abajo_Derecha,1,0,1,0);
      LCD_Sprite(128+160, 216, 24, 24, J2_Abajo_Derecha,1,0,1,0); 
      break;
  }
}

//***************************************************************************************************************************************
// Función hacer colocar la meta en cada mapa
//***************************************************************************************************************************************
void Posicion_meta(int nivel_pos_i){ //SEGUN EL NUMERO DE MAPA SE COLOCAN LA META
  
  switch (nivel_pos_i){
    case 1:
      //RECTANGULOS DE LUGAR DE SALIDA
      Rect(8,0,24,24,0xFF40); //J1
      Rect(12,4,16,16,0xFF40);
      Rect(16,8,8,8,0xFF40);
      Rect(168,0,24,24,0xFF40);//J2
      Rect(172,4,16,16,0xFF40);
      Rect(176,8,8,8,0xFF40);
    break;
    
    case 2:
      //RECTANGULOS DE LUGAR DE SALIDA
      Rect(128,0,23,24,0xFF40);//J1
      Rect(132,4,16,16,0xFF40);
      Rect(136,8,8,8,0xFF40);
      
      Rect(288,0,23,24,0xFF40); //J2
      Rect(292,4,16,16,0xFF40);
      Rect(296,8,8,8,0xFF40);
    break; 
    
    case 3:
      //RECTANGULOS DE LUGAR DE SALIDA
      Rect(72,0,23,24,0xFF40); //J1
      Rect(76,4,16,16,0xFF40);
      Rect(80,8,8,8,0xFF40);
      Rect(232,0,23,24,0xFF40);//J2
      Rect(236,4,16,16,0xFF40);
      Rect(240,8,8,8,0xFF40); 
    break;
  }
}

//***************************************************************************************************************************************
// Función hacer colocar las estrellas en cada mapa
//***************************************************************************************************************************************
void Posicion_estrellas(int nivel_pos_i){ //SEGUN EL NUMERO DE MAPA SE COLOCAN LA META
  
  switch (nivel_pos_i){
    case 1:
      LCD_Bitmap(12, 220, 16, 16, estrella_16);
      LCD_Bitmap(172, 220, 16, 16, estrella_16);
      
      LCD_Bitmap(44, 164, 16, 16, estrella_16);
      LCD_Bitmap(204, 164, 16, 16, estrella_16);

      LCD_Bitmap(132, 188, 16, 16, estrella_16);
      LCD_Bitmap(292, 188+160, 16, 16, estrella_16);

      LCD_Bitmap(44, 92, 16, 16, estrella_16);
      LCD_Bitmap(204, 92, 16, 16, estrella_16);

      LCD_Bitmap(132, 4, 16, 16, estrella_16);
      LCD_Bitmap(292, 4, 16, 16, estrella_16);
      
      break;
    case 2:
      LCD_Bitmap(92, 220, 16, 16, estrella_16);
      LCD_Bitmap(92+160, 220, 16, 16, estrella_16);
      
      LCD_Bitmap(52, 148, 16, 16, estrella_16);
      LCD_Bitmap(52+160, 148, 16, 16, estrella_16);

      LCD_Bitmap(92, 76, 16, 16, estrella_16);
      LCD_Bitmap(92+160, 76, 16, 16, estrella_16);

      LCD_Bitmap(132, 124, 16, 16, estrella_16);
      LCD_Bitmap(132+160, 124, 16, 16, estrella_16);

      LCD_Bitmap(52, 28, 16, 16, estrella_16);
      LCD_Bitmap(52+160, 28, 16, 16, estrella_16);
      
      break; 
    case 3:
      LCD_Bitmap(12, 196, 16, 16, estrella_16);
      LCD_Bitmap(12+160, 196, 16, 16, estrella_16);
      
      LCD_Bitmap(76, 188, 16, 16, estrella_16);
      LCD_Bitmap(76+160, 188, 16, 16, estrella_16);

      LCD_Bitmap(12, 84, 16, 16, estrella_16);
      LCD_Bitmap(12+160, 84, 16, 16, estrella_16);

      LCD_Bitmap(44, 4, 16, 16, estrella_16);
      LCD_Bitmap(44+160, 4, 16, 16, estrella_16);

      LCD_Bitmap(132, 4, 16, 16, estrella_16);
      LCD_Bitmap(132+160, 4, 16, 16, estrella_16);
      break;
  }
}

//***************************************************************************************************************************************
// Función para mostrar cantidad de estrellas en mapas
//***************************************************************************************************************************************
void Mostrar_stars_en_mapa(int nivel_pos_i){
  switch (nivel_pos_i){
    case 1:{
      String Str_estrellas_N1_J1; 
      Str_estrellas_N1_J1 = String(STARS_N1_J1);
      String Str_estrellas_N1_J2;
      Str_estrellas_N1_J2 = String(STARS_N1_J2);
      LCD_Print(Str_estrellas_N1_J1, 48, 0, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_N1_J2, 48+160, 0, 2, 0x07FF, 0x0000);
    }
    break;

    case 2:{
      String Str_estrellas_N2_J1; 
      Str_estrellas_N2_J1= String(STARS_N2_J1);
      String Str_estrellas_N2_J2;
      Str_estrellas_N2_J2= String(STARS_N2_J2);
      LCD_Print(Str_estrellas_N2_J1, 16, 0, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_N2_J2, 16+160, 0, 2, 0x07FF, 0x0000);
      }
    break;

    case 3:{
      String Str_estrellas_N3_J1;
      Str_estrellas_N3_J1= String(STARS_N3_J1);
      String Str_estrellas_N3_J2;
      Str_estrellas_N3_J2= String(STARS_N3_J2);
      LCD_Print(Str_estrellas_N3_J1, 104, 0, 2, 0x07FF, 0x0000);
      LCD_Print(Str_estrellas_N3_J2, 104+160, 0, 2, 0x07FF, 0x0000);
    }
    break;
   }
 }
//***************************************************************************************************************************************
// Función para los bloques de cada mapa de los niveles
//***************************************************************************************************************************************
void Mapa_nivel(int nivel_mapa){
  FillRect(0, 0, 320, 240, 0x0000);
  switch (nivel_mapa){

// ------------------------------------------------------------//
// ----------------------- MAPA 1 -----------------------------//
// -----------------------------------------------------------//
    case 1:
      Posicion_inicial_munecos(1); //poner a los munecos en su posicion inicial
      Posicion_meta(1);
      Posicion_estrellas(1);

      for(int y = 0; y <240; y++){ // 1 LINEAS DE LAS ORILLAS
          LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(160, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int x = 240; x <312; x++){ //2
          LCD_Bitmap(x, 208, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x-160, 208, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int x = 200; x <240; x++){ // 3
          LCD_Bitmap(x, 184, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x-160, 184, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int y = 184; y <208; y++){ // 4
          LCD_Bitmap(80, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(240, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }

      for(int y = 216; y <240; y++){ // 5
          LCD_Bitmap(208, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(192, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(48, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(32, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      LCD_Bitmap(40, 216, 8, 8, Bloque_8_morado); //6
      LCD_Bitmap(200, 216, 8, 8, Bloque_8_morado); 
          
      for(int y = 152; y <192; y++){ //7
          LCD_Bitmap(32, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(192, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }

      for(int x = 200; x <280; x++){ //8
          LCD_Bitmap(x, 152, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x-160, 152, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int y = 80; y <128; y++){ //9
          LCD_Bitmap(32, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(192, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }

      for(int y = 48; y <88; y++){ //10
          LCD_Bitmap(56, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(88, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(216, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(248, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int y = 0; y <24; y++){ //11
          LCD_Bitmap(34, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(72, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(194, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(232, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int y = 24; y <56; y++){ //12
          LCD_Bitmap(104, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(120, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(264, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(280, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }

      for(int x = 8; x <32; x++){ //13
          LCD_Bitmap(x, 48, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 48, 8, 8, Bloque_8_morado);
          x += 7;
          }

      for(int x = 40; x <56; x++){ //14
          LCD_Bitmap(x, 80, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 80, 8, 8, Bloque_8_morado);
          x += 7;
          }

      for(int x = 40; x <72; x++){ //15
          LCD_Bitmap(x, 16, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 16, 8, 8, Bloque_8_morado);
          x += 7;
          }

      for(int x = 128; x <152; x++){ //16
          LCD_Bitmap(x, 80, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 80, 8, 8, Bloque_8_morado);
          x += 7;
          }

      for(int y = 40; y <56; y++){ //17
          LCD_Bitmap(112, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(272, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      LCD_Bitmap(96, 48, 8, 8, Bloque_8_morado);
      LCD_Bitmap(256, 48, 8, 8, Bloque_8_morado); 
      LCD_Bitmap(112, 24, 8, 8, Bloque_8_morado); 
      LCD_Bitmap(272, 24, 8, 8, Bloque_8_morado);     
        //delay(1000);
      break;
// ------------------------------------------------------------//
// ----------------------- MAPA 2 -----------------------------//
// -----------------------------------------------------------//
    case 2:
      Posicion_inicial_munecos(2); //poner a los munecos en su posicion inicial
      Posicion_meta(2);
      Posicion_estrellas(2);
      
      for(int y = 0; y <240; y++){ // 1 LINEAS DE LAS ORILLAS
          LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(160, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int y = 216; y <240; y++){ // 2
          LCD_Bitmap(72, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(80, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(232, y, 8, 8, Bloque_8_morado); 
          LCD_Bitmap(240, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int y = 120; y <168; y++){ // 3
          LCD_Bitmap(72, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(232, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }   
          
      for(int y = 120; y <144; y++){ // 4
          LCD_Bitmap(80, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(240, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }

      for(int y = 144; y <168; y++){ // 5
          LCD_Bitmap(80, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(240, y, 8, 8, Bloque_8_celeste); 
          y += 7;
          }
          
      LCD_Bitmap(32, 176, 8, 8, Bloque_8_morado); //6
      LCD_Bitmap(32+160, 176, 8, 8, Bloque_8_morado);
      
      LCD_Bitmap(120, 176, 8, 8, Bloque_8_morado);
      LCD_Bitmap(120+160, 176, 8, 8, Bloque_8_morado);
      
      LCD_Bitmap(24, 104, 8, 8, Bloque_8_morado);
      LCD_Bitmap(24+160, 104, 8, 8, Bloque_8_morado);
      
      LCD_Bitmap(128, 104, 8, 8, Bloque_8_morado);
      LCD_Bitmap(128+160, 104, 8, 8, Bloque_8_morado);
      
      LCD_Bitmap(32, 56, 8, 8, Bloque_8_morado); //6
      LCD_Bitmap(32+160, 56, 8, 8, Bloque_8_morado);
      
      LCD_Bitmap(120, 56, 8, 8, Bloque_8_morado);
      LCD_Bitmap(120+160, 56, 8, 8, Bloque_8_morado);
        
      for(int x = 32; x <128; x++){ //7
          LCD_Bitmap(x, 184, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(x+160, 184, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(x, 168, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 168, 8, 8, Bloque_8_morado);
          
          LCD_Bitmap(x, 64, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 64, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x, 48, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 48, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int x = 8; x <32; x++){ //8
          LCD_Bitmap(x, 96, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 96, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x, 112, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 112, 8, 8, Bloque_8_morado);
          
          LCD_Bitmap(x+120, 96, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+280, 96, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+120, 112, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+280, 112, 8, 8, Bloque_8_morado);

          LCD_Bitmap(x, 0, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 0, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x, 16, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 16, 8, 8, Bloque_8_morado);
          x += 7;
          }

      for(int y = 72; y <96; y++){ // 9
          LCD_Bitmap(72, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(232, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(80, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(240, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }
          
      for(int y = 24; y <48; y++){ // 10
          LCD_Bitmap(72, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(232, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(80, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(240, y, 8, 8, Bloque_8_celeste); 
          y += 7;
          }
          
      for(int y = 0; y <24; y++){ // 11
          LCD_Bitmap(32, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(32+160, y, 8, 8, Bloque_8_celeste);
          
          LCD_Bitmap(120, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(120+160, y, 8, 8, Bloque_8_morado); 
          y += 7;
          }       
      break; 
 // ------------------------------------------------------------//
// ----------------------- MAPA 3 -----------------------------//
// -----------------------------------------------------------//     

    case 3:
      Posicion_inicial_munecos(3); //poner a los munecos en su posicion inicial
      Posicion_meta(3);
      Posicion_estrellas(3);
      //LCD_Bitmap(296, 224, 24, 24, J2_Abajo_Derecha);
      //LCD_Bitmap(296, 224, 24, 24, J2_Abajo_Derecha);  

      for(int y = 216; y <240; y++){ // 1
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(160, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(152+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int y = 192; y <216; y++){ // 2
          LCD_Bitmap(0, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(160, y, 8, 8, Bloque_8_celeste);
          y += 7;
          }
          
      for(int y = 176; y <208; y++){ // 3
          LCD_Bitmap(152, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(152+160, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(64, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(64+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }

      for(int y = 0; y <192; y++){ // 4
          LCD_Bitmap(0, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int y = 136; y <176; y++){ // 5
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(152+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int y = 88; y <136; y++){ // 6
          LCD_Bitmap(152, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(152+160, y, 8, 8, Bloque_8_celeste);
          y += 7;
          }

      for(int y = 0; y <88; y++){ // 7
          LCD_Bitmap(152, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(152+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int y = 24; y <144; y++){ // 8
          LCD_Bitmap(32, y, 8, 8, Bloque_8_celeste);
          LCD_Bitmap(32+160, y, 8, 8, Bloque_8_celeste);
          y += 7;
          }   
          
      for(int y = 112; y <144; y++){ // 9
          LCD_Bitmap(96, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(96+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int x = 64; x <160; x++){ //10
          LCD_Bitmap(x, 208, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 208, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int x = 104; x <152; x++){ //11
          LCD_Bitmap(x, 136, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 136, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int x = 8; x <40; x++){ //12
          LCD_Bitmap(x, 168, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 168, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x, 184, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 184, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      LCD_Bitmap(32, 176, 8, 8, Bloque_8_morado); //13   
      LCD_Bitmap(32+160, 176, 8, 8, Bloque_8_morado); 

      for(int y = 0; y <64; y++){ // 14
          LCD_Bitmap(64, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(64+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int x = 72; x <96; x++){ //15
          LCD_Bitmap(x, 56, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 56, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x, 48, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 48, 8, 8, Bloque_8_morado);
          x += 7;
          }
          
      for(int y = 0; y <24; y++){ // 16
          LCD_Bitmap(96, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(96+160, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(120, y, 8, 8, Bloque_8_morado);
          LCD_Bitmap(120+160, y, 8, 8, Bloque_8_morado);
          y += 7;
          }
          
      for(int x = 104; x <120; x++){ //17
          LCD_Bitmap(x, 16, 8, 8, Bloque_8_morado);
          LCD_Bitmap(x+160, 16, 8, 8, Bloque_8_morado);
          x += 7;
          }
      break;
      
// ------------------------------------------------------------//
// ----------------------- MAPA GANADOR -----------------------------//
// -----------------------------------------------------------//     

    case 4:
    for(int y = 0; y <240; y++){  //lineas decoracion 
      LCD_Bitmap(0, y, 8, 8, Bloque_8_morado);
      LCD_Bitmap(312, y, 8, 8, Bloque_8_morado);
      y += 7;
     }

     for(int x = 0; x <320; x++){  //lineas decoracion 
      LCD_Bitmap(x, 0, 8, 8, Bloque_8_morado);
      LCD_Bitmap(x, 232, 8, 8, Bloque_8_morado);
      x += 7;
     }
    if (STARS_J1 > STARS_J2){
      String text_ganador = "GANADOR J1";
      LCD_Print(text_ganador, 80, 60, 2, 0x07FF, 0x0000);
      switch (num_personaje_J1){
        
        case 0:
          while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Muneco_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Muneco_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
          } 
        break;
        case 1:
          while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Calavera_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Calavera_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
          }
        break;
        case 2:
          while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Koala_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Koala_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
          }
        break;
        case 3:
        while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Mono_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Mono_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
        }
        break;
        }
      }
    else if  (STARS_J1 < STARS_J2){
      String text_ganador = "GANADOR J2";
      LCD_Print(text_ganador, 70, 60, 2, 0x07FF, 0x0000);
      switch (num_personaje_J2){
        case 0:
        while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Muneco_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Muneco_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
        }
        break;
        case 1:
        while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Calavera_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Calavera_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
        }
        break;
        case 2:
        while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Koala_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Koala_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
        }
        break;
        case 3:
        while(JUEGO_EN_PROGRESO == HIGH){
            for(int x = 85; x <200; x++){
            LCD_Sprite(x,120,50,50,Mono_50,1,0,0,0);
            V_line( x -1, 120, 50, 0x0000);
          }
          for(int x = 200; x >85; x--){
            //void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],int columns, int index, char flip, char offset);
            LCD_Sprite(x,120,50,50,Mono_50,1,0,1,0);
            V_line( x + 50, 120, 50, 0x0000);
           }
        }
        break;
        }
      
      }
    else if  (STARS_J1 == STARS_J2){
      String text_ganador = "EMPATE";
      LCD_Print(text_ganador, 110, 80, 2, 0x07FF, 0x0000);
      while(JUEGO_EN_PROGRESO == HIGH){
        for(int x = 70; x <140; x++){
              LCD_Sprite(x,140,24,24, J1_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 140, 24, 0x0000);
              LCD_Sprite(x+100,140,24,24,J2_Abajo_Derecha,1,0,0,0);
              V_line( x + 99, 140, 24, 0x0000);
            }
            for(int x = 140; x >70; x--){
              LCD_Sprite(x,140,24,24,J1_Abajo_Derecha,1,0,1,0);
              V_line( x + 24, 140, 24, 0x0000);
              LCD_Sprite(x+100,140,24,24,J2_Abajo_Derecha,1,0,1,0);
              V_line( x + 124, 140, 24, 0x0000);
             }
      }
      }
  }
}

void bajar_banderas_stars(void){
  posicion_J1 = 0;
  posicion_J2 = 0;
  STAR_1_J1 = LOW;
  STAR_1_J2 = LOW;
  STAR_2_J1 = LOW;
  STAR_2_J2 = LOW;
  STAR_3_J1 = LOW;
  STAR_3_J2 = LOW;
  STAR_4_J1 = LOW;
  STAR_4_J2 = LOW;
  STAR_5_J1 = LOW;
  STAR_5_J2 = LOW;
    
  b_LEFT_J1 = LOW;
  b_LEFT_J2 = LOW;
  b_RIGHT_J1 = LOW;
  b_RIGHT_J2 = LOW;
  b_UP_J1 = LOW;
  b_UP_J2 = LOW;
  b_DOWN_J1 = LOW;
  b_DOWN_J2 = LOW;
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
  }
  else{
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

//***************************************************************************************************************************************
// FUNCION PARA LEER VALORES ASCII DE LA SD
//***************************************************************************************************************************************
void open_SD_bitmap(unsigned char Bitmap_SD[], unsigned long Size_bitmap, char* filename) {
  File myFile = SD.open(filename);     // ABRIR EL ARCHIVO 
  unsigned long i = 0;            
  char Bitmap_SD_HEX[] = {0, 0};          //SE HACE ARREGLO DE DOS NUM, PARA CADA UNA DE LAS POSICIONES
  int Pos_1, Pos_2;                     //VARIABLES DE LAS POSICIONES
  if (myFile) {                 
    do {
      Bitmap_SD_HEX[0] = myFile.read(); //LEE
      Pos_1 = ACII_to_HEX(Bitmap_SD_HEX);       //TRANSOFRMA
      Bitmap_SD_HEX[0] = myFile.read(); //LEE
      Pos_2 = ACII_to_HEX(Bitmap_SD_HEX);       //TRANSFORMA
      Bitmap_SD[i] = (Pos_1 << 4) | (Pos_2 & 0xF);  //SE CONCATENA CONCATENA
      i++;                        
    } while (i < (Size_bitmap + 1));
  }
  myFile.close();                       
}

//***************************************************************************************************************************************
// FUNCION PARA PASAR INFO DE SD A VALORES HEXADECIMALES
//***************************************************************************************************************************************
int ACII_to_HEX(char *puntero) {
  int i = 0;
  for (;;) {
    char num = *puntero;
    if (num >= '0' && num <= '9') {
      i *= 16;
      i += num - '0';
    }
    else if (num >= 'a' && num <= 'f') {
      i *= 16;
      i += (num - 'a') + 10;
    }
    else break;
    puntero++;
  }
  return i;
}


//***************************************************************************************************************************************
// Función cuando se presiona el boton izquierdo del J1 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_LEFT_J1(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 IZQUIERDA J1 ###############################//
    case 1: //NIVEL 1 MOVERSE A LA IZQUIERDA
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
            for(int x = 128; x >56; x = x-1){
              LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,1,0);
              V_line( x + 24, 216, 24, 0x0000);
            }
            posicion_J1 = 1;
            b_LEFT_J1 = LOW;
        break; //break pos 0 /LEFT/N1/J1
        
        case 1:
          b_LEFT_J1 = LOW;
        break; //break pos 1 /LEFT/N1/J1

        case 2:
          b_LEFT_J1 = LOW;
        break; //break pos 2 /LEFT/N1/J1
        
        case 3:
          for(int x = 56; x >8; x = x-1){
             LCD_Sprite(x,192,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 192, 24, 0x0000);
             FillRect(56,208,24,16,0x0000);
            }
          posicion_J1 = 4;
          b_LEFT_J1 = LOW;
        break; //break pos 3 /LEFT/N1/J1
        
        case 4:
          b_LEFT_J1 = LOW;
        break; //break pos 4 /LEFT/N1/J1
        
        case 5:
            for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,184,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 184, 24, 0x0000);
             FillRect(128,176,24,16,0x0000);
            }
          posicion_J1 = 6;
          b_LEFT_J1 = LOW;
        break; //break pos 5 /LEFT/N1/J1
        
        case 6:
          b_LEFT_J1 = LOW;
        break; //break pos 6 /LEFT/N1/J1
        
        case 7:
          for(int x = 128; x >40; x = x-1){
             LCD_Sprite(x,160,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 160, 24, 0x0000);
            }
            if (STAR_5_J1 == LOW){
              STARS_N1_J1++;
              STARS_J1++;
              STAR_5_J1 = HIGH;
            }
          posicion_J1 = 9;
          b_LEFT_J1 = LOW;
        break; //break pos 7 /LEFT/N1/J1
        
        case 8:
          for(int x = 88; x >40; x = x-1){
             LCD_Sprite(x,160,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 160, 24, 0x0000);
             FillRect(88,176,24,16,0x0000);
            }
            if (STAR_5_J1 == LOW){
              STARS_N1_J1++;
              STARS_J1++;
              STAR_5_J1 = HIGH;
            }
          posicion_J1 = 9;
          b_LEFT_J1 = LOW;
        break; //break pos 8 /LEFT/N1/J1
        
        case 9:
          b_LEFT_J1 = LOW;
        break; //break pos 9 /LEFT/N1/J1
        
        case 10:
          for(int x = 128; x >8; x = x-1){
             LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 128, 24, 0x0000);
            }
          posicion_J1 = 13;
          b_LEFT_J1 = LOW;
        break; //break pos 10 /LEFT/N1/J1
        
        case 11:
          for(int x = 96; x >8; x = x-1){
             LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 128, 24, 0x0000);
             FillRect(96,120,24,16,0x0000);
            }
          posicion_J1 = 13;
          b_LEFT_J1 = LOW;
        break; //break pos 11 /LEFT/N1/J1
        
        case 12:
          for(int x = 40; x >8; x = x-1){
            LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,1,0);
            V_line( x + 24, 128, 24, 0x0000);
            FillRect(40,120,24,16,0x0000);
          }
          posicion_J1 = 13;
          b_LEFT_J1 = LOW;
        break; //break pos 12 /LEFT/N1/J1
        
        case 13:
          b_LEFT_J1 = LOW;
        break; //break pos 13 /LEFT/N1/J1
        
        case 14:
          for(int x = 128; x >40; x = x-1){
             LCD_Sprite(x,88,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 88, 24, 0x0000);
             FillRect(128,104,24,16,0x0000);
            }
            if (STAR_3_J1 == LOW){
              STARS_N1_J1++;
              STARS_J1++;
              STAR_3_J1 = HIGH;
            }
          posicion_J1 = 15;
          b_LEFT_J1 = LOW;
        break; //break pos 14 /LEFT/N1/J1
        
        case 15:
          b_LEFT_J1 = LOW;
        break; //break pos 15 /LEFT/N1/J1
        
        case 16:
         for(int x = 128; x >96; x = x-1){
             LCD_Sprite(x,56,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 56, 24, 0x0000);
             FillRect(128,48,24,16,0x0000);
            }
          posicion_J1 = 17;
          b_LEFT_J1 = LOW;
        break; //break pos 16 /LEFT/N1/J1
        
        case 17:
          b_LEFT_J1 = LOW;
        break; //break pos 17 /LEFT/N1/J1
        
        case 18:
          for(int x = 32; x >8; x = x-1){
             LCD_Sprite(x,56,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 56, 24, 0x0000);
             FillRect(32,48,24,16,0x0000);
            }
          posicion_J1 = 19;
          b_LEFT_J1 = LOW;
        break; //break pos 18 /LEFT/N1/J1
        
        case 19:
          b_LEFT_J1 = LOW;
        break; //break pos 19 /LEFT/N1/J1
        
        case 20:
          for(int x = 80; x >8; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(80,16,24,16,0x0000);
            }
          posicion_J1 = 22;
          b_LEFT_J1 = LOW;
        break; //break pos 20 /LEFT/N1/J1
        
        case 21:
          for(int x = 32; x >8; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(32,40,24,16,0x0000);
            }
          posicion_J1 = 22;
          b_LEFT_J1 = LOW;
        break; //break pos 21 /LEFT/N1/J1
        
        case 22:
          b_LEFT_J1 = LOW;
        break; //break pos 22 /LEFT/N1/J1
        
        case 23:
          for(int x = 128; x >80; x = x-1){
             LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(128,16,24,16,0x0000);
            }
          posicion_J1 = 24;
          b_LEFT_J1 = LOW;
        break; //break pos 23 /LEFT/N1/J1
        
        case 24:
          b_LEFT_J1 = LOW;
        break; //break pos 24 /LEFT/N1/J1
        
        case 25:
          b_LEFT_J1 = LOW;
        break; //break pos 25 /LEFT/N1/J1
        }
      
    break; //break del case 1 (nivel 1 - LEFT - J1)

//#################### NIVEL 2 IZQUIERDA J1 ###############################//
    case 2: 
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 1 
            b_LEFT_J1 = LOW;
        break; //break pos 0 /LEFT/N2/J1
        
        case 1:
          for(int x = 48; x >8; x = x-1){
             LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
            }
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 1 /LEFT/N2/J1

        case 2:
          b_LEFT_J1 = LOW;
        break; //break pos 2 /LEFT/N2/J1
        
        case 3:
          for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
             FillRect(128,208,24,16,0x0000);
            }
            if (STAR_5_J1 == LOW){
              STARS_N2_J1++;
              STARS_J1++;
              STAR_5_J1 = HIGH;
            }
          posicion_J1 = 2;
          b_LEFT_J1 = LOW;
        break; //break pos 3 /LEFT/N2/J1
        
        case 4:
          b_LEFT_J1 = LOW;
        break; //break pos 4 /LEFT/N2/J1
        
        case 5:
          b_LEFT_J1 = LOW;
        break; //break pos 5 /LEFT/N2/J1
        
        case 6:
          b_LEFT_J1 = LOW;
        break; //break pos 6 /LEFT/N2/J1
        
        case 7:
          for(int x = 48; x >8; x = x-1){
             LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
             FillRect(48,136,24,16,0x0000);
            }
          posicion_J1 = 6;
          b_LEFT_J1 = LOW;
        break; //break pos 7 /LEFT/N2/J1
        
        case 8:
          FillRect(88, 144, 28, 24, 0x0000);
          FillRect(88,136,24,16,0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;          
          b_LEFT_J1 = LOW;
        break; //break pos 8 /LEFT/N2/J1
        
        case 9:
        for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
            }
          FillRect(88, 144, 28, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0; 
          b_LEFT_J1 = LOW;
        break; //break pos 9 /LEFT/N2/J1
        
        case 10:
          b_LEFT_J1 = LOW;
        break; //break pos 10 /LEFT/N2/J1
        
        case 11:
          for(int x = 48; x >8; x = x-1){
             LCD_Sprite(x,120,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 120, 24, 0x0000);
            }
          posicion_J1 = 10;
          b_LEFT_J1 = LOW;
        break; //break pos 11 /LEFT/N2/J1
        
        case 12:
          b_LEFT_J1 = LOW;
        break; //break pos 12 /LEFT/N2/J1
        
        case 13:
          for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,120,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 120, 24, 0x0000);
             FillRect(128,136,24,16,0x0000);
            }
          posicion_J1 = 12;        
          b_LEFT_J1 = LOW;
        break; //break pos 13 /LEFT/N2/J1
        
        case 14:
          b_LEFT_J1 = LOW;
        break; //break pos 14 /LEFT/N2/J1
        
        case 15:
          for(int x = 48; x >8; x = x-1){
             LCD_Sprite(x,72,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 72, 24, 0x0000);
             FillRect(48,88,24,16,0x0000);
            }
          posicion_J1 = 14;        
          b_LEFT_J1 = LOW;
        break; //break pos 15 /LEFT/N2/J1
        
        case 16:
          b_LEFT_J1 = LOW;
        break; //break pos 16 /LEFT/N2/J1
        
        case 17:
          for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,72,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 72, 24, 0x0000);
             FillRect(128,64,24,16,0x0000);
            }
          if (STAR_3_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            }  
          posicion_J1 = 16;        
          b_LEFT_J1 = LOW;
        break; //break pos 17 /LEFT/N2/J1
        
        case 18:
          b_LEFT_J1 = LOW;
        break; //break pos 18 /LEFT/N2/J1
        
        case 19:
          for(int x = 48; x >8; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          posicion_J1 = 18;
          b_LEFT_J1 = LOW;
        break; //break pos 19 /LEFT/N2/J1
        
        case 20:
          for(int x = 96; x >88; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(96,16,24,16,0x0000);
             FillRect(112,24,16,24,0x0000);
            }
          FillRect(88, 24, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 20 /LEFT/N2/J1
        
        case 21:
          for(int x = 128; x >88; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          FillRect(88, 24, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 21 /LEFT/N2/J1
        
        case 22:
          for(int x = 48; x >40; x = x-1){
             LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(48,16,24,16,0x0000);
             FillRect(56,0,12,24,0x0000);
            }
          FillRect(40, 0, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 22 /LEFT/N2/J1
        
        case 23:
          for(int x = 96; x >40; x = x-1){
             LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,0,0);
             V_line( x + 24, 0, 24, 0x0000);
            }
          FillRect(56,0,12,24,0x0000);
          FillRect(40, 0, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,1,0);
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 23 /LEFT/N2/J1
        
        case 24:
          b_LEFT_J1 = LOW;
        break; //break pos 24 /LEFT/N2/J1
        }
    break;//break del case 2 (nivel 2 - LEFT - J1)
    
//#################### NIVEL 3 IZQUIERDA J1 ###############################//
    case 3:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 1 
          for(int x = 128; x >8; x = x-1){
             LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
            }
           posicion_J1 = 2;        
           b_LEFT_J1 = LOW;
        break; //break pos 0 /LEFT/N3/J1
        
        case 1:
          for(int x = 40; x >8; x = x-1){
             LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
             FillRect(40,208,24,16,0x0000);
            }
           posicion_J1 = 2; 
           b_LEFT_J1 = LOW;
        break; //break pos 1 /LEFT/N3/J1

        case 2:
          b_LEFT_J1 = LOW;
        break; //break pos 2 /LEFT/N3/J1
        
        case 3:
          for(int x = 40; x >8; x = x-1){
             LCD_Sprite(x,192,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 192, 24, 0x0000);
            }
          if (STAR_1_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            } 
          FillRect(8, 192, 25, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);            
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 3 /LEFT/N3/J1
        
        case 4:
          FillRect(8, 192, 28, 28, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);            
          posicion_J1 = 0;        
          b_LEFT_J1 = LOW;
        break; //break pos 4 /LEFT/N3/J1
        
        case 5:
          for(int x = 128; x >72; x = x-1){
             LCD_Sprite(x,184,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 184, 24, 0x0000);
             FillRect(128,175,24,16,0x0000);
            }
            if (STAR_4_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            } 
           posicion_J1 = 6;         
          b_LEFT_J1 = LOW;
        break; //break pos 5 /LEFT/N3/J1
        
        case 6:
          b_LEFT_J1 = LOW;
        break; //break pos 6 /LEFT/N2/J1
        
        case 7:
          for(int x = 128; x >8; x = x-1){
             LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
            }
          posicion_J1 = 8;
          b_LEFT_J1 = LOW;
        break; //break pos 7 /LEFT/N3/J1
        
        case 8:
          b_LEFT_J1 = LOW;
        break; //break pos 8 /LEFT/N3/J1
        
        case 9:
        for(int x = 128; x >104; x = x-1){
             LCD_Sprite(x,112,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 112, 24, 0x0000);
             FillRect(128,104,24,16,0x0000);
            }
          posicion_J1 = 10; 
          b_LEFT_J1 = LOW;
        break; //break pos 9 /LEFT/N3/J1
        
        case 10:
          b_LEFT_J1 = LOW;
        break; //break pos 10 /LEFT/N3/J1
        
        case 11:
          for(int x = 128; x >40; x = x-1){
             LCD_Sprite(x,64,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 64, 24, 0x0000);
             FillRect(56,64,16,24,0x0000);
            }
          FillRect(40, 64, 24, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);            
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 11 /LEFT/N3/J1
        
        case 12:
          for(int x = 72; x >40; x = x-1){
             LCD_Sprite(x,64,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 64, 24, 0x0000);
             FillRect(72, 80, 24, 16, 0x0000);
            }
          FillRect(40, 64, 28, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);            
          posicion_J1 = 0;        
          b_LEFT_J1 = LOW;
        break; //break pos 12 /LEFT/N3/J1
        
        case 13:
          FillRect(40, 64, 24, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);            
          posicion_J1 = 0;       
          b_LEFT_J1 = LOW;
        break; //break pos 13 /LEFT/N3/J1
        
        case 14:
          for(int x = 128; x >72; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          posicion_J1 = 16;           
          b_LEFT_J1 = LOW;
        break; //break pos 14 /LEFT/N3/J1
        
        case 15:
          for(int x = 104; x >72; x = x-1){
             LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(104,40,24,16,0x0000);
            }
          posicion_J1 = 16;        
          b_LEFT_J1 = LOW;
        break; //break pos 15 /LEFT/N3/J1
        
        case 16:
          b_LEFT_J1 = LOW;
        break; //break pos 16 /LEFT/N3/J1
        
        case 17:      
          b_LEFT_J1 = LOW;
        break; //break pos 17 /LEFT/N3/J1
        
        case 18:
          b_LEFT_J1 = LOW;
        break; //break pos 18 /LEFT/N3/J1
        
        case 19:
          for(int x = 40; x >8; x = x-1){
             LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(40,16,24,16,0x0000);
            }
          posicion_J1 = 20;
          b_LEFT_J1 = LOW;
        break; //break pos 19 /LEFT/N3/J1
        
        case 20:
          b_LEFT_J1 = LOW;
        break; //break pos 20 /LEFT/N3/J1
        } 
    break;//break del case 3 (nivel 3 - LEFT - J1)
    }
          
  }

  //***************************************************************************************************************************************
// Función cuando se presiona el boton izquierdo del J2 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_LEFT_J2(int num_nivel){
    switch(num_nivel){
//#################### NIVEL 1 IZQUIERDA J2 ###############################//
    case 1:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          for(int x = 288; x >216; x = x-1){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,1,0);
            V_line( x + 24, 216, 24, 0x0000);
          }
          posicion_J2 = 1;
          b_LEFT_J2 = LOW;
        break; //break pos 0 /LEFT/N1/J2
        
        case 1:
          b_LEFT_J2 = LOW;
        break; //break pos 1 /LEFT/N1/J2

        case 2:
          b_LEFT_J2 = LOW;
        break; //break pos 2 /LEFT/N1/J2
        
        case 3:
          for(int x = 216; x >168; x=x-1){
            LCD_Sprite(x,192,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x+24, 192, 24, 0x0000);
            FillRect(56+160,208,24,16,0x0000);
          }
          posicion_J2 = 4;
          b_LEFT_J2 = LOW;
        break; //break pos 3 /LEFT/N1/J2
        
        case 4:
          b_LEFT_J2 = LOW;
        break; //break pos 4 /LEFT/N1/J2
        
        case 5:
         for(int x = 288; x >248; x = x-1){
             LCD_Sprite(x,184,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 184, 24, 0x0000);
             FillRect(128+160,176,24,16,0x0000);
            }
          posicion_J2 = 6;
          b_LEFT_J2 = LOW;
        break; //break pos 5 /LEFT/N1/J2
        
        case 6:
          b_LEFT_J2 = LOW;
        break; //break pos 6 /LEFT/N1/J2
        
        case 7:
          for(int x = 288; x >200; x = x-1){
             LCD_Sprite(x,160,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 160, 24, 0x0000);
            }
            if (STAR_5_J2 == LOW){
              STARS_N1_J2++;
              STARS_J2++;
              STAR_5_J2 = HIGH;
            }
          posicion_J2 = 9;
          b_LEFT_J2 = LOW;
        break; //break pos 7 /LEFT/N1/J2
        
        case 8:
          for(int x = 248; x >200; x = x-1){
             LCD_Sprite(x,160,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 160, 24, 0x0000);
             FillRect(88+160,176,24,16,0x0000);
            }
            if (STAR_5_J2 == LOW){
              STARS_N1_J2++;
              STARS_J2++;
              STAR_5_J2 = HIGH;
            }
          posicion_J2 = 9;
          b_LEFT_J2 = LOW;
        break; //break pos 8 /LEFT/N1/J2
        
        case 9:
          b_LEFT_J2 = LOW;
        break; //break pos 9 /LEFT/N1/J2
        
        case 10:
          for(int x = 288; x >168; x = x-1){
             LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 128, 24, 0x0000);
            }
          posicion_J2 = 13;
          b_LEFT_J2 = LOW;
        break; //break pos 10 /LEFT/N1/J2
        
        case 11:
          for(int x = 256; x >168; x = x-1){
             LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 128, 24, 0x0000);
             FillRect(96+160,120,24,16,0x0000);
            }
          posicion_J2 = 13;
          b_LEFT_J2 = LOW;
        break; //break pos 11 /LEFT/N1/J2
        
        case 12:
         for(int x = 200; x >168; x = x-1){
            LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,1,0);
            V_line( x + 24, 128, 24, 0x0000);
            FillRect(40+160,120,24,16,0x0000);
          }
          posicion_J2 = 13;
          b_LEFT_J2 = LOW;
        break; //break pos 12 /LEFT/N1/J2
        
        case 13:
          b_LEFT_J2 = LOW;
        break; //break pos 13 /LEFT/N1/J2
        
        case 14:
         for(int x = 288; x >200; x = x-1){
             LCD_Sprite(x,88,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 88, 24, 0x0000);
             FillRect(128+160,104,24,16,0x0000);
            }
            if (STAR_3_J2 == LOW){
              STARS_N1_J2++;
              STARS_J2++;
              STAR_3_J2 = HIGH;
            }
          posicion_J2 = 15;
          b_LEFT_J2 = LOW;
        break; //break pos 14 /LEFT/N1/J2
        
        case 15:
          b_LEFT_J2 = LOW;
        break; //break pos 15 /LEFT/N1/J2
        
        case 16:
          for(int x = 288; x >256; x = x-1){
             LCD_Sprite(x,56,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 56, 24, 0x0000);
             FillRect(128+160,48,24,16,0x0000);
            }
          posicion_J2 = 17;
          b_LEFT_J2 = LOW;
        break; //break pos 16 /LEFT/N1/J2
        
        case 17:
          b_LEFT_J2 = LOW;
        break; //break pos 17 /LEFT/N1/J2
        
        case 18:
          for(int x = 192; x >168; x = x-1){
             LCD_Sprite(x,56,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 56, 24, 0x0000);
             FillRect(32+160,48,24,16,0x0000);
            }
          posicion_J2 = 19;
          b_LEFT_J2 = LOW;
        break; //break pos 18 /LEFT/N1/J2
        
        case 19:
          b_LEFT_J2 = LOW;
        break; //break pos 19 /LEFT/N1/J2
        
        case 20:
          for(int x = 240; x >168; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(80+160,16,24,16,0x0000);
            }
          posicion_J2 = 22;
          b_LEFT_J2 = LOW;
        break; //break pos 20 /LEFT/N1/J2
        
        case 21:
          for(int x = 192; x >168; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(32+160,40,24,16,0x0000);
            }
          posicion_J2 = 22;
          b_LEFT_J2 = LOW;
        break; //break pos 21 /LEFT/N1/J2
        
        case 22:
          b_LEFT_J2 = LOW;
        break; //break pos 22 /LEFT/N1/J2
        
        case 23:
          for(int x = 288; x >240; x = x-1){
             LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(128+160,16,24,16,0x0000);
            }
          posicion_J2 = 24;
          b_LEFT_J2 = LOW;
        break; //break pos 23 /LEFT/N1/J2
        
        case 24:
          b_LEFT_J2 = LOW;
        break; //break pos 24 /LEFT/N1/J2
        
        case 25:
          b_LEFT_J2 = LOW;
        break; //break pos 25 /LEFT/N1/J2
        }
    break;//break del case 1 (nivel 1 - LEFT - J2)

//#################### NIVEL 2 IZQUIERDA J2 ###############################//
    case 2:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 2 
            b_LEFT_J2 = LOW;
        break; //break pos 0 /LEFT/N2/J2
        
        case 1:
          for(int x = 48+160; x >8+160; x = x-1){
             LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
            }
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 1 /LEFT/N2/J2

        case 2:
          b_LEFT_J2 = LOW;
        break; //break pos 2 /LEFT/N2/J2
        
        case 3:
          for(int x = 128+160; x >88+160; x = x-1){
             LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
             FillRect(128+160,208,24,16,0x0000);
            }
            if (STAR_5_J2 == LOW){
              STARS_N2_J2++;
              STARS_J2++;
              STAR_5_J2 = HIGH;
            }
          posicion_J2 = 2;
          b_LEFT_J2 = LOW;
        break; //break pos 3 /LEFT/N2/J2
        
        case 4:
          b_LEFT_J2 = LOW;
        break; //break pos 4 /LEFT/N2/J2
        
        case 5:
          b_LEFT_J2 = LOW;
        break; //break pos 5 /LEFT/N2/J2
        
        case 6:
          b_LEFT_J2 = LOW;
        break; //break pos 6 /LEFT/N2/J2
        
        case 7:
          for(int x = 48+160; x >8+160; x = x-1){
             LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
             FillRect(48+160,136,24,16,0x0000);
            }
          posicion_J2 = 6;
          b_LEFT_J2 = LOW;
        break; //break pos 7 /LEFT/N2/J2
        
        case 8:
          FillRect(88+160, 144, 28, 24, 0x0000);
          FillRect(88+160,136,24,16,0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;          
          b_LEFT_J2 = LOW;
        break; //break pos 8 /LEFT/N2/J2
        
        case 9:
        for(int x = 128+160; x >88+160; x = x-1){
             LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
            }
          FillRect(88+160, 144, 28, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0; 
          b_LEFT_J2 = LOW;
        break; //break pos 9 /LEFT/N2/J2
        
        case 10:
          b_LEFT_J2 = LOW;
        break; //break pos 10 /LEFT/N2/J2
        
        case 11:
          for(int x = 48+160; x >8+160; x = x-1){
             LCD_Sprite(x,120,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 120, 24, 0x0000);
            }
          posicion_J2 = 10;
          b_LEFT_J2 = LOW;
        break; //break pos 11 /LEFT/N2/J2
        
        case 12:
          b_LEFT_J2 = LOW;
        break; //break pos 12 /LEFT/N2/J2
        
        case 13:
          for(int x = 128+160; x >88+160; x = x-1){
             LCD_Sprite(x,120,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 120, 24, 0x0000);
             FillRect(128+160,136,24,16,0x0000);
            }
          posicion_J2 = 12;        
          b_LEFT_J2 = LOW;
        break; //break pos 13 /LEFT/N2/J2
        
        case 14:
          b_LEFT_J2 = LOW;
        break; //break pos 14 /LEFT/N2/J2
        
        case 15:
          for(int x = 48+160; x >8+160; x = x-1){
             LCD_Sprite(x,72,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 72, 24, 0x0000);
             FillRect(48+160,88,24,16,0x0000);
            }
          posicion_J2 = 14;        
          b_LEFT_J2 = LOW;
        break; //break pos 15 /LEFT/N2/J2
        
        case 16:
          b_LEFT_J2 = LOW;
        break; //break pos 16 /LEFT/N2/J2
        
        case 17:
          for(int x = 128+160; x >88+160; x = x-1){
             LCD_Sprite(x,72,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 72, 24, 0x0000);
             FillRect(128+160,64,24,16,0x0000);
            }
          if (STAR_3_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            }  
          posicion_J2 = 16;        
          b_LEFT_J2 = LOW;
        break; //break pos 17 /LEFT/N2/J2
        
        case 18:
          b_LEFT_J2 = LOW;
        break; //break pos 18 /LEFT/N2/J2
        
        case 19:
          for(int x = 48+160; x >8+160; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          posicion_J2 = 18;
          b_LEFT_J2 = LOW;
        break; //break pos 19 /LEFT/N2/J2
        
        case 20:
          for(int x = 96+160; x >88+160; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(96+160,16,24,16,0x0000);
             FillRect(112+160,24,16,24,0x0000);
            }
          FillRect(88+160, 24, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 20 /LEFT/N2/J2
        
        case 21:
          for(int x = 128+160; x >88+160; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          FillRect(88+160, 24, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 21 /LEFT/N2/J2
        
        case 22:
          for(int x = 48+160; x >40+160; x = x-1){
             LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(48+160,16,24,16,0x0000);
             FillRect(56+160,0,12,24,0x0000);
            }
          FillRect(40+160, 0, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 22 /LEFT/N2/J2
        
        case 23:
          for(int x = 96+160; x >40+160; x = x-1){
             LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,0,0);
             V_line( x + 24, 0, 24, 0x0000);
            }
          FillRect(56+160,0,12,24,0x0000);
          FillRect(40+160, 0, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 23 /LEFT/N2/J2
        
        case 24:
          b_LEFT_J2 = LOW;
        break; //break pos 24 /LEFT/N2/J2
        }
    break;//break del case 2 (nivel 2 - LEFT - J2)
    
//#################### NIVEL 3 IZQUIERDA J2 ###############################//
    case 3:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 2 
          for(int x = 128+160; x >8+160; x = x-1){
             LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
            }
           posicion_J2 = 2;        
           b_LEFT_J2 = LOW;
        break; //break pos 0 /LEFT/N3/J2
        
        case 1:
          for(int x = 40+160; x >8+160; x = x-1){
             LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 216, 24, 0x0000);
             FillRect(40+160,208,24,16,0x0000);
            }
           posicion_J2 = 2; 
           b_LEFT_J2 = LOW;
        break; //break pos 1 /LEFT/N3/J2

        case 2:
          b_LEFT_J2 = LOW;
        break; //break pos 2 /LEFT/N3/J2
        
        case 3:
          for(int x = 40+160; x >8+160; x = x-1){
             LCD_Sprite(x,192,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 192, 24, 0x0000);
            }
          if (STAR_1_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            } 
          FillRect(8+160, 192, 25, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);            
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 3 /LEFT/N3/J2
        
        case 4:
          FillRect(8+160, 192, 28, 28, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);            
          posicion_J2 = 0;        
          b_LEFT_J2 = LOW;
        break; //break pos 4 /LEFT/N3/J2
        
        case 5:
          for(int x = 128+160; x >72+160; x = x-1){
             LCD_Sprite(x,184,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 184, 24, 0x0000);
             FillRect(128+160,175,24,16,0x0000);
            }
            if (STAR_4_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            } 
           posicion_J2 = 6;         
          b_LEFT_J2 = LOW;
        break; //break pos 5 /LEFT/N3/J2
        
        case 6:
          b_LEFT_J2 = LOW;
        break; //break pos 6 /LEFT/N2/J2
        
        case 7:
          for(int x = 128+160; x >8+160; x = x-1){
             LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 144, 24, 0x0000);
            }
          posicion_J2 = 8;
          b_LEFT_J2 = LOW;
        break; //break pos 7 /LEFT/N3/J2
        
        case 8:
          b_LEFT_J2 = LOW;
        break; //break pos 8 /LEFT/N3/J2
        
        case 9:
        for(int x = 128+160; x >104+160; x = x-1){
             LCD_Sprite(x,112,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 112, 24, 0x0000);
             FillRect(128+160,104,24,16,0x0000);
            }
          posicion_J2 = 10; 
          b_LEFT_J2 = LOW;
        break; //break pos 9 /LEFT/N3/J2
        
        case 10:
          b_LEFT_J2 = LOW;
        break; //break pos 10 /LEFT/N3/J2
        
        case 11:
          for(int x = 128+160; x >40+160; x = x-1){
             LCD_Sprite(x,64,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 64, 24, 0x0000);
             FillRect(56+160,64,16,24,0x0000);
            }
          FillRect(40+160, 64, 24, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);            
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 11 /LEFT/N3/J2
        
        case 12:
          for(int x = 72+160; x >40+160; x = x-1){
             LCD_Sprite(x,64,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 64, 24, 0x0000);
             FillRect(72+160, 80, 24, 16, 0x0000);
            }
          FillRect(40+160, 64, 28, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);            
          posicion_J2 = 0;        
          b_LEFT_J2 = LOW;
        break; //break pos 12 /LEFT/N3/J2
        
        case 13:
          FillRect(40+160, 64, 24, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);            
          posicion_J2 = 0;       
          b_LEFT_J2 = LOW;
        break; //break pos 13 /LEFT/N3/J2
        
        case 14:
          for(int x = 128+160; x >72+160; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
            }
          posicion_J2 = 16;           
          b_LEFT_J2 = LOW;
        break; //break pos 14 /LEFT/N3/J2
        
        case 15:
          for(int x = 104+160; x >72+160; x = x-1){
             LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 24, 24, 0x0000);
             FillRect(104+160,40,24,16,0x0000);
            }
          posicion_J2 = 16;        
          b_LEFT_J2 = LOW;
        break; //break pos 15 /LEFT/N3/J2
        
        case 16:
          b_LEFT_J2 = LOW;
        break; //break pos 16 /LEFT/N3/J2
        
        case 17:      
          b_LEFT_J2 = LOW;
        break; //break pos 17 /LEFT/N3/J2
        
        case 18:
          b_LEFT_J2 = LOW;
        break; //break pos 18 /LEFT/N3/J2
        
        case 19:
          for(int x = 40+160; x >8+160; x = x-1){
             LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 0, 24, 0x0000);
             FillRect(40+160,16,24,16,0x0000);
            }
          posicion_J2 = 20;
          b_LEFT_J2 = LOW;
        break; //break pos 19 /LEFT/N3/J2
        
        case 20:
          b_LEFT_J2 = LOW;
        break; //break pos 20 /LEFT/N3/J2
        } 
    break;//break del case 3 (nivel 3 - LEFT - J2)
    }
}
//***************************************************************************************************************************************
// Función cuando se presiona el boton derecho del J1 y revisa la posicion y se mueve MAPA 1
//***************************************************************************************************************************************
void switch_posicion_RIGHT_J1(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 DERECHA J1 ###############################//
    case 1:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_RIGHT_J1 = LOW;
        break; //break pos 0 /RIGTH/N1/J1
        
        case 1:
          for(int x = 56; x <128; x++){
            LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(56,208,24,16,0x0000);
          }
          posicion_J1 = 0;
          b_RIGHT_J1 = LOW;
        break; //break pos 1 /RIGTH/N1/J1

        case 2:
          b_RIGHT_J1 = LOW;
        break; //break pos 2 /RIGTH/N1/J1
        
        case 3:
          b_RIGHT_J1 = LOW;
        break; //break pos 3 /RIGTH/N1/J1
        
        case 4:
          for(int x = 8; x <56; x++){
            LCD_Sprite(x,192,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 192, 24, 0x0000);
          }
          posicion_J1 = 3;
          b_RIGHT_J1 = LOW;
        break; //break pos 4 /RIGTH/N1/J1
        
        case 5:
          b_RIGHT_J1 = LOW;
        break; //break pos 5 /RIGTH/N1/J1
        
        case 6:
          for(int x = 88; x <128; x++){
            LCD_Sprite(x,184,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 184, 24, 0x0000);
            FillRect(88,176,24,16,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 5;
          b_RIGHT_J1 = LOW;
        break; //break pos 6 /RIGTH/N1/J1
        
        case 7:
          b_RIGHT_J1 = LOW;
        break; //break pos 7 /RIGTH/N1/J1
        
        case 8:
          for(int x = 88; x <128; x++){
            LCD_Sprite(x,160,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 160, 24, 0x0000);
            FillRect(88,176,24,16,0x0000);
          }
          
          posicion_J1 = 7;
          b_RIGHT_J1 = LOW;
        break; //break pos 8 /RIGTH/N1/J1
        
        case 9:
          for(int x = 40; x <128; x++){
            LCD_Sprite(x,160,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 160, 24, 0x0000);
          }
          
          posicion_J1 = 7;
          b_RIGHT_J1 = LOW;
        break; //break pos 9 /RIGTH/N1/J1
        
        case 10:
          b_RIGHT_J1 = LOW;
        break; //break pos 10 /RIGTH/N1/J1
        
        case 11:
          for(int x = 96; x <128; x++){
              LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 128, 24, 0x0000);
              FillRect(96,120,24,16,0x0000);
            }
            posicion_J1 = 10;
            b_RIGHT_J1 = LOW;
        
        break; //break pos 11 /RIGTH/N1/J1
        
        case 12:
        for(int x = 40; x <128; x++){
            LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 128, 24, 0x0000);
            FillRect(40,120,24,16,0x0000);
          }
          posicion_J1 = 10;
          b_RIGHT_J1 = LOW;
        break; //break pos 12 /RIGTH/N1/J1
        
        case 13:
          for(int x = 8; x <128; x++){
            LCD_Sprite(x,128,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 128, 24, 0x0000);
          }
          posicion_J1 = 10;
          b_RIGHT_J1 = LOW;
        break; //break pos 13 /RIGTH/N1/J1
        
        case 14:
          b_RIGHT_J1 = LOW;
        break; //break pos 14 /RIGTH/N1/J1
        
        case 15:
          for(int x = 40; x <128; x++){
            LCD_Sprite(x,88,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 88, 24, 0x0000);
            FillRect(40,104,24,16,0x0000);
          }
          posicion_J1 = 14;  
          b_RIGHT_J1 = LOW;
        break; //break pos 15 /RIGTH/N1/J1
        
        case 16:
          b_RIGHT_J1 = LOW;
        break; //break pos 16 /RIGTH/N1/J1
        
        case 17:
          for(int x = 96; x <128; x++){
            LCD_Sprite(x,56,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 56, 24, 0x0000);
            FillRect(96,72,24,16,0x0000);
          }
          posicion_J1 = 16;
          b_RIGHT_J1 = LOW;
        break; //break pos 17 /RIGTH/N1/J1
        
        case 18:
          b_RIGHT_J1 = LOW;
        break; //break pos 18 /RIGTH/N1/J1
        
        case 19:
          for(int x = 8; x <32; x++){
            LCD_Sprite(x,56,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 56, 24, 0x0000);
            FillRect(8,72,24,16,0x0000);
          }
          posicion_J1 = 18;
          b_RIGHT_J1 = LOW;
        break; //break pos 19 /RIGTH/N1/J1
        
        case 20:
          b_RIGHT_J1 = LOW;
        break; //break pos 20 /RIGTH/N1/J1
        
        case 21:
          for(int x = 32; x <80; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(32,40,24,16,0x0000);
          }
          posicion_J1 = 20;
          b_RIGHT_J1 = LOW;
        break; //break pos 21 /RIGTH/N1/J1
        
        case 22:
          for(int x = 8; x <80; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
          }
          FillRect(8,16,24,16,0x0000);
          Rect(8,0,24,24,0xFF40); //J1
          Rect(12,4,16,16,0xFF40);
          Rect(16,8,8,8,0xFF40);
          posicion_J1 = 20;
          b_RIGHT_J1 = LOW;
        break; //break pos 22 /RIGTH/N1/J1
        
        case 23:
          b_RIGHT_J1 = LOW;
        break; //break pos 23 /RIGTH/N1/J1
        
        case 24:
          for(int x = 80; x <128; x++){
            LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(80,16,24,16,0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            }
          posicion_J1 = 23;
          b_RIGHT_J1 = LOW;
        break; //break pos 24 /RIGTH/N1/J1
        
        case 25:
          b_RIGHT_J1 = LOW;
        break; //break pos 25 /RIGTH/N1
        }
    break; //break del case 1 (nivel 1 - RIGTH - J1)

//#################### NIVEL 2 DERECHA J1 ###############################//
    case 2:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 1 
          for(int x = 8; x <48; x++){
            LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(8,208,24,16,0x0000);
          }
          posicion_J1 = 1;        
          b_RIGHT_J1 = LOW;
        break; //break pos 0 /RIGTH/N2/J1
        
        case 1:
          b_RIGHT_J1 = LOW;
        break; //break pos 1 /RIGTH/N2/J1

        case 2:
          for(int x = 88; x <128; x++){
            LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
          }
          posicion_J1 = 3;        
          b_RIGHT_J1 = LOW;
        break; //break pos 2 /RIGTH/N2/J1
        
        case 3:
          b_RIGHT_J1 = LOW;
        break; //break pos 3 /RIGTH/N2/J1
        
        case 4:
          b_RIGHT_J1 = LOW;
        break; //break pos 4 /RIGTH/N2/J1
        
        case 5:
          b_RIGHT_J1 = LOW;
        break; //break pos 5 /RIGTH/N1/J1
        
        case 6:
          for(int x = 8; x <48; x++){
            LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
          }
          FillRect(32,144,16,24,0x0000);
          if (STAR_1_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 7;
          b_RIGHT_J1 = LOW;
        break; //break pos 6 /RIGTH/N2/J1
        
        case 7:
          b_RIGHT_J1 = LOW;
        break; //break pos 7 /RIGTH/N2/J1
        
        case 8:
          for(int x = 88; x <128; x++){
            LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
            FillRect(88,136,24,16,0x0000);
          }
          posicion_J1 = 9;
          b_RIGHT_J1 = LOW;
        break; //break pos 8 /RIGTH/N2/J1
        
        case 9:
          b_RIGHT_J1 = LOW;
        break; //break pos 9 /RIGTH/N2/J1
        
        case 10:
          for(int x = 8; x <48; x++){
              LCD_Sprite(x,120,24,24,J1_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 120, 24, 0x0000);
              FillRect(8,136,24,16,0x0000);
            }
            posicion_J1 = 11;
          b_RIGHT_J1 = LOW;
        break; //break pos 10 /RIGTH/N2/J1
        
        case 11:
          b_RIGHT_J1 = LOW;
        
        break; //break pos 11 /RIGTH/N2/J1
        
        case 12:
        for(int x = 88; x <128; x++){
            LCD_Sprite(x,120,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 120, 24, 0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 13;
          b_RIGHT_J1 = LOW;
        break; //break pos 12 /RIGTH/N2/J1
        
        case 13:
          b_RIGHT_J1 = LOW;
        break; //break pos 13 /RIGTH/N2/J1
        
        case 14:
          for(int x = 8; x >48; x = x-1){
             LCD_Sprite(x,72,24,24,J1_Abajo_Derecha,1,0,1,0);
             V_line( x + 24, 72, 24, 0x0000);
            }
          FillRect(8,72, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_LEFT_J1 = LOW;
        break; //break pos 14 /RIGTH/N2/J1
        
        case 15:
          FillRect(48, 72, 24, 24, 0x0000);
          FillRect(48,88,24,16,0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;  
          b_RIGHT_J1 = LOW;
        break; //break pos 15 /RIGTH/N2/J1
        
        case 16:
          for(int x = 88; x <128; x++){
            LCD_Sprite(x,72,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 72, 24, 0x0000);
            FillRect(88,94,24,16,0x0000);
          }
          posicion_J1 = 17;
          b_RIGHT_J1 = LOW;
        break; //break pos 16 /RIGTH/N2/J1
        
        case 17:
          b_RIGHT_J1 = LOW;
        break; //break pos 17 /RIGTH/N2/J1
        
        case 18:
          for(int x = 8; x <48; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(8,40,24,16,0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            }
          posicion_J1 = 19;        
          b_RIGHT_J1 = LOW;
        break; //break pos 18 /RIGTH/N2/J1
        
        case 19:
          b_RIGHT_J1 = LOW;
        break; //break pos 19 /RIGTH/N2/J1
        
        case 20:
          for(int x = 96; x <128; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(96,16,24,16,0x0000);
          }
          posicion_J1 = 21;        
          b_RIGHT_J1 = LOW;
        break; //break pos 20 /RIGTH/N2/J1
        
        case 21:
          b_RIGHT_J1 = LOW;
        break; //break pos 21 /RIGTH/N1/J1
        
        case 22:
          for(int x = 48; x <96; x++){
            LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(48,16,24,16,0x0000);
          }
          posicion_J1 = 23;
          b_RIGHT_J1 = LOW;
        break; //break pos 22 /RIGTH/N2/J1
        
        case 23:
          b_RIGHT_J1 = LOW;
        break; //break pos 23 /RIGTH/N1/J1
        
        case 24:
          b_RIGHT_J1 = LOW;
        break; //break pos 24 /RIGTH/N1/J1
      }
    break;//break del case 2 (nivel 2 - RIGTH - J1)
    
//#################### NIVEL 3 DERECHA J1 ###############################//
    case 3:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 1        
          b_RIGHT_J1 = LOW;
        break; //break pos 0 /RIGTH/N3/J1
        
        case 1:
          for(int x = 40; x <128; x++){
            LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(40,208,24,16,0x0000);
          }
          posicion_J1 = 0;         
          b_RIGHT_J1 = LOW;
        break; //break pos 1 /RIGTH/N3/J1

        case 2:
          for(int x = 8; x <128; x++){
            LCD_Sprite(x,216,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(8,215,24,16,0x0000);
          }
          posicion_J1 = 0;        
          b_RIGHT_J1 = LOW;
        break; //break pos 2 /RIGTH/N3/J1
        
        case 3:
          b_RIGHT_J1 = LOW;
        break; //break pos 3 /RIGTH/N3/J1
        
        case 4:
          for(int x = 8; x <40; x++){
            LCD_Sprite(x,192,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 192, 24, 0x0000);
            FillRect(8,208,24,17,0x0000);
          }
          posicion_J1 = 3;        
          b_RIGHT_J1 = LOW;
        break; //break pos 4 /RIGTH/N3/J1
        
        case 5:
          FillRect(128, 184, 24, 24, 0x0000);
          FillRect(128,175,24,16,0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);
          posicion_J1 = 0;                  
          b_RIGHT_J1 = LOW;
        break; //break pos 5 /RIGTH/N3/J1
        
        case 6:
          for(int x = 72; x <128; x++){
            LCD_Sprite(x,184,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 184, 24, 0x0000);
            FillRect(72,175,24,16,0x0000);
            FillRect(120,184,16,24,0x0000);
          }
          FillRect(128, 184, 24, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);           
          posicion_J1 = 0;
          b_RIGHT_J1 = LOW;
        break; //break pos 6 /RIGTH/N3/J1
        
        case 7:
          b_RIGHT_J1 = LOW;
        break; //break pos 7 /RIGTH/N3/J1
        
        case 8:
          for(int x = 8; x <128; x++){
            LCD_Sprite(x,144,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
            FillRect(8,136,24,16,0x0000);
          }
          posicion_J1 = 7;
          b_RIGHT_J1 = LOW;
        break; //break pos 8 /RIGTH/N3/J1
        
        case 9:
          FillRect(120, 104, 32, 32, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);           
          posicion_J1 = 0;        
          b_RIGHT_J1 = LOW;
        break; //break pos 9 /RIGTH/N3/J1
        
        case 10:
          for(int x = 104; x <128; x++){
              LCD_Sprite(x,112,24,24,J1_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 112, 24, 0x0000);
            }
          FillRect(120, 112, 35, 24, 0x0000);
          LCD_Sprite(128,216,24,24,J1_Abajo_Derecha,1,0,1,0);           
          posicion_J1 = 0;
          b_RIGHT_J1 = LOW;
        break; //break pos 10 /RIGTH/N3/J1
        
        case 11:
          b_RIGHT_J1 = LOW;
        break; //break pos 11 /RIGTH/N2/J1
        
        case 12:
        for(int x = 72; x <128; x++){
            LCD_Sprite(x,64,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 64, 24, 0x0000);
            FillRect(72,80,24,16,0x0000);
          }
          posicion_J1 = 11;
          b_RIGHT_J1 = LOW;
        break; //break pos 12 /RIGTH/N3/J1
        
        case 13:
        for(int x = 40; x <128; x++){
            LCD_Sprite(x,64,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 64, 24, 0x0000);
          }
          posicion_J1 = 11;        
          b_RIGHT_J1 = LOW;
        break; //break pos 13 /RIGTH/N3/J1
        
        case 14:
          b_LEFT_J1 = LOW;
        break; //break pos 14 /RIGTH/N3/J1
        
        case 15:
          for(int x = 104; x <128; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(104,40,24,16,0x0000);
          }
          posicion_J1 = 14; 
          b_RIGHT_J1 = LOW;
        break; //break pos 15 /RIGTH/N3/J1
        
        case 16:
          for(int x = 72; x <128; x++){
            LCD_Sprite(x,24,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
          }
          posicion_J1 = 14; 
          b_RIGHT_J1 = LOW;
        break; //break pos 16 /RIGTH/N3/J1
        
        case 17:
          b_RIGHT_J1 = LOW;
        break; //break pos 17 /RIGTH/N3/J1
        
        case 18:       
          b_RIGHT_J1 = LOW;
        break; //break pos 18 /RIGTH/N3/J1
        
        case 19:
          b_RIGHT_J1 = LOW;
        break; //break pos 19 /RIGTH/N3/J1
        
        case 20:
          for(int x = 8; x <40; x++){
            LCD_Sprite(x,0,24,24,J1_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(8,16,24,16,0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            } 
          posicion_J1 = 19;        
          b_RIGHT_J1 = LOW;
        break; //break pos 20 /RIGTH/N3/J1
      }
    break;//break del case 3 (nivel 3 - RIGTH - J1)
    }
}

   //***************************************************************************************************************************************
// Función cuando se presiona el boton derecho del J2 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_RIGHT_J2(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 DERECHA J2 ###############################//
    case 1:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_RIGHT_J2 = LOW;
        break; //break pos 0 /RIGTH/N1/J2
        
        case 1:
          for(int x = 216; x <288; x++){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(56+160,208,24,16,0x0000);
          }
          posicion_J2 = 0;
          b_RIGHT_J2 = LOW;
        break; //break pos 1 /RIGTH/N1/J2

        case 2:
          b_RIGHT_J2 = LOW;
        break; //break pos 2 /RIGTH/N1/J2
        
        case 3:
          b_RIGHT_J2 = LOW;
        break; //break pos 3 /RIGTH/N1/J2
        
        case 4:
          for(int x = 168; x <216; x++){
            LCD_Sprite(x,192,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 192, 24, 0x0000);
          }
          posicion_J2 = 3;
          b_RIGHT_J2 = LOW;
        break; //break pos 4 /RIGTH/N1/J2
        
        case 5:
          b_RIGHT_J2 = LOW;
        break; //break pos 5 /RIGTH/N1/J2
        
        case 6:
          for(int x = 248; x <288; x++){
            LCD_Sprite(x,184,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 184, 24, 0x0000);
            FillRect(88+160,176,24,16,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 5;
          b_RIGHT_J2 = LOW;
        break; //break pos 6 /RIGTH/N1/J2
        
        case 7:
          b_RIGHT_J2 = LOW;
        break; //break pos 7 /RIGTH/N1/J2
        
        case 8:
          for(int x = 248; x <288; x++){
            LCD_Sprite(x,160,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 160, 24, 0x0000);
            FillRect(88+160,176,24,16,0x0000);
          }
          posicion_J2 = 7;
          b_RIGHT_J2 = LOW;
        break; //break pos 8 /RIGTH/N1/J2
        
        case 9:
          for(int x = 200; x <288; x++){
            LCD_Sprite(x,160,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 160, 24, 0x0000);
          }
          posicion_J2 = 7;
          b_RIGHT_J2 = LOW;
        break; //break pos 9 /RIGTH/N1/J2
        
        case 10:
        b_RIGHT_J2 = LOW;
        break; //break pos 10 /RIGTH/N1/J2
        
        case 11:
         for(int x = 256; x <288; x++){
            LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 128, 24, 0x0000);
            FillRect(96+160,120,24,16,0x0000);
          }
          posicion_J2 = 10;
          b_RIGHT_J2 = LOW;
        break; //break pos 11 /RIGTH/N1/J2
        
        case 12:
          for(int x = 200; x <288; x++){
            LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 128, 24, 0x0000);
            FillRect(40+160,120,24,16,0x0000);
          }
          posicion_J2 = 10;
          b_RIGHT_J2 = LOW;
        break; //break pos 12 /RIGTH/N1/J2
        
        case 13:
          for(int x = 168; x <288; x++){
            LCD_Sprite(x,128,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 128, 24, 0x0000);
          }
          posicion_J2 = 10;
          b_RIGHT_J2 = LOW;
        break; //break pos 13 /RIGTH/N1/J2
        
        case 14:
          b_RIGHT_J2 = LOW;
        break; //break pos 14 /RIGTH/N1/J2
        
        case 15:
          for(int x = 200; x <288; x++){
            LCD_Sprite(x,88,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 88, 24, 0x0000);
            FillRect(40+160,104,24,16,0x0000);
          }
          posicion_J2 = 14; 
          b_RIGHT_J2 = LOW;
        break; //break pos 15 /RIGTH/N1/J2
        
        case 16:
          b_RIGHT_J2 = LOW;
        break; //break pos 16 /RIGTH/N1/J2
        
        case 17:
          for(int x = 256; x <288; x++){
            LCD_Sprite(x,56,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 56, 24, 0x0000);
            FillRect(96,72,24,16,0x0000);
          }
          posicion_J2 = 16;
          b_RIGHT_J2 = LOW;
        break; //break pos 17 /RIGTH/N1/J2
        
        case 18:
          b_RIGHT_J2 = LOW;
        break; //break pos 18 /RIGTH/N1/J2
        
        case 19:
          for(int x = 168; x <192; x++){
            LCD_Sprite(x,56,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 56, 24, 0x0000);
            FillRect(8+160,72,24,16,0x0000);
          }
          posicion_J2 = 18;
          b_RIGHT_J2 = LOW;
        break; //break pos 19 /RIGTH/N1/J2
        
        case 20:
          b_RIGHT_J2 = LOW;
        break; //break pos 20 /RIGTH/N1/J2
        
        case 21:
          for(int x = 192; x <240; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(32+160,40,24,16,0x0000);
          }
          posicion_J2 = 20;
          b_RIGHT_J2 = LOW;
        break; //break pos 21 /RIGTH/N1/J2
        
        case 22:
          for(int x = 168; x <240; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);  
          }
          FillRect(8+160,16,24,16,0x0000);
          Rect(8+160,0,24,24,0xFF40); //J1
          Rect(12+160,4,16,16,0xFF40);
          Rect(16+160,8,8,8,0xFF40);
          posicion_J2 = 20;
          b_RIGHT_J2 = LOW;
        break; //break pos 22 /RIGTH/N1/J2
        
        case 23:
          b_RIGHT_J2 = LOW;
        break; //break pos 23 /RIGTH/N1/J2
    
        case 24:
          for(int x = 240; x <288; x++){
            LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(80+160,16,24,16,0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            }
          posicion_J2 = 23;
          b_RIGHT_J2 = LOW;
        break; //break pos 24 /RIGTH/N1/J2
        
        case 25:
          b_RIGHT_J2 = LOW;
        break; //break pos 25 /RIGTH/N1/J2
        }
    break;//break del case 1 (nivel 1 - RIGTH - J2)

//#################### NIVEL 2 DERECHA J2 ###############################//
    case 2:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 2 
          for(int x = 8+160; x <48+160; x++){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(8+160,208,24,16,0x0000);
          }
          posicion_J2 = 1;        
          b_RIGHT_J2 = LOW;
        break; //break pos 0 /RIGTH/N2/J2
        
        case 1:
          b_RIGHT_J2 = LOW;
        break; //break pos 1 /RIGTH/N2/J2

        case 2:
          for(int x = 88+160; x <128+160; x++){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
          }
          posicion_J2 = 3;        
          b_RIGHT_J2 = LOW;
        break; //break pos 2 /RIGTH/N2/J2
        
        case 3:
          b_RIGHT_J2 = LOW;
        break; //break pos 3 /RIGTH/N2/J2
        
        case 4:
          b_RIGHT_J2 = LOW;
        break; //break pos 4 /RIGTH/N2/J2
        
        case 5:
          b_RIGHT_J2 = LOW;
        break; //break pos 5 /RIGTH/N1/J2
        
        case 6:
          for(int x = 8+160; x <48+160; x++){
            LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
          }
          FillRect(32+160,144,16,24,0x0000);
          if (STAR_1_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 7;
          b_RIGHT_J2 = LOW;
        break; //break pos 6 /RIGTH/N2/J2
        
        case 7:
          b_RIGHT_J2 = LOW;
        break; //break pos 7 /RIGTH/N2/J2
        
        case 8:
          for(int x = 88+160; x <128+160; x++){
            LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
            FillRect(88+160,136,24,16,0x0000);
          }
          posicion_J2 = 9;
          b_RIGHT_J2 = LOW;
        break; //break pos 8 /RIGTH/N2/J2
        
        case 9:
          b_RIGHT_J2 = LOW;
        break; //break pos 9 /RIGTH/N2/J2
        
        case 10:
          for(int x = 8+160; x <48+160; x++){
              LCD_Sprite(x,120,24,24,J2_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 120, 24, 0x0000);
              FillRect(8+160,136,24,16,0x0000);
            }
            posicion_J2 = 11;
          b_RIGHT_J2 = LOW;
        break; //break pos 10 /RIGTH/N2/J2
        
        case 11:
          b_RIGHT_J2 = LOW;
        
        break; //break pos 11 /RIGTH/N2/J2
        
        case 12:
        for(int x = 88+160; x <128+160; x++){
            LCD_Sprite(x,120,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 120, 24, 0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 13;
          b_RIGHT_J2 = LOW;
        break; //break pos 12 /RIGTH/N2/J2
        
        case 13:
          b_RIGHT_J2 = LOW;
        break; //break pos 13 /RIGTH/N2/J2
        
        case 14:
          for(int x = 8+160; x >48+160; x = x-1){
            LCD_Sprite(x,72,24,24,J2_Abajo_Derecha,1,0,1,0);
            V_line( x + 24, 72, 24, 0x0000);
          }
          FillRect(8+160,72, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;
          b_LEFT_J2 = LOW;
        break; //break pos 14 /RIGTH/N2/J2
        
        case 15:
          FillRect(48+160, 72, 24, 24, 0x0000);
          FillRect(48+160,88,24,16,0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;  
          b_RIGHT_J2 = LOW;
        break; //break pos 15 /RIGTH/N2/J2
        
        case 16:
          for(int x = 88+160; x <128+160; x++){
            LCD_Sprite(x,72,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 72, 24, 0x0000);
            FillRect(88+160,94,24,16,0x0000);
          }
          posicion_J2 = 17;
          b_RIGHT_J2 = LOW;
        break; //break pos 16 /RIGTH/N2/J2
        
        case 17:
          b_RIGHT_J2 = LOW;
        break; //break pos 17 /RIGTH/N2/J2
        
        case 18:
          for(int x = 8+160; x <48+160; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(8+160,40,24,16,0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            }
          posicion_J2 = 19;        
          b_RIGHT_J2 = LOW;
        break; //break pos 18 /RIGTH/N2/J2
        
        case 19:
          b_RIGHT_J2 = LOW;
        break; //break pos 19 /RIGTH/N2/J2
        
        case 20:
          for(int x = 96+160; x <128+160; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(96+160,16,24,16,0x0000);
          }
          posicion_J2 = 21;        
          b_RIGHT_J2 = LOW;
        break; //break pos 20 /RIGTH/N2/J2
        
        case 21:
          b_RIGHT_J2 = LOW;
        break; //break pos 21 /RIGTH/N1/J2
        
        case 22:
          for(int x = 48+160; x <96+160; x++){
            LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(48+160,16,24,16,0x0000);
          }
          posicion_J2 = 23;
          b_RIGHT_J2 = LOW;
        break; //break pos 22 /RIGTH/N2/J2
        
        case 23:
          b_RIGHT_J2 = LOW;
        break; //break pos 23 /RIGTH/N1/J2
        
        case 24:
          b_RIGHT_J2 = LOW;
        break; //break pos 24 /RIGTH/N1/J2
      }
    break;//break del case 2 (nivel 2 - RIGTH - J2)
//#################### NIVEL 3 DERECHA J2 ###############################//
    case 3:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 2       
          b_RIGHT_J2 = LOW;
        break; //break pos 0 /RIGTH/N3/J2
        
        case 1:
          for(int x = 40+160; x <128+160; x++){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(40+160,208,24,16,0x0000);
          }
          posicion_J2 = 0;         
          b_RIGHT_J2 = LOW;
        break; //break pos 1 /RIGTH/N3/J2

        case 2:
          for(int x = 8+160; x <128+160; x++){
            LCD_Sprite(x,216,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 216, 24, 0x0000);
            FillRect(8+160,215,24,16,0x0000);
          }
          posicion_J2 = 0;        
          b_RIGHT_J2 = LOW;
        break; //break pos 2 /RIGTH/N3/J2
        
        case 3:
          b_RIGHT_J2 = LOW;
        break; //break pos 3 /RIGTH/N3/J2
        
        case 4:
          for(int x = 8+160; x <40+160; x++){
            LCD_Sprite(x,192,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 192, 24, 0x0000);
            FillRect(8+160,208,24,17,0x0000);
          }
          posicion_J2 = 3;        
          b_RIGHT_J2 = LOW;
        break; //break pos 4 /RIGTH/N3/J2
        
        case 5:
          FillRect(128+160, 184, 24, 24, 0x0000);
          FillRect(128+160,175,24,16,0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);
          posicion_J2 = 0;                  
          b_RIGHT_J2 = LOW;
        break; //break pos 5 /RIGTH/N3/J2
        
        case 6:
          for(int x = 72+160; x <128+160; x++){
            LCD_Sprite(x,184,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 184, 24, 0x0000);
            FillRect(72+160,175,24,16,0x0000);
            FillRect(120+160,184,16,24,0x0000);
          }
          FillRect(128+160, 184, 24, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);           
          posicion_J2 = 0;
          b_RIGHT_J2 = LOW;
        break; //break pos 6 /RIGTH/N3/J2
        
        case 7:
          b_RIGHT_J2 = LOW;
        break; //break pos 7 /RIGTH/N3/J2
        
        case 8:
          for(int x = 8+160; x <128+160; x++){
            LCD_Sprite(x,144,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 144, 24, 0x0000);
            FillRect(8+160,136,24,16,0x0000);
          }
          posicion_J2 = 7;
          b_RIGHT_J2 = LOW;
        break; //break pos 8 /RIGTH/N3/J2
        
        case 9:
          FillRect(120+160, 104, 32, 32, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);           
          posicion_J2 = 0;        
          b_RIGHT_J2 = LOW;
        break; //break pos 9 /RIGTH/N3/J2
        
        case 10:
          for(int x = 104+160; x <128+160; x++){
              LCD_Sprite(x,112,24,24,J2_Abajo_Derecha,1,0,0,0);
              V_line( x -1, 112, 24, 0x0000);
            }
          FillRect(120+160, 112, 35, 24, 0x0000);
          LCD_Sprite(128+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);           
          posicion_J2 = 0;
          b_RIGHT_J2 = LOW;
        break; //break pos 10 /RIGTH/N3/J2
        
        case 11:
          b_RIGHT_J2 = LOW;
        break; //break pos 11 /RIGTH/N2/J2
        
        case 12:
        for(int x = 72+160; x <128+160; x++){
            LCD_Sprite(x,64,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 64, 24, 0x0000);
            FillRect(72+160,80,24,16,0x0000);
          }
          posicion_J2 = 11;
          b_RIGHT_J2 = LOW;
        break; //break pos 12 /RIGTH/N3/J2
        
        case 13:
        for(int x = 40+160; x <128+160; x++){
            LCD_Sprite(x,64,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 64, 24, 0x0000);
          }
          posicion_J2 = 11;        
          b_RIGHT_J2 = LOW;
        break; //break pos 13 /RIGTH/N3/J2
        
        case 14:
          b_LEFT_J2 = LOW;
        break; //break pos 14 /RIGTH/N3/J2
        
        case 15:
          for(int x = 104+160; x <128+160; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
            FillRect(104+160,40,24,16,0x0000);
          }
          posicion_J2 = 14; 
          b_RIGHT_J2 = LOW;
        break; //break pos 15 /RIGTH/N3/J2
        
        case 16:
          for(int x = 72+160; x <128+160; x++){
            LCD_Sprite(x,24,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 24, 24, 0x0000);
          }
          posicion_J2 = 14; 
          b_RIGHT_J2 = LOW;
        break; //break pos 16 /RIGTH/N3/J2
        
        case 17:
          b_RIGHT_J2 = LOW;
        break; //break pos 17 /RIGTH/N3/J2
        
        case 18:       
          b_RIGHT_J2 = LOW;
        break; //break pos 18 /RIGTH/N3/J2
        
        case 19:
          b_RIGHT_J2 = LOW;
        break; //break pos 19 /RIGTH/N3/J2
        
        case 20:
          for(int x = 8+160; x <40+160; x++){
            LCD_Sprite(x,0,24,24,J2_Abajo_Derecha,1,0,0,0);
            V_line( x -1, 0, 24, 0x0000);
            FillRect(8+160,16,24,16,0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            } 
          posicion_J2 = 19;        
          b_RIGHT_J2 = LOW;
        break; //break pos 20 /RIGTH/N3/J2
      }
    break;//break del case 3 (nivel 3 - RIGTH - J2)
    }  
} 
      
//***************************************************************************************************************************************
// Función cuando se presiona el boton arriba del J1 y revisa la posicion y se mueve 
//***************************************************************************************************************************************
void switch_posicion_UP_J1(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 ARRIBA J1 ###############################//
    case 1:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1
          b_UP_J1 = LOW; 
        break; //break pos 0 /UP/N1/J1
        
        case 1:
          for(int y = 216; y >192; y--){
            LCD_Sprite(56,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 56, y+25, 24, 0x0000);
          }
          posicion_J1 = 3;
          b_UP_J1 = LOW;
        break; //break pos 1 /UP/N1/J1

        case 2:
          for(int y = 216; y >56; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          posicion_J1 = 19;
          b_UP_J1 = LOW;
        break; //break pos 2 /UP/N1/J1
        
        case 3:
          b_UP_J1 = LOW;
        break; //break pos 3 /UP/N1/J1
        
        case 4:
          for(int y = 192; y >56; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          posicion_J1 = 19;
          b_UP_J1 = LOW;
        break; //break pos 4 /UP/N1/J1
        
        case 5:
          for(int y = 184; y >88; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
          }
          posicion_J1 = 14;
          b_UP_J1 = LOW;
        break; //break pos 5 /UP/N1/J1
        
        case 6:
          for(int y = 184; y >160; y--){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y+25, 24, 0x0000);
          }
          posicion_J1 = 8;
          b_UP_J1 = LOW;
        break; //break pos 6 /UP/N1/J1
        
        case 7:
          for(int y = 160; y >88; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,160,16,24,0x0000);
          }
          posicion_J1 = 14;
          b_UP_J1 = LOW;
        break; //break pos 7 /UP/N1/J1
        
        case 8:
          b_UP_J1 = LOW;
        break; //break pos 8 /UP/N1/J1
        
        case 9:
          b_UP_J1 = LOW;
        break; //break pos 9 /UP/N1/J1
        
        case 10:
          for(int y = 128; y >88; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,128,16,24,0x0000);
          }
          posicion_J1 = 14;
          b_UP_J1 = LOW;
        break; //break pos 10 /UP/N1/J1
        
        case 11:
          for(int y = 128; y >56; y--){
            LCD_Sprite(96,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 96, y+25, 24, 0x0000);
          }
          posicion_J1 = 17;
          b_UP_J1 = LOW;
        break; //break pos 11 /UP/N1/J1
        
        case 12:
          for(int y = 128; y >88; y--){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 40, y+25, 24, 0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            }
          posicion_J1 = 15;
          b_UP_J1 = LOW;
        break; //break pos 12 /UP/N1/J1
        
        case 13:
          for(int y = 128; y >56; y--){
              LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
              H_line( 8, y+25, 24, 0x0000);
            }
            posicion_J1 = 19;
            b_UP_J1 = LOW;
        break; //break pos 13 /UP/N1/J1
        
        case 14:
          b_UP_J1 = LOW;
        break; //break pos 14 /UP/N1/J1
        
        case 15:
          b_UP_J1 = LOW;
        break; //break pos 15 /UP/N1/J1
        
        case 16:
          for(int y = 56; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,56,16,24,0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            }
          posicion_J1 = 23;
          b_UP_J1 = LOW;
        break; //break pos 16 /UP/N1/J1
        
        case 17:
          b_UP_J1 = LOW;
        break; //break pos 17 /UP/N1/J1
        
        case 18:
          for(int y = 56; y >24; y--){
            LCD_Sprite(32,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 32, y+25, 24, 0x0000);
            FillRect(24,56,16,24,0x0000);
          }
          posicion_J1 = 21;
          b_UP_J1 = LOW;
        break; //break pos 18 /UP/N1/J1
        
        case 19:
          b_UP_J1 = LOW;
        break; //break pos 19 /UP/N1/J1
        
        case 20:
          for(int y = 24; y >0; y--){
            LCD_Sprite(80,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 80, y+25, 24, 0x0000);
            FillRect(72,24,16,24,0x0000);
          }
          posicion_J1 = 24;
          b_UP_J1 = LOW;
        break; //break pos 20 /UP/N1/J1
        
        case 21:
          b_UP_J1 = LOW;
        break; //break pos 21 /UP/N1/J1
        
        case 22:
          for(int y = 24; y >0; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          if (STARS_N1_J1 == 5){
            ganar_N1 = HIGH;
            Juegos_ganados_J1++;
            nivel = 2;
            }
          posicion_J1 = 25;
          b_UP_J1 = LOW;
        break; //break pos 22 /UP/N1/J1
        
        case 23:
          b_UP_J1 = LOW;
        break; //break pos 23 /UP/N1/J1
        
        case 24:
          b_UP_J1 = LOW;
        break; //break pos 24 /UP/N1/J1
        
        case 25:
          b_UP_J1 = LOW;
        break; //break pos 25 /UP/N1/J1
        }
    break;//break del case 1 (nivel 1 - UP - J1)

//#################### NIVEL 2 ARRIBA J1 ###############################//
    case 2:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 1
          for(int y = 216; y >120; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          posicion_J1 = 10;        
          b_UP_J1 = LOW; 
        break; //break pos 0 /UP/N2/J1
        
        case 1:
          for(int y = 216; y >192; y--){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y+25, 24, 0x0000);
            FillRect(48,208,24,16,0x0000);
            FillRect(40,216,16,24,0x0000);
          }
          FillRect(48, 192, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_UP_J1 = LOW;
        break; //break pos 1 /UP/N2/J1

        case 2:
          for(int y = 216; y >192; y--){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y+25, 24, 0x0000);
            FillRect(88,208,24,16,0x0000);
          }
          FillRect(88, 192, 24, 24, 0x0000);
          LCD_Sprite(8,216,24,24,J1_Abajo_Derecha,1,0,0,0);
          posicion_J1 = 0;
          b_UP_J1 = LOW;
        break; //break pos 2 /UP/N2/J1
        
        case 3:
          for(int y = 216; y >120; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,216,16,24,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 13;        
          b_UP_J1 = LOW;
        break; //break pos 3 /UP/N2/J1
        
        case 4:
          b_UP_J1 = LOW;
        break; //break pos 4 /UP/N2/J1
        
        case 5:
          b_UP_J1 = LOW;
        break; //break pos 5 /UP/N2/J1
        
        case 6:
          for(int y = 144; y >120; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          posicion_J1 = 10;
          b_UP_J1 = LOW;
        break; //break pos 6 /UP/N2/J1
        
        case 7:
          for(int y = 144; y >72; y--){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y+25, 24, 0x0000);
          }
          posicion_J1 = 15;
          b_UP_J1 = LOW;
        break; //break pos 7 /UP/N2/J1
        
        case 8:
          for(int y = 144; y >72; y--){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y+25, 24, 0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            }
          posicion_J1 = 16;        
          b_UP_J1 = LOW;
        break; //break pos 8 /UP/N2/J1
        
        case 9:
          for(int y = 144; y >120; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,144,16,24,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 13;        
          b_UP_J1 = LOW;
        break; //break pos 9 /UP/N2/J1
        
        case 10:
          b_UP_J1 = LOW;
        break; //break pos 10 /UP/N1/J1
        
        case 11:
          for(int y = 120; y >72; y--){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y+25, 24, 0x0000);
            FillRect(40,120,16,24,0x0000);
          }
          posicion_J1 = 15;
          b_UP_J1 = LOW;
        break; //break pos 11 /UP/N2/J1
        
        case 12:
          for(int y = 120; y >72; y--){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y+25, 24, 0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            }
          posicion_J1 = 16;
          b_UP_J1 = LOW;
        break; //break pos 12 /UP/N2/J1
        
        case 13:
            b_UP_J1 = LOW;
        break; //break pos 13 /UP/N1/J1
        
        case 14:
          for(int y = 72; y >24; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y+25, 24, 0x0000);
          }
          posicion_J1 = 18;        
          b_UP_J1 = LOW;
        break; //break pos 14 /UP/N2/J1
        
        case 15:
          b_UP_J1 = LOW;
        break; //break pos 15 /UP/N1/J1
        
        case 16:
          b_UP_J1 = LOW;
        break; //break pos 16 /UP/N1/J1
        
        case 17:
          for(int y = 72; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,72,16,24,0x0000);
          }
          if (STARS_N2_J1 == 5){
            ganar_N2 = HIGH;
            Juegos_ganados_J1++;
            nivel = 3;
            }
          posicion_J1 = 24;        
          b_UP_J1 = LOW;
        break; //break pos 17 /UP/N2/J1
        
        case 18:
          b_UP_J1 = LOW;
        break; //break pos 18 /UP/N2/J1
        
        case 19:
          for(int y = 24; y >0; y--){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y+25, 24, 0x0000);
            FillRect(40,24,16,24,0x0000);
          }
          posicion_J1 = 22;        
          b_UP_J1 = LOW;
        break; //break pos 19 /UP/N2/J1
        
        case 20:
          for(int y = 24; y >0; y--){
            LCD_Sprite(96,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 96, y+25, 24, 0x0000);
          }
          posicion_J1 = 23;
          b_UP_J1 = LOW;
        break; //break pos 20 /UP/N2/J1
        
        case 21:
          for(int y = 24; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
            FillRect(120,24,16,24,0x0000);
          }
          if (STARS_N2_J1 == 5){
            ganar_N2 = HIGH;
            Juegos_ganados_J1++;
            nivel = 3;
            }
          posicion_J1 = 24;        
          b_UP_J1 = LOW;
        break; //break pos 21 /UP/N2/J1
        
        case 22:
          b_UP_J1 = LOW;
        break; //break pos 22 /UP/N2/J1
        
        case 23:
          b_UP_J1 = LOW;
        break; //break pos 23 /UP/N2/J1
        
        case 24:
          b_UP_J1 = LOW;
        break; //break pos 24 /UP/N2/J1
        }
    break;//break del case 2 (nivel 2 - UP - J1)
//#################### NIVEL 3 ARRIBA J1 ###############################//
    case 3:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 1      
          b_UP_J1 = LOW; 
        break; //break pos 0 /UP/N3/J1
        
        case 1:
          for(int y = 216; y >0; y--){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(40, y+25, 24, 0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            } 
          posicion_J1 = 19;
          b_UP_J1 = LOW;
        break; //break pos 1 /UP/N3/J1

        case 2:
          for(int y = 216; y >192; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(8, y+25, 24, 0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            } 
          posicion_J1 = 4;
          b_UP_J1 = LOW;
        break; //break pos 2 /UP/N3/J1
        
        case 3:
          for(int y = 192; y >0; y--){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 40, y+25, 24, 0x0000);
            FillRect(32,192,16,24,0x0000);
          }
          if (STAR_3_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_3_J1 = HIGH;
            } 
          posicion_J1 = 19;        
          b_UP_J1 = LOW;
        break; //break pos 3 /UP/N3/J1
        
        case 4:
          b_UP_J1 = LOW;
        break; //break pos 4 /UP/N3/J1
        
        case 5:
          for(int y = 184; y >144; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y+25, 24, 0x0000);
          }
          posicion_J1 = 7;        
          b_UP_J1 = LOW;
        break; //break pos 5 /UP/N3/J1
        
        case 6:
          for(int y = 184; y >64; y--){
            LCD_Sprite(72,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(72, y+25, 24, 0x0000);
          }
          posicion_J1 = 12;
          b_UP_J1 = LOW;
        break; //break pos 6 /UP/N3/J1
        
        case 7:
          b_UP_J1 = LOW;
        break; //break pos 7 /UP/N2/J1
        
        case 8:
          for(int y = 144; y >0; y--){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(8, y+25, 24, 0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            } 
          posicion_J1 = 20;        
          b_UP_J1 = LOW;
        break; //break pos 8 /UP/N3/J1
        
        case 9:
          for(int y = 112; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y+25, 24, 0x0000);
          }
          if (STAR_5_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_5_J1 = HIGH;
            } 
          posicion_J1 = 17;        
          b_UP_J1 = LOW;
        break; //break pos 9 /UP/N3/J1
        
        case 10:
          for(int y = 112; y >24; y--){
            LCD_Sprite(104,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(104, y+25, 24, 0x0000);
          }
          posicion_J1 = 15;         
          b_UP_J1 = LOW;
        break; //break pos 10 /UP/N3/J1
        
        case 11:
          for(int y = 64; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y+25, 24, 0x0000);
            FillRect(120,64,16,24,0x0000);
          }
          if (STAR_5_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_5_J1 = HIGH;
            }
          posicion_J1 = 17;
          b_UP_J1 = LOW;
        break; //break pos 11 /UP/N3/J1
        
        case 12:
          b_UP_J1 = LOW;
        break; //break pos 12 /UP/N3/J1
        
        case 13:
          for(int y = 64; y >0; y--){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(40, y+25, 24, 0x0000);
          }
          posicion_J1 = 19;             
            b_UP_J1 = LOW;
        break; //break pos 13 /UP/N3/J1
        
        case 14:
          for(int y = 24; y >0; y--){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y+25, 24, 0x0000);
            FillRect(120,24,16,24,0x0000);
          }
          if (STAR_5_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_5_J1 = HIGH;
            }
          posicion_J1 = 17;        
          b_UP_J1 = LOW;
        break; //break pos 14 /UP/N3/J1
        
        case 15:
          b_UP_J1 = LOW;
        break; //break pos 15 /UP/N3/J1
        
        case 16:
          for(int y = 24; y >0; y--){
            LCD_Sprite(72,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(72, y+25, 24, 0x0000);
          }
          if (STARS_N3_J1 == 5){
            ganar_N3 = HIGH;
            Juegos_ganados_J1++;
            nivel = 4;
            }
          posicion_J1 = 18;         
          b_UP_J1 = LOW;
        break; //break pos 16 /UP/N3/J1
        
        case 17:        
          b_UP_J1 = LOW;
        break; //break pos 17 /UP/N3/J1
        
        case 18:
          b_UP_J1 = LOW;
        break; //break pos 18 /UP/N3/J1
        
        case 19:    
          b_UP_J1 = LOW;
        break; //break pos 19 /UP/N3/J1
        
        case 20:
          b_UP_J1 = LOW;
        break; //break pos 20 /UP/N3/J1
        }
    break;//break del case 3 (nivel 3 - UP - J1)
    }  
}

//***************************************************************************************************************************************
// Función cuando se presiona el boton arriba del J2 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_UP_J2(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 ARRIBA J2 ###############################//
    case 1:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 2 
          b_UP_J2 = LOW;
        break; //break pos 0 /UP/N1/J2
        
        case 1:
          for(int y = 216; y >192; y--){
            LCD_Sprite(56+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 56+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 3;
          b_UP_J2 = LOW;
        break; //break pos 1 /UP/N1/J2

        case 2:
          for(int y = 216; y >56; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 19;
          b_UP_J2 = LOW;
        break; //break pos 2 /UP/N1/J2
        
        case 3:
          b_UP_J2 = LOW;
        break; //break pos 3 /UP/N1/J2
        
        case 4:
          for(int y = 192; y >56; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 19;
          b_UP_J2 = LOW;
        break; //break pos 4 /UP/N1/J2
        
        case 5:
          for(int y = 184; y >88; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 14;
          b_UP_J2 = LOW;
        break; //break pos 5 /UP/N1/J2
        
        case 6:
          for(int y = 184; y >160; y--){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 8;
          b_UP_J2 = LOW;
        break; //break pos 6 /UP/N1/J2
        
        case 7:
          for(int y = 160; y >88; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,160,16,24,0x0000);
          }
          posicion_J2 = 14;
          b_UP_J2 = LOW;
        break; //break pos 7 /UP/N1/J2
        
        case 8:
          b_UP_J2 = LOW;
        break; //break pos 8 /UP/N1/J2
        
        case 9:
          b_UP_J2 = LOW;
        break; //break pos 9 /UP/N1/J2
        
        case 10:
          for(int y = 128; y >88; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,128,16,24,0x0000);
          }
          posicion_J2 = 14;
          b_UP_J2 = LOW;
        break; //break pos 10 /UP/N1/J2
        
        case 11:
          for(int y = 128; y >56; y--){
            LCD_Sprite(96+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 96+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 17;
          b_UP_J2 = LOW;
        break; //break pos 11 /UP/N1/J2
        
        case 12:
          for(int y = 128; y >88; y--){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 40+160, y+25, 24, 0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            }
          posicion_J2 = 15;
          b_UP_J2 = LOW;
        break; //break pos 12 /UP/N1/J2
        
        case 13:
          for(int y = 128; y >56; y--){
              LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
              H_line( 8+160, y+25, 24, 0x0000);
            }
            posicion_J2 = 19;
          b_UP_J2 = LOW;
        break; //break pos 13 /UP/N1/J2
        
        case 14:
          b_UP_J2 = LOW;
        break; //break pos 14 /UP/N1/J2
        
        case 15:
          b_UP_J2 = LOW;
        break; //break pos 15 /UP/N1/J2
        
        case 16:
          for(int y = 56; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,56,16,24,0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            }
          posicion_J2 = 23;
          b_UP_J2 = LOW;
        break; //break pos 16 /UP/N1/J2
        
        case 17:
          b_UP_J2 = LOW;
        break; //break pos 17 /UP/N1/J2
        
        case 18:
          for(int y = 56; y >24; y--){
            LCD_Sprite(32+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 32+160, y+25, 24, 0x0000);
            FillRect(24+160,56,16,24,0x0000);
          }
          posicion_J2 = 21;
          b_UP_J2 = LOW;
        break; //break pos 18 /UP/N1/J2
        
        case 19:
          b_UP_J2 = LOW;
        break; //break pos 19 /UP/N1/J2
        
        case 20:
          for(int y = 24; y >0; y--){
            LCD_Sprite(80+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 80+160, y+25, 24, 0x0000);
            FillRect(72+160,24,16,24,0x0000);
          }
          posicion_J2 = 24;
          b_UP_J2 = LOW;
        break; //break pos 20 /UP/N1/J2
        
        case 21:
          b_UP_J2 = LOW;
        break; //break pos 21 /UP/N1/J2
        
        case 22:
          for(int y = 24; y >0; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          if (STARS_N1_J2 == 5){
            ganar_N1 = HIGH;
            Juegos_ganados_J2++;
            nivel = 2;
            }
          posicion_J2 = 25;
          b_UP_J2 = LOW;
        break; //break pos 22 /UP/N1/J2
        
        case 23:
          b_UP_J2 = LOW;
        break; //break pos 23 /UP/N1/J2
        
        case 24:
          b_UP_J2 = LOW;
        break; //break pos 24 /UP/N1/J2
        
        case 25:
          b_UP_J2 = LOW;
        break; //break pos 25 /UP/N1/J2
        }
    break;//break del case 1 (nivel 1 - UP - J2)

//#################### NIVEL 2 ARRIBA J2 ###############################//
    case 2:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 2
          for(int y = 216; y >120; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 10;        
          b_UP_J2 = LOW; 
        break; //break pos 0 /UP/N2/J2
        
        case 1:
          for(int y = 216; y >192; y--){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y+25, 24, 0x0000);
            FillRect(48+160,208,24,16,0x0000);
            FillRect(40+160,216,16,24,0x0000);
          }
          FillRect(48+160, 192, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,0,0);
          posicion_J2 = 0;
          b_UP_J2 = LOW;
        break; //break pos 1 /UP/N2/J2

        case 2:
          for(int y = 216; y >192; y--){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y+25, 24, 0x0000);
            FillRect(88+160,208,24,16,0x0000);
          }
          FillRect(88+160, 192, 24, 24, 0x0000);
          LCD_Sprite(8+160,216,24,24,J2_Abajo_Derecha,1,0,1,0);
          posicion_J2 = 0;
          b_UP_J2 = LOW;
        break; //break pos 2 /UP/N2/J2
        
        case 3:
          for(int y = 216; y >120; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,216,16,24,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 13;        
          b_UP_J2 = LOW;
        break; //break pos 3 /UP/N2/J2
        
        case 4:
          b_UP_J2 = LOW;
        break; //break pos 4 /UP/N2/J2
        
        case 5:
          b_UP_J2 = LOW;
        break; //break pos 5 /UP/N2/J2
        
        case 6:
          for(int y = 144; y >120; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 10;
          b_UP_J2 = LOW;
        break; //break pos 6 /UP/N2/J2
        
        case 7:
          for(int y = 144; y >72; y--){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 15;
          b_UP_J2 = LOW;
        break; //break pos 7 /UP/N2/J2
        
        case 8:
          for(int y = 144; y >72; y--){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y+25, 24, 0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N2_J1++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            }
          posicion_J2 = 16;        
          b_UP_J2 = LOW;
        break; //break pos 8 /UP/N2/J2
        
        case 9:
          for(int y = 144; y >120; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,144,16,24,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 13;        
          b_UP_J2 = LOW;
        break; //break pos 9 /UP/N2/J2
        
        case 10:
          b_UP_J2 = LOW;
        break; //break pos 10 /UP/N1/J2
        
        case 11:
          for(int y = 120; y >72; y--){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y+25, 24, 0x0000);
            FillRect(40+160,120,16,24,0x0000);
          }
          posicion_J2 = 15;
          b_UP_J2 = LOW;
        break; //break pos 11 /UP/N2/J2
        
        case 12:
          for(int y = 120; y >72; y--){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y+25, 24, 0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            }
          posicion_J2 = 16;
          b_UP_J2 = LOW;
        break; //break pos 12 /UP/N2/J2
        
        case 13:
            b_UP_J2 = LOW;
        break; //break pos 13 /UP/N1/J2
        
        case 14:
          for(int y = 72; y >24; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 18;        
          b_UP_J2 = LOW;
        break; //break pos 14 /UP/N2/J2
        
        case 15:
          b_UP_J2 = LOW;
        break; //break pos 15 /UP/N2/J2
        
        case 16:
          b_UP_J2 = LOW;
        break; //break pos 16 /UP/N2/J2
        
        case 17:
          for(int y = 72; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,72,16,24,0x0000);
          }
          if (STARS_N2_J2 == 5){
            ganar_N2 = HIGH;
            Juegos_ganados_J2++;
            nivel = 3;
            }
          posicion_J2 = 24;        
          b_UP_J2 = LOW;
        break; //break pos 17 /UP/N2/J2
        
        case 18:
          b_UP_J2 = LOW;
        break; //break pos 18 /UP/N2/J2
        
        case 19:
          for(int y = 24; y >0; y--){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y+25, 24, 0x0000);
            FillRect(40+160,24,16,24,0x0000);
          }
          posicion_J2 = 22;        
          b_UP_J2 = LOW;
        break; //break pos 19 /UP/N2/J2
        
        case 20:
          for(int y = 24; y >0; y--){
            LCD_Sprite(96+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 96+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 23;
          b_UP_J2 = LOW;
        break; //break pos 20 /UP/N2/J2
        
        case 21:
          for(int y = 24; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
            FillRect(120+160,24,16,24,0x0000);
          }
          if (STARS_N2_J2 == 5){
            ganar_N2 = HIGH;
            Juegos_ganados_J2++;
            nivel = 3;
            }
          posicion_J2 = 24;        
          b_UP_J2 = LOW;
        break; //break pos 21 /UP/N2/J2
        
        case 22:
          b_UP_J2 = LOW;
        break; //break pos 22 /UP/N2/J2
        
        case 23:
          b_UP_J2 = LOW;
        break; //break pos 23 /UP/N2/J2
        
        case 24:
          b_UP_J2 = LOW;
        break; //break pos 24 /UP/N2/J2
        }
    break;//break del case 2 (nivel 2 - UP - J2)
//#################### NIVEL 3 ARRIBA J2 ###############################//
    case 3:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 3 JUGADOR 2      
          b_UP_J2 = LOW; 
        break; //break pos 0 /UP/N3/J2
        
        case 1:
          for(int y = 216; y >0; y--){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(40+160, y+25, 24, 0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            } 
          posicion_J2 = 19;
          b_UP_J2 = LOW;
        break; //break pos 1 /UP/N3/J2

        case 2:
          for(int y = 216; y >192; y--){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(8+160, y+25, 24, 0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            } 
          posicion_J2 = 4;
          b_UP_J2 = LOW;
        break; //break pos 2 /UP/N3/J2
        
        case 3:
          for(int y = 192; y >0; y--){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 40+160, y+25, 24, 0x0000);
            FillRect(32+160,192,16,24,0x0000);
          }
          if (STAR_3_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_3_J2 = HIGH;
            } 
          posicion_J2 = 19;        
          b_UP_J2 = LOW;
        break; //break pos 3 /UP/N3/J2
        
        case 4:
          b_UP_J2 = LOW;
        break; //break pos 4 /UP/N3/J2
        
        case 5:
          for(int y = 184; y >144; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 7;        
          b_UP_J2 = LOW;
        break; //break pos 5 /UP/N3/J2
        
        case 6:
          for(int y = 184; y >64; y--){
            LCD_Sprite(72+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(72+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 12;
          b_UP_J2 = LOW;
        break; //break pos 6 /UP/N3/J2
        
        case 7:
          b_UP_J2 = LOW;
        break; //break pos 7 /UP/N3/J2
        
        case 8:
          for(int y = 144; y >0; y--){
            LCD_Sprite(8+160,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(8+160, y+25, 24, 0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            } 
          posicion_J2 = 20;        
          b_UP_J2 = LOW;
        break; //break pos 8 /UP/N3/J2
        
        case 9:
          for(int y = 112; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y+25, 24, 0x0000);
          }
          if (STAR_5_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_5_J2 = HIGH;
            } 
          posicion_J2 = 17;        
          b_UP_J2 = LOW;
        break; //break pos 9 /UP/N3/J2
        
        case 10:
          for(int y = 112; y >24; y--){
            LCD_Sprite(104+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(104+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 15;         
          b_UP_J2 = LOW;
        break; //break pos 10 /UP/N3/J2
        
        case 11:
          for(int y = 64; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y+25, 24, 0x0000);
            FillRect(120+160,64,16,24,0x0000);
          }
          if (STAR_5_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_5_J2 = HIGH;
            }
          posicion_J2 = 17;
          b_UP_J2 = LOW;
        break; //break pos 11 /UP/N3/J2
        
        case 12:
          b_UP_J2 = LOW;
        break; //break pos 12 /UP/N3/J2
        
        case 13:
          for(int y = 64; y >0; y--){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(40+160, y+25, 24, 0x0000);
          }
          posicion_J2 = 19;             
            b_UP_J2 = LOW;
        break; //break pos 13 /UP/N3/J2
        
        case 14:
          for(int y = 24; y >0; y--){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y+25, 24, 0x0000);
            FillRect(120+160,24,16,24,0x0000);
          }
          if (STAR_5_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_5_J2 = HIGH;
            }
          posicion_J1 = 17;        
          b_UP_J2 = LOW;
        break; //break pos 14 /UP/N3/J2
        
        case 15:
          b_UP_J2 = LOW;
        break; //break pos 15 /UP/N3/J2
        
        case 16:
          for(int y = 24; y >0; y--){
            LCD_Sprite(72+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(72+160, y+25, 24, 0x0000);
          }
          if (STARS_N3_J2 == 5){
            ganar_N3 = HIGH;
            Juegos_ganados_J2++;
            nivel = 4;
            }
          posicion_J2 = 18;         
          b_UP_J2 = LOW;
        break; //break pos 16 /UP/N3/J2
        
        case 17:        
          b_UP_J2 = LOW;
        break; //break pos 17 /UP/N3/J2
        
        case 18:
          b_UP_J2 = LOW;
        break; //break pos 18 /UP/N3/J2
        
        case 19:    
          b_UP_J2 = LOW;
        break; //break pos 19 /UP/N3/J2
        
        case 20:
          b_UP_J2 = LOW;
        break; //break pos 20 /UP/N3/J2
        }
    break;//break del case 3 (nivel 3 - UP - J2)
    }  
}


//***************************************************************************************************************************************
// Función cuando se presiona el boton abajo del J1 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_DOWN_J1(int num_nivel){
    switch(num_nivel){
//#################### NIVEL 1 ABAJO J1 ###############################//
    case 1:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_DOWN_J1 = LOW;  
        break; //break pos 0 /DOWN/N1/J1
        
        case 1:
          b_DOWN_J1 = LOW;
        break; //break pos 1 /DOWN/N1/J1

        case 2:
          b_DOWN_J1 = LOW;
        break; //break pos 2 /DOWN/N1/J1
        
        case 3:
          for(int y = 192; y <216; y++){
            LCD_Sprite(56,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 56, y-1, 24, 0x0000);
            FillRect(48,192,16,24,0x0000);
          }
          posicion_J1 = 1;
          b_DOWN_J1 = LOW;
        break; //break pos 3 /DOWN/N1/J1
        
        case 4:
          for(int y = 192; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 2;
          b_DOWN_J1 = LOW;
        break; //break pos 4 /DOWN/N1/J1
        
        case 5:
          b_DOWN_J1 = LOW;
        break; //break pos 5 /DOWN/N1/J1
        
        case 6:
          b_DOWN_J1 = LOW;
        break; //break pos 6 /DOWN/N1/J1
        
        case 7:
          for(int y = 160; y <184; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
            FillRect(120,160,16,24,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 5;
          b_DOWN_J1 = LOW;
        break; //break pos 7 /DOWN/N1/J1
        
        case 8:
          for(int y = 160; y <184; y++){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y-1, 24, 0x0000);
          }
          posicion_J1 = 6;
          b_DOWN_J1 = LOW;
        break; //break pos 8 /DOWN/N1/J1
        
        case 9:
          b_DOWN_J1 = LOW;
        break; //break pos 9 /DOWN/N1/J1
        
        case 10:
          for(int y = 128; y <184; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
            FillRect(120,128,16,24,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 5;
          b_DOWN_J1 = LOW;
        break; //break pos 10 /DOWN/N1/J1
        
        case 11:
          b_DOWN_J1 = LOW;
        break; //break pos 11 /DOWN/N1/J1
        
        case 12: 
          b_DOWN_J1 = LOW;
        break; //break pos 12 /DOWN/N1/J1
        
        case 13:
          for(int y = 128; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 2;
          b_DOWN_J1 = LOW;
        break; //break pos 13 /DOWN/N1/J1
        
        case 14:
          for(int y = 88; y <184; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
            FillRect(120,88,16,24,0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            }
          posicion_J1 = 5;
          b_DOWN_J1 = LOW;
        break; //break pos 14 /DOWN/N1/J1
        
        case 15:
          for(int y = 88; y <128; y++){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 40, y-1, 24, 0x0000);
          }
          posicion_J1 = 12;
          b_DOWN_J1 = LOW;
        break; //break pos 15 /DOWN/N1/J1
        
        case 16:
          b_DOWN_J1 = LOW;
        break; //break pos 16 /DOWN/N1/J1
        
        case 17:
          for(int y = 56; y <128; y++){
            LCD_Sprite(96,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 96, y-1, 24, 0x0000);
          }
          posicion_J1 = 11;
          b_DOWN_J1 = LOW;
        break; //break pos 17 /DOWN/N1/J1
        
        case 18:
          b_DOWN_J1 = LOW;
        break; //break pos 18 /DOWN/N1/J1
        
        case 19:
          for(int y = 56; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N1_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 2;
          b_DOWN_J1 = LOW;
        break; //break pos 19 /DOWN/N1/J1
        
        case 20:
          b_DOWN_J1 = LOW;
        break; //break pos 20 /DOWN/N1/J1
        
        case 21:
          for(int y = 24; y <56; y++){
            LCD_Sprite(32,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 32, y-1, 24, 0x0000);
          }
          posicion_J1 = 18;
          b_DOWN_J1 = LOW;
        break; //break pos 21 /DOWN/N1/J1
        
        case 22:
          b_DOWN_J1 = LOW;
        break; //break pos 22 /DOWN/N1/J1
        
        case 23:
          for(int y = 0; y <56; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
            FillRect(120,0,16,24,0x0000);
          }
          posicion_J1 = 16;
          b_DOWN_J1 = LOW;
        break; //break pos 23 /DOWN/N1/J1
        
        case 24:
          for(int y = 0; y <24; y++){
            LCD_Sprite(80,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 80, y-1, 24, 0x0000);
          }
          posicion_J1 = 20;
          b_DOWN_J1 = LOW;
        break; //break pos 24 /DOWN/N1/J1
        
        case 25:
          for(int y = 0; y <24; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
            Rect(8,0,24,24,0xFF40); //J1
            Rect(12,4,16,16,0xFF40);
            Rect(16,8,8,8,0xFF40);
            
          }
          posicion_J1 = 22;
          b_DOWN_J1 = LOW;
        break; //break pos 25 /DOWN/N1/J1
        }
    break;//break del case 1 (nivel 1 - DOWN - J1)

//#################### NIVEL 2 ABAJO J1 ###############################//
    case 2:
     switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 1 
          b_DOWN_J1 = LOW;  
        break; //break pos 0 /DOWN/N2/J1
        
        case 1:
          b_DOWN_J1 = LOW;
        break; //break pos 1 /DOWN/N2/J1

        case 2:
          b_DOWN_J1 = LOW;
        break; //break pos 2 /DOWN/N2/J1
        
        case 3:
          b_DOWN_J1 = LOW;
        break; //break pos 3 /DOWN/N2/J1
        
        case 4:
          b_DOWN_J1 = LOW;
        break; //break pos 4 /DOWN/N2/J1
        
        case 5:
          b_DOWN_J1 = LOW;
        break; //break pos 5 /DOWN/N2/J1
        
        case 6:
          for(int y = 144; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          posicion_J1 = 0;        
          b_DOWN_J1 = LOW;
        break; //break pos 6 /DOWN/N2/J1
        
        case 7:
          b_DOWN_J1 = LOW;
        break; //break pos 7 /DOWN/N2/J1
        
        case 8:
          b_DOWN_J1 = LOW;
        break; //break pos 8 /DOWN/N2/J1
        
        case 9:
          for(int y = 144; y <216; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
            FillRect(112,144,16,24,0x0000);
          }
          posicion_J1 = 3;        
          b_DOWN_J1 = LOW;
        break; //break pos 9 /DOWN/N2/J1
        
        case 10:
          for(int y = 120; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          posicion_J1 = 0;
          b_DOWN_J1 = LOW;
        break; //break pos 10 /DOWN/N2/J1
        
        case 11:
          for(int y = 120; y <144; y++){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y-1, 24, 0x0000);
            FillRect(40,120,16,24,0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 7;        
          b_DOWN_J1 = LOW;
        break; //break pos 11 /DOWN/N2/J1
        
        case 12: 
          for(int y = 120; y <144; y++){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y-1, 24, 0x0000);
          }
          posicion_J1 = 8;        
          b_DOWN_J1 = LOW;
        break; //break pos 12 /DOWN/N2/J1
        
        case 13:
          for(int y = 120; y <216; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
            FillRect(120,120,16,24,0x0000);
          }
          posicion_J1 = 3;
          b_DOWN_J1 = LOW;
        break; //break pos 13 /DOWN/N2/J1
        
        case 14:
          b_DOWN_J1 = LOW;
        break; //break pos 14 /DOWN/N2/J1
        
        case 15:
          for(int y = 72; y <144; y++){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y-1, 24, 0x0000);
          }
          if (STAR_1_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_1_J1 = HIGH;
            }
          posicion_J1 = 7;
          b_DOWN_J1 = LOW;
        break; //break pos 15 /DOWN/N2/J1
        
        case 16:
          for(int y = 72; y <144; y++){
            LCD_Sprite(88,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 88, y-1, 24, 0x0000);
          }
          posicion_J1 = 8;        
          b_DOWN_J1 = LOW;
        break; //break pos 16 /DOWN/N2/J1
        
        case 17:
          b_DOWN_J1 = LOW;
        break; //break pos 17 /DOWN/N2/J1
        
        case 18:
          for(int y = 24; y <72; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 8, y-1, 24, 0x0000);
          }
          posicion_J1 = 14;        
          b_DOWN_J1 = LOW;
        break; //break pos 18 /DOWN/N2/J1
        
        case 19:
          b_DOWN_J1 = LOW;
        break; //break pos 19 /DOWN/N2/J1
        
        case 20:
          b_DOWN_J1 = LOW;
        break; //break pos 20 /DOWN/N2/J1
        
        case 21:
          for(int y = 24; y <72; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
            FillRect(120,24,16,24,0x0000);
          }
          posicion_J1 = 17;
          b_DOWN_J1 = LOW;
        break; //break pos 21 /DOWN/N2/J1
        
        case 22:
          for(int y = 0; y <24; y++){
            LCD_Sprite(48,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 48, y-1, 24, 0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N2_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            }
          posicion_J1 = 19;        
          b_DOWN_J1 = LOW;
        break; //break pos 22 /DOWN/N2/J1
        
        case 23:
          for(int y = 0; y <24; y++){
            LCD_Sprite(96,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 96, y-1, 24, 0x0000);
            FillRect(88,0,16,24,0x0000);
          }
          posicion_J1 = 20;
          b_DOWN_J1 = LOW;
        break; //break pos 23 /DOWN/N2/J1
        
        case 24:
          for(int y = 0; y <72; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 128, y-1, 24, 0x0000);
          }
          Rect(128,0,23,24,0xFF40);//J1
          Rect(132,4,16,16,0xFF40);
          Rect(136,8,8,8,0xFF40);
          posicion_J1 = 17;
          b_DOWN_J1 = LOW;
        break; //break pos 24 /DOWN/N2/J1
        } 
    break;//break del case 2 (nivel 2 - DOWN - J1)
//#################### NIVEL 3 ABAJO J1 ###############################//
    case 3:
      switch(posicion_J1){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_DOWN_J1 = LOW;  
        break; //break pos 0 /DOWN/N3/J1
        
        case 1:
          b_DOWN_J1 = LOW;
        break; //break pos 1 /DOWN/N3/J1

        case 2:
          b_DOWN_J1 = LOW;
        break; //break pos 2 /DOWN/N3/J1
        
        case 3:
          for(int y = 192; y <216; y++){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(40, y-1, 24, 0x0000);
            FillRect(32,192,16,24,0x0000);
          }
          posicion_J1 = 1;          
          b_DOWN_J1 = LOW;
        break; //break pos 3 /DOWN/N3/J1
        
        case 4:
          for(int y = 192; y <216; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(8, y-1, 24, 0x0000);
          }
          posicion_J1 = 2;          
          b_DOWN_J1 = LOW;
        break; //break pos 4 /DOWN/N3/J1
        
        case 5:
          b_DOWN_J1 = LOW;
        break; //break pos 5 /DOWN/N3/J1
        
        case 6:      
          b_DOWN_J1 = LOW;
        break; //break pos 6 /DOWN/N3/J1
        
        case 7:
          for(int y = 144; y <184; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
            FillRect(120,144,16,24,0x0000);
          }
          posicion_J1 = 5;          
          b_DOWN_J1 = LOW;
        break; //break pos 7 /DOWN/N3/J1
        
        case 8:
          b_DOWN_J1 = LOW;
        break; //break pos 8 /DOWN/N3/J1
        
        case 9:        
          b_DOWN_J1 = LOW;
        break; //break pos 9 /DOWN/N3/J1
        
        case 10:
          b_DOWN_J1 = LOW;
        break; //break pos 10 /DOWN/N3/J1
        
        case 11:
          for(int y = 64; y <112; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
            FillRect(120,64,16,24,0x0000);
          }
          posicion_J1 = 9;        
          b_DOWN_J1 = LOW;
        break; //break pos 11 /DOWN/N3/J1
        
        case 12: 
          for(int y = 64; y <184; y++){
            LCD_Sprite(72,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 72, y-1, 24, 0x0000);
          }
          if (STAR_4_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_4_J1 = HIGH;
            } 
          posicion_J1 = 6;        
          b_DOWN_J1 = LOW;
        break; //break pos 12 /DOWN/N3/J1
        
        case 13:
          for(int y = 64; y <216; y++){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(40, y-1, 24, 0x0000);
          }
          posicion_J1 = 1;
          b_DOWN_J1 = LOW;
        break; //break pos 13 /DOWN/N3/J1
        
        case 14:
          for(int y = 24; y <112; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
            FillRect(120,24,16,24,0x0000);
          }
          posicion_J1 = 9;          
          b_DOWN_J1 = LOW;
        break; //break pos 14 /DOWN/N3/J1
        
        case 15:
          for(int y = 24; y <112; y++){
            LCD_Sprite(104,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line( 104, y-1, 24, 0x0000);
          }
          posicion_J1 = 10;
          b_DOWN_J1 = LOW;
        break; //break pos 15 /DOWN/N3/J1
        
        case 16:      
          b_DOWN_J1 = LOW;
        break; //break pos 16 /DOWN/N3/J1
        
        case 17:
          for(int y = 0; y <112; y++){
            LCD_Sprite(128,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(128, y-1, 24, 0x0000);
          }
          posicion_J1 = 9;          
          b_DOWN_J1 = LOW;
        break; //break pos 17 /DOWN/N3/J1
        
        case 18:
          for(int y = 0; y <24; y++){
            LCD_Sprite(72,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(72, y-1, 24, 0x0000);
          }
          Rect(72,0,23,24,0xFF40); //J1
          Rect(76,4,16,16,0xFF40);
          Rect(80,8,8,8,0xFF40);
          posicion_J1 = 16;        
          b_DOWN_J1 = LOW;
        break; //break pos 18 /DOWN/N3/J1
        
        case 19:
          for(int y = 0; y <216; y++){
            LCD_Sprite(40,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(40, y-1, 24, 0x0000);
            FillRect(32,0,16,24,0x0000);
          }
          posicion_J1 = 1;          
          b_DOWN_J1 = LOW;
        break; //break pos 19 /DOWN/N3/J1
        
        case 20:
          for(int y = 0; y <144; y++){
            LCD_Sprite(8,y,24,24,J1_Abajo_Derecha,1,0,0,0);
            H_line(8, y-1, 24, 0x0000);
          }
          if (STAR_2_J1 == LOW){
            STARS_N3_J1++;
            STARS_J1++;
            STAR_2_J1 = HIGH;
            } 
          posicion_J1 = 8;          
          b_DOWN_J1 = LOW;
        break; //break pos 20 /DOWN/N3/J1
        } 
    break;//break del case 3 (nivel 3 - DOWN - J1)
    }  
}

//***************************************************************************************************************************************
// Función cuando se presiona el boton abajo del J2 y revisa la posici[on y se mueve
//***************************************************************************************************************************************
void switch_posicion_DOWN_J2(int num_nivel){
  switch(num_nivel){
//#################### NIVEL 1 ABAJO J2 ###############################//
    case 1:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_DOWN_J2 = LOW;
        break; //break pos 0 /DOWN/N1/J2
        
        case 1:
          b_DOWN_J2 = LOW;
        break; //break pos 1 /DOWN/N1/J2

        case 2:
          b_DOWN_J2 = LOW;
        break; //break pos 2 /DOWN/N1/J2
        
        case 3:
          for(int y = 192; y <216; y++){
            LCD_Sprite(56+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 56+160, y-1, 24, 0x0000);
            FillRect(48+160,192,16,24,0x0000);
          }
          posicion_J2 = 1;
          b_DOWN_J2 = LOW;
        break; //break pos 3 /DOWN/N1/J2
        
        case 4:
          for(int y = 192; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 2;
          b_DOWN_J2 = LOW;
        break; //break pos 4 /DOWN/N1/J2
        
        case 5:
          b_DOWN_J2 = LOW;
        break; //break pos 5 /DOWN/N1/J2
        
        case 6:
          b_DOWN_J2 = LOW;
        break; //break pos 6 /DOWN/N1/J2
        
        case 7:
          for(int y = 160; y <184; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
            FillRect(120+160,160,16,24,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 5;
          b_DOWN_J2 = LOW;
        break; //break pos 7 /DOWN/N1/J2
        
        case 8:
          for(int y = 160; y <184; y++){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 6;
          b_DOWN_J2 = LOW;
        break; //break pos 8 /DOWN/N1/J2
        
        case 9:
          b_DOWN_J2 = LOW;
        break; //break pos 9 /DOWN/N1/J2
        
        case 10:
          for(int y = 128; y <184; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
            FillRect(120+160,128,16,24,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 5;
          b_DOWN_J2 = LOW;
        break; //break pos 10 /DOWN/N1/J2
        
        case 11:
          b_DOWN_J2 = LOW;
        break; //break pos 11 /DOWN/N1/J2
        
        case 12:
          b_DOWN_J2 = LOW;
        break; //break pos 12 /DOWN/N1/J2
        
        case 13:
           for(int y = 128; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 2;
          b_DOWN_J2 = LOW;
        break; //break pos 13 /DOWN/N1/J2
        
        case 14:
          for(int y = 88; y <184; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
            FillRect(120+160,88,16,24,0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            }
          posicion_J2 = 5;
          b_DOWN_J2 = LOW;
        break; //break pos 14 /DOWN/N1/J2
        
        case 15:
          for(int y = 88; y <128; y++){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 40+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 12;
          b_DOWN_J2 = LOW;
        break; //break pos 15 /DOWN/N1/J2
        
        case 16:
          b_DOWN_J2 = LOW;
        break; //break pos 16 /DOWN/N1/J2
        
        case 17:
          for(int y = 56; y <128; y++){
            LCD_Sprite(96+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 96+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 11;
          b_DOWN_J2 = LOW;
        break; //break pos 17 /DOWN/N1/J2
        
        case 18:
          b_DOWN_J2 = LOW;
        break; //break pos 18 /DOWN/N1/J2
        
        case 19:
          for(int y = 56; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N1_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 2;
          b_DOWN_J2 = LOW;
        break; //break pos 19 /DOWN/N1/J2
        
        case 20:
          b_DOWN_J2 = LOW;
        break; //break pos 20 /DOWN/N1/J2
        
        case 21:
          for(int y = 24; y <56; y++){
            LCD_Sprite(32+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 32+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 18;
          b_DOWN_J2 = LOW;
        break; //break pos 21 /DOWN/N1/J2
        
        case 22:
          b_DOWN_J2 = LOW;
        break; //break pos 22 /DOWN/N1/J2
        
        case 23:
        for(int y = 0; y <56; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
            FillRect(120+160,0,16,24,0x0000);
          }
          posicion_J2 = 16;
          b_DOWN_J2 = LOW;
        break; //break pos 23 /DOWN/N1/J2
        
        case 24:
        for(int y = 0; y <24; y++){
            LCD_Sprite(80+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 80+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 20;
          b_DOWN_J2 = LOW;
        break; //break pos 24 /DOWN/N1/J2
        
        case 25:
        for(int y = 0; y <24; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
            Rect(168,0,24,24,0xFF40);//J2
            Rect(172,4,16,16,0xFF40);
            Rect(176,8,8,8,0xFF40);
          }
          posicion_J2 = 22;
          b_DOWN_J2 = LOW;
        break; //break pos 25 /DOWN/N1/J2
        }
    break;//break del case 1 (nivel 1 - DOWN - J2)

//#################### NIVEL 2 ABAJO J2 ###############################//
    case 2:
     switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 2 JUGADOR 2 
          b_DOWN_J2 = LOW;  
        break; //break pos 0 /DOWN/N2/J2
        
        case 1:
          b_DOWN_J2 = LOW;
        break; //break pos 1 /DOWN/N2/J2

        case 2:
          b_DOWN_J2 = LOW;
        break; //break pos 2 /DOWN/N2/J2
        
        case 3:
          b_DOWN_J2 = LOW;
        break; //break pos 3 /DOWN/N2/J2
        
        case 4:
          b_DOWN_J2 = LOW;
        break; //break pos 4 /DOWN/N2/J2
        
        case 5:
          b_DOWN_J2 = LOW;
        break; //break pos 5 /DOWN/N2/J2
        
        case 6:
          for(int y = 144; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 0;        
          b_DOWN_J2 = LOW;
        break; //break pos 6 /DOWN/N2/J2
        
        case 7:
          b_DOWN_J2 = LOW;
        break; //break pos 7 /DOWN/N2/J2
        
        case 8:
          b_DOWN_J2 = LOW;
        break; //break pos 8 /DOWN/N2/J2
        
        case 9:
          for(int y = 144; y <216; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
            FillRect(112+160,144,16,24,0x0000);
          }
          posicion_J2 = 3;        
          b_DOWN_J2 = LOW;
        break; //break pos 9 /DOWN/N2/J2
        
        case 10:
          for(int y = 120; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 0;
          b_DOWN_J2 = LOW;
        break; //break pos 10 /DOWN/N2/J2
        
        case 11:
          for(int y = 120; y <144; y++){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y-1, 24, 0x0000);
            FillRect(40+160,120,16,24,0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 7;        
          b_DOWN_J2 = LOW;
        break; //break pos 11 /DOWN/N2/J2
        
        case 12: 
          for(int y = 120; y <144; y++){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 8;        
          b_DOWN_J2 = LOW;
        break; //break pos 12 /DOWN/N2/J2
        
        case 13:
          for(int y = 120; y <216; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
            FillRect(120+160,120,16,24,0x0000);
          }
          posicion_J2 = 3;
          b_DOWN_J2 = LOW;
        break; //break pos 13 /DOWN/N2/J2
        
        case 14:
          b_DOWN_J2 = LOW;
        break; //break pos 14 /DOWN/N2/J2
        
        case 15:
          for(int y = 72; y <144; y++){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y-1, 24, 0x0000);
          }
          if (STAR_1_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_1_J2 = HIGH;
            }
          posicion_J2 = 7;
          b_DOWN_J2 = LOW;
        break; //break pos 15 /DOWN/N2/J2
        
        case 16:
          for(int y = 72; y <144; y++){
            LCD_Sprite(88+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 88+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 8;        
          b_DOWN_J2 = LOW;
        break; //break pos 16 /DOWN/N2/J2
        
        case 17:
          b_DOWN_J2 = LOW;
        break; //break pos 17 /DOWN/N2/J2
        
        case 18:
          for(int y = 24; y <72; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 8+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 14;        
          b_DOWN_J2 = LOW;
        break; //break pos 18 /DOWN/N2/J2
        
        case 19:
          b_DOWN_J2 = LOW;
        break; //break pos 19 /DOWN/N2/J2
        
        case 20:
          b_DOWN_J2 = LOW;
        break; //break pos 20 /DOWN/N2/J2
        
        case 21:
          for(int y = 24; y <72; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
            FillRect(120+160,24,16,24,0x0000);
          }
          posicion_J2 = 17;
          b_DOWN_J2 = LOW;
        break; //break pos 21 /DOWN/N2/J2
        
        case 22:
          for(int y = 0; y <24; y++){
            LCD_Sprite(48+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 48+160, y-1, 24, 0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N2_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            }
          posicion_J2 = 19;        
          b_DOWN_J2 = LOW;
        break; //break pos 22 /DOWN/N2/J2
        
        case 23:
          for(int y = 0; y <24; y++){
            LCD_Sprite(96+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 96+160, y-1, 24, 0x0000);
            FillRect(88+160,0,16,24,0x0000);
          }
          posicion_J2 = 20;
          b_DOWN_J2 = LOW;
        break; //break pos 23 /DOWN/N2/J2
        
        case 24:
          for(int y = 0; y <72; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 128+160, y-1, 24, 0x0000);
          }
          Rect(128+160,0,23,24,0xFF40);//J1
          Rect(132+160,4,16,16,0xFF40);
          Rect(136+160,8,8,8,0xFF40);
          posicion_J2 = 17;
          b_DOWN_J2 = LOW;
        break; //break pos 24 /DOWN/N2/J2
        } 
    break;//break del case 2 (nivel 2 - DOWN - J2)
//#################### NIVEL 3 ABAJO J2 ###############################//
    case 3:
      switch(posicion_J2){
        case 0://POSICION INICIO NIVEL 1 JUGADOR 1 
          b_DOWN_J2 = LOW;  
        break; //break pos 0 /DOWN/N3/J2
        
        case 1:
          b_DOWN_J2 = LOW;
        break; //break pos 1 /DOWN/N3/J2

        case 2:
          b_DOWN_J2 = LOW;
        break; //break pos 2 /DOWN/N3/J2
        
        case 3:
          for(int y = 192; y <216; y++){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(40+160, y-1, 24, 0x0000);
            FillRect(32+160,192,16,24,0x0000);
          }
          posicion_J2 = 1;          
          b_DOWN_J2 = LOW;
        break; //break pos 3 /DOWN/N3/J2
        
        case 4:
          for(int y = 192; y <216; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(8+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 2;          
          b_DOWN_J2 = LOW;
        break; //break pos 4 /DOWN/N3/J2
        
        case 5:
          b_DOWN_J2 = LOW;
        break; //break pos 5 /DOWN/N3/J2
        
        case 6:      
          b_DOWN_J2 = LOW;
        break; //break pos 6 /DOWN/N3/J2
        
        case 7:
          for(int y = 144; y <184; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
            FillRect(120+160,144,16,24,0x0000);
          }
          posicion_J2 = 5;          
          b_DOWN_J2 = LOW;
        break; //break pos 7 /DOWN/N3/J2
        
        case 8:
          b_DOWN_J2 = LOW;
        break; //break pos 8 /DOWN/N3/J2
        
        case 9:        
          b_DOWN_J2 = LOW;
        break; //break pos 9 /DOWN/N3/J2
        
        case 10:
          b_DOWN_J2 = LOW;
        break; //break pos 10 /DOWN/N3/J2
        
        case 11:
          for(int y = 64; y <112; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
            FillRect(120+160,64,16,24,0x0000);
          }
          posicion_J2 = 9;        
          b_DOWN_J2 = LOW;
        break; //break pos 11 /DOWN/N3/J2
        
        case 12: 
          for(int y = 64; y <184; y++){
            LCD_Sprite(72+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 72+160, y-1, 24, 0x0000);
          }
          if (STAR_4_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_4_J2 = HIGH;
            } 
          posicion_J2 = 6;        
          b_DOWN_J2 = LOW;
        break; //break pos 12 /DOWN/N3/J2
        
        case 13:
          for(int y = 64; y <216; y++){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(40+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 1;
          b_DOWN_J2 = LOW;
        break; //break pos 13 /DOWN/N3/J2
        
        case 14:
          for(int y = 24; y <112; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
            FillRect(120+160,24,16,24,0x0000);
          }
          posicion_J2 = 9;          
          b_DOWN_J2 = LOW;
        break; //break pos 14 /DOWN/N3/J2
        
        case 15:
          for(int y = 24; y <112; y++){
            LCD_Sprite(104+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line( 104+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 10;
          b_DOWN_J2 = LOW;
        break; //break pos 15 /DOWN/N3/J2
        
        case 16:      
          b_DOWN_J2 = LOW;
        break; //break pos 16 /DOWN/N3/J2
        
        case 17:
          for(int y = 0; y <112; y++){
            LCD_Sprite(128+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(128+160, y-1, 24, 0x0000);
          }
          posicion_J2 = 9;          
          b_DOWN_J2 = LOW;
        break; //break pos 17 /DOWN/N3/J2
        
        case 18:
          for(int y = 0; y <24; y++){
            LCD_Sprite(72+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(72+160, y-1, 24, 0x0000);
          }
          Rect(232,0,23,24,0xFF40);//J2
          Rect(236,4,16,16,0xFF40);
          Rect(240,8,8,8,0xFF40); 
          posicion_J2 = 16;        
          b_DOWN_J2 = LOW;
        break; //break pos 18 /DOWN/N3/J2
        
        case 19:
          for(int y = 0; y <216; y++){
            LCD_Sprite(40+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(40+160, y-1, 24, 0x0000);
            FillRect(32+160,0,16,24,0x0000);
          }
          posicion_J2 = 1;          
          b_DOWN_J2 = LOW;
        break; //break pos 19 /DOWN/N3/J2
        
        case 20:
          for(int y = 0; y <144; y++){
            LCD_Sprite(8+160,y,24,24,J2_Abajo_Derecha,1,0,0,0);
            H_line(8+160, y-1, 24, 0x0000);
          }
          if (STAR_2_J2 == LOW){
            STARS_N3_J2++;
            STARS_J2++;
            STAR_2_J2 = HIGH;
            } 
          posicion_J2 = 8;          
          b_DOWN_J2 = LOW;
        break; //break pos 20 /DOWN/N3/J2
        } 
    break;//break del case 3 (nivel 3 - DOWN - J2)
    }    
}
