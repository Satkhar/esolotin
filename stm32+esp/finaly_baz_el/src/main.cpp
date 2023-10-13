#include <Arduino.h>

#include "global.h"
#include "WiFi.h"
#include <HTTPClient.h>

#include <string>

#include "page.h"



#define RXD2 16
#define TXD2 17


#define BUFFER_SIZE_TX 5
#define BUFFER_SIZE_RX 1

const char* wifi_ssid = WIFISSID;
const char* wifi_pass = WIFIPASS;

WiFiServer server(80);

String header;    // Переменная для хранения HTTP-запроса

// Дополнительные переменные для хранения текущего состояния вывода
String ledEspState = "off";
String output26State = "off";
String output27State = "off";

// Назначьте выходные переменные контактам GPIO
const int output26 = 26;
const int output27 = 27;

// Текущее время
unsigned long currentTime = millis();
// Предыдущее время
unsigned long previousTime = 0; 
// Таймер на 2 секунды
const long timeoutTime = 2000;

bool led_state = false;

// digitalWrite(LED_BUILTIN, LOW);

//для записи в буфер от uart
int count_uart = 0;

//для данных от web
uint8_t level = 125; //по умолчанию порог           0...254
uint8_t light = 125; //по умолчанию освещенность    0...254

const char* input_parameter1 = "input_string";
const char* input_parameter2 = "input_integer";
const char* input_parameter3 = "input_float";


char* find_char(const char* str, char c)
{
    for( ; *str; ++str ) {
        if( *str == c ) break;
    }
    return (char*)str;
}

uint8_t transmitBuffer[BUFFER_SIZE_TX];
uint8_t receiveBuffer[BUFFER_SIZE_RX];

uint8_t debug_val = 0;
String strData = "";
boolean recievedFlag;
boolean transmitFlag;

void setup() {
  
  for (unsigned char i = 0; i < BUFFER_SIZE_TX; i++)
  {
    transmitBuffer[i] = i + 1;
  }
    for (unsigned char i = 0; i < BUFFER_SIZE_RX; i++)
  {
    receiveBuffer[i] = 0;
  }

  transmitFlag = 0;


  //uart к ПК
    Serial.begin(115200);
    delay(1000);
    WiFi.begin(wifi_ssid,wifi_pass);
    Serial.println("Setup done");

  //uart с nucleo

  // Serial2.begin(baud-rate, protocol, RX pin, TX pin);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.println("Serial Txd is on pin: "+String(TXD2));
  Serial.println("Serial Rxd is on pin: "+String(RXD2));

    //для vaficon
    std::function<void(void)> handleFavicon;

    
    //Wi-Fi 
    while (WiFi.status() != wl_status_t::WL_CONNECTED){
        Serial.print(".");
        delay(1000);
    }
    Serial.println("\nConected! To WiFi");
    String esp_mac = WiFi.macAddress(); // WiFi.mac....
    // Serial.println(WiFi.macAddress());

    String esp_ip  = WiFi.localIP().toString();
        // Serial.println(WiFi.localIP().toString());

 
    Serial.println("our mac = " + esp_mac);
    Serial.println("our ip = " + esp_ip);

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    server.begin();
    }

