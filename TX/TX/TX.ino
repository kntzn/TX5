/*
 Name:		TX.ino
 Created:	8/3/2019 6:54:18 PM
 Author:	kntzn
*/


void initialize ()
    { 
    // inits the microcontroller
    init ();

    // setups the pins' modes
    
    // SET pin for the HC-12
    pinMode (2, OUTPUT);

    // Buttons
    pinMode (3, INPUT_PULLUP);
    pinMode (5, INPUT_PULLUP);
    pinMode (6, INPUT_PULLUP);

    // LEDS
    // #1
    pinMode (9,  OUTPUT);
    pinMode (10, OUTPUT);
    pinMode (11, OUTPUT);
    // #2
    pinMode (12, OUTPUT);
    pinMode (13, OUTPUT);
    pinMode (A0, OUTPUT);

    // Thumb pot
    pinMode (A1, INPUT);  // Pot in
    pinMode (A2, OUTPUT); // Pot power

    // VCC in
    pinMode (A3, INPUT);
    }

int main ()
    { 
    initialize ();

    }



