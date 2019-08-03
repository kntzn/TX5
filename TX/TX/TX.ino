/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/

#include "Pinout.h"

#include <Adafruit_SSD1306.h>

#ifndef SSD1306_128_32 
#error incorrect display size, check Adafruit_SSD1306.h
#endif // !SSD1306_128_32 
 



void initialize ()
    { 
    // inits the microcontroller
    init ();

    // setups the pins' modes
    
    // SET pin for the HC-12
    pinMode (HC12_SET, OUTPUT);

    // Buttons
    pinMode (GPI_TOP,    INPUT_PULLUP);
    pinMode (GPI_MIDDLE, INPUT_PULLUP);
    pinMode (GPI_BOTTOM, INPUT_PULLUP);

    // LEDS
    pinMode (LED_2_B,    OUTPUT);
    pinMode (LED_2_G,    OUTPUT);
    pinMode (LED_2_R,    OUTPUT);
    pinMode (LED_1_G,    OUTPUT);
    pinMode (LED_1_B,    OUTPUT);
    pinMode (LED_1_R,    OUTPUT);

    // Thumb pot
    pinMode (POT_IN,     INPUT);
    pinMode (POT_PWR,    OUTPUT);

    // VCC in
    pinMode (A3,         INPUT);
    }

int main ()
    { 
    initialize ();

    Adafruit_SSD1306 display;
    display.begin (SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
//    display.drawLine (0, 0, 16, 16, WHITE);
    display.clearDisplay ();
    display.display ();

    const auto d = 5;

    while (true)
        { 
        for (int i = 0; i < 64; i++)
            { 
            analogWrite (LED_2_R, 63);
            analogWrite (LED_2_G, i);
            analogWrite (LED_2_B, 0);
            delay (d);
            }

        for (int i = 0; i < 64; i++)
            {
            analogWrite (LED_2_R, 63 - i);
            analogWrite (LED_2_G, 63);
            analogWrite (LED_2_B, 0);
            delay (d);
            }

        for (int i = 0; i < 64; i++)
            {
            analogWrite (LED_2_R, 0);
            analogWrite (LED_2_G, 63);
            analogWrite (LED_2_B, i);
            delay (d);
            }

        for (int i = 0; i < 64; i++)
            {
            analogWrite (LED_2_R, 0);
            analogWrite (LED_2_G, 63 - i);
            analogWrite (LED_2_B, 63);
            delay (d);
            }

        for (int i = 0; i < 64; i++)
            {
            analogWrite (LED_2_R, i);
            analogWrite (LED_2_G, 0);
            analogWrite (LED_2_B, 63);
            delay (d);
            }

        for (int i = 0; i < 64; i++)
            {
            analogWrite (LED_2_R, 63);
            analogWrite (LED_2_G, 0);
            analogWrite (LED_2_B, 63 - i);
            delay (d);
            }
        }


    display.display ();
    }



