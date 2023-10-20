#ifndef MY_PINS_H
#define MY_PINS_H


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
D5      -   CS              //define in setup()
D18     -   SCK             //define in pins_arduino.h
D21     -   A0 (or D/C)     //define in setup()
D22     -   RES             //define in setup()
D23     -   SDA             //define in  pins_arduino.h
*/
#ifdef DEBUG_MAKET

#define SCK_LCD     5   //
#define SDA_LCD     18  //
#define A0_LCD      19  //
#define RES_LCD     21  //
#define CS_LCD      22  //

#else

#define SCK_LCD     18   //
#define SDA_LCD     23  //
#define A0_LCD      21  //
#define RES_LCD     22  //
#define CS_LCD      5  //

#endif  //DEBUG_MAKET

#endif  //MY_PINS_H
