/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/

#include "Button.h"
#include "Pinout.h"
#include "Comm.h"
#include "Battery.h"
#include "SysConfig.h"

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

void drawMainScreen  (Adafruit_SSD1306 &display,
                      Battery &battery, Communication &HC12,
                      double esk8_voltage, double esk8_speed,
                      uint16_t current_mode, unsigned long latency)
    { 
    display.clearDisplay ();
    display.setCursor (0, 0);
    display.setTextColor (WHITE);
    display.setTextSize (1);
    display.print ("TX:   ");
    display.print (battery.getBatVoltage ());
    display.print (" V");
    display.setCursor (0, 8);
    display.print ("ESK8: ");
    if (HC12.rawinputActive ())
        {
        display.print ("RAW");
        }
    else if (esk8_voltage == -1)
        display.print ("-.-- V");
    else
        {
        display.print (((esk8_voltage) / 6));
        display.print (" V");
        }

    display.setCursor (SSD1306_LCDWIDTH / 8, SSD1306_LCDHEIGHT / 2);
    display.setTextSize (2);

    if (HC12.rawinputActive ())
        {
        display.print ("RAW km/h");
        }
    else if (esk8_speed == -1)
        display.print ("--  km/h");
    else
        {
        if (esk8_speed < 10.0)
            display.print (' ');

		// TODO: improve rounding fix
        display.print (double (esk8_speed + 0.5), 0);
        display.print (" km/h");
        }

    display.setCursor (SSD1306_LCDWIDTH * 5 / 8 + 3, 0 + 1);
    display.drawRect (SSD1306_LCDWIDTH * 5 / 8, 0, 16 + 1, 16 + 1, WHITE);
    display.setTextSize (2);

    switch (current_mode)
        {
        case mode::lock:
            display.print ('P');
            break;
        case mode::neutral:
            display.print ('N');
            break;
        case mode::eco:
            display.print ('E');
            break;
        case mode::normal:
            display.print ('D');
            break;
        case mode::sport:
            display.print ('S');
            break;
        default:
            break;
        }

    display.setTextSize (1);
    display.print (" ");
    display.print (latency);
    display.print ("ms");
    display.display ();
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

    double esk8_voltage = 0.0;
    double esk8_speed   = 0.0;
    uint8_t current_mode = mode::normal;

    Communication HC12;
    unsigned long int latency = 0;
    uint8_t last_req_id = 0;
    unsigned long int last_request = millis ();
	bool lights_on = false;

    bool waitingForRequest = false;

    while (true)
        {
        // Button ev. handlers
        button_left.upd ();
        button_select.upd ();
        button_right.upd ();

		// TODO: move content to separate functions
        if (button_select.state () == Button::State::hold)
            {
            HC12.sendCommand (Communication::command::raw,
                              static_cast <uint16_t>
                              (Communication::command::raw_safety));
            HC12.activateRawinput ();
            }
        if (button_left.state () == Button::State::released)
            {
			current_mode = 2 + (++current_mode - 2) % 3;

            HC12.sendCommand (Communication::command::mode, current_mode * 256);
            }
		if (button_left.state() == Button::State::hold)
			{
			current_mode = 1;

			HC12.sendCommand(Communication::command::mode, current_mode * 256);
			}
        if (button_right.state () == Button::State::press)
            {
			lights_on = !lights_on;
			//       key = 255 (byte #1, fl)     + 255 (byte #2, bl)
			uint16_t key = 255 * (256)*lights_on + 255 * lights_on;

			HC12.sendCommand(Communication::command::lights, key);
            }

        drawMainScreen (display,
                        battery, HC12, 
                        esk8_voltage, esk8_speed,
                        current_mode, latency);

        // Main scr.
        battery.batMeasure (VCC_IN);
       
        // If single-byte one-way communication is active
        if (HC12.rawinputActive ())
            Serial.write (255 - (analogRead (POT_IN) / 4));
        // else default two-way comm. is used
        else
            { 
            if (waitingForRequest)
                {
                // Timeout
                if (millis () - last_request > RESPONSE_TIMEOUT)
                    {
                    waitingForRequest = false;
                    esk8_voltage = -1;
                    esk8_speed   = -1;
                    }

                Communication::response resp;
                if ((resp = HC12.receiveResponse ()) !=
                    Communication::response::noresp)
                    {
                    // Calculates the delay between TX/RX
                    latency = (millis () - last_request) / 2;

                    switch (resp)
                        {
                        case Communication::response::voltage:
                            {
                            esk8_voltage = (HC12.argbuf () [0] * 256 +
                                            HC12.argbuf () [1]) / 1000.0;
                            break;
                            }
                        case Communication::response::speed:
                            { 
                            esk8_speed = (HC12.argbuf () [0] * 256 +
                                          HC12.argbuf () [1]) / 1000.0;
                            
                            break;
                            }
                        default:
                            break;
                        }

                    HC12.flush ();
                    waitingForRequest = false;
                    }
                }
            else
                {
                if (millis () - last_request > REQUEST_PERIOD)
                    {
                    last_request += REQUEST_PERIOD;

                    waitingForRequest = true;

                    if (last_req_id == 0)
                        HC12.sendRequest (Communication::command::voltage);
                    if (last_req_id == 1)
                        HC12.sendRequest (Communication::command::speed);

                    (++last_req_id) %= 2;
                    }
                else
                    {
                    int pot = 1023 - analogRead (POT_IN);

                    // TODO: RM the software belt slipp. fix
                    pot = constrain (pot, 100, 1024);

                    HC12.sendCommand (Communication::command::throttle, pot);
                    }
                }
            }
        
        // prevents RX being overfed with packets
        delay (5);
        }
    }

