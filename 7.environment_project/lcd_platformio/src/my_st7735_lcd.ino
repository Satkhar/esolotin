/*

// сделать:
// - когда устройство по какой-либо причине отключится от сервера, сервер примерно через 60 секунд опубликует "offline"
// уход устройства в оффлайн - режим работы при отключении от mqtt

*/
/**
 *   Nano/Atmega328 PINS: connect LCD to D5 (D/C), D4 (CS), D3 (RES), D11(DIN), D13(CLK)
 *   Attiny SPI PINS:     connect LCD to D4 (D/C), GND (CS), D3 (RES), D1(DIN), D2(CLK)
 *   ESP8266: connect LCD to D1(D/C), D2(CS), RX(RES), D7(DIN), D5(CLK)
 */

//for debug
// #define DEBUG_MAKET

//for pins
#include "my_pins.h"

// Логические уровни для включения реле
// реле включается высоким уровнем на выходе
const char lvlOn  = 0x1; 
const char lvlOff = 0x0;

//for all
#include <Arduino.h>

//для прерываний/таймера
#include <esp32-hal-timer.h>

hw_timer_t *timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

volatile uint32_t isrCounter = 0;
volatile uint32_t lastIsrAt = 0;

#include "ssd1306.h"
#include "nano_engine.h"

//for SPI
#include <SPI.h>
#include <Wire.h>

//for wifi
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "my_pass.h"

//for mqtt
#include "PubSubClient.h"
#include "my_mqtt.h"        //key+cert+CA

WiFiClientSecure net_client_pub;
WiFiClientSecure net_client_sub;

PubSubClient mqtt_client_pub;
PubSubClient mqtt_client_sub;

const char* mqttServer   = "mqtt.cloud.yandex.net";
const int   mqttPort     = 1883;
const int   mqttPortTLS  = 8883;

// Топики для коннекта
const char* mqttTopicMyName     = "1863_SES";
const char* mqttTopicBase       = "gb_iot/1863_SES";
// const char* mqttTopicControlRelay3   = "demo/relay3/control";

const char* mqttTopicLcdDeviceStatus    = "gb_iot/1863_SES/LcdDevice";
const char* mqttDeviceStatusOn       = "online";    
const char* mqttDeviceStatusOff      = "offline";
const int   mqttDeviceStatusQos      = 1;
const bool  mqttDeviceStatusRetained = true; 

// Топик для внешнего управления освещенностью
const char* mqttTopicControlBrightValue   = "gb_iot/1863_SES/BrHumTempDevice/Bright/controlLvl";

//топики для устройства
const char* mqttTopicBrHumTempDeviceStatus       = "gb_iot/1863_SES/BrHumTempDevice/";
const char* mqttTopicBrightCur                   = "gb_iot/1863_SES/BrHumTempDevice/Bright";
const char* mqttTopicHumidityCur                 = "gb_iot/1863_SES/BrHumTempDevice/Humadity";
const char* mqttTopicTempCur                     = "gb_iot/1863_SES/BrHumTempDevice/Temp";

// Текущее состояние сенсора
char sensorStatus1 = lvlOff;

// текущее состояние нагрузки
char* StatusRelay1 = "off";

// Текущее значение сенсора
uint sensorBright       = 0;
float sensorHumadity    = 0;
float sensorTemp        = 0;

// Топик для внешнего управления реле
const char* mqttTopicControlRelay1   = "gb_iot/1863_SES/BrHumTempDevice/Bright/relay1/control";

const int   mqttRelayControlQos      = 0;

// Топик для публикации состояния реле
const char* mqttTopicStatusRelay1    = "gb_iot/1863_SES/BrHumTempDevice/Bright/relay1/status";


// Текстовое отображение для состояния реле
const char* mqttRelayStatusOn1       = "1";
const char* mqttRelayStatusOn2       = "on";
const char* mqttRelayStatusOff1      = "0";
const char* mqttRelayStatusOff2      = "off";
const bool  mqttRelayStatusRetained  = false; 

// Топики для публикации состояния цифровых входов
const char* mqttTopicStatusInput1    = "demo/input1/status";
const bool  mqttInputStatusRetained  = false; 
const char* mqttInputStatusHigh      = "high";
const char* mqttInputStatusLow       = "low";

