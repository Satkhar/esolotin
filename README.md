# esolotin
Hello there!

Little bit aboult this repo.
Repository create as an extension to my resume (CV).
The purpose of these projects is to show mastery of skills and technologies.
Some projects have only text file with description.

<details>
<summary>My projects</summary>

| Num | Project name |
|-----:|-----------|
|     1| Analys temp from *.csv file |
|     2| Calculate area with E pricision |
|     3| LCD_gyro_ino       |
|     4| Сircuit-based rand generator  |
|     5| ESP32 + Gyro + TinyML  |
|     6| Home environment project  |

</details>

---

More detalied description:

<details>
<summary>Analys temp from *.csv file</summary>

folder: /analys_temp
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

advanced C - second full project on Github. programm calculates area figure forming by three curves with E pricision.
input keys:  
-h Help.  
-s <len> Test function find square for square with <len> length. Float positive values.  
-r Start test function find root by Chord method.  
-e <eps> Input float positive value of precision. Standart usage precision is 0.001  

output:  
calculated area.

---

stm32+esp - project with two chapters.  
finaly_baz_el - project on ESP32. 
finaly_freertos - project on STM32.  
first for esp32, design in visual studio code and using platformio plugin, the second for stm32, design in cubeIDE.
there is a server on the esp that allows you to flash a diode on the esp board and has an input field for setting the mask. the mask is sent to the stm.
on the stm (nucleo f103rb board), the mask is read by uart, the illumination value is sent. the uart itself is implemented via dma. 4 tasks for freertos:  
1 task. reading the illumination (the ADC is also started via dma), and starting the uart exchange task to send the illumination value to the esp / reading the reception buffer from the esp.  
2 task - uart, where the received buffer is read and the mask is updated. and the dma is started to receive/send values.  
3 task - reading the status of the buttons and changing according to the eventGroup.  
4 task - event status query and LED control.
