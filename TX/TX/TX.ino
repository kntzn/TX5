/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/

#include "Button.h"
#include "Pinout.h"
#include "Comm.h"
#include "Battery.h"

#include <Adafruit_SSD1306.h>

#define drawSexyStartupLogo drawStartupLogo

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

void drawStartupLogo (Adafruit_SSD1306 &display)
    {
    display.display ();
    delay (500);
    for (int i = 0; i < SSD1306_LCDHEIGHT / 2; i++)
        {
        display.drawFastHLine (0, SSD1306_LCDHEIGHT / 2 - i - 1,
                               SSD1306_LCDWIDTH, BLACK);
        display.drawFastHLine (0, SSD1306_LCDHEIGHT / 2 + i,
                               SSD1306_LCDWIDTH, BLACK);

        display.display ();
        }
    
    // First bunch of text
    delay (750);
    display.setCursor (SSD1306_LCDHEIGHT / 4, SSD1306_LCDHEIGHT / 4);
    display.setTextColor (WHITE);
    display.setTextSize (2);
    display.print ("ESK8 ");
    display.display ();
    // Second one
    delay (750);
    display.setTextSize (1);
    display.print ("by kntzn");
    display.display ();

    // Fade to black
    delay (500);
    for (int i = 0; i < SSD1306_LCDHEIGHT / 2; i++)
        {
        display.drawFastHLine (0, i,
                               SSD1306_LCDWIDTH, BLACK);
        display.drawFastHLine (0, SSD1306_LCDHEIGHT - 1 - i,
                               SSD1306_LCDWIDTH, BLACK);


        display.display ();
        }
    }

int main ()
    { 
    initialize ();

    // Communication
    Serial.begin (9600);
    
    // Display
    Adafruit_SSD1306 display;
    display.begin (SSD1306_SWITCHCAPVCC, SSD1306_I2C_ADDRESS);
    drawSexyStartupLogo (display);
    
    // Buttons
    Button button_left   (GPI_TOP);
    Button button_select (GPI_MIDDLE);
    Button button_right  (GPI_BOTTOM);
    
    // on/off indicator
    analogWrite (LED_2_G, 5);

    Battery battery;
    double voltage = 0.0;
    
    Communication HC12;
    unsigned long int last_request = millis ();
    bool waitingForRequest = false;

    while (true)
        {
        // Button ev. handlers
        button_left.upd ();
        button_select.upd ();
        button_right.upd ();


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
        if (button_right.state () == Button::State::press)
            {
            display.clearDisplay ();
            display.setCursor (0, 0);
            display.setTextColor (WHITE);
            display.setTextSize (2);
            display.print (voltage);
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
        
        //Serial.print (char (analogRead (POT_IN) / 4));

        // Comm. v2.1 // two-way
        if (waitingForRequest)
            { 
            // Timeout
            if (millis () - last_request > RESPONSE_TIMEOUT)
                {
                waitingForRequest = false;
                voltage = -1;
                }

            Communication::response resp;
            if ((resp = HC12.receiveResponse ()) != 
                Communication::response::noresp)
                {
                switch (resp)
                    {
                    case Communication::response::voltage:
                        {
                        voltage = (HC12.argbuf ()[0] * 256 + 
                                   HC12.argbuf ()[1]) / 1000.0;
                        break;
                        }
                    default:
                        break;
                    }

                waitingForRequest = false;
                }
            }
        else
            { 
            if (millis () - last_request > REQUEST_PERIOD)
                { 
                last_request += REQUEST_PERIOD;

                waitingForRequest = true;
                
                HC12.sendRequest (Communication::command::voltage);
                }
            else
                {
                int pot = 1023 - analogRead (POT_IN);

                HC12.sendCommand (Communication::command::throttle, pot);
                }
            }

        

        // prevents RX being overfed with packets
        delay (5);
        }


    }
