# esolotin
Hello there!
My name is Evgeny Solotin.

Little bit aboult this repo.
Repository create as an extension to my resume (CV) - esolotin_CV.docx.
The purpose of these projects is to show mastery of skills and technologies.
Some projects have only text file with description.

<details>
<summary>My projects</summary>

| Num | Project name |
|-----:|-----------|
|     1| Analys temp from *.csv file Analys temp from *.csv file  |
|     2| Calculate area with E pricision |
|     3| STM32 FreeRTOS + ESP32 |
|     4| LCD_gyro_ino       |
|     5| Сircuit-based rand generator  |
|     6| ESP32 + Gyro + TinyML  |
|     7| Home environment project  |

</details>

---

> More detailed description:

<details>
<summary>Analys temp from *.csv file</summary>

folder: /1.analys_temp  
first full C project on Github.   
include source files + makefile.  
programm can do base analityc for temperature array. value takes from *.csv file.
input keys:  
-h Help.  
-f <filename.csv> input file.  
-m <month_number> statistick for current month.

output value:  
mean temperature;  
min temperature;  
max temperature.  
</details>

---

<details>
<summary>Calculate area with E pricision</summary>
  
folder: /2.calculate_area  
second full C project on Github.  
include source files + makefile.  
programm calculates area figure forming by three curves with E pricision.
input keys:  
-h Help.  
-s <len> Test function find square for square with <len> length. Float positive values.  
-r Start test function find root by Chord method.  
-e <eps> Input float positive value of precision. Standart usage precision is 0.001  

output:  
calculated area.
</details>

---

<details>
<summary>STM32 FreeRTOS + ESP32</summary>
    
folder: /3.stm_freertos_esp
project with two chapters.  
finaly_baz_el - project on ESP32.  
finaly_freertos - project on STM32.  
first for esp32, design in visual studio code and using platformio plugin, the second for stm32, design in cubeIDE.  
there is a web-server(HTML) on the esp32 that allows you to flash a diode on the esp board and has an input field for setting the mask. the mask is sent to the stm.  
on the stm (nucleo f103rb board), the mask is read by uart, the illumination value is sent. uart is realized via dma. 4 tasks for freertos:  
1 task. reading the illumination (the ADC is also started via dma), and starting the uart exchange task to send the illumination value to the esp / reading the reception buffer from the esp.  
2 task - uart, where the received buffer is read and the mask is updated. and the dma is started to receive/send values.  
3 task - reading the status of the buttons and changing according to the eventGroup.  
4 task - event status query and LED control.  
</details>

---

<details>
  <summary>
    LCD_gyro_ino
  </summary>

  
folder: /4.lcd_gyro_ino  
project designed in adruinoIDE for introduction with IDE and ino platform.  
include source file + readme.  
functions:  
resistance measurement by the included ADC;  
recieve data from gyro adxl345 by I2C;  
display readed values on LCD ST7735S by SPI;  
switching low power mode by press button.  

</details>

---

<details>
  <summary>
    Сircuit-based rand generator
  </summary>

  
folder: /5.rand_generator  
project designed in adruinoIDE for introduction with IDE and ino platform.  
include source file + readme.  
Random number generator on seven-segment indicators.  

the task of the project is to get the skill of designing electronic circuits with free CAD systems, such as EasyEDA, and to practice of mounting and debugging the designed device.  
technical specification, calculation schematic and the result electronic scheme in the "generator_rand" file.  

</details>

---

<details>
  <summary>
    ESP32 + Gyro + TinyML
  </summary>

  
folder: /6.tinyML_esp32 
Motion interpreter.   
include readme with description and link on public project edgeImpulse.  


the project was implemented using the online platform edgeimpulse.  
The task of the project is to introduce and implement TinyML, a tiny machine learning.  

</details>

---

<details>
  <summary>
    Home environment project
  </summary>

  
folder: /8.environment_project 
Project with few parts.

include:  
High Level Design (HLD) for project;  
project for esp32 with sensors;  
project for esp32 with LCD and control panel.

>coming soon
</details>

---
