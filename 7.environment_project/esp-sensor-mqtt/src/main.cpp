
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
// в данном случае предполагается, что реле включается высоким уровнем на выходе
const char lvlRelayOn  = 0x1;
const char lvlRelayOff = 0x0;

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

//for DHT11
#include <DHT.h> 
#define DHTPIN 32
#define RELEYPIN 2

// DHT dht(DHTPIN, DHT22); //Инициация датчика
DHT dht(DHTPIN, DHT11);

long lastMeasure = 0;
long lastMeasureBright = 0;

//for bright
#define BRIGHTPIN 33

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
const char* mqttTopicMyName     = "1863_SES_sensor";
const char* mqttTopicBase       = "gb_iot/1863_SES";

// Топик BrHumTempDeviceStatus для статуса устройства:
// - в рабочем состоянии в этом топике будет сообщение "online"
// - когда устройство по какой-либо причине отключится от сервера, сервер примерно через 60 секунд опубликует LWT "offline"
// TODO: добавить время к статусу on-line

//топики для устройства
const char* mqttTopicBrHumTempDeviceStatus       = "gb_iot/1863_SES/BrHumTempDevice";
const char* mqttTopicBrightCur                   = "gb_iot/1863_SES/BrHumTempDevice/Bright";
const char* mqttTopicHumidityCur                 = "gb_iot/1863_SES/BrHumTempDevice/Humadity";
const char* mqttTopicTempCur                     = "gb_iot/1863_SES/BrHumTempDevice/Temp";

const char* mqttTopicControlLED                 = "gb_iot/1863_SES/BrHumTempDevice/LED";
const char* mqttTopicControlShade               = "gb_iot/1863_SES/BrHumTempDevice/Shade";

//состояния для устройства
const char* mqttDeviceStatusOn       = "online";    
const char* mqttDeviceStatusOff      = "offline";
const int   mqttDeviceStatusQos      = 1;
const bool  mqttDeviceStatusRetained = true; 

// Топики для внешнего управления освещенностью
const char* mqttTopicControlBrightValue   = "gb_iot/1863_SES/BrHumTempDevice/Bright/controlLvl";

// пороговое значение для сенсора
uint sensorBright       = 0;

// Топики для внешнего управления реле
const char* mqttTopicControlRelay1   = "gb_iot/1863_SES/BrHumTempDevice/Bright/relay1/control";

const char* mqttTopicControlRelay2   = "demo/relay2/control";
const char* mqttTopicControlRelay3   = "demo/relay3/control";
const int   mqttRelayControlQos      = 0;

// Топик для публикации состояния реле
const char* mqttTopicStatusRelay1    = "gb_iot/1863_SES/BrHumTempDevice/Bright/relay1/status";

// Текстовое отображение для состояния реле
const char* mqttRelayStatusOn       = "1";
const char* mqttRelayStatusOff      = "0";

const char* mqttRelayStatusOn1       = "1";
const char* mqttRelayStatusOn2       = "on";
const char* mqttRelayStatusOff1      = "0";
const char* mqttRelayStatusOff2      = "off";
const bool  mqttRelayStatusRetained  = false; 

// Топики для публикации состояния цифровых входов
const bool  mqttInputStatusRetained  = false; 
const char* mqttInputStatusHigh      = "high";
const char* mqttInputStatusLow       = "low";

// Текущее состояние реле
char relayStatus1 = lvlRelayOff;
char relayStatus2 = lvlRelayOff;
char relayStatus3 = lvlRelayOff;

// Полученное с MQTT ("новое") состояние реле
char relayCommand1 = lvlRelayOff;
char relayCommand2 = lvlRelayOff;
char relayCommand3 = lvlRelayOff;

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

  if (_topic.equals(mqttTopicControlBrightValue)) {
    // это топик управления уровнем освещенности
    sensorBright = _payload.toInt();

    Serial.print("i'm see level for bright - ");
    Serial.print(sensorBright);
    Serial.println();

  };
}

//for MQTT reconect
boolean reconnectPub() {
  if (mqtt_client_pub.connect(mqttTopicMyName)) {
    // Once connected, publish an announcement...
    mqtt_client_pub.publish(mqttTopicBase,"hello there, sensor device reconnect");
    mqtt_client_pub.publish(mqttTopicBrHumTempDeviceStatus, mqttDeviceStatusOn);

  }
  return mqtt_client_pub.connected();
}

