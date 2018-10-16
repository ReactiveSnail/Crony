#define SCLK_C 4 // пины ардуино для подключения Crony
#define RCLK_C 3
#define DIO_C  2

//Подключаем дисплей Nokia 5110 Синий
//Используем библиотеку «Adafruit_GFX_Library»
//https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
// Пины Arduino >> Контакты дисплея:____(Clk, Din, DC, CE, RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(5, 6, 7, 8, 9);
//******

//WoltMetr Вольтметр 0-50В..
// Время обновления показаний, мс (200-2000)
#define REFRESH_TIME  30
// Аналоговй вход
#define PIN_VOLT A0
// Внутреннее опорное напряжение (подобрать)
const float VRef = 1.08;
// Коэффициент входного резистивного делителя (Rh + Rl) / Rl. IN <-[ Rh ]--(analogInPin)--[ Rl ]--|GND
const float VoltMult = (103.8 + 10.430) / 10.430;
float InVolt, Volt;
//*******

//Цифровая кнопка
const int pin1 = 11; //кнопка
long previousMillis = 0;
boolean Lock = 0; //Защёлка/фиксатор для кнопки
//*******

int ledPin = 10;

unsigned long timer0; //Переменная для работы таймера
unsigned long timer1; //Переменная для работы таймера
unsigned long TiMer1; //Переменная для работы таймера

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