void loop() {

  // Слушаем входящих клиентов
  WiFiClient client = server.available();

  if(client)                    // Если подключается новый клиент
  {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // распечатать сообщение в последовательном порту
    String currentLine = "";                // создаем строку для входящих данных о клиента
    // while(client.connected())    //старое
    while (client.connected() && currentTime - previousTime <= timeoutTime) // цикл, пока клиент подключен
    {
      currentTime = millis();
      if(client.available())    // если есть байты для чтения от клиента,
      {
        char c = client.read();             // прочтите байт, затем
        Serial.write(c);                    // отправить в монитор порта
        header += c;

        // String line = client.readString();
        // Serial.print(line);
if (c == '\n') {                    // если байт является символом новой строки
          // если текущая строка пуста, у вас есть два символа новой строки подряд.
          // это конец клиентского HTTP-запроса, поэтому отправьте ответ:
          if (currentLine.length() == 0) {
            // Заголовки HTTP всегда начинаются с кода ответа (например, HTTP / 1.1 200 OK)
            // и тип содержимого, чтобы клиент знал, что будет дальше, затем пустая строка
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // включает и выключает GPIO
            if (header.indexOf("GET /LED/on") >= 0) {
              Serial.println("GPIO LED on");
              ledEspState = "on";
              // digitalWrite(output26, HIGH);


              led_state = true;
              digitalWrite(LED_BUILTIN, HIGH);

              // client.print(receiveBuffer[count_uart]); //это значение освещенности, что берется по uart с stm

            } else if (header.indexOf("GET /LED/off") >= 0) {
              Serial.println("GPIO LED off");
              ledEspState = "off";
              digitalWrite(LED_BUILTIN, LOW);
              // digitalWrite(output26, LOW);
            } else if (header.indexOf("GET /27/on") >= 0) {
              Serial.println("GPIO 27 on");
              output27State = "on";
              // digitalWrite(output27, HIGH);
            } else if (header.indexOf("GET /27/off") >= 0) {
              Serial.println("GPIO 27 off");
              output27State = "off";
              // digitalWrite(output27, LOW);
            } else if (header.indexOf("GET /get?input1=") >= 0) {
              // Serial.println(header.indexOf("GET /"));
              // String look ="";
              // look = header.find( "input1=" );
              // Serial.println(header[17]);
              char temp = header[16];
              uint8_t temp_int = (int)temp-48;
              for( int j = 0; j<5 ; j++)
              {
                temp = header[16+j];
                temp_int = (int)temp-48;
                if(j == 0 && temp_int > 0 & temp_int < 5)
                  transmitBuffer[j] = temp_int; 

                if(j > 0 && temp_int == 0 || temp_int == 1)
                  transmitBuffer[j] = temp_int; 
              }
              
              for(int j = 0; j<5; j++)
              {
                Serial.println(transmitBuffer[j]);
              }

              transmitFlag = true;

              Serial.println("    --send2--");
            }
            
            // Отображение веб-страницы HTML
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Не стесняйтесь изменять атрибуты background-color и font-size в соответствии со своими предпочтениями.
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Заголовок веб-страницы
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Отображение текущего состояния и кнопки включения / выключения для LED 
            client.println("<p>LED - State " + ledEspState + "</p>");
            // Если ledEspState выключен, отображается кнопка ВКЛ.       
            if (ledEspState=="off") {
              client.println("<p><a href=\"/LED/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/LED/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 

            client.println("<p>Input Mask for LEDs: </p>");
            client.println( "<form action=\"/get\">");
            client.println( "input1: <input type=\"text\" name=\"input1\">" );
            client.println( "<input type=\"submit\" value=\"Submit\">" );
            client.println( "</form><br>" );
            client.println("<p>Num LED, react on button1, react on button2, react on button3, react on button4</p>");
            client.println("<p>Example: \"31001\" - Third LED light on when pushed 1 and 4 button</p>");

            // client.println(F("<p>Brightness:</p>"));
            // client.println(F("<p><input name=\"light_val\" type=\"text\" value=\"125\" /></p>"));
            // client.println(F("<p>Trigger value:</p>"));
            // client.println(F("<p><input name=\"level_val\" type=\"text\" value=\"125\" /></p>"));
            // client.println("<p><input name=\"button\" type=\"button\" value=\"level_val\" /></p>");


            // // Отображение текущего состояния и кнопок включения / выключения для GPIO 27
            // client.println("<p>GPIO 27 - State " + output27State + "</p>");
            // // Если output27State выключен, отображается кнопка ON.       
            // if (output27State=="off") {
            //   client.println("<p><a href=\"/27/on\"><button class=\"button\">ON</button></a></p>");
            // } else {
            //   client.println("<p><a href=\"/27/off\"><button class=\"button button2\">OFF</button></a></p>");
            // }
            client.println("</body></html>");
            
            // HTTP-ответ заканчивается еще одной пустой строкой
            client.println();
            // Выйти из цикла while
            break;
          } else { // если у вас есть новая строка, очистите currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // если у вас есть что-то еще, кроме символа возврата каретки,
          currentLine += c;      // добавить его в конец currentLine
        }


      }
    }
    
    // Очистить переменную заголовка
    header = "";
    // Закройте соединение
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

    //обмен UART
  
    if (Serial2.available() > 0) 
    {                 // если есть что то на вход
    // strData = "";                               // очистить строку
      while (Serial2.available() > 0) 
      {            // пока идут данные
      // strData += (char)Serial2.read();           // забиваем строку принятыми данными
        receiveBuffer[0] = Serial2.read();
        delay(2);                                 // обязательно задержка, иначе вылетим из цикла раньше времени
      }
      recievedFlag = true;                        // поднять флаг что получили данные
      // transmitFlag = true;
    }
    
    if (recievedFlag) 
    {                           // если есть принятые данные
    // int intVal = strData.substring(2).toInt();  // перевести в int всю строку кроме первых двух символов!
        // int intVal = strData.toInt();  // перевести в int всю строку кроме первых двух символов!
        // Serial.print(intVal);
        
        //работает
                // Serial.print(receiveBuffer[0]); 
                // Serial.println("    --read--");

    // String header = strData.substring(0, 2);    // создать мини строку, содержащую первые два символа
    // if (strData.startsWith("m1"))               // если строка начинается с m1
    //   motor1 = intVal;
    // if (strData.startsWith("m2"))               // если строка начинается с m2
    //   motor2 = intVal;
    // if (strData.startsWith("b1"))               // если строка начинается с b1
    //   button1 = intVal;
    // if (strData.startsWith("b2"))               // если строка начинается с b2
    //   button2 = intVal;
    recievedFlag = false;                       // данные приняты, мой капитан
    }

    if(transmitFlag)
    {
      for(int k = 0; k<5; k++)
      {
        Serial2.write(transmitBuffer[k]);

        Serial.println(transmitBuffer[k]);


        // Serial.print(receiveBuffer[0] + k);
        // Serial.print(" debug val ");
        // Serial.print(debug_val);
        // debug_val++;
        // Serial.println("    --send--");
       
      }
    transmitFlag = false;
    }
}