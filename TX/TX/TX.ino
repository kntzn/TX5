/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/

#include "Button.h"
#include "Pinout.h"

#include <Adafruit_SSD1306.h>

#ifndef SSD1306_128_32 
#error incorrect display size, check Adafruit_SSD1306.h
#endif // !SSD1306_128_32 
 
#include <LowPower.h>



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


    digitalWrite (HC12_SET, HIGH);
    digitalWrite (POT_PWR,  HIGH);



    }

int main ()
    { 
    initialize ();

    Serial.begin (9600);

    // Display
    Adafruit_SSD1306 display;
    display.begin (SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    display.clearDisplay ();  // TODO: add logo as default value of buffer
    display.display ();
    display.setTextSize (2);
    display.setTextColor (WHITE);

    // Buttons
    Button button_left   (GPI_TOP);
    Button button_select (GPI_MIDDLE);
    Button button_right  (GPI_BOTTOM);
    
    digitalWrite (POT_PWR, HIGH);
    
    while (true)
        {
        while (Serial.available ())
            { 
            char read = Serial.read ();
        
            if (read == '\n')
                {
                display.display ();
                display.clearDisplay ();
                display.setCursor (0, 0);
                }
            else
                display.print (read);
            }

        Serial.println (char (constrain (map (analogRead (POT_IN), 0, 1023, 0, 242), 0, 243)));
        }


    }