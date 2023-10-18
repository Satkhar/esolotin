/*описание программы.
 * вывод и отрисовка значения резистора - постоянное.
 * измерение датчика I2C (гироскопа) по таймеру ( ?? мс)
 * по нажатию кнопки - отправка в режим sleep.
 * 
*/


//для записи на экран
#include <TFT.h>
#include <SPI.h>

#define cs   10
#define dc   9
#define rst  8

//для гироскопа
#include <Wire.h>

//для прерываний/таймера
#include <GyverTimers.h>

//для энергосбережения
#include <avr/sleep.h>

TFT TFTscreen = TFT(cs, dc, rst);

// позиция линии, отображающеей сопротивление, на экране
int xPos = 0;
int xArr[136]={0};
int drawHeight;
//состояние кнопки
uint8_t buttonState = 0;

//переменная для гиро
const int adxl345 = 0x53;
uint8_t gyro_flag = 0;

char gyroPrintout_x[5];
char gyroPrintout_y[5];
char gyroPrintout_z[5];

uint8_t test = 0;

void setup() {
  //вход кнопки
  pinMode(2, INPUT_PULLUP); 

  //прерывание по нажатию кнопки
  //buttontick - назваение ф-ии прерывания
  attachInterrupt(digitalPinToInterrupt(2), buttontick, FALLING);

//таймер 
  Timer1.setPeriod(1000000);  //1 сек
  Timer1.enableISR(CHANNEL_A);

  Wire.begin();
  // запишем адрес регистра DEVID
  Wire.beginTransmission(adxl345);
  Wire.write(byte(0x00));
  Wire.endTransmission();

  // прочитаем регистр DEVID:
  Wire.requestFrom(adxl345, 1);
  while (Wire.available()) {
  byte c = Wire.read();
  Serial.print("ID = ");
  Serial.println(c, HEX);
  }

  // переведём акселерометр в режим измерений
  Wire.beginTransmission(adxl345);
  Wire.write(byte(0x2D));
  Wire.write(byte(0x08));
  Wire.endTransmission();
  
  
  //для отладки выводим в порт
  Serial.begin(9600);

  // инициализируем дисплей
  TFTscreen.begin();

  display_setup(); 

}

void buttontick()
{
  buttonState = !buttonState;
}

ISR(TIMER1_A)
{
  gyro_flag = 1;  
}

void loop() {

  if(buttonState)
    goToSleep();
  
  if(gyro_flag)
  {
  // запишем адрес начала данных по осям X, Y и Z:
  Wire.beginTransmission(adxl345);
  Wire.write(byte(0x32));
  Wire.endTransmission();

  // прочитаем 6 байтов значений XYZ:
  int i = 0;
  byte xyz[6];
  Wire.requestFrom(adxl345, 6);
  while (Wire.available()) {
    byte c = Wire.read();
    xyz[i] = c;
    i++;
  }
    //erase old value on display
    TFTscreen.stroke(252, 255, 255);
    TFTscreen.text(gyroPrintout_x, 50, 35);
    TFTscreen.text(gyroPrintout_y, 50, 45);
    TFTscreen.text(gyroPrintout_z, 50, 55);
    
    // посчитаем и отобразим значения X, Y, Z:
    int x = word(xyz[1], xyz[0]);
    int y = word(xyz[3], xyz[2]);
    int z = word(xyz[5], xyz[4]);
    
    String(x).toCharArray(gyroPrintout_x, 5);
    String(y).toCharArray(gyroPrintout_y, 5);
    String(z).toCharArray(gyroPrintout_z, 5);
    
    update_disp_val();
    gyro_flag = 0;
  }
  //обнуляем пред знач
    TFTscreen.stroke(255, 255, 255);  //цвет

    TFTscreen.rect(xPos+1, xArr[xPos+1], 2, 2);

  
  // считываем показания с потенциометра, и масштабируем его к половине высоты экрана
  int sensor = analogRead(A0);
  drawHeight = map(sensor, 0, 1023, 2, TFTscreen.height()/2);

  // рисуем линию 
  TFTscreen.stroke(0, 0, 0);  //цвет
//  xArr[xPosVal] = TFTscreen.height() - drawHeight;

  TFTscreen.rect(xPos, TFTscreen.height() - drawHeight, 2, 2);
  xArr[xPos] = TFTscreen.height() - drawHeight;

  // если линия подходит к краю экрана - обновляем экран
  if (xPos >= 135) {
      xPos = 0; //экран 3..127
//      display_setup();  
//      update_disp_val();  
  } else {
    // инкрементируем горизонтальную позицию
    xPos++;
    delay(16);  //это скорость вывода новой точки.
  }
}

void goToSleep()
{
  TFTscreen.background(255, 255, 255);
  TFTscreen.stroke(0, 255, 0);
  TFTscreen.text("Nice.", 6, 10);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text("I'm going to sleep", 12, 20);
  TFTscreen.stroke(0, 100, 200);
  TFTscreen.text("finally", 24, 30);
  delay(2000);
  
  TFTscreen.background(255, 255, 255);
  TFTscreen.stroke(255, 0, 0);
  TFTscreen.text("Go away.", 6, 10);
  TFTscreen.stroke(0, 255, 0);
  TFTscreen.text("I'm sleeping", 12, 20);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text("...or push button", 18, 50);
//  Serial.println(test);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); 
  sleep_enable();
  sleep_mode(); // включить энергосберегающий режим
  
  TFTscreen.background(255, 255, 255);
  TFTscreen.stroke(255, 0, 0);
  TFTscreen.text("Okey.", 6, 10);
  TFTscreen.stroke(0, 255, 100);
  TFTscreen.text("I'm wake up", 12, 20);
  TFTscreen.stroke(0, 0, 0);
  TFTscreen.text("...wait", 18, 30);
  
  xPos = 0;
  delay(2500);
  display_setup();
}

void display_setup()
{
    TFTscreen.background(255, 255, 255);

    TFTscreen.stroke(255, 0, 0);
    TFTscreen.text("Twist resistor", 12, 5);
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.text("or ", 18, 15);
    TFTscreen.stroke(0, 0, 255);
    TFTscreen.text("push button", 12, 25);
}

void update_disp_val()
{
    TFTscreen.stroke(0, 0, 0);
    TFTscreen.text("Gyro x:", 5, 35);
    TFTscreen.text(gyroPrintout_x, 50, 35);

    TFTscreen.text("Gyro y:", 5, 45);
    TFTscreen.text(gyroPrintout_y, 50, 45);
  
    TFTscreen.text("Gyro z:", 5, 55);
    TFTscreen.text(gyroPrintout_z, 50, 55);
}
