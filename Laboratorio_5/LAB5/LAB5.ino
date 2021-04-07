//Universidad del Valle de Guatemala              Katharine Senn Salazar
//Electrónica Digital 2                                      Carné 18012
//Sección 20
//                                  Laboratorio 5
//                                 

//Librerías
#include <SPI.h>
#include <SD.h>

//Variables
File myFile;
int file = 0; //variable para elegir el archivo que se muestra

//Prototipos funciones
void printDirectory(File dir, int NTabs); 

//Setup
void setup() {
  Serial.begin(115200); //Config comuniación serual
  SPI.setModule(0); //Config SPI
  while (!Serial){
    ; //wait for serial port to connect. 
  }
  Serial.print("Initializing SD card...");
  pinMode(PA_3, OUTPUT);
  if (!SD.begin(PA_3)){
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("Initialization done.");
  myFile = SD.open("/");

  printDirectory(myFile, 0);
  Serial.print("Por favor, ingrese el número del archivo que desea abrir:");
}

void loop() {
    if (Serial.available() > 0) {
      file = Serial.read(); //Leemos el dato que indica el ususario 
      }

    if(file == '1'){
      myFile = SD.open("ghost.txt"); //Si eligen la 1 se muestra el fantasma de PACMAN
        Serial.println("ghost.txt:");

        // read from the file until there's nothing else in it:
        while (myFile.available()){
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
        Serial.println("Si desea ver otro archivo presione el número correspondiente: ");
    }

    if(file =='2'){
      myFile = SD.open("hongo.txt"); //Si eligen la 2 se muestra el honguito de Mario
        Serial.println("hongo.txt:");

        // read from the file until there's nothing else in it:
        while (myFile.available()){
            Serial.write(myFile.read());
        }
        // close the file:
        myFile.close();
        Serial.println("Si desea ver otro archivo presione el número correspondiente: ");
    }

     if(file == '3'){
       myFile = SD.open("yinyang.txt"); //Si eligen la 3 se muestra el dibujo de Yin Yang
        Serial.println("yinyang.txt:");

        // read from the file until there's nothing else in it:
        while (myFile.available()){
            Serial.write(myFile.read());
        // close the file:
        }
        myFile.close();
        Serial.println("Si desea ver otro archivo presione el número correspondiente: ");
     }
        else{
        }
}


    void printDirectory(File dir, int NTabs) {
   while(true) {
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<NTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, NTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
  Serial.println("");
  
}
