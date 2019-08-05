// 
// 
// 

#include "Button.h"

#define N_TESTS 10
#define HOLD_TIME 500


bool Button::isPrsd ()
    {
    int prsd = 0;


    for (int i = 0; i < N_TESTS; i++)
        if (!digitalRead (pin))
            prsd++;

    return (prsd > (N_TESTS / 2));
    }

Button::Button (byte pinId):
    last_isPrsd (false),
    pin (pinId),
    curr_state (State::unpressed),
    last_press (millis ())
    {
    }

void Button::upd ()
    {
    bool current_isPrsd = isPrsd ();

    // If button is already pressed
    if (current_isPrsd && last_isPrsd)
        curr_state = State::pressed;
    // If button is being pressed now
    else if (current_isPrsd && !last_isPrsd)
        { 
        last_press = millis ();
        curr_state = State::press;
        }
    // If button is being released now
    else if (!current_isPrsd && last_isPrsd)
        {
        if (millis () - last_press > HOLD_TIME)
            curr_state = State::hold;
        else
            curr_state = State::released;
        }
    // If button is not pressed 
    else
        curr_state = State::unpressed;

    // Updates the last_isPrsd
    if (current_isPrsd)
        last_isPrsd = true;
    else         
        last_isPrsd = false;

    }

Button::State Button::state ()
    {
    return curr_state;
    }


#undef N_TESTS
#undef HOLD_TIME