// Топики для публикации температуры и влажности
const char* mqttTopicTemperature     = "demo/dht22/temperature";
const char* mqttTopicHumidity        = "demo/dht22/humidity";
const bool  mqttSensorsRetained      = false; 

// Текущее состояние реле
char relayStatus1 = lvlOff;
char relayStatus2 = lvlOff;
char relayStatus3 = lvlOff;

// Полученное с MQTT ("новое") состояние реле
char relayCommand1 = lvlOff;
char relayCommand2 = lvlOff;
char relayCommand3 = lvlOff;

long lastReconnectAttemptPub = 0;
long lastReconnectAttemptSub = 0;
/*
"standarts" ESP32 pins to SPI
D5      -   CS              //define in setup()
D18     -   SCK             //define in pins_arduino.h
D21     -   A0 (or D/C)     //define in setup()
D22     -   RES             //define in setup()
D23     -   SDA             //define in  pins_arduino.h
*/

/*
my ESP32 pins to SPI
look at my_pins.h
*/

//for encoder
#include <EncButton.h>
EncButton<EB_TICK, 13, 4, 15> enc;  // энкодер с кнопкой <A, B, KEY>

int16_t enc_count_old = 0;
uint8_t press = 0;
uint8_t update_lcd = 0;

SAppMenu menu;
SAppMenu menuVal;

//for menu
typedef enum 
{
     base,
     bright,
     view,
     more   
}eNumberMenu;

eNumberMenu NumberMenu = base;

//init value
struct 
{
     uint8_t light_level = 100;
     uint8_t cur_value = 100;
     uint8_t new_value = 100;
}my_param;

uint8_t new_action = 0;

const char *menuItems[] =
{
    //max num symbols in round. 19
    "Base menu",           // 0 item
    "Set lvl bright",      // 1 item
    "View from sensor",      // 2 item
};

NanoEngine8 engine;

void textDemo()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(0,  8, "Normal text", STYLE_NORMAL);
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    ssd1306_printFixed8(0, 16, "bold text?", STYLE_BOLD);
    ssd1306_setColor(RGB_COLOR8(0,255,255));
    ssd1306_printFixed8(0, 24, "Italic text?", STYLE_ITALIC);
    ssd1306_negativeMode();
    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(0, 32, "Inverted bold?", STYLE_BOLD);
    ssd1306_positiveMode();
    delay(3000);
}

void lcdLight()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(0,  8, "Select bright level 0..255", STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    char view_val = (char)(my_param.new_value);       
    ssd1306_printFixed8(0,  16, &view_val, STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(0,255,0));
    view_val = (char)(my_param.cur_value);       
    ssd1306_printFixed8(0,  24, &view_val, STYLE_NORMAL);

    if(enc.counter != enc_count_old)
    {
        enc.counter > enc_count_old ? my_param.new_value++ : my_param.new_value--;
    } 
}

void drawLinesDemo()
{
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    for (uint8_t y = 0; y < ssd1306_displayHeight(); y += 8)
    {
        ssd1306_drawLine8(0,0, ssd1306_displayWidth() -1, y);
    }
    ssd1306_setColor(RGB_COLOR8(0,255,0));
    for (uint8_t x = ssd1306_displayWidth() - 1; x > 7; x -= 8)
    {
        ssd1306_drawLine8(0,0, x, ssd1306_displayHeight() - 1);
    }
    delay(3000);
}

void look_enc()
{
    //encoder
    // опрос происходит здесь
    enc.tick();                     

  // =============== ЭНКОДЕР ===============
  // обычный поворот
  if (enc.turn()) 
  {
    // Serial.println("turn");

    // можно ещё:
    // Serial.println(enc.counter);  // вывести счётчик
    // Serial.println(enc.fast());   // проверить быстрый поворот
    // Serial.println(enc.dir());    // вывести направление поворота
    new_action = 1;
  }

  // =============== КНОПКА ===============
  if (enc.press()) 
  {
    press = 1;
    new_action = 1;
  }
}

  // =============== обновляем меню ===============

