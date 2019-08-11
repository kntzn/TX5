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

        long aver_analog (uint8_t pin, byte times = 50);

        long readVcc ();
            

    public:
        Battery ();

        void batMeasure (byte pin)
            {
            bat_voltage = readVcc ()*aver_analog (pin, 25U) / 1023 / 1000.0;
            }
        double getBatVoltage ()
            {
            return bat_voltage;
            }

    };

#endif






