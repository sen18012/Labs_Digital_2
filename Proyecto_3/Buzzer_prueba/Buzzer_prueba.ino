/*
 * ------ Proyecto # 3 ------ 
 * Natalia de León 18193
 * Katharine Senn 18012
 * Sección 20
 * Abril 2021
 */

 //***************************************************************************************************************************************
/* Código para reproducción de audio en Buzzer para TivaC
 * Basado en el código de Dung Dang - https://github.com/robsoncouto/arduino-songs/blob/master/greenhill/greenhill.ino //https://musescore.com/user/248346/scores/461661
 * y el ejemplo de energia BuzzerImperialMarchTune
 * Adaptación para reproducción de canción Green Hill Zone - Sonic the Hedgehog
 * Modificado por Natalia de León y Katharine Senn
 * IE3027: Electrónica Digital 2 - 2021
 */
//***************************************************************************************************************************************

#include "pitches.h"
const int buttonPin = PC_5;
int tempo = 140; //tiempo de la música (Para acelerar o alentar la canción)
int buzzer = 40; //Pin para conectar el buzzer
volatile byte flag = LOW;

// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody[] = {
  NOTE_NOTONE,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //1
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

  NOTE_NOTONE,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8, //7
  NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,2,
  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //13
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8,
  NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8,
  NOTE_E4,-1,
  NOTE_NOTONE,8, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,-1,

  NOTE_NOTONE,8, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8, NOTE_F4,-1, //20
  NOTE_NOTONE,8, NOTE_D4,8, NOTE_F4,8, NOTE_E4,-1, //end 1

  //repeats from 1

  NOTE_NOTONE,2, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //1
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

  NOTE_NOTONE,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8, //7
  NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,2,
  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_A4,8, NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8,
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 

  NOTE_NOTONE,4, NOTE_D5,8, NOTE_B4,4, NOTE_D5,8, //13
  NOTE_CS5,4, NOTE_D5,8, NOTE_CS5,4, NOTE_A4,2, 
  NOTE_NOTONE,8, NOTE_B4,8, NOTE_B4,8, NOTE_G4,4, NOTE_B4,8,
  NOTE_A4,4, NOTE_B4,8, NOTE_A4,4, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8,
  NOTE_E4,-1,
  NOTE_NOTONE,8, NOTE_D4,8, NOTE_E4,8, NOTE_FS4,-1,

  NOTE_NOTONE,8, NOTE_D4,8, NOTE_D4,8, NOTE_FS4,8, NOTE_F4,-1, //20
  NOTE_NOTONE,8, NOTE_D4,8, NOTE_F4,8, NOTE_E4,8, //end 2
  NOTE_E4,-2, NOTE_A4,8, NOTE_CS5,8, 
  NOTE_FS5,8, NOTE_E5,4, NOTE_D5,8, NOTE_A5,-4,

};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes = sizeof(melody) / sizeof(melody[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote = (60000 * 4) / tempo;
int divider = 0, noteDuration = 0;

void setup() {
  //pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode (RED_LED, OUTPUT);
  
} 

void loop() {
    for (int thisNote = 0; thisNote < notes * 2; thisNote = thisNote + 2) {
      // calculates the duration of each note
     if (digitalRead(buttonPin) == HIGH){
      divider = melody[thisNote + 1];
      if (divider > 0) {
        // regular note, just proceed
        noteDuration = (wholenote) / divider;
      } else if (divider < 0) {
        // dotted notes are represented with negative durations!!
        noteDuration = (wholenote) / abs(divider);
        noteDuration *= 1.5; // increases the duration in half for dotted notes
      }
      // we only play the note for 90% of the duration, leaving 10% as a pause
      tone(buzzer, melody[thisNote], noteDuration * 0.9);

      // Wait for the specief duration before playing the next note.
      delay(noteDuration);
  
      // stop the waveform generation before the next note.
      noTone(buzzer);
    }
    else {
      thisNote = 0;
    }
  }
}
