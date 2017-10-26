/*
  dkak_texscan.h - bluetooth control library
  Created by Andy Koezle, June 24, 2017.
  private
*/
  
#ifndef dkak_texscan_h
#define dkak_texscan_h

#include "Arduino.h"
#include "dkak_stepper.h"

class dkak_texscan
{
    private:
        int path_distance = 0;
    public:
        void init(dkak_stepper, dkak_stepper, dkak_stepper, dkak_stepper, int);
        bool exec_cmd(String);
        bool errorfree();
        dkak_stepper stepper_transl_x, stepper_transl_y;
        dkak_stepper stepper_led_x, stepper_led_y;
        void serial_feedback(int);
};

#endif

