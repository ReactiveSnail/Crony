#define SCLK 4 // пины ардуинo
#define RCLK 3
#define DIO  2

// Время обновления показаний, мс (200-2000)
#define REFRESH_TIME  3
// Аналоговй вход
#define PIN_VOLT A0
// Внутреннее опорное напряжение (подобрать)
const float VRef = 1.08;
// Коэффициент входного резистивного делителя (Rh + Rl) / Rl. IN <-[ Rh ]--(analogInPin)--[ Rl ]--|GND
const float VoltMult = (103.8 + 10.430) / 10.430;

float InVolt, Volt;

unsigned long timing;
int NUMBER = 0;
int NUMBER1 = 0;
int NUMBER2 = 0;
int NUMBER3 = 0;
byte ZPT = 0b10000000; //Запятая ","

const byte digit[10] = {  // маска для 7 сигментного индикатора
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111, // 9
  //0b10000000, // ","
};
const byte chr[3] = { // маска для разряда 0, 1, 2
  0b00000110, //100
  0b00000101, //010
  0b00000011, //001
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

  analogReference(INTERNAL);//Включаем внутряннее опорное напряжение
  pinMode(PIN_VOLT, INPUT);

}

void loop() {
  unsigned long CalcStart = millis();
  int ReadCnt = 0;
  InVolt = 0;
  // Чтение из порта с усреднением
  while ((millis() - CalcStart) < REFRESH_TIME) {
    InVolt += analogRead(PIN_VOLT);
    ReadCnt++;
  }
  InVolt = InVolt / ReadCnt;
  // Смещение 0 для конкретного ADC (подобрать или отключить)
  //if (InVolt > 0.2) InVolt += 3;
  // Перевод в вольты (Value: 0..1023 -> (0..VRef) scaled by Mult)
  Volt = InVolt * VoltMult * VRef / 1023;

  NUMBER = (Volt * 10);  // float Volt умножается на 10 что бы переместить запятую
  Serial.println(Volt);   //и  преобразуется в int NUMBER (123.4 = 123)
  NUMBER3 = NUMBER % 10; //001
  NUMBER2 = NUMBER / 10; //010
  if (NUMBER2 >= 10) {
    NUMBER2 = (NUMBER / 10) % 10;
  }
  NUMBER1 = NUMBER / 100; //100
  if (NUMBER1 >= 10) {
    NUMBER1 = 0;
  }

  int NUMBERS[3] = {
    NUMBER1,
    NUMBER2,
    NUMBER3,
  };

  for (int i = 0; i <= 2; i++) {
    digitalWrite(RCLK, LOW); // открываем защелку
    shiftOut(DIO, SCLK, MSBFIRST,  chr[i]);  // включаем разряд
    if (i == 1) { //Включаем запятую второго разряда
      shiftOut(DIO, SCLK, MSBFIRST, digit[NUMBERS[i]] + ZPT);
    }
    else {
      shiftOut(DIO, SCLK, MSBFIRST, digit[NUMBERS[i]]);
    }
    digitalWrite(RCLK, HIGH); // защелкиваем регистры
    delay(1);
  }

  //Serial.println(NUMBER);
  //Serial.print("___");
  //Serial.print(NUMBER1);
  //Serial.print("___");
  //Serial.print(NUMBER2);
  //Serial.print("___");
  //Serial.println(NUMBER3);
  //delay(500);


}
