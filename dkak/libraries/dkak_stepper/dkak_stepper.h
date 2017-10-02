/*
  dkak_stepper.h - stepper control library
  Created by Andy Koezle, November 10, 2016.
  private
*/
  
//CONTINUE HERE - force default mode to move command using pulse count. changing stepper mode requires additional parameters to be set for mode initialization (steps per rev, angle, lead_mm etc.)

#ifndef dkak_stepper_h
#define dkak_stepper_h

#include "Arduino.h"

class dkak_stepper
{
    private:
        byte pin_pul, pin_dir, pin_ena, stepsPerRev = 400;
        byte pin_limit = 0, pin_limitHome = 0;
        int delay_usec = 500;
        int mode = 0;
        double lead_mm = 8.0;
        double deg_perstep = 0.9, mm_perstep = 0.02;
    public:
        dkak_stepper(byte = 0, byte = 0, byte = 0);
        bool errorfree = true;
        void enable();
        void cycle();
        bool isHome();
        void serial_feedback(int = 3, int = 0, int = 0);
        void set_dir(auto);
        void set_mode(int, int = 400, int = 8);
        void set_name(String);
        void set_pins_limit(byte, byte);
        void set_pulsedelay(int);
        int step(int);
        bool triggeredLimit(int = 3, bool = true);
        bool move(int);
        bool movehome(bool = true);
        bool rotate(double);
        bool validate_steps(int, int, bool = true);
        double pos_current = 0.0;
        double rot_current = 0.0;
        String name = "";
        String state = "OK";
};

#endif

