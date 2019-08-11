/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/

#include "Button.h"
#include "Pinout.h"

#include "Battery.h"

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

    // Disables AT command mode of the HC-12
    digitalWrite (HC12_SET, HIGH);
    // Activates the pot
    digitalWrite (POT_PWR,  HIGH);
    }

int main ()
    { 
    initialize ();

    // Communication
    Serial.begin (9600);
    
    unsigned long last_send = millis ();
    
    // Display
    Adafruit_SSD1306 display;
    display.begin (SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    display.clearDisplay ();  // TODO: add logo as default value of buffer
    display.display ();
    
    // Buttons
    Button button_left   (GPI_TOP);
    Button button_select (GPI_MIDDLE);
    Button button_right  (GPI_BOTTOM);
    
    // on/off indicator
    analogWrite (LED_2_G, 5);

    Battery battery;
    

    while (true)
        {
        // Button ev. handlers
        button_left.upd ();
        button_select.upd ();

        if (button_left.state () == Button::State::press)
            { 
            battery.batMeasure (VCC_IN);

            display.clearDisplay ();
            display.setCursor (0, 0);
            display.setTextColor (WHITE);
            display.setTextSize (2);
            display.print (battery.getBatVoltage ());
            display.display ();
            }
        if (button_select.state () == Button::State::press)
            { 
            display.clearDisplay ();

            display.display ();
            }

        // Currently unused
        /*
        while (Serial.available ())
            { 
            analogWrite (LED_2_R, 50);

            byte read = Serial.read ();

            display.print (int (read));
            display.display ();
            display.clearDisplay ();
            display.setCursor (0, 0);
            }
            */
       
        // Constantly send bytes with throttle
        Serial.print (char (analogRead (POT_IN) / 4));
        }


    }
