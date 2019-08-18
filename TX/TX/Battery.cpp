#include "Battery.h"



long Battery::aver_analog (uint8_t pin)
    {
    const int n_measurements = 25;
    uint16_t analog_readings [n_measurements] = { };

    for (int i = 0; i < n_measurements; i++)
        analog_readings [i] = analogRead (pin);
    
    for (int i = 0; i < n_measurements; i++)
        for (int j = 0; j < n_measurements - 1; j++)
            if (analog_readings [j] > analog_readings [j + 1])
                {
                uint16_t tmp            = analog_readings [j];
                analog_readings [j]     = analog_readings [j + 1];
                analog_readings [j + 1] = tmp;
                }   

    return ((n_measurements % 2) ? 
            (analog_readings [n_measurements / 2]) :
            (analog_readings [(n_measurements / 2)] + 
             analog_readings [(n_measurements / 2) - 1]) / 2);
    }

long Battery::readVcc ()
    {
    #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
        ADMUX = _BV (REFS0) | _BV (MUX4) | _BV (MUX3) | _BV (MUX2) | _BV (MUX1);
    #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
        ADMUX = _BV (MUX5) | _BV (MUX0);
    #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
        ADMUX = _BV (MUX3) | _BV (MUX2);
    #else
        ADMUX = _BV (REFS0) | _BV (MUX3) | _BV (MUX2) | _BV (MUX1);
    #endif
    delay (2); // Waiting for reference voltage to settle
    ADCSRA |= _BV (ADSC); // Start conversion
    while (bit_is_set (ADCSRA, ADSC)); // Measuring
    uint8_t low = ADCL; // must read ADCL first - it then locks ADCH
    uint8_t high = ADCH; // unlocks both
    long result = (high << 8) | low;

    result = REF_VOLTAGE * 1023 * 1000 / result;
    return result;
    }


Battery::Battery ():
    bat_voltage (3.7)
    {
    }