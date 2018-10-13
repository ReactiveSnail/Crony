#define SCLK 4  // пины ардуинo
#define RCLK 3
#define DIO 2

int NUMBER = 0;
int NUMBER1 = 0;
int NUMBER2 = 0;
int NUMBER3 = 0;

const byte symbol[15] = {  // маска для 7 сигментного индикатора
  0b00000000, // 
  0b01110110, // H
  0b01111001, // E
  0b00111000, // L
  0b00111000, // L
  0b00111111, // O
  0b00000000, // 
  0b00111110, // 1/2 W = U
  0b00111110, // 1/2 W = U
  0b00111111, // O
  0b01110111, // R
  0b00111000, // L
  0b11011110, // d.
  0b00000000, //
  0b01000000, // -
};
const byte chr[3] = { // маска для разряда 0, 1, 2
  0b00000110, //100
  0b00000101, //010
  0b00000011, //001
  //0b00000000, //000
};
unsigned long timing;

void setup() {
  // put your setup code here, to run once:
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - timing >= 1000) {  //Задержка 1000 мСек
    timing = millis();
    NUMBER = NUMBER + 1;           //Счетчик
    if (NUMBER == 15)
      NUMBER = 0;
  }
  NUMBER1 = NUMBER;
  NUMBER2 = NUMBER1 + 1;
  if (NUMBER2 == 15)
      NUMBER2 = 0;
  NUMBER3 = NUMBER2 + 1;
  if (NUMBER3 == 15)
      NUMBER3 = 0;
  
  

  int NUMBERS[3] = {
    NUMBER1,
    NUMBER2,
    NUMBER3,
  };

  for (int i = 0; i <= 2; i++) {
    digitalWrite(RCLK, LOW); // открываем защелку
    shiftOut(DIO, SCLK, MSBFIRST,  chr[i]);  // включаем разряды
    shiftOut(DIO, SCLK, MSBFIRST, symbol[NUMBERS[i]]);
    digitalWrite(RCLK, HIGH); // защелкиваем регистры
    delay(1);
  }



  //Serial.print(NUMBER);
  //Serial.print("___");
  //Serial.print(NUMBER1);
  //Serial.print("___");
  //Serial.print(NUMBER2);
  //Serial.print("___");
  //Serial.println(NUMBER3);
  //delay(500);


}
