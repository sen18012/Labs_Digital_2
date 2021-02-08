/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DISP7_H_
#define	DISP7_H_

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdint.h>


void DISPLAY7(uint8_t val);



//void DISPLAY7(uint8_t val) {
//    TRISD = 0;
//    PORTD = 0;
//    switch (val) {
//        case 0:
//            PORTD = 0x3F; //0
//            break;
//        case 1:
//            PORTD = 0x06; //1
//            break;
//        case 2:
//            PORTD = 0x5B; //2
//            break;
//        case 3:
//            PORTD = 0x4F; //3
//            break;
//        case 4:
//            PORTD = 0x66; //4
//            break;
//        case 5:
//            PORTD = 0x6D; //5
//            break;
//        case 6:
//            PORTD = 0x7D; //6
//            break;
//        case 7:
//            PORTD = 0x07; //7
//            break;
//        case 8:
//            PORTD = 0x7F; //8
//            break;
//        case 9:
//            PORTD = 0x6F; //9
//            break;
//        case 10:
//            PORTD = 0x77; //A
//            break;
//        case 11:
//            PORTD = 0x1F; //B
//            break;
//        case 12:
//            PORTD = 0x4E; //C
//            break;
//        case 13:
//            PORTD = 0x3D; //D
//            break;
//        case 14:
//            PORTD = 0x4F; //E
//            break;
//        case 15:
//            PORTD = 0x47; //F
//            break;
//
//        case 128:
//            PORTD = 0x06; //1
//            break;
//        case 64:
//            PORTD = 0x5B; //2
//            break;
//        case 192:
//            PORTD = 0x4F; //3
//            break;
//        case 32:
//            PORTD = 0x66; //4
//            break;
//        case 160:
//            PORTD = 0x6D; //5
//            break;
//        case 96:
//            PORTD = 0x7D; //6
//            break;
//        case 224:
//            PORTD = 0x07; //7
//            break;
//        case 16:
//            PORTD = 0x7F; //8
//            break;
//        case 144:
//            PORTD = 0x6F; //9
//            break;
//        case 80:
//            PORTD = 0x77; //A
//            break;
//        case 208:
//            PORTD = 0x1F; //B
//            break;
//        case 48:
//            PORTD = 0x4E; //C
//            break;
//        case 176:
//            PORTD = 0x3D; //D
//            break;
//        case 112:
//            PORTD = 0x4F; //E
//            break;
//        case 240:
//            PORTD = 0x47; //F
//            break;
//        default:
//            PORTD = 0x3F; //0
//            break;
//    }
//}
#endif	/* DISP7_H_*/