boolean reconnectSub() {
  if (mqtt_client_sub.connect(mqttTopicMyName)) {
      // ... and resubscribe
    mqtt_client_sub.subscribe("gb_iot/1863_SES/#",mqttRelayControlQos);
  }
  return mqtt_client_sub.connected();
}


void setup()
{
    // SPISettings(1000000, MSBFIRST, SPI_MODE0);
    Serial.begin(9600);

    //start DHT11
    dht.begin();

    //какбы нагрузка
    pinMode(RELEYPIN, OUTPUT);

    //for bright
    // pinMode(BRIGHTPIN, INPUT_PULLUP);
    // int value = analogRead(BRIGHTPIN);

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

    if(mqtt_client_pub.connect(mqttTopicMyName, mqttTopicBrHumTempDeviceStatus, mqttDeviceStatusQos, mqttDeviceStatusRetained,  mqttDeviceStatusOff))
    {
        mqtt_client_pub.publish(mqttTopicBrHumTempDeviceStatus, mqttDeviceStatusOn);

        Serial.print("mqtt_client_pub connect status: ");
        Serial.println(mqtt_client_pub.connected());

        mqtt_client_pub.publish(mqttTopicBase, "Hello! Sensor is here!");
    }

    if(mqtt_client_sub.connect(mqttTopicMyName))
    {
        Serial.print("mqtt_client_sub connect status: ");
        Serial.println(mqtt_client_sub.connected());
        
        Serial.print("mqtt_client_sub subscribe status: ");
        Serial.print("gb_iot/1863_SES/#");
        Serial.print(" = ");
        Serial.println(mqtt_client_sub.subscribe("gb_iot/1863_SES/#",mqttRelayControlQos));
    }

//таймер 
  // Create semaphore to inform us when the timer has fired
  timerSemaphore = xSemaphoreCreateBinary();

// Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 800, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 1000000, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

uint8_t rotation = 0;

void loop()
{
 long now = millis();
  if (now - lastMeasure > 5000) // 5 секунд задержки
  {
    lastMeasure = now;

    float h = dht.readHumidity(); //Измеряем влажность
    float t = dht.readTemperature(); //Измеряем температуру
    if (isnan(h) || isnan(t)) {  // Проверка. Если не удается считать показания, выводится «Ошибка считывания», и программа завершает работу
      Serial.println("Ошибка считывания");
      return;
    }
    // Serial.print("millis: ");
    // Serial.println(now);

    Serial.print("Влажность: ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Температура: ");
    Serial.print(t);
    Serial.println(" *C "); //Вывод показателей на экран

    String str_temp(t);
    String str_hum(h);

    mqtt_client_pub.publish(mqttTopicTempCur, str_temp.c_str());
    mqtt_client_pub.publish(mqttTopicHumidityCur, str_hum.c_str());
 }
  now = millis();
  if(now - lastMeasureBright > 5000)
  {
    lastMeasureBright = now;
    // Serial.print("millis: ");
    // Serial.println(now);

    int value = analogRead(BRIGHTPIN)/16;
    Serial.print("Bright: ");
    Serial.println(value);

    String str_bright(value);
    mqtt_client_pub.publish(mqttTopicBrightCur, str_bright.c_str());

    if(value+25 < sensorBright)
    {
      digitalWrite(RELEYPIN, HIGH);
      mqtt_client_pub.publish(mqttTopicStatusRelay1, mqttRelayStatusOn);      
    }
    else if(value-25 > sensorBright)
    {
      digitalWrite(RELEYPIN, LOW);
      mqtt_client_pub.publish(mqttTopicStatusRelay1, mqttRelayStatusOff);
    }
  }

    //mqtt look
  if (!mqtt_client_pub.connected()) {
    now = millis();
    if (now - lastReconnectAttemptPub > 5000) {
      lastReconnectAttemptPub = now;
      // Attempt to reconnect
      if (reconnectPub()) {
        lastReconnectAttemptPub = 0;
      }
    }
  } else {
    // Client connected

    //mqtt2 look
    mqtt_client_pub.loop();
  }

    if (!mqtt_client_sub.connected()) {
    long now = millis();
    if (now - lastReconnectAttemptSub > 5000) {
      lastReconnectAttemptSub = now;
      // Attempt to reconnect
      if (reconnectSub()) {
        lastReconnectAttemptSub = 0;
      }
    }
  } else {
    // Client connected

    mqtt_client_sub.loop();
  }
}
