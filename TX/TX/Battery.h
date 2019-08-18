#pragma once
#include "Config.h"

#ifndef _BATTERY_h
#define _BATTERY_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class Battery
    {
    private:
        double bat_voltage;

        long aver_analog (uint8_t pin);

        long readVcc ();
            
    public:
        Battery ();

        void batMeasure (byte pin)
            {
            double new_value = readVcc ()*aver_analog (pin) / 1023 / 1000.0;
            
            bat_voltage = bat_voltage * 0.9 + new_value * 0.1;
            }
        double getBatVoltage ()
            {
            return bat_voltage;
            }

    };

#endif