void lcd_menu_base()
{
    ssd1306_clearScreen8( );
    ssd1306_setColor(RGB_COLOR16(255,255,255));
    ssd1306_showMenu8(&menu);

    if(enc.counter != enc_count_old)
    {
        enc.counter > enc_count_old ? ssd1306_menuDown(&menu) : ssd1306_menuUp(&menu);
        ssd1306_showMenu8(&menu);
    } 

    //choose menu to display
    if(press)
    {
        switch (ssd1306_menuSelection(&menu))
    {
        case 0:
            new_action = 1;
            NumberMenu = base;
            break;

        case 1:
            new_action = 1;
            NumberMenu = bright;
            break;

        case 2:
            new_action = 1;
            NumberMenu = view;
            break;

        case 3:
            new_action = 1;
            NumberMenu = more;
            break;

        default:
            new_action = 1;
            NumberMenu = base;
            break;
    }
    press = 0;
            //   if (!press) Serial.println("press 0");
    ssd1306_clearScreen8( );
    ssd1306_setColor(RGB_COLOR16(255,255,255));
    ssd1306_showMenu8(&menu);
    }
    enc_count_old = enc.counter;
}

void lcd_menu_bright()
{
    char view_val[4] = {0};

    if(enc.counter != enc_count_old)
    {
        enc.counter > enc_count_old ? my_param.new_value++ : my_param.new_value--;
        enc_count_old = enc.counter;
    } 

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(2,  8, "Select bright level", STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,0));
    ssd1306_printFixed8(32,  16, "0..255", STYLE_NORMAL);

    ssd1306_negativeMode();
    ssd1306_setColor(RGB_COLOR8(255,255,255));

    myUint8ToCharArr(my_param.new_value, view_val);
    ssd1306_printFixed8(40,  24, view_val, STYLE_BOLD);
    ssd1306_positiveMode();

    ssd1306_setColor(RGB_COLOR8(0,255,0));
    ssd1306_printFixed8(8,  32, "setted value ", STYLE_NORMAL);
    myUint8ToCharArr(my_param.cur_value, view_val);
    ssd1306_printFixed8(40,  40, view_val, STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(0,255,255));
    ssd1306_printFixed8(8,  48, "from sensor now ", STYLE_NORMAL);
    myUint8ToCharArr(sensorBright, view_val);
    ssd1306_printFixed8(40,  56, view_val, STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(8,  64, "reley now ", STYLE_NORMAL);
    myUint8ToCharArr(sensorBright, view_val);
    ssd1306_printFixed8(40,  72, StatusRelay1, STYLE_NORMAL);

    //press
    if(press)
    {
        //
        my_param.cur_value = my_param.new_value; 
        press = !press;
        NumberMenu = base;
        new_action = 1;
        myUint8ToCharArr(my_param.new_value, view_val); 
        mqtt_client_pub.publish(mqttTopicControlBrightValue, view_val, true) ;
        Serial.print("mqtt_client publish ");
        Serial.println(view_val);
    }
}

