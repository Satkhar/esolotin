# esolotin
my repo

---
base C - first full project on Github. programm can do base analityc for temperature array. value takes from *.csv file.
input keys:  
-h Help.  
-f <filename.csv> input file.  
-m <month_number> statistick for current month.

output value:  
mean temperature;  
min temperature;  
max temperature.  

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
one for esp 32 in visual studio code using platformio, the second for stm32.
there is a server on the esp that allows you to flash a diode on the esp board and has an input field for setting the mask. the mask is sent to the stm.
on the stm (nucleo f103rb board), the mask is read by uart, the illumination value is sent. the uart itself is implemented via dma. 4 tasks for freertos:  
1 task. reading the illumination (the ADC is also started via dma), and starting the uart exchange task to send the illumination value to the esp / reading the reception buffer from the esp.  
2 task - uart, where the received buffer is read and the mask is updated. and the dma is started to receive/send values.  
3 task - reading the status of the buttons and changing according to the eventGroup.  
4 task - event status query and LED control.