void setup() {
  // put your setup code here, to run once:
  pinMode(RCLK_C, OUTPUT);
  pinMode(SCLK_C, OUTPUT);
  pinMode(DIO_C, OUTPUT);

  // инициализация и установка параметров дисплея Nokia5110
  display.begin();
  //display.clearDisplay();
  //display.display();
  display.setContrast(58); // установка контраста
  display.setTextSize(1);  // установка размера шрифта
  display.setTextColor(BLACK); // установка цвета текста
  //********

  //WoltMetr ВолтМетр
  analogReference(INTERNAL);//Включаем внутряннее опорное напряжение
  pinMode(PIN_VOLT, INPUT);
  //********

  //Цифровая кнопка
  pinMode(pin1, INPUT_PULLUP);  // определяет pin1 как вход с внутренней подтяжкой
  pinMode(ledPin, OUTPUT);
  //********

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:

  //Цифровая кнопка от TimeMachine();
  if  (digitalRead(pin1) == LOW ) {        //если pin1 включили,
    if (millis() - previousMillis > 100) { //то ждем 100мс для
      previousMillis = millis();           //отсечки дребезга контактов
      //funct++;                             // и в переменную прибавим один
      Lock = !Lock;   //Стоп - Пуск TimeMachine();
      digitalWrite(ledPin, LOW);
    }
  }
  //TimeMachine();
  if (millis() - timer1 > 100) {
    TiMer1 = TiMer1 + 1;
    if (TiMer1 == 256) TiMer1 = 1;
    timer1 = millis();
     //digitalWrite(ledPin, LOW);
  }
  //Выделятор интервалов времени
  if (TiMer1 >= 1 && TiMer1 <= 2 && !Lock) digitalWrite(ledPin, HIGH);
  if (TiMer1 >= 3 && TiMer1 <= 50 && !Lock) funct = 0;
  if (TiMer1 >= 51 && TiMer1 <= 52 && !Lock) digitalWrite(ledPin, HIGH);
  if (TiMer1 >= 53 && TiMer1 <= 100 && !Lock) funct = 1;
  if (TiMer1 >= 101 && TiMer1 <= 102 && !Lock) digitalWrite(ledPin, HIGH);
  if (TiMer1 >= 103 && TiMer1 <= 150 && !Lock) funct = 2;
  if (TiMer1 >= 151 && TiMer1 <= 152 && !Lock) digitalWrite(ledPin, HIGH);
  if (TiMer1 >= 153 && TiMer1 <= 200 && !Lock) funct = 3;
  if (TiMer1 >= 201 && TiMer1 <= 202 && !Lock) digitalWrite(ledPin, HIGH);
  if (TiMer1 >= 203 && TiMer1 <= 255 && !Lock) funct = 4;
  //if (TiMer1 >= 318 && TiMer1 <= 433 && !Lock) funct = 5;
  //if (TiMer1 >= 534 && TiMer1 <= 687 && !Lock) funct = 6;


  if (millis() - timer0 >= del) {  // Таймер //Переключатель, галетный, програмный...%)
    timer0 = millis();
    if (funct == 0) {
      HelloWorld();
    }
    if (funct == 1) {
      Anymation();
    }
    if (funct == 2) {
      Crony();
    }
    if (funct == 3) {
      Ciferblat();
    }
    if (funct == 4) {
      WoltMetr();
    }
    //if (funct == 5) {
      // TimeMachine();
    //}
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

  if (funct == 5) {
    funct = 0;
  }
  Nokia(); //Ввводим информацию на дисплей Nokia 5110
  //Serial.print(NUMBER1);
  //Serial.print("___");
  //Serial.print(NUMBER2);
  //Serial.print("___");
  //Serial.println(NUMBER3);

}

void Crony() {
  line = 2;
  del = random(1000, 5000); //Случайное время изменения индикации
  other = symbol[4][0]; //Включаем "-" на всех разрядах
  iIi = 10;
  int EYES = random(0, 5);  //Случайная маска 0-5 столбец
  int MOUTH = random (6, 9);//Случайная маска 6-9 столбец
  NUMBER1 = EYES;
  NUMBER2 = MOUTH;
  NUMBER3 = EYES;

}

void HelloWorld() {
  //Эффект бегущей строки, N >> N1 >> N2 >> N3
  del = 1000;
  line = 0; //Выбор линии из массива с символами
  other = symbol[4][0]; //Включаем "-" на всех разрядах
  iIi = 10;
  NUMBER = NUMBER + 1;
  // Получаем маски для каждого разряда
  // смещающиеся справа налево
  // (визуально, на индикаторе :)
  if (NUMBER >= column)
    NUMBER = 0;
  NUMBER1 = NUMBER;
  NUMBER2 = NUMBER1 + 1;
  if (NUMBER2 == column)
    NUMBER2 = 0;
  NUMBER3 = NUMBER2 + 1;
  if (NUMBER3 == column)
    NUMBER3 = 0;
}

void Ciferblat() {
  // Счетчик
  del = 1000;
  other = symbol[4][0]; //Включаем "-" на всех разрядах
  line = 1; //Выбор линии из массива с символами
  iIi = 10;
  NUMBER = NUMBER + 1;         //Счетчик 1-999
  if (NUMBER == 1000)
    NUMBER = 0;
  //Получаем маску для каждого разряда
  NUMBER3 = NUMBER % 10;       //001
  NUMBER2 = NUMBER / 10;       //010
  if (NUMBER2 >= 10) {
    NUMBER2 = (NUMBER / 10) % 10;
  }
  NUMBER1 = NUMBER / 100;      //100
  if (NUMBER1 >= 10) {
    NUMBER1 = 0;
  }
}

void Anymation() {
  //Анимация, 6 сегментов загораются и гаснут по очереди,
  //создавая эффект движения по кругу
  line = 4;
  del = 78;
  iIi = 10;
  other = symbol[4][7]; //Включаем "-" на всех разрядах
  NUMBER = NUMBER + 1;
  if (NUMBER >= 7) NUMBER = 1;
  //На все разряды выводим одинаковую маску
  NUMBER1 = NUMBER;
  NUMBER2 = NUMBER;
  NUMBER3 = NUMBER;
}

void WoltMetr() {
  line = 1;
  del = 300;
  other = symbol[4][0]; //Включаем "-" на всех разрядах
  iIi = 1;
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

  NUMBER = (Volt * 100);  // float Volt умножается на 10 что бы переместить запятую
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
}

void Nokia() {
  //Ввводим информацию на дисплей Nokia 5110
  display.clearDisplay();//Очистка дисплея
  //Для наглядности выводим состояние разных переменных

  display.setCursor(0, 0); // установка позиции курсора
  display.print("funct...");
  display.setCursor(53, 0);
  display.print(funct);

  display.setCursor(35, 10); // установка позиции курсора
  display.print("N3 N2 N1"); //NUMBER
  display.setCursor(0, 20); // установка позиции курсора
  display.print(Volt); //NUMBER
  display.setCursor(35, 20);
  display.print(NUMBER3);
  display.setCursor(53, 20);
  display.print(NUMBER2);
  display.setCursor(71, 20);
  display.print(NUMBER1);

  display.setCursor(0, 30); // установка позиции курсора
  display.print("del..");
  display.setCursor(35, 30);
  display.print(del);

  display.display();
}

void allCikl() {
  int CNT;
  unsigned long timing1;
  if (millis() - timing1 > 100) {
    CNT = CNT + 1;
    if (CNT == 288) CNT = 1;
    timing1 = millis();
  }
  if (CNT >= 1 && CNT <= 15) Anymation();
  if (CNT >= 16 && CNT <= 117) Crony();
  if (CNT >= 118 && CNT <= 133) Anymation();
  if (CNT >= 134 && CNT <= 287) Ciferblat();
}

//Serial.print(NUMBER);
//Serial.print("___");
//Serial.print(NUMBER1);
//Serial.print("___");
//Serial.print(NUMBER2);
//Serial.print("___");
//Serial.println(NUMBER3);
//delay(500);




