#define SCLK_C 4 // пины ардуино для подключения Crony
#define RCLK_C 3
#define DIO_C  2

unsigned long timer0; //Переменная для работы таймера

int NUMBER = 0;  //Счетчик для определения номера ячейки в столбце массива symbol[][]
int NUMBER1 = 0; //Переменная для хранения значения 1го разряда
int NUMBER2 = 0; //Переменная для хранения значения 2го разряда
int NUMBER3 = 0; //Переменная для хранения значения 3го разряда

int column = 10; //Количество столбцов массива
int line = 0;    //Переменная для выбора строки из массива

int funct = 0;   //Выбор функции (режима работы)
int del = 1000;  //Интервалл переключения индикатора
int iIi  = 10;   //Переменная иИи для включения FloatPoint только на одном определенном разряде, на пример плавающую точку (12,3 - 1,23)
byte FloatPoint = 0b10000000; // h. Байт включаюший сегмент "h" индикатора
byte other = 0b00000000; //Дополнительный байт, позволяет
//модифицировать байт отправляемый
//на порт DIO и сдвиговый регистр

// маска для 7 сигментного индикатора
const byte symbol[5][10] = {
  {
    0b00000000, //
    0b01110110, // H
    0b01111001, // E
    0b00111000, // L
    0b00111000, // L
    0b00111111, // O
    0b00000000, //
    0b00000000, //
    0b01000000, // -
    0b00000000, //
  }, {
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
  }, {
    //Crony
    0b01100011, //"o_o"
    0b01000001, //"=_="
    0b01000000, //"-_-"
    0b01000011, //">_>"
    0b01100001, //"<_<"
    0b00000001, //"-high -"
    0b00001000, //"_"_
    0b01001000, //"="_
    0b01011100, //"o"_
    0b01000000, //"-"_
  }, {
    //"__________"
    0b01011100, //"o"
    0b01010100, //"n"
    0b00011100, //"u"
    0b01000000, //"-"
    0b00001000, //"_"
    0b01001100, //">"
    0b01011000, //"<"
    0b00111111, //"0"
    0b00000000, //
    0b00000000, //
  }, {
    //каждый сегмент в отдельности
    0b00000000, // пробел
    0b00000001, // a
    0b00000010, // b
    0b00000100, // c
    0b00001000, // d_
    0b00010000, // e
    0b00100000, // f
    0b01000000, // g-
    0b10000000, // h.
    0b00000000, //
  }
};
const byte chr[3] = { // маска для разряда 0, 1, 2
  0b00000110, //100
  0b00000101, //010
  0b00000011, //001
  //0b00000000, //111
  //0b00000111, //000
};
//********************

//Knopka Amperka
#define BUTTON_PIN  11
#define LED_PIN     10

boolean buttonWasUp = true;  // была ли кнопка отпущена?
boolean ledEnabled = false;  // включен ли свет?
long previousMillis = 0;
long previousMillis1 = 0;
//
int count = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(RCLK_C, OUTPUT);
  pinMode(SCLK_C, OUTPUT);
  pinMode(DIO_C, OUTPUT);

  //Knopka Amperka
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  line = 1;
  del = 100;
  other = symbol[4][0]; //Включаем "-" на всех разрядах
  iIi = 10;
  //***********************

  //Knopka Amperka
  // определить момент «клика» несколько сложнее, чем факт того,
  // что кнопка сейчас просто нажата. Для определения клика мы
  // сначала понимаем, отпущена ли кнопка прямо сейчас...
  boolean buttonIsUp = digitalRead(BUTTON_PIN);

  // ...если «кнопка была отпущена и (&&) не отпущена сейчас»...
  if (buttonWasUp && !buttonIsUp) {
    Serial.println("1");
    // ...может это «клик», а может и ложный сигнал (дребезг),
    // возникающий в момент замыкания/размыкания пластин кнопки,
    // поэтому даём кнопке полностью «успокоиться»...
    if (millis() - previousMillis >= 10) {
        previousMillis = millis();
        Serial.println("2");
        // ...и считываем сигнал снова
        buttonIsUp = digitalRead(BUTTON_PIN);
        Serial.println(buttonIsUp);
        if (!buttonIsUp) {
          Serial.println("3");
          // если она всё ещё нажата...
          //if (millis() - previousMillis1 >= 10) {
            //previousMillis1 = millis();
            buttonIsUp = digitalRead(BUTTON_PIN);
            Serial.println(buttonIsUp);
            for (int i; i <= 10000; i++) {
              count++;
            if (buttonIsUp) {
              count = 0;
              Serial.println("4");
              // ...это клик! Переворачиваем сигнал светодиода
              ledEnabled = !ledEnabled;
              digitalWrite(LED_PIN, ledEnabled);
              NUMBER++;
            }
          }
        }
      }
    }
   // if (buttonWasUp && buttonIsUp) {
    //  count = 0;
   // }

  // запоминаем последнее состояние кнопки для новой итерации
  buttonWasUp = buttonIsUp;
  Serial.print("count = ");
  Serial.println(count);


                //***********************
                // NUMBER = 123;  // float Volt умножается на 10 что бы переместить запятую
                //Serial.println();   //и  преобразуется в int NUMBER (123.4 = 123)
                NUMBER3 = NUMBER % 10; //001
                NUMBER2 = NUMBER / 10; //010
  if (NUMBER2 >= 10) {
  NUMBER2 = (NUMBER / 10) % 10;
  }
  NUMBER1 = NUMBER / 100; //100
  if (NUMBER1 >= 10) {
  NUMBER1 = 0;
}

int NUMBERS[3] = { // Массив масок каждого разряда
  NUMBER1,
  NUMBER2,
  NUMBER3,
};

int i;
for (i = 0; i <= 2; i++) {
  digitalWrite(RCLK_C, LOW); // открываем защелку
    shiftOut(DIO_C, SCLK_C, MSBFIRST,  chr[i]);  // включаем разряд

    if (i == iIi) { //Включаем запятую второго разряда
      shiftOut(DIO_C, SCLK_C, MSBFIRST, symbol[line][NUMBERS[i]] + FloatPoint);
    }
    else {
      shiftOut(DIO_C, SCLK_C, MSBFIRST, symbol[line][NUMBERS[i]] + other); //Отправляем байт-маску на вход регистра
    }
    digitalWrite(RCLK_C, HIGH); // защелкиваем регистры
    delay(1);
  }

}
