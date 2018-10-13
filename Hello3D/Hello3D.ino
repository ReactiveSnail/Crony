#define SCLK 4//7  // пины ардуины
#define RCLK 3//6
#define DIO 2//5

unsigned long timing; //Переменная для работы таймера

int NUMBER = 0;  //Счетчик для определения номера ячейки в столбце массива symbol[][]
int NUMBER1 = 0; //Переменная для хранения значения 1го разряда
int NUMBER2 = 0; //Переменная для хранения значения 2го разряда
int NUMBER3 = 0; //Переменная для хранения значения 3го разряда

int column = 10; //Количество столбцов массива
int line = 0;    //Переменная для выбора строки из массива

int funct = 2;   //Выбор функции (режима работы)
int del = 1000;  //Интервалл переключения индикатора
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
    0b00000001, //
    0b00000010, //
    0b00000100, //
    0b00001000, // _
    0b00010000, //
    0b00100000, //
    0b01000000, // -
    0b10000000, // .
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

void setup() {
  // put your setup code here, to run once:
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - timing >= del) {  // Таймер
    timing = millis();
    if (funct == 0) Anymation();
    if (funct == 1) HelloWorld();
    if (funct == 2) Crony();
    if (funct == 3) Ciferblat();

  }


  int NUMBERS[3] = { // Массив масок каждого разряда
    NUMBER1,
    NUMBER2,
    NUMBER3,
  };

  for (int i = 0; i <= 2; i++) {
    digitalWrite(RCLK, LOW); // открываем защелку
    shiftOut(DIO, SCLK, MSBFIRST,  chr[i]);  // включаем разряд
    shiftOut(DIO, SCLK, MSBFIRST, symbol[line][NUMBERS[i]] + other); //Отправляем байт-маску на вход регистра
    digitalWrite(RCLK, HIGH); // защелкиваем регистры
    delay(1);
  }

}

void Crony() {
  line = 2;
  del = random(1000, 5000); //Случайное время изменения индикации
  int EYES = random(0, 5);  //Случайная маска 0-5 столбец 
  int MOUTH = random (6, 9);//Случайная маска 6-9 столбец 
  NUMBER1 = EYES;
  NUMBER2 = MOUTH;
  NUMBER3 = EYES;

}

void HelloWorld() {
  //Эффект бегущей строки, N >> N1 >> N2 >> N3
  line = 0; //Выбор линии из массива с символами
  NUMBER = NUMBER + 1;
  // Получаем маски для каждого разряда
  // смещающиеся справа налево
  // (визуально, на индикаторе :)
  if (NUMBER == column)
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
  line = 1; //Выбор линии из массива с символами
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
  other = symbol[4][7]; //Включаем "-" на всех разрядах
  NUMBER = NUMBER + 1;
  if (NUMBER == 7) NUMBER = 1;
  //На все разряды выводим одинаковую маску
  NUMBER1 = NUMBER;
  NUMBER2 = NUMBER;
  NUMBER3 = NUMBER;
}

//Serial.print(NUMBER);
//Serial.print("___");
//Serial.print(NUMBER1);
//Serial.print("___");
//Serial.print(NUMBER2);
//Serial.print("___");
//Serial.println(NUMBER3);
//delay(500);