void lcd_menu_view()
{
    char view_val[4] = {0};
    String _sensorHumadity(sensorHumadity);
    String _sensorTemp(sensorTemp);

    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(8,  8, "Bright", STYLE_NORMAL);
    myUint8ToCharArr(sensorBright, view_val);
    ssd1306_printFixed8(8,  16, view_val, STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(8,  32, "Humadity", STYLE_NORMAL);
    ssd1306_printFixed8(8,  40, _sensorHumadity.c_str(), STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(8,  56, "Temp", STYLE_NORMAL);
    ssd1306_printFixed8(8,  64, _sensorTemp.c_str(), STYLE_NORMAL);

    ssd1306_setColor(RGB_COLOR8(255,255,255));
    ssd1306_printFixed8(8,  72, "reley now ", STYLE_NORMAL);
    myUint8ToCharArr(sensorBright, view_val);
    ssd1306_printFixed8(40,  80, StatusRelay1, STYLE_NORMAL);

    //press
    if(press)
    {
        press = !press;
        NumberMenu = base;
        new_action = 1;
    }
}

void lcd_menu_reconnect()
{
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    ssd1306_clearScreen8();
    ssd1306_setColor(RGB_COLOR8(255,0,0));
    ssd1306_printFixed8(40,  32, "mqtt reconnect now, wait", STYLE_BOLD);
}

void myUint8ToCharArr(uint8_t input, char* outputChar)
{

    for(uint8_t i = 0; i < 3 ;i++)
    {
        outputChar[i] = '0';
    }

    // outputChar[3]='\n';
    outputChar[2]=input%10 + '0';
    input = input/10;
    outputChar[1]=input%10 + '0';
    input = input/10;
    outputChar[0]=input%10 + '0'; 
    return;
}

// =============== для таймера ===============
void ARDUINO_ISR_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  isrCounter++;
  lastIsrAt = millis();
  portEXIT_CRITICAL_ISR(&timerMux);
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output
}

//callBack for reconnect MQTT
void callbackMqtt(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  // Для более корректного сравнения строк приводим их к нижнему регистру и обрезаем пробелы с краев
  String _payload;
  for (unsigned int i = 0; i < length; i++) {
    _payload += String((char)payload[i]);
  };
  _payload.toLowerCase();
  _payload.trim();

  // Вывод поступившего сообщения в лог, больше никакого смысла этот блок кода не несет, можно исключить
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.print(_payload.c_str());
  Serial.println();

  // Сравниваем с топиками
  String _topic(topic);

  if (_topic.equals(mqttTopicBrHumTempDeviceStatus)) 
  {
    // это топик управления уровнем освещенности
    if (_payload.equals(mqttDeviceStatusOn)) {
      relayStatus1 = lvlOn;
    };
    if (_payload.equals(mqttDeviceStatusOff)) {
      relayStatus1 = lvlOff;
    };

  } else if (_topic.equals(mqttTopicBrightCur)) {
    // это топик значения освещенности
    sensorBright = _payload.toInt();

  } else if (_topic.equals(mqttTopicStatusRelay1)) {
    // это топик управления реле 1

    if (_payload.equals(mqttRelayStatusOn1) || _payload.equals(mqttRelayStatusOn2)) {
      StatusRelay1 = "on";
    };
    if (_payload.equals(mqttRelayStatusOff1) || _payload.equals(mqttRelayStatusOff2)) {
      StatusRelay1 = "off";
    };
  } else if (_topic.equals(mqttTopicHumidityCur)) {
    // это топик значения влажности
    sensorHumadity = _payload.toFloat();
  } else if (_topic.equals(mqttTopicTempCur)) {
    // это топик значения температуры
    sensorTemp = _payload.toFloat();
  } else {
    Serial.println("Failed to recognize incoming topic!");
  };
}

//for MQTT reconect
boolean reconnectPub() {
  if (mqtt_client_pub.connect(mqttTopicMyName)) {
    // Once connected, publish an announcement...
    mqtt_client_pub.publish(mqttTopicBase,"hello there, lcd device reconnect");
    mqtt_client_pub.publish(mqttTopicBrHumTempDeviceStatus, mqttDeviceStatusOn);

  }
  new_action = 1;
  return mqtt_client_pub.connected();
}

boolean reconnectSub() {
  if (mqtt_client_sub.connect(mqttTopicMyName)) {
      // ... and resubscribe
    mqtt_client_sub.subscribe("gb_iot/1863_SES/#",mqttRelayControlQos);
  }
  new_action = 1;
  return mqtt_client_sub.connected();
}

void setup()
{
    // SPISettings(1000000, MSBFIRST, SPI_MODE0);
    Serial.begin(9600);

    //for WiFi
    WiFi.begin(WIFISSID, WIFIPASS);

    while(WiFi.status() != wl_status_t::WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConnected to WiFi");

    //setup for publication
    net_client_pub.setCACert(rootCA);
    net_client_pub.setCertificate(cert_devices);
    net_client_pub.setPrivateKey(key_devices);
    mqtt_client_pub.setClient(net_client_pub);
    mqtt_client_pub.setServer(mqttServer, mqttPortTLS);

    //setup for subscibe
    net_client_sub.setCACert(rootCA);
    net_client_sub.setCertificate(cert_registers);
    net_client_sub.setPrivateKey(key_registers);
    mqtt_client_sub.setClient(net_client_sub);
    mqtt_client_sub.setServer(mqttServer, mqttPortTLS);
    mqtt_client_sub.setCallback(callbackMqtt);

    lastReconnectAttemptPub = 0;
    lastReconnectAttemptSub = 0;

    Serial.println("\nConnected mqtt_client_pub");

    if(mqtt_client_pub.connect(mqttTopicMyName))
    {
        mqtt_client_pub.publish(mqttTopicLcdDeviceStatus, mqttDeviceStatusOn);

        Serial.print("mqtt_client_pub connect status: ");
        Serial.println(mqtt_client_pub.connected());

        mqtt_client_pub.publish(mqttTopicBase, "Hello! LCD is here!");
    }

    Serial.println("\nConnected mqtt_client_sub");

    if(mqtt_client_sub.connect(mqttTopicMyName))
    {
        Serial.print("mqtt_client_sub connect status: ");
        Serial.println(mqtt_client_sub.connected());
        
        Serial.print("mqtt_client_sub subscribe status: ");
        Serial.print("gb_iot/1863_SES/#");
        Serial.print(" = ");
        Serial.println(mqtt_client_sub.subscribe("gb_iot/1863_SES/#",mqttRelayControlQos));
    }

    //for LCD
    ssd1306_setFixedFont(ssd1306xled_font6x8);
    // int8_t rstPin, int8_t cesPin, int8_t dcPin
    // st7735_128x160_spi_init(3, 4, 5);       
   st7735_128x160_spi_init(RES_LCD, CS_LCD, A0_LCD); // Use this line for ESP32 (VSPI)  (gpio22=RST, gpio5=CE for VSPI, gpio21=D/C)

    // RGB functions do not work in default SSD1306 compatible mode
    ssd1306_setMode( LCD_MODE_NORMAL );
    ssd1306_clearScreen8( );
    ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
    ssd1306_showMenu8( &menu );

//таймер 
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

// Use 1st timer of 4 (counted from zero).
  // Set 800 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 800, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 100000, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

uint8_t rotation = 0;

void loop()
{
  long now = millis();

  //encoder
  // опрос энкодера происходит здесь
  look_enc();                     

  if (new_action)
  {
    uint32_t isrCount = 0, isrTime = 0;
    // Read the interrupt count and time
    portENTER_CRITICAL(&timerMux);
    isrCount = isrCounter;
    isrTime = lastIsrAt;
    portEXIT_CRITICAL(&timerMux);
    // Print it
    // Serial.print("onTimer no. ");
    // Serial.print(isrCount);
    // Serial.print(" at ");
    // Serial.print(isrTime);
    // Serial.println(" ms");
    new_action = 0;
      
    switch(NumberMenu)
    {
      case 0:
        lcd_menu_base();
      break;
      case 1:
        lcd_menu_bright();
      break;
      case 2:
        lcd_menu_view();
      break;
      case 3:
              // lcd_menu_more();
      break;
      default:
        lcd_menu_base();
      break;
    }        
  }

  if ((xSemaphoreTake(timerSemaphore, 0) == pdTRUE))
  {
      //mqtt look
    if (!mqtt_client_pub.connected()) 
    {
      lcd_menu_reconnect();
      now = millis();

      if (now - lastReconnectAttemptPub > 5000) 
      {
        lastReconnectAttemptPub = now;
        Serial.println("mqtt_client_pub try reconnect");
        // Attempt to reconnect
        if (reconnectPub()) 
        {
          lastReconnectAttemptPub = 0;
          Serial.println("mqtt_client_pub is reconnected");
        }
      }
    } else {
      // Client connected
      //mqtt2 look
      mqtt_client_pub.loop();    
    }

      if (!mqtt_client_sub.connected()) 
      {
      lcd_menu_reconnect();
      now = millis();
      if (now - lastReconnectAttemptSub > 5000) 
      {
        lastReconnectAttemptSub = now;
        Serial.println("mqtt_client_sub try reconnect");
        // Attempt to reconnect
        if (reconnectSub()) 
        {
          lastReconnectAttemptSub = 0;
          Serial.println("mqtt_client_sub is reconnected");
        }
      }
    } else {
      // Client connected
      mqtt_client_sub.loop();    
    }
  }
}
