//Universidad del Valle de Guatemala              Katharine Senn Salazar
//Electrónica Digital 2                                      Carné 18012
//Sección 20
//                             Laboratorio #4                   

//DECLARACIÓN PINES PLAYER 1
int PLAY11 = PA_7;
int PLAY12 = PA_6;
int PLAY13 = PA_5;
int PLAY14 = PB_4;
int PLAY15 = PE_5;
int PLAY16 = PE_4;
int PLAY17 = PB_1;
int PLAY18 = PB_0;

//DECLARACIÓN PINES PLAYER 2
int PLAY21 = PB_3;
int PLAY22 = PC_4;
int PLAY23 = PC_5;
int PLAY24 = PC_6;
int PLAY25 = PC_7;
int PLAY26 = PA_4;
int PLAY27 = PA_3;
int PLAY28 = PA_2;

//DECLARACIÓN VARIABLES
int cont1 = 0; //Contador player 1
int cont2 = 0; //Contador player 2
int debounce1 = 0; //antirebote
int debounce2 = 0; //antirebote
int inicio = 0; //bandera para saber si el juego ya inició

//PROTOTIPOS DE FUNCIONES
void semaf(void);
void player1(void);
void player2(void);
void winner(void);


void setup() {
pinMode(PLAY11, OUTPUT);
pinMode(PLAY12, OUTPUT);
pinMode(PLAY13, OUTPUT);
pinMode(PLAY14, OUTPUT);
pinMode(PLAY15, OUTPUT);
pinMode(PLAY16, OUTPUT);
pinMode(PLAY17, OUTPUT);
pinMode(PLAY18, OUTPUT);
pinMode(PLAY21, OUTPUT);
pinMode(PLAY22, OUTPUT);
pinMode(PLAY23, OUTPUT);
pinMode(PLAY24, OUTPUT);
pinMode(PLAY25, OUTPUT);
pinMode(PLAY26, OUTPUT);
pinMode(PLAY27, OUTPUT);
pinMode(PLAY28, OUTPUT);
pinMode(RED_LED, OUTPUT);
pinMode(BLUE_LED, OUTPUT);
pinMode(GREEN_LED, OUTPUT);
pinMode(PUSH1, INPUT_PULLUP);
pinMode(PUSH2, INPUT_PULLUP);

}

void loop() {
  // put your main code here, to run repeatedly:
  winner(); 
  player1();
  player2();
}

void player1(void){
  if (digitalRead(PUSH1) == LOW){
    debounce1 = 1;
  }
  if (digitalRead(PUSH1) == HIGH && debounce1 == 1){
    debounce1 = 0;
    if (inicio == 0){
      semaf();
    }
    else if (inicio == 1){
      delay(50);
      cont1++;
    }
  }
  switch (cont1){
    case 0:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 1:
      digitalWrite(PLAY11, HIGH);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 2:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, HIGH);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 3:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, HIGH);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 4:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, HIGH);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 5:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, HIGH);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 6:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, HIGH);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, LOW);
      break;

    case 7:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, HIGH);
      digitalWrite(PLAY18, LOW);
      break;

    case 8:
      digitalWrite(PLAY11, LOW);
      digitalWrite(PLAY12, LOW);
      digitalWrite(PLAY13, LOW);
      digitalWrite(PLAY14, LOW);
      digitalWrite(PLAY15, LOW);
      digitalWrite(PLAY16, LOW);
      digitalWrite(PLAY17, LOW);
      digitalWrite(PLAY18, HIGH);
      delay(100);
      break;
  }
}

void player2(void){
  if (digitalRead(PUSH2) == LOW){
    debounce2 = 1;
  }
  if (digitalRead(PUSH2) == HIGH && debounce2 == 1){
    debounce2 = 0;
    if (inicio == 0){
      semaf(); 
    }
    else if (inicio == 1){
      delay(50);
      cont2++;
    }
  }
  switch (cont2){
    case 0:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 1:
      digitalWrite(PLAY21, HIGH);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 2:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, HIGH);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 3:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, HIGH);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 4:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, HIGH);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 5:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, HIGH);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 6:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, HIGH);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, LOW);
      break;

    case 7:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, HIGH);
      digitalWrite(PLAY28, LOW);
      break;

    case 8:
      digitalWrite(PLAY21, LOW);
      digitalWrite(PLAY22, LOW);
      digitalWrite(PLAY23, LOW);
      digitalWrite(PLAY24, LOW);
      digitalWrite(PLAY25, LOW);
      digitalWrite(PLAY26, LOW);
      digitalWrite(PLAY27, LOW);
      digitalWrite(PLAY28, HIGH);
      delay(100);
      break;
  }
}

void semaf(void){
  digitalWrite(RED_LED, HIGH);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  delay(1000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(1000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(1000);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  inicio = 1;
}

void winner(void){
  if (cont1 == 8){
    digitalWrite(BLUE_LED, HIGH);
    delay(1000);
    cont1 = 0;
    cont2 = 0;
    inicio = 0;
  }
  if (cont2 == 8){
    digitalWrite(GREEN_LED, HIGH);
    delay(1000);
    cont1 = 0;
    cont2 = 0;
    inicio = 0;
  }
}
