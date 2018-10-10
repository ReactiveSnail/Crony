#define SCLK 4  // пины ардуины
#define RCLK 3
#define DIO 2

//Глазки
byte Eyes1 = 0b01100011; //"o_o"
byte Eyes2 = 0b00100011; //"n_n"
byte Eyes3 = 0b01100010; //"u_u"
byte Eyes4 = 0b01000001; //"=_="
byte Eyes5 = 0b01000000; //"-_-"
byte Eyes6 = 0b01000011; //">_>"
byte Eyes7 = 0b01100001; //"<_<"
byte Eyes8 = 0b00000001; //"-high -"

//Ротик
byte Mouth1 = 0b01011100; //"o"
byte Mouth2 = 0b01010100; //"n"
byte Mouth3 = 0b00011100; //"u"
byte Mouth4 = 0b01000000; //"-"
byte Mouth5 = 0b00001000; //"_"
byte Mouth6 = 0b01001100; //">"
byte Mouth7 = 0b01011000; //"<"
byte Mouth8 = 0b00111111; //"0"

int Face;
byte EYES;
byte MOUTH;

int DELL;
unsigned long timing; // Переменная для хранения точки отсчета

void setup() {
  pinMode(RCLK, OUTPUT);
  pinMode(SCLK, OUTPUT);
  pinMode(DIO, OUTPUT);
  //Serial.begin(9600);
}



void loop() {
  DELL = random(1000, 50000);
  if (millis() - timing > DELL) { // Вместо DELL подставьте нужное вам значение паузы
    timing = millis();
    Face = random(7);
  }

  if (Face == 1) {
    EYES = Eyes5;
    MOUTH = Mouth5;
  }
  if (Face == 2) {
    EYES = Eyes4;
    MOUTH = Mouth5;
  }
  if (Face == 3) {
    EYES = Eyes1;
    MOUTH = Mouth5;
  }
  if (Face == 4) {
    EYES = Eyes1;
    MOUTH = Mouth1;
  }
  if (Face == 0) {
    EYES = Eyes6;
    MOUTH = Mouth5;
  }
  if (Face == 5) {
    EYES = Eyes7;
    MOUTH = Mouth5;
  }

//Serial.println(Face);
//Serial.println(EYES);
//Serial.println(MOUTH);

//глазки
digitalWrite(RCLK, LOW); // открываем защелку
shiftOut(DIO, SCLK, MSBFIRST,  0b00000010);  // включаем разряд
shiftOut(DIO, SCLK, MSBFIRST, EYES);
digitalWrite(RCLK, HIGH); // защелкиваем регистры
//delay(1); // ждем немного перед отправкой следующего "числа"

//Ротик
digitalWrite(RCLK, LOW); // открываем защелку
shiftOut(DIO, SCLK, MSBFIRST,  0b00000101);  // включаем разряд
shiftOut(DIO, SCLK, MSBFIRST, MOUTH);
digitalWrite(RCLK, HIGH); // защелкиваем регистры
//delay(1); // ждем немного перед отправкой следующего "числа"

}
